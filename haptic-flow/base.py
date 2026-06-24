# =============================================================================
#  base.py  --  haptic force/torque controller for the "base" flow scene
# =============================================================================
#
#  This file is an H3DAPI Python script. In the X3D scene (base.x3d / base2.x3d)
#  it is instantiated as:
#
#      <PythonScript DEF="PS" url="base.py"> ... </PythonScript>
#
#  H3D exposes every *module-level* field instance defined here as a field of
#  that "PS" node, addressable by name from the scene graph. Two consequences
#  drive the naming in this file:
#
#    1. The module-level names that hold field objects (max_force, max_torque,
#       inputmode, outputmode, seedAtEdge, seedPos, paddleAxis, vecstring,
#       calculated_force, calculated_torque, kbS, kbSpace, vf) are PART OF THE
#       PUBLIC INTERFACE. base.x3d ROUTEs reference "seedPos", "paddleAxis" and
#       "vecstring" by name, and the companion scripts IrisReader.py and
#       state.py reach in as PS.max_force, PS.inputmode, PS.outputmode, etc.
#       These names therefore MUST NOT change.
#
#    2. Everything that is purely internal -- the helper classes, the evaluator
#       object, local variables -- is free to carry longer, descriptive names.
#
#  What the script does, in one sentence: it samples the flow vector at the
#  haptic device tip, turns that vector into a force (and optionally a torque)
#  according to the currently selected interaction mode, and routes the result
#  into the ForceTorqueField node that renders it on the device.
#
# -----------------------------------------------------------------------------

from H3DInterface import *
import math, time


# -----------------------------------------------------------------------------
#  Tunable strength limits (exposed as PS.max_force / PS.max_torque).
#
#  These are SFFloat fields rather than plain numbers precisely so that other
#  scripts can read and overwrite them. IrisReader.py recomputes sensible
#  values from the loaded dataset and routes them back into these fields.
# -----------------------------------------------------------------------------
max_force = SFFloat()
max_torque = SFFloat()
max_force.setValue(8)
max_torque.setValue(600)

# Whether stream-surface seeding should happen at the edge of the paddle
# rather than at its centre. state.py toggles this when entering the
# force+torque (6-DOF output) mode.
seedAtEdge = SFBool(False)

# inputmode  : 0 = 3-DOF input  (use the raw flow vector)
#              1 = 6-DOF input  (project the flow onto the paddle normal)
# outputmode : 0 = output OFF   (render nothing)
#              1 = force only
#              2 = force + torque
# Both are driven by state.py via PS.inputmode / PS.outputmode.
inputmode = SFInt32(0)
outputmode = SFInt32(0)


# The "references" field of the PythonScript node delivers, in order, the
# four scene nodes this script wires itself to:
#   - force_torque_field : the ForceTorqueField that renders force/torque
#   - haptic_device      : the haptics device (position + orientation source)
#   - vector_volume      : the sampled vector volume (FieldedVectorImageVolume)
#   - local_info         : a LocalInfo node carrying the probe transform
# (The unpacking order is fixed by the scene file and must be kept.)
force_torque_field, haptic_device, vector_volume, local_info = references.getValue()


def compute_cross_product(first_vector, second_vector):
	"""Return the 3D cross product first_vector x second_vector.

	H3D's Vec3f does not expose a cross-product operator in this binding, so
	we spell it out component by component.
	"""
	x = first_vector.y * second_vector.z - first_vector.z * second_vector.y
	y = first_vector.z * second_vector.x - first_vector.x * second_vector.z
	z = first_vector.x * second_vector.y - first_vector.y * second_vector.x
	return Vec3f(x, y, z)


class SampledFlowVectorField(SFVec3f):
	"""Caches the most recent flow vector sampled from the vector volume.

	It behaves as an SFVec3f whose value tracks whatever vector is routed in
	(the output of the vector-volume sampler). getVal() is a debugging hook
	left from development.
	"""
	val = Vec3f(0, 0, 0)

	def getVal(self, position):
		unused_transform = Matrix4f()
		print position, self.val
		return self.val

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		self.val = incoming_routes[0].getValue()
		print self.val
		return self.val


class HapticForceTorqueEvaluator:
	"""Turns a (flow vector, device orientation) pair into a (force, torque).

	A single instance is shared by the force field and the torque field so the
	relatively expensive evaluation happens once per unique input. The last
	result is memoised: if neither the flow vector nor the orientation changed
	since the previous call, the cached force/torque are returned unchanged.
	"""
	# The inputs and outputs of the most recent evaluation (used for caching).
	vector = SFVec3f(Vec3f(0, 0, 0))
	orientation = SFRotation()
	force = Vec3f(0, 0, 0)
	torque = Vec3f(0, 0, 0)

	# Diagnostics: a rough per-second evaluation-rate counter and the largest
	# squared flow magnitude seen so far.
	freq = 0
	last_update_time = 0
	max_sq_vec = 0

	def __init__(self, sampled_flow_field):
		self.sampled_flow_field = sampled_flow_field

	def evaluateForceTorque(self, flow_vector, device_orientation):
		# Return the cached result when nothing has changed.
		if flow_vector == self.vector and device_orientation == self.orientation:
			return self.force, self.torque
		self.vector = flow_vector
		self.orientation = device_orientation

		# Track the strongest flow seen (handy when calibrating max_force).
		squared_flow_magnitude = flow_vector * flow_vector
		if squared_flow_magnitude > self.max_sq_vec:
			self.max_sq_vec = squared_flow_magnitude
			#print "new max vec with:", flow_vector

		# Maintain a crude "evaluations per wall-clock second" estimate.
		current_second = int(time.time())
		if self.last_update_time != current_second:
			#print "update frequency: ", self.freq
			self.freq = 0
			self.last_update_time = current_second
		else:
			self.freq += 1

		local_x_axis = Vec3f(1, 0, 0)
		local_y_axis = Vec3f(0, 1, 0)
		local_z_axis = Vec3f(0, 0, 1)

		# The paddle is modelled as a flat plate whose normal is the device's
		# local +Y axis expressed in world space.
		orientation_matrix = Matrix3f(device_orientation)
		paddle_normal = orientation_matrix * local_y_axis

		if outputmode.getValue() == 0:  # output OFF -> no haptic response
			return (Vec3f(0, 0, 0), Vec3f(0, 0, 0))

		if inputmode.getValue() == 0:  # 3-DOF input -> push along the raw flow
			self.force = max_force.getValue() * flow_vector
			self.torque = Vec3f(0, 0, 0)
			return self.force, self.torque

		# --- 6-DOF input from here on -------------------------------------
		# Only the component of the flow along the paddle normal produces a
		# force, and that force is directed along the normal: the user feels
		# the plate being pushed by the part of the flow that hits it head-on.
		flow_along_normal = paddle_normal * flow_vector
		self.force = max_force.getValue() * flow_along_normal * paddle_normal

		if outputmode.getValue() == 2:
			# Torque spins the plate about the axis perpendicular both to the
			# paddle normal and to the device's local X axis, scaled by how
			# strongly the flow presses on the plate.
			torque_axis = compute_cross_product(orientation_matrix * local_x_axis, paddle_normal)
			self.torque = max_torque.getValue() * flow_along_normal * torque_axis
		else:
			self.torque = Vec3f(0, 0, 0)
		return (self.force, self.torque)

	def getForce(self, flow_vector, device_orientation):
		return self.evaluateForceTorque(flow_vector, device_orientation)[0]

	def getTorque(self, flow_vector, device_orientation):
		return self.evaluateForceTorque(flow_vector, device_orientation)[1]


class RenderedForceField(TypedField(SFVec3f, (SFVec3f, SFRotation))):
	"""SFVec3f field whose value is the force computed by the evaluator.

	Routed inputs: [0] the sampled flow vector, [1] the device orientation.
	"""
	max_sq_force = 0

	def __init__(self, evaluator):
		SFVec3f.__init__(self)
		self.evaluator = evaluator

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		flow_vector = incoming_routes[0].getValue()
		device_orientation = incoming_routes[1].getValue()
		force = self.evaluator.getForce(flow_vector, device_orientation)
		squared_force_magnitude = force * force
		if squared_force_magnitude > self.max_sq_force:
			self.max_sq_force = squared_force_magnitude
			#print "new square force max :", squared_force_magnitude
		return force


class RenderedTorqueField(TypedField(SFVec3f, (SFVec3f, SFRotation))):
	"""SFVec3f field whose value is the torque computed by the evaluator.

	Routed inputs: [0] the sampled flow vector, [1] the device orientation.
	"""
	def __init__(self, evaluator):
		SFVec3f.__init__(self)
		self.evaluator = evaluator

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		flow_vector = incoming_routes[0].getValue()
		device_orientation = incoming_routes[1].getValue()
		torque = self.evaluator.getTorque(flow_vector, device_orientation)
		return torque


class KeyPressMatcher(TypedField(SFBool, (SFString))):
	"""Boolean field that is True while one of a set of keys is pressed.

	Accepts either a single key string or a list of equivalent keys.
	"""
	def __init__(self, watched_keys):
		SFBool.__init__(self)
		if type(watched_keys) == list:
			self.keys = watched_keys
		else:
			self.keys = [watched_keys]

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		pressed_key = incoming_routes[0].getValue()
		if pressed_key in self.keys:
			return True
		return False


class PaddleAxisFromOrientation(TypedField(SFVec3f, (SFRotation))):
	"""Maps a device orientation to the paddle's local +Z axis in world space.

	Used to orient the stream-surface seed line along the paddle.
	"""
	def __init__(self):
		SFVec3f.__init__(self)

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		device_orientation = incoming_routes[0].getValue()
		orientation_matrix = Matrix3f(device_orientation)
		return orientation_matrix * Vec3f(0, 0, 1)


class SeedPositionCalculator(TypedField(SFVec3f, (SFRotation, SFVec3f))):
	"""Computes where to drop the stream-surface seed.

	Routed inputs: [0] device orientation, [1] device position. When
	seedAtEdge is set (force+torque mode) the seed is offset to the edge of
	the paddle along its local +X axis; otherwise it sits at the device
	position itself.
	"""
	def __init__(self):
		SFVec3f.__init__(self)

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		device_orientation = incoming_routes[0].getValue()
		device_position = incoming_routes[1].getValue()
		#seed_type = incoming_routes[2].getValue()
		#if (seed_type % 3) != 2:
		if not seedAtEdge.getValue():
			return device_position
		orientation_matrix = Matrix3f(device_orientation)
		return orientation_matrix * Vec3f(1, 0, 0) * 0.02 + device_position


class Vec3fToMFStringFormatter(TypedField(MFString, (SFVec3f))):
	"""Formats a Vec3f as a single "x y z" string (for on-screen text)."""
	def __init__(self):
		MFString.__init__(self)

	def update(self, event):
		incoming_vector_field, = self.getRoutesIn()
		vector_value = incoming_vector_field.getValue()
		formatted = "%f %f %f" % (vector_value.x, vector_value.y, vector_value.z)
		return [formatted]


# -----------------------------------------------------------------------------
#  Build the field network.
#
#  The module-level names below are exposed as fields of the "PS" node, so they
#  keep their original spellings even though the classes behind them now have
#  longer names.
# -----------------------------------------------------------------------------
vecstring = Vec3fToMFStringFormatter()

vf = SampledFlowVectorField()
#vector_volume.value.route(vf)
force_torque_evaluator = HapticForceTorqueEvaluator(vf)
calculated_force = RenderedForceField(force_torque_evaluator)
calculated_torque = RenderedTorqueField(force_torque_evaluator)
kbS = KeyPressMatcher(['s', 'S'])
kbSpace = KeyPressMatcher(" ")

paddleAxis = PaddleAxisFromOrientation()
haptic_device.trackerOrientation.route(paddleAxis)

seedPos = SeedPositionCalculator()
#haptic_device.trackerOrientation.route(seedPos)
#haptic_device.trackerPosition.route(seedPos)

#paddleAxis.route(sss.geometry.seedOrientation)

# Feed the device position (transformed into volume space by the LocalInfo
# node) into the vector volume, then route the sampled flow vector and the
# device orientation into the force/torque calculators, and finally the
# results into the ForceTorqueField that renders them.
haptic_device.trackerPosition.route(vector_volume.vector)
local_info.transform.route(vector_volume.vector)
vector_volume.vector.route(calculated_force)
vector_volume.vector.route(calculated_torque)
haptic_device.trackerOrientation.route(calculated_force)
haptic_device.trackerOrientation.route(calculated_torque)
calculated_force.route(force_torque_field.force)
calculated_torque.route(force_torque_field.torque)

# Keep the mode/strength fields "live" by routing them to the event sink, so
# that changes made by state.py are processed even with no other consumer.
for tunable_field in [max_force, max_torque, seedAtEdge, inputmode, outputmode]:
	tunable_field.route(eventSink)
print "max_torque %s, max_force %s" % (max_torque.getValue(), max_force.getValue())

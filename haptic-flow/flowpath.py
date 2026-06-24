# =============================================================================
#  flowpath.py  --  haptic force/torque controller for the "flowpath" scene
# =============================================================================
#
#  H3DAPI Python script, loaded by flowpath.x3d as a <PythonScript> node.
#
#  It samples the flow vector at the haptic device tip and renders it as a
#  force + torque on the device, modelling the probe as a flat paddle: the user
#  feels the component of the flow that presses on the plate, plus a torque
#  that tries to align the plate with the flow.
#
#  This script is one of a family of nearly identical per-scene controllers
#  (flowpath.py, flowpath2.py, flume.py). They differ ONLY in:
#    * the MAX_FORCE / MAX_TORQUE strength constants below, and
#    * whether the device position is routed straight into the vector volume
#      (see the wiring section at the foot of the file).
#  Compared with base.py this variant has no input/output mode switching: it
#  always produces the 6-DOF (force + torque) response.
#
#  Naming note: the module-level field objects (vf, calculated_force,
#  calculated_torque, kbS, kbSpace, paddleAxis, seedPos) are exposed by H3D as
#  fields of this script node and may be addressed by name from the scene
#  graph, so their spellings are preserved. Internal helpers use longer names.
#
# -----------------------------------------------------------------------------

from H3DInterface import *
import math, time


# --- per-scene strength constants -------------------------------------------
MAX_FORCE = 2
MAX_TORQUE = 100
#MAX_TORQUE=30

# The four scene nodes wired to this script, in the order fixed by the
# "references" field of the PythonScript node:
#   force_torque_field : ForceTorqueField that renders the force/torque
#   haptic_device      : haptics device (position + orientation source)
#   vector_volume      : sampled vector volume
#   local_info         : LocalInfo node carrying the probe transform
force_torque_field, haptic_device, vector_volume, local_info = references.getValue()


def compute_cross_product(first_vector, second_vector):
	"""Return the 3D cross product first_vector x second_vector."""
	x = first_vector.y * second_vector.z - first_vector.z * second_vector.y
	y = first_vector.z * second_vector.x - first_vector.x * second_vector.z
	z = first_vector.x * second_vector.y - first_vector.y * second_vector.x
	return Vec3f(x, y, z)


class SampledFlowVectorField(SFVec3f):
	"""Caches the most recent flow vector routed in from the vector volume."""
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

	One instance is shared by the force and torque fields, and the last result
	is memoised so an unchanged input is not recomputed.
	"""
	vector = SFVec3f(Vec3f(0, 0, 0))
	orientation = SFRotation()
	force = Vec3f(0, 0, 0)
	torque = Vec3f(0, 0, 0)

	# Diagnostics.
	freq = 0
	last_update_time = 0
	max_sq_vec = 0

	def __init__(self, sampled_flow_field):
		self.sampled_flow_field = sampled_flow_field

	def evaluateForceTorque(self, flow_vector, device_orientation):
		# Reuse the cached result when neither input changed.
		if flow_vector == self.vector and device_orientation == self.orientation:
			return self.force, self.torque
		self.vector = flow_vector
		self.orientation = device_orientation

		# Track the strongest flow seen so far.
		squared_flow_magnitude = flow_vector * flow_vector
		if squared_flow_magnitude > self.max_sq_vec:
			self.max_sq_vec = squared_flow_magnitude
			#print "new max vec with:", flow_vector

		# Crude evaluations-per-second estimate.
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

		# Paddle normal: the device's local +Y axis, expressed in world space.
		orientation_matrix = Matrix3f(device_orientation)
		paddle_normal = orientation_matrix * local_y_axis

		# Force: only the component of the flow along the paddle normal counts,
		# and it pushes along that normal.
		flow_along_normal = paddle_normal * flow_vector
		self.force = MAX_FORCE * flow_along_normal * paddle_normal

		# Torque about the axis perpendicular to both the paddle normal and the
		# device's local X axis, scaled by how hard the flow presses on it.
		#torque_axis = orientation_matrix * local_z_axis
		torque_axis = compute_cross_product(orientation_matrix * local_x_axis, paddle_normal)
		self.torque = MAX_TORQUE * flow_along_normal * torque_axis
		return self.force, self.torque

	def getForce(self, flow_vector, device_orientation):
		return self.evaluateForceTorque(flow_vector, device_orientation)[0]

	def getTorque(self, flow_vector, device_orientation):
		return self.evaluateForceTorque(flow_vector, device_orientation)[1]


class RenderedForceField(TypedField(SFVec3f, (SFVec3f, SFRotation))):
	"""SFVec3f field holding the force from the evaluator.

	Routed inputs: [0] sampled flow vector, [1] device orientation.
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
	"""SFVec3f field holding the torque from the evaluator.

	Routed inputs: [0] sampled flow vector, [1] device orientation.
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
	"""Boolean field that is True while one of a set of keys is pressed."""
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
	"""Maps a device orientation to the paddle's local +Z axis in world space."""
	def __init__(self):
		SFVec3f.__init__(self)

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		device_orientation = incoming_routes[0].getValue()
		orientation_matrix = Matrix3f(device_orientation)
		return orientation_matrix * Vec3f(0, 0, 1)


class SeedPositionCalculator(TypedField(SFVec3f, (SFRotation, SFVec3f, SFInt32))):
	"""Computes where to drop the stream-surface seed.

	Routed inputs: [0] device orientation, [1] device position, [2] seed type.
	For most seed types the seed sits at the device position; for the
	"edge" seed type (seed_type % 3 == 2) it is offset to the paddle edge
	along the paddle's local +X axis.
	"""
	def __init__(self):
		SFVec3f.__init__(self)

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		device_orientation = incoming_routes[0].getValue()
		device_position = incoming_routes[1].getValue()
		seed_type = incoming_routes[2].getValue()
		if (seed_type % 3) != 2:
			return device_position
		orientation_matrix = Matrix3f(device_orientation)
		return orientation_matrix * Vec3f(1, 0, 0) * 0.02 + device_position


# -----------------------------------------------------------------------------
#  Build the field network. Module-level field names are preserved because H3D
#  exposes them as fields of this script node.
# -----------------------------------------------------------------------------
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

# Device position drives the vector-volume sampler (transformed into volume
# space by the LocalInfo node); the sampled flow and the device orientation
# drive the force/torque calculators; the results drive the ForceTorqueField.
haptic_device.trackerPosition.route(vector_volume.vector)
local_info.transform.route(vector_volume.vector)
vector_volume.vector.route(calculated_force)
vector_volume.vector.route(calculated_torque)
haptic_device.trackerOrientation.route(calculated_force)
haptic_device.trackerOrientation.route(calculated_torque)
calculated_force.route(force_torque_field.force)
calculated_torque.route(force_torque_field.torque)

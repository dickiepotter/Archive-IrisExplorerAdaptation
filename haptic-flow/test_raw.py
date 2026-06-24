# =============================================================================
#  test_raw.py  --  minimal force/torque test for a raw vector volume
# =============================================================================
#
#  H3DAPI Python script used to exercise the haptic force/torque pipeline
#  against a vector volume loaded from a raw dataset, without the full scene
#  machinery (no LocalInfo, no stream surface).
#
#  Unlike flowpath.py, the torque here spins the paddle about the device's own
#  local Z axis (rather than an axis derived from a cross product). That
#  behaviour is preserved exactly.
#
#  Module-level field names (vf, calculated_force, calculated_torque) are
#  exposed by H3D as fields of this script node, so they are left unchanged.
#
# -----------------------------------------------------------------------------

from H3DInterface import *
import math, time

MAX_FORCE = 2
MAX_TORQUE = 75
#MAX_TORQUE=30

# references order for this test: ForceTorqueField, haptics device, vector volume.
force_torque_field, haptic_device, vector_volume = references.getValue()

# experimental min-max vals :
# X :  [-0.41781607270240784, 0.27050405740737915]
# Y :  [-0.26837897300720215, 0.17145438492298126]
# Z :  [-0.11769801378250122, 0.3062838613986969]

# The commented-out vectorField below is an earlier experiment that recorded
# the min/max device positions seen, to discover the working volume extent.
#class vectorField:
#	extr_x=None
#	extr_y=None
#	extr_z=None
#
#	def getVal(self, position):
#		if not self.extr_x:
#			self.extr_x=[position.x,position.x]
#			self.extr_y=[position.y,position.y]
#			self.extr_z=[position.z,position.z]
#		else:
#			if position.x < self.extr_x[0]:
#				self.extr_x[0]=position.x
#			elif position.x > self.extr_x[1]:
#				self.extr_x[1] = position.x
#			if position.y < self.extr_y[0]:
#				self.extr_y[0]=position.y
#			elif position.y > self.extr_y[1]:
#				self.extr_y[1] = position.y
#			if position.z < self.extr_z[0]:
#				self.extr_z[0]=position.z
#			elif position.z > self.extr_z[1]:
#				self.extr_z[1] = position.z
#		print "X : ", self.extr_x, " Y : ", self.extr_y, " Z : ", self.extr_z
#		return Vec3f(0,1,0)

class SampledFlowVectorField(SFVec3f):
	"""Caches the most recent flow vector routed in from the vector volume."""
	val = Vec3f(0, 0, 0)

	def getVal(self, position):
		unused_transform = Matrix4f()
#		print type(data)
#		return data.getVector(unused_transform, position)
		#return Vec3f(0,1,0)
		print position, self.val
		return self.val

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		self.val = incoming_routes[0].getValue()
		print self.val
		return self.val

class HapticForceTorqueEvaluator:
	"""Turns a (flow vector, device orientation) pair into a (force, torque).

	The last result is memoised so an unchanged input is not recomputed.
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

		local_y_axis = Vec3f(0, 1, 0)
		local_z_axis = Vec3f(0, 0, 1)

		# Paddle normal: the device's local +Y axis, expressed in world space.
		orientation_matrix = Matrix3f(device_orientation)
		paddle_normal = orientation_matrix * local_y_axis

		# Force pushes along the paddle normal, scaled by the component of the
		# flow that hits the plate head-on.
		flow_along_normal = paddle_normal * flow_vector
		self.force = MAX_FORCE * flow_along_normal * paddle_normal

		# Torque about the device's local Z axis (in world space).
		torque_axis = orientation_matrix * local_z_axis
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

vf = SampledFlowVectorField()
#vector_volume.value.route(vf)
force_torque_evaluator = HapticForceTorqueEvaluator(vf)
calculated_force = RenderedForceField(force_torque_evaluator)
calculated_torque = RenderedTorqueField(force_torque_evaluator)

# Earlier wiring kept for reference: route position straight into the
# calculators rather than through the vector volume.
#calculated_force.route(force_torque_field.force)
#haptic_device.trackerPosition.route(calculated_force)
#haptic_device.trackerOrientation.route(calculated_force)
#
#calculated_torque.route(force_torque_field.torque)
#haptic_device.trackerPosition.route(calculated_torque)
#haptic_device.trackerOrientation.route(calculated_torque)
#
#haptic_device.trackerPosition.route(vector_volume.vector)

haptic_device.trackerPosition.route(vector_volume.vector)
vector_volume.vector.route(calculated_force)
vector_volume.vector.route(calculated_torque)
haptic_device.trackerOrientation.route(calculated_force)
haptic_device.trackerOrientation.route(calculated_torque)
calculated_force.route(force_torque_field.force)
calculated_torque.route(force_torque_field.torque)

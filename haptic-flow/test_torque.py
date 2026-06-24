#!/usr/bin/python
# =============================================================================
#  test_torque.py  --  force/torque test against a constant upward flow field
# =============================================================================
#
#  H3DAPI Python script. Instead of sampling a real dataset, the "flow" here is
#  a constant (0, 1, 0) vector everywhere, so the test isolates how the paddle
#  force/torque model responds to the device's orientation alone.
#
#  Force pushes along the paddle normal (device local +Y in world space); the
#  torque spins the paddle about its local Z axis. Module-level field names
#  (calculated_force, calculated_torque) are exposed by H3D and so are kept.
#
# -----------------------------------------------------------------------------
from H3DInterface import *
import math

MAX_FORCE = 1
MAX_TORQUE = 30

# references order for this test: ForceTorqueField, haptics device.
force_torque_field, haptic_device = references.getValue()

# experimental min-max vals :
# X :  [-0.41781607270240784, 0.27050405740737915]
# Y :  [-0.26837897300720215, 0.17145438492298126]
# Z :  [-0.11769801378250122, 0.3062838613986969]

# Earlier experiment that recorded the min/max device positions seen, to
# discover the working volume extent. Kept for reference.
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

class ConstantFlowField:
	"""A trivial flow field that returns (0, 1, 0) at every position."""
	def getVal(self, position):
		return Vec3f(0, 1, 0)

class HapticForceTorqueEvaluator:
	"""Turns a (device position, device orientation) pair into (force, torque).

	The flow vector is looked up from the (constant) flow field at the device
	position. The last result is memoised so an unchanged input is not
	recomputed.
	"""
	pos = SFVec3f(Vec3f(0, 0, 0))
	orientation = SFRotation()
	force = Vec3f(0, 0, 0)
	torque = Vec3f(0, 0, 0)

	def __init__(self, flow_field):
		self.flow_field = flow_field

	def evaluateForceTorque(self, device_position, device_orientation):
		# Reuse the cached result when neither input changed.
		if device_position == self.pos and device_orientation == self.orientation:
			return self.force, self.torque
		self.pos = device_position
		self.orientation = device_orientation

		local_y_axis = Vec3f(0, 1, 0)
		local_z_axis = Vec3f(0, 0, 1)

		flow_vector = self.flow_field.getVal(self.pos)
		orientation_matrix = Matrix3f(device_orientation)
		# Paddle normal: device local +Y axis in world space.
		paddle_normal = orientation_matrix * local_y_axis
		flow_along_normal = paddle_normal * flow_vector
		self.force = MAX_FORCE * flow_along_normal * paddle_normal

		# Torque about the device's local Z axis (in world space).
		torque_axis = orientation_matrix * local_z_axis
		self.torque = MAX_TORQUE * flow_along_normal * torque_axis
		return self.force, self.torque

	def getForce(self, device_position, device_orientation):
		return self.evaluateForceTorque(device_position, device_orientation)[0]

	def getTorque(self, device_position, device_orientation):
		return self.evaluateForceTorque(device_position, device_orientation)[1]

class RenderedForceField(TypedField(SFVec3f, (SFVec3f, SFRotation))):
	"""SFVec3f field holding the force from the evaluator.

	Routed inputs: [0] device position, [1] device orientation.
	"""
	def __init__(self, evaluator):
		SFVec3f.__init__(self)
		self.evaluator = evaluator

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		device_position = incoming_routes[0].getValue()
		device_orientation = incoming_routes[1].getValue()
		force = self.evaluator.getForce(device_position, device_orientation)
		return force

class RenderedTorqueField(TypedField(SFVec3f, (SFVec3f, SFRotation))):
	"""SFVec3f field holding the torque from the evaluator.

	Routed inputs: [0] device position, [1] device orientation.
	"""
	def __init__(self, evaluator):
		SFVec3f.__init__(self)
		self.evaluator = evaluator

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		device_position = incoming_routes[0].getValue()
		device_orientation = incoming_routes[1].getValue()
		torque = self.evaluator.getTorque(device_position, device_orientation)
		return torque

constant_flow_field = ConstantFlowField()
force_torque_evaluator = HapticForceTorqueEvaluator(constant_flow_field)
calculated_force = RenderedForceField(force_torque_evaluator)
calculated_force.route(force_torque_field.force)
haptic_device.trackerPosition.route(calculated_force)
haptic_device.trackerOrientation.route(calculated_force)

calculated_torque = RenderedTorqueField(force_torque_evaluator)
calculated_torque.route(force_torque_field.torque)
haptic_device.trackerPosition.route(calculated_torque)
haptic_device.trackerOrientation.route(calculated_torque)

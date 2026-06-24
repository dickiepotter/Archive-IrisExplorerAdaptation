#!/usr/bin/python
# =============================================================================
#  force_field.py  --  orientation-only paddle force, no flow dataset
# =============================================================================
#
#  H3DAPI Python script. The simplest member of the force family: there is no
#  vector volume at all. The "flow" is taken to be the fixed world-up
#  direction (0, 1, 0), and the force is the component of that flow along the
#  paddle normal, pushed back along the normal -- so the user feels the paddle
#  being pressed by a steady upward current whose strength depends only on how
#  the paddle is tilted.
#
#  calculated_force is exposed by H3D as a field of this script node and so
#  keeps its name.
#
# -----------------------------------------------------------------------------
from H3DInterface import *
import math

MAX_FORCE = 2

class RenderedForceField(TypedField(SFVec3f, SFRotation)):
	"""SFVec3f field whose value is the paddle force for a fixed upward flow.

	Routed input: [0] device orientation.
	"""
	val = SFVec3f()

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		device_orientation = incoming_routes[0].getValue()
		world_up = Vec3f(0, 1, 0)
		orientation_matrix = Matrix3f(device_orientation)
		# Paddle normal: the device's local +Y axis, expressed in world space.
		paddle_normal = orientation_matrix * world_up
		#if paddle_normal.y < 0: paddle_normal = -paddle_normal
		force = MAX_FORCE * (paddle_normal * world_up) * paddle_normal
		self.val.setValue(force)
		return self.val.getValue()

calculated_force = RenderedForceField()

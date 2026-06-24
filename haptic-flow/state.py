# =============================================================================
#  state.py  --  interaction-mode state machine
# =============================================================================
#
#  H3DAPI Python script, instantiated as the "STATESCRIPT" node. Three keyboard
#  counters in the scene (input / output / graphics) are routed into the
#  exposed "state" field; each time one of them changes, this script maps the
#  combined (haptic-input, haptic-output, graphics) triple onto:
#    * the haptic interaction mode in the base.py controller
#      (PS.inputmode / PS.outputmode / PS.seedAtEdge),
#    * which probe shape is shown (probe_shape_switch.whichChoice),
#    * and how the stream surface is drawn (stream_surface.seedType /
#      .display / .wireFrame).
#
#  "state" is exposed by H3D as a field of this node (the scene ROUTEs into it
#  by name), so that name is preserved.
#
# -----------------------------------------------------------------------------

from H3DInterface import *
import PS
import sys, time

# references order: the Switch selecting the probe shape, and the
# IBFVStreamSurface whose display style we control.
probe_shape_switch, stream_surface = references.getValue()


def flush():
	sys.stdout.flush()


# A short audible cue on state changes. The console-bell version is kept for
# reference; the active definition is a no-op (the second def wins).
#TODO: use winsound.Beep() under windows, and this else
def beep():
	print chr(7)
	print chr(7)
	print chr(7)
	flush()
def beep():
	pass


class InteractionStateMachine(TypedField(MFInt32, (SFInt32, SFInt32, SFInt32))):
	"""Maps the (haptic-input, haptic-output, graphics) counter triple onto the
	haptic and visualisation modes.

	Routed inputs (each a free-running counter, taken modulo its range):
	  [0] haptic input mode  -> 0 = 3-DOF, 1 = 6-DOF
	  [1] haptic output mode -> 0 = off,   1 = force, 2 = force + torque
	  [2] graphics mode      -> 0 = hidden, 1 = wireframe, 2 = surface
	"""
	val = None

	def __init__(self):
		MFInt32.__init__(self)
		self.do_update(0, 0, 0)
		beep()
		print "%s: starting" % time.ctime()
		flush()

	def update(self, event):
		incoming_routes = self.getRoutesIn()
		haptic_input_mode = incoming_routes[0].getValue() % 2
		haptic_output_mode = incoming_routes[1].getValue() % 3
		graphics_mode = incoming_routes[2].getValue() % 3
		return self.do_update(haptic_input_mode, haptic_output_mode, graphics_mode)

	def do_update(self, haptic_input_mode, haptic_output_mode, graphics_mode):
		requested_state = (haptic_input_mode, haptic_output_mode, graphics_mode)
		if requested_state == self.val:
			return list(requested_state)
		# 3-DOF input combined with force+torque output is not a valid
		# combination; ignore the change and keep the previous state.
		if requested_state[:2] == (0, 2):
			return list(self.val)

		# --- push the haptic mode into the base.py controller ----------------
		PS.inputmode.setValue(haptic_input_mode)
		PS.outputmode.setValue(haptic_output_mode)
		# Seed at the paddle edge only in the force+torque mode.
		if haptic_output_mode == 2:
			PS.seedAtEdge.setValue(True)
		else:
			PS.seedAtEdge.setValue(False)

		# --- choose the probe shape -----------------------------------------
		# 0 = point probe (3-DOF input), 2 = edge paddle (force+torque),
		# 1 = flat paddle (everything else).
		if haptic_input_mode == 0:
			probe_shape_switch.whichChoice.setValue(0)
		elif haptic_output_mode == 2:
			probe_shape_switch.whichChoice.setValue(2)
		else:
			probe_shape_switch.whichChoice.setValue(1)

		# --- choose the stream-surface seed style ---------------------------
		# seedType 2 fans out along the paddle for 6-DOF input; 1 is the plain
		# straight seed line for 3-DOF input.
		if haptic_input_mode == 1:
			stream_surface.seedType.setValue(2)
		else:
			stream_surface.seedType.setValue(1)

		# --- choose the graphics style --------------------------------------
		if graphics_mode == 0:
			stream_surface.display.setValue(False)
		else:
			stream_surface.display.setValue(True)
			if graphics_mode == 1:
				stream_surface.wireFrame.setValue(True)
			else:  # graphics_mode == 2
				stream_surface.wireFrame.setValue(False)

		print "%s: changing to state (%d,%d,%d)" % ((time.ctime(),) + requested_state)
		flush()
		self.val = requested_state
		return list(requested_state)


state = InteractionStateMachine()
state.route(eventSink)

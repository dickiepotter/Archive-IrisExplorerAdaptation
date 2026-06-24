# Copyright 2005, Karljohan Lundin
#

"""This file provides extra functionality to the VHTK package. For
more information read the comments for the included classes.


 This file is part of Volume Haptics Toolkit.

 Volume Haptics Toolkit is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 Volume Haptics Toolkit is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Volume Haptics Toolkit; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

"""

from H3D import *
from H3DInterface import *

# references order: the two haptic modes to cycle between with the space bar.
first_haptic_mode, second_haptic_mode = references.getValue()

# Start with the first mode active and the second one off.
first_haptic_mode.active.setValue(1)
second_haptic_mode.active.setValue(0)

class Typist( AutoUpdate( SFString ) ):
  """This field reads of a KeySensor and activate/deactivates the
  specified haptic modes when space is pressed.

  Initially the first mode is active and the second deactivated. When
  space is pressed, the first is deactivated and the second
  activated. The next time space is pressed, the modes are back to the
  initial state.
  """

  def __init__( self ):
    AutoUpdate( SFString ).__init__(self)
    # A four-step cycle advanced by each press of the space bar:
    #   1 -> 2 : turn on snap-drag for the first mode
    #   2 -> 3 : switch from the first mode to the second
    #   3 -> 4 : turn on snap-drag for the second mode
    #   4 -> 1 : switch back to the initial state
    self.state = 1

  def update( self, event ):
    pressed_key = event.getValue()
    if pressed_key == ' ':
      if self.state == 1:
        self.state = 2
        first_haptic_mode.snapdrag.setValue(1)
      elif self.state == 2:
        self.state = 3
        first_haptic_mode.active.setValue(0)
        second_haptic_mode.active.setValue(1)
        second_haptic_mode.snapdrag.setValue(0)
      elif self.state == 3:
        self.state = 4
        second_haptic_mode.snapdrag.setValue(1)
      elif self.state == 4:
        self.state = 1
        first_haptic_mode.active.setValue(1)
        second_haptic_mode.active.setValue(0)
        first_haptic_mode.snapdrag.setValue(0)
    return ""

typist = Typist()
keyboard = createX3DNodeFromString("<KeySensor />")[0]
keyboard.keyPress.route(typist)

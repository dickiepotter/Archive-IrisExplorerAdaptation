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

# Set to a true value to print each seed position as a tube is finished.
SETTINGS_PRINT_SEEDS = 0

# references order:
#   tubes_group        : the Group node the new StreamTubes shapes are added to
#   tube_appearance    : the Appearance applied to every created shape
#   template_tubes     : a template StreamTubes node whose settings are copied
tubes_group, tube_appearance, template_tubes, = references.getValue()


class PutStreamTubes(TypedField(AutoUpdate(SFBool),(SFBool,SFVec3f))):
    """PutStreamTubes allows the user to interactively release stream
    tubes in a specified vector volume.

    The following must be provided through the "references" field:
    1) the group node in which the stream tubes should be put,
    2) an appearance to use, and
    3) a template StreamTubes node

    To the provided field putStreamTubes the following must be routed:
    1) the button to put the stream tubes with and
    2) the seed position in the same coordinate system as the volume
    and the group node.

    A KeySensor is used to provide the following commands
    z   undo
    Z   undo all
    1-9 release multiple stream tubes
    """
    tubes=None

    def __init__( self ):
        AutoUpdate(SFBool).__init__(self)
        # State carried between updates while the button is held:
        #   button : the button state seen on the previous update
        #   type   : how many tubes to release at once (0 means a single tube)
        self.button = 0
        self.type = 0

    def undo( self ):
        # Remove the most recently added shape from the group, if any.
        try:
            current_children = tubes_group.children.getValue()
            tubes_group.children.setValue( current_children[:-1] )
        except:
            pass

    def update( self, event ):
        # Routed inputs: [0] the trigger button, [1] the current seed position.
        try:
            routed_inputs = self.getRoutesIn()
            button_now = routed_inputs[0].getValue()
            seed_position = routed_inputs[1].getValue()
            button_before = self.button
        except:
            return 0

        if (button_now == 1 and button_before == 0) or not self.tubes:
            # Button just pressed (or no tube node yet): create a fresh
            # StreamTubes node, copy the template's settings onto it, seed it
            # at the current position, and add it to the group.
            #print "Creating tube node (%d,%d)." % ( button_now, self.button )
            shape = createX3DNodeFromString( """
	     <Shape></Shape>
            """ )[0]

            shape.appearance.setValue(tube_appearance)

            self.tubes = createX3DNodeFromString( """
             <StreamTubes />
            """ )[0]

            # While the tube is being placed it is integrated with a coarse
            # (doubled) step for responsiveness; it is refined when finished.
            self.tubes.radius.setValue      ( template_tubes.radius.getValue() )
            self.tubes.step.setValue        ( 2*template_tubes.step.getValue() )
            self.tubes.maxLength.setValue   ( template_tubes.maxLength.getValue() )
            self.tubes.threshold.setValue   ( template_tubes.threshold.getValue() )
            self.tubes.twoSided.setValue    ( template_tubes.twoSided.getValue() )
            self.tubes.useEuler.setValue    ( template_tubes.useEuler.getValue() )

            self.tubes.scalar2red.setValue  ( template_tubes.scalar2red.getValue() )
            self.tubes.scalar2green.setValue( template_tubes.scalar2green.getValue() )
            self.tubes.scalar2blue.setValue ( template_tubes.scalar2blue.getValue() )

            if( template_tubes.radiusVolume.getValue() != None ):
                self.tubes.radiusVolume.setValue( template_tubes.radiusVolume.getValue() )
            self.tubes.scalarVolume.setValue( template_tubes.scalarVolume.getValue() )
            self.tubes.vectorVolume.setValue( template_tubes.vectorVolume.getValue() )

            seed_positions = self.tubes.source.getValue()
            seed_positions.append( seed_position )
            self.tubes.source.setValue( seed_positions )

            group_children = tubes_group.children.getValue()
            shape.geometry.setValue( self.tubes )
            group_children.append( shape )
            tubes_group.children.setValue(group_children)

            # Remember where this drag started and how many seeds we just
            # added, so the next update can replace them.
            self.position = seed_position
            self.amount = 1

        elif button_now == 1 and button_before == 1:
            # Button held: update the seed(s) to follow the moving position.
            #print "Modifying ribbon node (%d,%d)." % ( button_now, self.button )
            if self.type == 0:
                # Single tube: replace the last seed with the current position.
                seed_positions = self.tubes.source.getValue()
                seed_positions = seed_positions[:-self.amount]

                seed_positions.append( seed_position )
                self.amount = 1

                self.tubes.source.setValue( seed_positions )

            else:
                # Multiple tubes: spread `type` seeds evenly along the line
                # from where the drag started to the current position.
                seed_positions = self.tubes.source.getValue()
                seed_positions = seed_positions[:-self.amount]

                for tube_index in range(self.type):
                    interpolation = float(tube_index) / float(self.type-1)
                    seed_positions.append( (1-interpolation)*self.position + ( interpolation )*seed_position )

                self.amount = self.type
                self.tubes.source.setValue( seed_positions )

        elif button_now == 0 and button_before == 1:
            # Button released: refine the tube by halving its step back to the
            # template resolution and finalise the seed list.
            #print "Finishing ribbon node (%d,%d)." % ( button_now, self.button )
            self.tubes.step.setValue( .5*self.tubes.step.getValue() )
            seed_positions = self.tubes.source.getValue()
            if SETTINGS_PRINT_SEEDS:
                for finished_seed in seed_positions[-self.amount:]:
                    print finished_seed
            self.tubes.source.setValue( seed_positions )

        self.button = button_now
        return 1

putStreamTubes = PutStreamTubes()


class Typist( AutoUpdate( SFString ) ):
    # Reads single key presses from a KeySensor:
    #   X   clear every released tube
    #   x   undo the most recent tube
    #   2-9 set how many tubes a single drag releases (1 or less -> single)
    def update( self, event ):
        pressed_key = event.getValue()
        if pressed_key == 'X':
            tubes_group.children.setValue([])
        if pressed_key == 'x':
            putStreamTubes.undo()
        else:
            try:
              requested_count = int(pressed_key)
              if requested_count < 2:
                putStreamTubes.type = 0
              else:
                putStreamTubes.type = requested_count
            except:
              pass
        return ""

typist = Typist()
keyboard = createX3DNodeFromString("<KeySensor />")[0]
keyboard.keyPress.route(typist)

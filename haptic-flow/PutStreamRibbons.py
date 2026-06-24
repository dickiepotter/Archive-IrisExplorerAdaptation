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

# Set to a true value to print each seed position as a ribbon is finished.
SETTINGS_PRINT_SEEDS = 0

# references order:
#   ribbons_group       : the Group node the new StreamRibbons shapes go into
#   ribbon_appearance   : the Appearance applied to every created shape
#   template_ribbons    : a template StreamRibbons node whose settings are copied
ribbons_group, ribbon_appearance, template_ribbons, = references.getValue()


class PutStreamRibbons(TypedField(AutoUpdate(SFBool),(SFBool,SFVec3f))):
    """PutStreamRibbons allows the user to interactively release
    stream ribbons in a specified vector volume.

    The following must be provided through the "references" field:
    1) the group node in which the stream ribbons should be put,
    2) an appearance to use, and
    3) a template StreamRibbons node

    To the provided field putStreamRibbons the following must be routed:
    1) the button to put the stream ribbons with and
    2) the seed position in the same coordinate system as the volume
    and the group node.

    A KeySensor is used to provide the following commands
    z   undo
    Z   undo all
    1-9 release multiple stream ribbons
    """

    def __init__( self ):
        AutoUpdate(SFBool).__init__(self)
        # State carried between updates while the button is held:
        #   button : the button state seen on the previous update
        #   type   : how many ribbons to release at once (0 means a single one)
        self.button = 0
        self.type = 0

    def undo( self ):
        # Remove the most recently added shape from the group, if any.
        try:
            current_children = ribbons_group.children.getValue()
            ribbons_group.children.setValue( current_children[:-1] )
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

        if button_now == 1 and button_before == 0:
            # Button just pressed: create a fresh StreamRibbons node, copy the
            # template's settings, seed it, and add it to the group.
            #print "Creating ribbon node (%d,%d)." % ( button_now, self.button )
            shape = createX3DNodeFromString( """
	     <Shape></Shape>
            """ )[0]

            shape.appearance.setValue(ribbon_appearance)

            print "----------------- creating ribbons"
            self.ribbons = createX3DNodeFromString( """
             <StreamRibbons />
            """ )[0]

            # Use a coarse (doubled) step while placing for responsiveness; it
            # is refined when the ribbon is finished.
            self.ribbons.width.setValue       ( template_ribbons.width.getValue() )
            self.ribbons.step.setValue        ( 2*template_ribbons.step.getValue() )
            self.ribbons.maxLength.setValue   ( template_ribbons.maxLength.getValue() )
            self.ribbons.threshold.setValue   ( template_ribbons.threshold.getValue() )
            self.ribbons.twoSided.setValue    ( template_ribbons.twoSided.getValue() )
            self.ribbons.useEuler.setValue    ( template_ribbons.useEuler.getValue() )

            self.ribbons.scalar2red.setValue  ( template_ribbons.scalar2red.getValue() )
            self.ribbons.scalar2green.setValue( template_ribbons.scalar2green.getValue() )
            self.ribbons.scalar2blue.setValue ( template_ribbons.scalar2blue.getValue() )

            self.ribbons.scalarVolume.setValue( template_ribbons.scalarVolume.getValue() )
            self.ribbons.vectorVolume.setValue( template_ribbons.vectorVolume.getValue() )

            seed_positions = self.ribbons.source.getValue()
            seed_positions.append( seed_position )
            self.ribbons.source.setValue( seed_positions )

            group_children = ribbons_group.children.getValue()
            shape.geometry.setValue( self.ribbons )
            group_children.append( shape )
            ribbons_group.children.setValue(group_children)

            # Remember where this drag started and how many seeds we added.
            self.position = seed_position
            self.amount = 1

        elif button_now == 1 and button_before == 1:
            # Button held: update the seed(s) to follow the moving position.
            #print "Modifying ribbon node (%d,%d)." % ( button_now, self.button )
            if self.type == 0:
                # Single ribbon: replace the last seed with the current one.
                seed_positions = self.ribbons.source.getValue()
                seed_positions = seed_positions[:-self.amount]

                seed_positions.append( seed_position )
                self.amount = 1

                self.ribbons.source.setValue( seed_positions )

            else:
                # Multiple ribbons: spread `type` seeds evenly from the drag
                # start to the current position.
                seed_positions = self.ribbons.source.getValue()
                seed_positions = seed_positions[:-self.amount]

                for ribbon_index in range(self.type):
                    interpolation = float(ribbon_index) / float(self.type-1)
                    seed_positions.append( (1-interpolation)*self.position + ( interpolation )*seed_position )

                self.amount = self.type
                self.ribbons.source.setValue( seed_positions )

        elif button_now == 0 and button_before == 1:
            # Button released: refine the ribbon by halving its step back to
            # the template resolution and finalise the seed list.
            #print "Finishing ribbon node (%d,%d)." % ( button_now, self.button )
            self.ribbons.step.setValue( .5*self.ribbons.step.getValue() )
            seed_positions = self.ribbons.source.getValue()
            if SETTINGS_PRINT_SEEDS:
                for finished_seed in seed_positions[-self.amount:]:
                    print finished_seed
            self.ribbons.source.setValue( seed_positions )

        self.button = button_now
        return 1

putStreamRibbons = PutStreamRibbons()


class Typist( AutoUpdate( SFString ) ):
    # Reads single key presses from a KeySensor:
    #   Z   clear every released ribbon
    #   z   undo the most recent ribbon
    #   2-9 set how many ribbons a single drag releases
    def update( self, event ):
        pressed_key = event.getValue()
        if pressed_key == 'Z':
            ribbons_group.children.setValue([])
        if pressed_key == 'z':
            putStreamRibbons.undo()
        else:
            try:
              requested_count = int(pressed_key)
              if requested_count < 2:
                # NOTE: original code refers to "putStreamTubes" here, which is
                # not defined in this module; the resulting NameError is
                # swallowed by the except below. Preserved as-is.
                putStreamTubes.type = 0
              else:
                putStreamRibbons.type = requested_count
            except:
                pass
        return ""

typist = Typist()
keyboard = createX3DNodeFromString("<KeySensor />")[0]
keyboard.keyPress.route(typist)

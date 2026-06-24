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

from Tkinter import *
from H3D import *
from H3DInterface import *

import math

# references order: the VolumeProbe whose sampled values we display.
probed_volume, = references.getValue()


class ProbeDisplay(Frame):
  """ProbeDisplay opens a Tk window and displays information from a
  specified VolumeProbe.

  The following must be provided through the "references" field:
  1) the VolumeProbe instance from which to extract the data

  This script only reads data from the VolumeProbe node and displays
  it. For the VolumeProbe to update the values some probe position
  must be routed to the "probe" field. See the documentation for
  VolumeProbe for more information.
  """

  def __init__(self,parent,volume_to_probe):
    Frame.__init__(self,parent)

    self.volume = volume_to_probe

    # One Tk string variable per VolumeProbe output, bound to its label.
    self.var_scalarValue = StringVar()
    self.var_scalarGradient = StringVar()
    self.var_vectorValue = StringVar()
    self.var_vectorMagnitude = StringVar()
    self.var_vectorCurl = StringVar()
    self.var_vectorDivergence = StringVar()

    # Lay out the readout as a two-column grid of "name: value" rows.
    Label(self,text="scalarValue: ").grid(row=0,column=0)
    Label(self,textvariable=self.var_scalarValue).grid(row=0,column=1)

    Label(self,text="scalarGradient: ").grid(row=1,column=0)
    Label(self,textvariable=self.var_scalarGradient).grid(row=1,column=1)

    Label(self,text="vectorValue: ").grid(row=2,column=0)
    Label(self,textvariable=self.var_vectorValue).grid(row=2,column=1)

    Label(self,text="vectorMagnitude: ").grid(row=3,column=0)
    Label(self,textvariable=self.var_vectorMagnitude).grid(row=3,column=1)

    Label(self,text="vectorCurl: ").grid(row=4,column=0)
    Label(self,textvariable=self.var_vectorCurl).grid(row=4,column=1)

    Label(self,text="vectorDivergence: ").grid(row=5,column=0)
    Label(self,textvariable=self.var_vectorDivergence).grid(row=5,column=1)

  def update(self):
    # Pull the latest sampled values out of the VolumeProbe...
    scalar_value = self.volume.scalarValue.getValue()
    scalar_gradient = self.volume.scalarGradient.getValue()
    vector_value = self.volume.vectorValue.getValue()
    vector_magnitude = self.volume.vectorMagnitude.getValue()
    vector_curl = self.volume.vectorCurl.getValue()
    vector_divergence = self.volume.vectorDivergence.getValue()

    # ...and push them into the bound Tk variables to refresh the labels.
    self.var_scalarValue.set( scalar_value )
    self.var_scalarGradient.set( scalar_gradient )
    self.var_vectorValue.set( vector_value )
    self.var_vectorMagnitude.set( vector_magnitude )
    self.var_vectorCurl.set( vector_curl )
    self.var_vectorDivergence.set( vector_divergence )

# H3D Runtime ---

root = Tk(None, "./")
root.title("Volume Probe")

probeDisplay = ProbeDisplay(root,probed_volume)
probeDisplay.grid(sticky=N+E+W+S)

class Ctrl( AutoUpdate( SFTime ) ):
  # Driven by the global "time" field: once per frame, pump the Tk event loop
  # and refresh the readout. Errors (e.g. the window being closed) are ignored.
  def update(self, event):
    try:
      root.update()
      probeDisplay.update()
    except:
      pass
    return 0

ctrl = Ctrl()
time.route(ctrl)

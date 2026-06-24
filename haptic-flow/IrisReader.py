# =============================================================================
#  IrisReader.py  --  loads an IRIS Explorer flow dataset and derives all the
#                     scene parameters that depend on its size and magnitude
# =============================================================================
#
#  H3DAPI Python script, instantiated in the scene (base.x3d) as the
#  "FILE_LOADER" <PythonScript> node. It reads a vector-field dataset written
#  in IRIS Explorer's textual ".asc" lattice format, feeds the grid and the
#  vectors into the ImprovedVectorVolume node, and computes a whole set of
#  derived parameters (scale, stream-surface step/interspace, stream-tube
#  radius, force/torque limits, bounding-box geometry) so the rest of the
#  scene adapts automatically to whatever dataset is loaded.
#
#  Cross-script coupling:
#    * It imports the base.py controller as the module "PS" (that script is
#      DEF'd "PS" in the scene) and writes the dataset-appropriate force/torque
#      limits back into PS.max_force / PS.max_torque.
#    * Every module-level field object below (xPos, yPos, zPos, scale,
#      dataSize, ..., linesCoordPoints) is exposed by H3D as a field of the
#      FILE_LOADER node, so those names are preserved verbatim.
#
# -----------------------------------------------------------------------------

from H3DInterface import *
import re
import sys
import stats
import math
import PS

# The four scene nodes this loader feeds, in the order fixed by the
# "references" field:
#   vector_volume  : ImprovedVectorVolume   (receives the grid + vectors)
#   box_coordinate : Coordinate             (receives the bounding-box points)
#   stream_surface : IBFVStreamSurface       (receives step + interspace)
#   stream_tubes   : StreamTubes            (receives step + radius)
vector_volume, box_coordinate, stream_surface, stream_tubes = references.getValue()

# -----------------------------------------------------------------------------
#  Tuning constants. Each derived scene parameter is expressed as a small
#  constant divided by something dataset-dependent (the scale and/or the median
#  flow magnitude), so that the visualisation and the haptics feel consistent
#  across datasets of very different physical sizes and speeds.
# -----------------------------------------------------------------------------
MAX_SIZE = 0.4          # the loaded volume is scaled to fit a MAX_SIZE cube
STEP_CONST = 0.008      # numerator for the stream-surface integration step
INTERSPACE_CONST = 0.028  # numerator for the stream-surface seed-line spacing
R_STEP_CONST = 0.01     # numerator for the stream-ribbon step
R_WIDTH_CONST = 1       # numerator for the stream-ribbon width
T_STEP_CONST = 0.003    # numerator for the stream-tube step
T_RADIUS_CONST = 0.003  # numerator for the stream-tube radius
MAX_LENGTH = 0.7        # numerator for the stream-ribbon maximum length
FORCE_CONST = 1         # numerator for the maximum rendered force
TORQUE_CONST = 40       # numerator for the maximum rendered torque

# -----------------------------------------------------------------------------
#  Module-level field objects exposed as fields of the FILE_LOADER node. These
#  names are part of the public interface (the scene and other scripts may
#  address them) and so must not be renamed.
# -----------------------------------------------------------------------------
xPos = MFFloat()        # grid sample coordinates along X
yPos = MFFloat()        # grid sample coordinates along Y
zPos = MFFloat()        # grid sample coordinates along Z
scale = SFFloat()       # uniform scale that fits the data into a MAX_SIZE cube
dataSize = SFVec3f()    # physical extent of the dataset
dataCenter = SFVec3f()  # physical centre of the dataset
ssStep = SFFloat()      # IBFVStreamSurface integration step
maxForce = SFFloat()    # maximum rendered force
maxTorque = SFFloat()   # maximum rendered torque

#ssWidth = SFInt32()
#ssLength = SFInt32()
ssInterspace = SFFloat()  # IBFVStreamSurface seed-line spacing
srStep = SFFloat()        # stream-ribbon step
srWidth = SFFloat()       # stream-ribbon width
srMaxLength = SFFloat()   # stream-ribbon maximum length
stStep = SFFloat()        # stream-tube step
stRadius = SFFloat()      # stream-tube radius

linesCoordPoints = MFVec3f()  # the 16 points that trace the data bounding box


class IrisReader((TypedField(AutoUpdate(MFVec3f), (SFString)))):
	"""
	Reads the file at the url given in param to the constructor. It can be
	routed as an MFVec3f that contains the values of the flow field. It
	sets the following values accordingly, which can be routed:
	xPos, yPos, zPos -- defines the data grid
	scale -- defines the scale to apply in the surrounding Transform node
		so that the data fits in a MAX_SIZE x MAX_SIZE x MAX_SIZE cube
	dataSize -- the actual size of the dataset
	dataCenter -- the pos of the center of the data (might be useless)
	ssStep -- the step parameter for an IBFVStreamSurface
	maxForce -- the maximum force parameter
	maxTorque -- the maximum torque parameter
	ssInterspace -- the interspace parameter for an IBFVStreamSurface
	srStep -- the step parameter for a stream ribbon
	srWidth -- the width parameter for a stream ribbon
	srMaxLength -- the maxLength parameter for a stream ribbon
	stStep -- the step parameter for a stream tube
	stRadius -- the radius parameter for a stream tube
	linesCoordPoints -- the coordpoints to use to draw a bounding box
		around the data

	"""
	median = float()
	highMean = float()
	url = ""

	def __init__(self, url):
		MFVec3f.__init__(self)
		print "init----------------------"
		self.url = url
		self.read()

	def update(self, event):
		# Called when a new url is routed in: reload from the new file.
		incoming_routes = self.getRoutesIn()
		self.url = incoming_routes[0].getValue()
		print "update-------------------- url = %s" % self.url
		if self.url:
			self.read()
		return self.getValue()

	def read(self):
		# ---------------------------------------------------------------------
		#  Parse the IRIS Explorer ".asc" lattice file. The format stores named
		#  sections as "(@<id> [...] <data>)" blocks, plus a small directory at
		#  the end that maps section names ("values", "dims", "perimCoord") to
		#  their block ids. We build that id->contents map, then look up the
		#  three sections we care about by name.
		# ---------------------------------------------------------------------
		section_contents_by_id = {}
		input_file = file(self.url, "r")
		print "using", self.url
		file_text = input_file.read()
		input_file.close()

		section_pattern = re.compile("\(@(\d+) (\[.*?\])?(.*?)\)", re.DOTALL)
		section_matches = section_pattern.findall(file_text)
		for section_match in section_matches:
			section_contents_by_id[int(section_match[0])] = section_match[1:]
		del(section_matches)

		# Resolve the named sections to their block ids.
		values_section_id = int(re.search("values @(\d+)", file_text).group(1))
		dims_section_id = int(re.search("dims @(\d+)", file_text).group(1))
		coords_section_id = int(re.search("perimCoord @(\d+)", file_text).group(1))

		# Flat list of all vector components, the per-axis grid sizes, and the
		# concatenated per-axis grid coordinates.
		flow_components = map(float, section_contents_by_id[values_section_id][1].split())
		grid_dimensions = map(int, section_contents_by_id[dims_section_id][1].split())
		grid_coordinates = map(float, section_contents_by_id[coords_section_id][1].split())
		del(section_contents_by_id)

		# The grid coordinates are stored as X coords, then Y coords, then Z
		# coords, concatenated; split them back out by axis size.
		for i in range(grid_dimensions[0]):
			xPos.push_back(grid_coordinates[i])
		for i in range(grid_dimensions[0], grid_dimensions[0] + grid_dimensions[1]):
			yPos.push_back(grid_coordinates[i])
		for i in range(grid_dimensions[0] + grid_dimensions[1],
				grid_dimensions[0] + grid_dimensions[1] + grid_dimensions[2]):
			zPos.push_back(grid_coordinates[i])

		# The flow components come as a flat x,y,z,x,y,z,... stream.
		for i in range(0, len(flow_components), 3):
			flow_vector = Vec3f(flow_components[i], flow_components[i + 1], flow_components[i + 2])
			self.push_back(flow_vector)

		# Physical extent and centre of the dataset, from the grid extremes.
		x_coords = xPos.getValue()
		y_coords = yPos.getValue()
		z_coords = zPos.getValue()
		dataSize.setValue(Vec3f((x_coords[-1] - x_coords[0]),
				  (y_coords[-1] - y_coords[0]),
				  (z_coords[-1] - z_coords[0])))
		extent = dataSize.getValue()
		dataCenter.setValue(Vec3f((x_coords[0] + extent.x / 2.),
					 (y_coords[0] + extent.y / 2.),
					 (z_coords[0] + extent.z / 2.)))
		self.makeLinesCoords()

		# Uniform scale that fits the longest axis into a MAX_SIZE cube.
		scale.setValue(MAX_SIZE / float(max([extent.x, extent.y, extent.z])))

		# Summary statistics of the flow magnitude, used to size the derived
		# parameters. The "high mean" (mean of the top 10% of magnitudes) is a
		# robust stand-in for the peak speed; the median stands in for a
		# typical speed.
		magnitudes = map(lambda v: math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z), self.getValue())
		magnitudes.sort()
		self.median = stats.median(magnitudes)
		magnitude_count = len(magnitudes)
		top_decile_start = 9 * magnitude_count / 10
		self.highMean = stats.mean(magnitudes[top_decile_start:])
		bottom_decile_end = magnitude_count / 10
		low_mean = stats.mean(magnitudes[:bottom_decile_end])
		print "%s: dataSize: %f,%f,%f scale: %f, median: %f,\n highMean: %f, lowMean: %f, min: %f, max: %f" % (
			self.url, extent.x, extent.y, extent.z, scale.getValue(), self.median, self.highMean, low_mean, magnitudes[0], magnitudes[-1])

		# Derive the scene parameters. Spatial parameters are divided by the
		# scale so they stay constant in the displayed (scaled) space; the
		# stream-surface parameters are additionally divided by the typical
		# speed, and the force/torque limits by the peak speed.
		ssInterspace.setValue(float(INTERSPACE_CONST) / (scale.getValue() * self.median))
		ssStep.setValue(float(STEP_CONST) / (scale.getValue() * self.median))
		srStep.setValue(R_STEP_CONST / scale.getValue())
		srWidth.setValue(R_WIDTH_CONST / scale.getValue())
		srMaxLength.setValue(MAX_LENGTH / scale.getValue())
		stStep.setValue(T_STEP_CONST / scale.getValue())
		stRadius.setValue(T_RADIUS_CONST / scale.getValue())
		maxForce.setValue(FORCE_CONST / self.highMean)
		maxTorque.setValue(TORQUE_CONST / self.highMean)

	def makeBoxPoints(self):
		"""Calculates the coords of the 8 points defining the bounding
		box of the data."""
		x_coords = xPos.getValue()
		y_coords = yPos.getValue()
		z_coords = zPos.getValue()
		corner_points = [	Vec3f(x_coords[ 0], y_coords[ 0], z_coords[ 0]),
			Vec3f(x_coords[-1], y_coords[ 0], z_coords[ 0]),
			Vec3f(x_coords[-1], y_coords[ 0], z_coords[-1]),
			Vec3f(x_coords[ 0], y_coords[ 0], z_coords[-1]),
			Vec3f(x_coords[ 0], y_coords[-1], z_coords[ 0]),
			Vec3f(x_coords[-1], y_coords[-1], z_coords[ 0]),
			Vec3f(x_coords[-1], y_coords[-1], z_coords[-1]),
			Vec3f(x_coords[ 0], y_coords[-1], z_coords[-1])]
		return corner_points

	def makeLinesCoords(self):
		# Trace a closed path through the 8 corners that draws all the edges of
		# the bounding box as a single line strip. The visiting order below is
		# tied to the order of the corners returned by makeBoxPoints().
		corner_points = self.makeBoxPoints()
		corner_visit_order = [0, 1, 2, 3, 0, 4, 5, 6, 7, 4, 7, 3, 2, 6, 5, 1]
		for corner_index in corner_visit_order:
			linesCoordPoints.push_back(corner_points[corner_index] - dataCenter.getValue())


# -----------------------------------------------------------------------------
#  Instantiate the loader and wire its derived values into the scene. The
#  "reader" field itself carries the flow vectors into the vector volume.
# -----------------------------------------------------------------------------
reader = IrisReader(vector_volume.url.getValue())
reader.route(vector_volume.data)
xPos.route(vector_volume.xPos)
yPos.route(vector_volume.yPos)
zPos.route(vector_volume.zPos)
vector_volume.url.route(reader)
linesCoordPoints.route(box_coordinate.point)
maxForce.route(PS.max_force)
maxTorque.route(PS.max_torque)
ssStep.route(stream_surface.step)
ssInterspace.route(stream_surface.interspace)
#srStep.route(sr.step)
#srWidth.route(sr.width)
#srMaxLength.route(sr.maxLength)
stStep.route(stream_tubes.step)
stRadius.route(stream_tubes.radius)

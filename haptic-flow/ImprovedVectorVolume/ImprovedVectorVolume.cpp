#include "ImprovedVectorVolume.h"
#include <Debug.hh>
#include <iostream>

// Register the node with H3D's database under the X3D type name
// "ImprovedVectorVolume". This string is the cross-boundary contract used by
// X3D scenes and IrisReader.py and must not change.
H3DNodeDatabase ImprovedVectorVolume::database( "ImprovedVectorVolume",
		&(newInstance<ImprovedVectorVolume>),
		typeid( ImprovedVectorVolume ),
		&VHTKVectorDataNode::database);

namespace ImprovedVectorVolumeInternals {
	// Field declarations exposed to X3D / Python by name. Do not rename.
	FIELDDB_ELEMENT( ImprovedVectorVolume, vector, OUTPUT_ONLY);
	FIELDDB_ELEMENT( ImprovedVectorVolume, data, INPUT_ONLY);
	FIELDDB_ELEMENT( ImprovedVectorVolume, xPos, INPUT_ONLY);
	FIELDDB_ELEMENT( ImprovedVectorVolume, yPos, INPUT_ONLY);
	FIELDDB_ELEMENT( ImprovedVectorVolume, zPos, INPUT_ONLY);
	FIELDDB_ELEMENT( ImprovedVectorVolume, url, INPUT_OUTPUT);
}


ImprovedVectorVolume::ImprovedVectorVolume(Inst< SFNode > _metadata):
	VHTKVectorDataNode(_metadata),
	vector(new SFVecVal),
	data(new MFVec3f),
	xPos(new MFFloat),
	yPos(new MFFloat),
	zPos(new MFFloat),
	url(new SFString),
	gridCenter(),
	gridMetadataInitialized(false)
{
	// Let the output field call back into this node when it recomputes.
	vector->container = this;
}

//------------------------------------------------------------------------------
// Raw sample access
//------------------------------------------------------------------------------

/// Return the sample vector stored at integer grid indices (i, j, k).
///
/// The samples are stored in a single flat MFVec3f (\ref data) using x-fastest,
/// then y, then z ordering. For a grid of size
/// (gridWidthInSamples x gridHeightInSamples x gridDepthInSamples) the flat
/// index of point (i, j, k) is:
///     (k * gridHeightInSamples + j) * gridWidthInSamples + i
Vec3f ImprovedVectorVolume::getVector(int xIndex, int yIndex, int zIndex)
{
	if(!gridMetadataInitialized){
		computeGridMetadata();
	}
	int flatIndex =
		(zIndex * gridHeightInSamples + yIndex) * gridWidthInSamples + xIndex;
	Vec3f sample = data->getValueByIndex( flatIndex );
	return sample;
}

//------------------------------------------------------------------------------
// Derived grid metadata
//------------------------------------------------------------------------------

/// Cache the per-axis sample counts and the geometric center of the grid.
///
/// The center is the midpoint between the first and last sample on each axis;
/// it is later used to translate between the grid-centered local frame (in
/// which the center is the origin) and raw data coordinates.
void ImprovedVectorVolume::computeGridMetadata()
{
	// Number of grid points along each axis. The field data may not have been
	// routed in yet, in which case the axis lists are empty and we bail out so
	// we can retry on the next call.
	gridWidthInSamples = xPos->getSize();
	if(gridWidthInSamples==0) // not ready yet
		return;

	gridHeightInSamples = yPos->getSize();
	// NOTE: bug-for-bug preserved. This guard (and the next) re-test
	// gridWidthInSamples even though it almost certainly meant to test
	// gridHeightInSamples / gridDepthInSamples respectively. The behavior is
	// intentionally left unchanged.
	if(gridWidthInSamples==0) // not ready yet
		return;

	gridDepthInSamples = zPos->getSize();
	if(gridWidthInSamples==0) // not ready yet (see NOTE above: should test depth)
		return;

	// Geometric center of the grid in data coordinates.
	gridCenter.x = ( xPos->getValueByIndex(0) + xPos->getValueByIndex(gridWidthInSamples-1) ) / 2.0;
	gridCenter.y = ( yPos->getValueByIndex(0) + yPos->getValueByIndex(gridHeightInSamples-1) ) / 2.0;
	gridCenter.z = ( zPos->getValueByIndex(0) + zPos->getValueByIndex(gridDepthInSamples-1) ) / 2.0;
	std::cerr << "gridcenter: " << gridCenter.x << ", "
				    << gridCenter.y << ", "
				    << gridCenter.z << std::endl;
	gridMetadataInitialized = true;

}

//------------------------------------------------------------------------------
// Per-axis cell lookup
//------------------------------------------------------------------------------

/// Find the lower grid index of the cell containing \p position along one axis.
///
/// See the header for the precise contract. In short: returns -1 when the
/// position is outside the axis range, otherwise returns the index of the cell
/// whose lower bound is the largest axis coordinate that is still < position.
int ImprovedVectorVolume::findLowerGridIndex(MFFloat axisCoordinates, float position)
{
	int sampleCount = axisCoordinates.getSize();
	float axisMin, axisMax;
	axisMin = axisCoordinates.getValueByIndex(0);
	axisMax = axisCoordinates.getValueByIndex(sampleCount-1);
	//std::cerr << "min: " << axisMin << " max: " << axisMax << " n: " << sampleCount << std::endl;

	// Reject positions outside [ first, last ] sample on this axis.
	if(position < axisCoordinates.getValueByIndex(0) || position > axisCoordinates.getValueByIndex(sampleCount-1)){
		//std::cerr << position << "   ...out !\n";
		return -1;
	}

	// Walk the interior samples; the first sample strictly greater than
	// position marks the upper bound of the containing cell, so its predecessor
	// is the lower index we want.
	for(int axisIndex=1; axisIndex<sampleCount-1; axisIndex++){
		if(axisCoordinates.getValueByIndex(axisIndex) > position){
			return axisIndex-1;
		}
	}
	// position lies in (or at the top of) the last cell.
	return sampleCount-2;

}

//------------------------------------------------------------------------------
// Trilinear interpolation of the vector field
//------------------------------------------------------------------------------

/// Return the interpolated field vector for an arbitrary query point.
///
/// Coordinate spaces involved:
///   - \p queryPoint is given in the caller's frame.
///   - \p localToGridTransform maps it into the grid's LOCAL frame, where by
///     convention the grid center (\ref gridCenter) is the origin (0,0,0).
///   - Adding \ref gridCenter back converts the local-frame point into DATA
///     coordinates, i.e. the same coordinate system as xPos / yPos / zPos.
///
/// Once in data coordinates we locate the enclosing grid cell on each axis,
/// compute normalized fractional positions within that cell, and trilinearly
/// blend the eight corner samples. Finally the result is rotated back into the
/// query frame and rescaled so its magnitude matches the un-rotated
/// interpolated vector (see the magnitude-preserving step at the end).
Vec3f ImprovedVectorVolume::getVector(const Matrix4f &localToGridTransform, const Vec3f & queryPoint)
{
	// localPoint is in local coords, where by convention gridCenter is (0,0,0).
	Vec3f localPoint = localToGridTransform * queryPoint;

	if(!gridMetadataInitialized){
		computeGridMetadata();
	}
	if(!gridMetadataInitialized){ // not ready yet
		return Vec3f(0,0,0);
	}
	//std::cerr << "Finally ready !!!!!!!!\n";

	// dataPoint is in (not 0-centered) data coordinates, the same space as the
	// xPos, yPos, zPos axis coordinates.
	Vec3f dataPoint = localPoint + gridCenter;
	//std::cerr << "dataPoint: " << dataPoint.x << ", " << dataPoint.y << ", " << dataPoint.z << std::endl;

	// Locate the lower corner (xIndex, yIndex, zIndex) of the cell that
	// contains the query point. A negative index means the point is outside the
	// grid on that axis, so there is no field value to return.
	int xIndex = findLowerGridIndex(*xPos, dataPoint.x);
	if(xIndex<0){
		return Vec3f(0,0,0);
	}
	int yIndex = findLowerGridIndex(*yPos, dataPoint.y);
	if(yIndex<0){
		return Vec3f(0,0,0);
	}
	int zIndex = findLowerGridIndex(*zPos, dataPoint.z);
	if(zIndex<0){
		return Vec3f(0,0,0);
	}
	//std::cerr << "inside !\n" ;

	// Normalized fractional position of the query point within its cell on each
	// axis, in [0, 1]. Because the grid is non-uniform, each fraction is
	// divided by that specific cell's own width (the gap between consecutive
	// axis samples) rather than a global cell size.
	float xFraction = (dataPoint.x - xPos->getValueByIndex(xIndex)) / (xPos->getValueByIndex(xIndex+1) - xPos->getValueByIndex(xIndex));
	float yFraction = (dataPoint.y - yPos->getValueByIndex(yIndex)) / (yPos->getValueByIndex(yIndex+1) - yPos->getValueByIndex(yIndex));
	float zFraction = (dataPoint.z - zPos->getValueByIndex(zIndex)) / (zPos->getValueByIndex(zIndex+1) - zPos->getValueByIndex(zIndex));

	// Trilinear interpolation: weight each of the eight cell corners by the
	// product of the (fraction or 1-fraction) on each axis, choosing "fraction"
	// for the high corner and "1-fraction" for the low corner. The eight
	// weights sum to 1.
	Vec3f interpolatedVector =
		( (  xFraction)*(  yFraction)*(  zFraction)*getVector(xIndex+1, yIndex+1, zIndex+1) +
		  (  xFraction)*(  yFraction)*(1-zFraction)*getVector(xIndex+1, yIndex+1, zIndex  ) +
		  (  xFraction)*(1-yFraction)*(  zFraction)*getVector(xIndex+1, yIndex  , zIndex+1) +
		  (  xFraction)*(1-yFraction)*(1-zFraction)*getVector(xIndex+1, yIndex  , zIndex  ) +
		  (1-xFraction)*(  yFraction)*(  zFraction)*getVector(xIndex  , yIndex+1, zIndex+1) +
		  (1-xFraction)*(  yFraction)*(1-zFraction)*getVector(xIndex  , yIndex+1, zIndex  ) +
		  (1-xFraction)*(1-yFraction)*(  zFraction)*getVector(xIndex  , yIndex  , zIndex+1) +
		  (1-xFraction)*(1-yFraction)*(1-zFraction)*getVector(xIndex  , yIndex  , zIndex  ) );


	// Rotate/scale the interpolated vector from the grid frame back into the
	// query frame using the inverse of the transform's scale-rotation part.
	Vec3f transformedVector = localToGridTransform.getScaleRotationPart().inverse() * interpolatedVector;

	// Magnitude-preserving rescale: the inverse transform above may have changed
	// the vector's length (e.g. via non-uniform scaling), but the field's
	// physical magnitude should match the interpolated value. So keep the
	// DIRECTION of transformedVector while restoring the MAGNITUDE of the
	// (untransformed) interpolatedVector.
	//
	//   transformedMagnitudeSquared = |transformedVector|^2
	//   result = transformedVector * sqrt( |interpolatedVector|^2 /
	//                                       |transformedVector|^2 )
	//
	// If transformedVector is zero (degenerate), return the zero vector to avoid
	// dividing by zero.
	H3DFloat transformedMagnitudeSquared = transformedVector * transformedVector;
	interpolatedVector = transformedMagnitudeSquared == 0
		? Vec3f(0,0,0)
		: transformedVector * H3DSqrt( ( interpolatedVector * interpolatedVector ) / transformedMagnitudeSquared );
	return interpolatedVector;

}

//------------------------------------------------------------------------------
// Unimplemented VHTKVectorDataNode virtuals
//------------------------------------------------------------------------------

Vec3f ImprovedVectorVolume::getVolumeSize()
{
	RUNTIME_E("Vec3f ImprovedVectorVolume::getVolumeSize()",
			"Not implemented");
	return Vec3f(0,0,0);

}

Vec3f ImprovedVectorVolume::getCurl(const Matrix4f &T, const Vec3f &x)
{
	RUNTIME_E("Vec3f ImprovedVectorVolume::getCurl(const Matrix4f &T, const Vec3f &x)",
			"Not implemented");
	return Vec3f(0,0,0);
}

H3DFloat ImprovedVectorVolume::getDivergence(const Matrix4f &T, const Vec3f &x)
{
	RUNTIME_E("H3DFloat ImprovedVectorVolume::getDivergence(const Matrix4f &T, const Vec3f &x)",
			"Not implemented");
	return 0;
}

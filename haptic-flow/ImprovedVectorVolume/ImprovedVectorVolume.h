#ifndef __IMPROVEDVECTORVOLUME_H
#define __IMPROVEDVECTORVOLUME_H

#include "ivv_api.h"
#include <VHTKVectorDataNode.hh>
#include <SFVec3f.h>
#include <SFMatrix4f.h>
#include <MFVec3f.h>
#include <MFInt32.h>
#include <MFFloat.h>
#include <SFString.h>

using namespace H3D;
using namespace VHTK;

/// \class ImprovedVectorVolume
/// \brief An H3D/VHTK vector-data node that stores a 3-D vector field sampled
///        on a NON-UNIFORM rectilinear grid and serves interpolated vectors
///        on demand.
///
/// The node holds:
///   - a flat list of sample vectors (\ref data), one per grid point, laid out
///     in x-fastest order (see getVector(int,int,int) for the index formula);
///   - three independent, strictly ascending coordinate axes (\ref xPos,
///     \ref yPos, \ref zPos) that give the world-space position of each grid
///     plane along x, y and z respectively. Because the spacing along each axis
///     may vary, the grid is "non-uniform rectilinear" rather than a regular
///     box of evenly spaced cells.
///
/// Other VHTK nodes (e.g. IBFVStreamSurface) query this node through the
/// VHTKVectorDataNode virtual interface, most importantly
/// getVector(const Matrix4f&, const Vec3f&), which performs trilinear
/// interpolation of the field and returns a vector for an arbitrary point.
///
/// \note The registered X3D type name is "ImprovedVectorVolume" and the field
///       names (vector, data, xPos, yPos, zPos, url) are referenced from X3D
///       scenes and from IrisReader.py, so neither the type name nor the field
///       names may be changed.
class IVV_API ImprovedVectorVolume : public VHTKVectorDataNode {
	public:
		/// Constructor.
		ImprovedVectorVolume(Inst< SFNode > _metadata = 0);

		/// Return the raw sample vector stored at integer grid indices
		/// (i, j, k) along the x, y and z axes respectively.
		/// Overrides VHTKVectorDataNode::getVector(int,int,int).
		Vec3f virtual getVector(int i, int j, int k);

		/// Return the (trilinearly interpolated, magnitude-preserving) vector
		/// for an arbitrary point. \p localToGridTransform maps the query point
		/// into the grid's local, grid-centered coordinate frame, and \p queryPoint
		/// is the point to sample. Overrides
		/// VHTKVectorDataNode::getVector(const Matrix4f&, const Vec3f&).
		Vec3f virtual getVector(const Matrix4f &localToGridTransform, const Vec3f &queryPoint);

		/// Not implemented (raises a runtime error). Kept to satisfy the
		/// VHTKVectorDataNode virtual interface.
		Vec3f virtual getVolumeSize();

		/// Not implemented (raises a runtime error). Kept to satisfy the
		/// VHTKVectorDataNode virtual interface.
		Vec3f virtual getCurl(const Matrix4f &T, const Vec3f &x);

		/// Not implemented (raises a runtime error). Kept to satisfy the
		/// VHTKVectorDataNode virtual interface.
		H3DFloat virtual getDivergence(const Matrix4f &T, const Vec3f &x);

		/// \class SFVecVal
		/// \brief Output field that recomputes the interpolated vector whenever
		///        its routed-in position or transform changes.
		///
		/// routes_in[0] is the query position (SFVec3f) and routes_in[1] is the
		/// local-to-grid transform (SFMatrix4f). The field delegates the actual
		/// computation to the owning node via \ref container.
		class SFVecVal: public TypedField< SFVec3f, Types<SFVec3f,SFMatrix4f> >{
			public:
				virtual void update()
				{
					Vec3f pos = static_cast< SFVec3f * > ( routes_in[0] )->getValue();
					Matrix4f trans = static_cast< SFMatrix4f * > ( routes_in[1] )->getValue();
					value = container->getVector(trans, pos);
					//std::cerr << pos.x << ", " << pos.y << ", " << pos.z
					//	<< " : " << value.x << ", " << value.y << ", " << value.z << std::endl;
				}
				/// Back-pointer to the node that owns this field. Set in the
				/// ImprovedVectorVolume constructor and used by update() to call
				/// getVector(). Required by the .cpp and by H3D's routing model.
				ImprovedVectorVolume * container;
		};

		static H3DNodeDatabase database;

		// ----- output field -----

		/// Interpolated vector for the most recently routed-in position/transform.
		auto_ptr< SFVecVal > vector;


		// ----- input fields -----

		/// Flat list of sample vectors, one per grid point, in x-fastest order.
		auto_ptr< MFVec3f > data;
		/// Strictly ascending x-axis grid-plane coordinates.
		auto_ptr< MFFloat > xPos;
		/// Strictly ascending y-axis grid-plane coordinates.
		auto_ptr< MFFloat > yPos;
		/// Strictly ascending z-axis grid-plane coordinates.
		auto_ptr< MFFloat > zPos;
		/// Optional source URL for the data set.
		auto_ptr< SFString > url;


	protected:
		/// Compute and cache derived grid metadata (axis sample counts and the
		/// geometric center of the grid). Populates \ref gridWidthInSamples,
		/// \ref gridHeightInSamples, \ref gridDepthInSamples and
		/// \ref gridCenter, and sets \ref gridMetadataInitialized once the data
		/// is available.
		void computeGridMetadata();

		/// Locate the lower grid index of the cell that contains \p position
		/// along a single axis.
		///
		/// \param axisCoordinates the (ascending) coordinates of one axis.
		/// \param position        the coordinate to locate.
		/// \return -1 if \p position lies outside [ axisCoordinates[0],
		///         axisCoordinates[n-1] ]; otherwise the index of the largest
		///         element of \p axisCoordinates that is less than \p position
		///         (i.e. the lower bound of the containing cell).
		/// \pre \p axisCoordinates is sorted in ascending order.
		int findLowerGridIndex(MFFloat axisCoordinates, float position);

		/// Geometric center of the grid in data coordinates: the midpoint
		/// between the first and last sample on each axis. Used to convert
		/// between the grid-centered local frame and data coordinates.
		Vec3f gridCenter;

		// The following three counts are the number of grid points (sample
		// planes) along each axis; they are NOT physical extents.
		int gridWidthInSamples;
		int gridHeightInSamples;
		int gridDepthInSamples;

		/// True once computeGridMetadata() has successfully cached the metadata.
		bool gridMetadataInitialized;

};


#endif // __IMPROVEDVECTORVOLUME_H

/// \file IBFVStreamSurface.h
/// \brief Header file for IBFVStreamSurface, an X3D scene-graph geometry node
///        that grows a stream surface from a seed line through a vector field
///        and (optionally) drapes an animated IBFV flow texture over it.
///
/// A "stream surface" is the surface swept out by a family of stream lines that
/// all start on a common "seed line". This node:
///   1. builds a seed line of \c width sample points around a \c source point
///      (the shape of that line is controlled by \c seedType / \c seedOrientation),
///   2. integrates the vector field forward for \c length steps to advance each
///      seed point downstream, producing a width-by-length grid of points, and
///   3. renders that grid either as a wireframe or as a lit, textured quad mesh.
///
/// When texturing is enabled the surface is covered with the animated
/// Image-Based Flow Visualisation texture produced by IBFVFBOTexture (see that
/// header), so the surface visually "flows" in the direction of integration.
///
/// IMPORTANT (cross-file / X3D contract -- do not rename):
///   - the registered type-name string "IBFVStreamSurface",
///   - the class name IBFVStreamSurface and nested class SFBound,
///   - every field exposed to X3D / Python: vectorVolume, source, width,
///     length, interspace, step, enableTexture, wireFrame, seedType,
///     seedOrientation, display, radius, bound,
///   - the public method render().
#ifndef __IBFVSTREAMSURFACE_H__
#define __IBFVSTREAMSURFACE_H__

#include <VHTK.hh>
#include <VHTKVectorDataNode.hh>
#include <vector>
#include <SFInt32.h>
#include <SFFloat.h>
#include "ibfvss_api.h"
#include "X3DGeometryNode.h"
#include "SFFloat.h"
#include "IBFVFBOTexture.h"
#include "texture_debug.h"

using namespace H3D;

namespace VHTK {

  class IBFVSS_API IBFVStreamSurface : public X3DGeometryNode {
  public:

    // SFBound is specialized update itself from the radius field
    // of the Sphere node.
    //
    // routes_in[0] is the radius field of the Sphere node.
    //

    /// Specialised bound field: derives an axis-aligned cube bounding box whose
    /// side equals twice the incoming radius value (routes_in[0]).
    class SFBound: public TypedField< X3DGeometryNode::SFBound,
                                      SFFloat >{
      /// Update the bound from the radius field.
      virtual void update() {
        // Box side = 2 * radius, so the cube circumscribes a sphere of that
        // radius.
        H3DFloat boxSide = 2 * static_cast< SFFloat * >( routes_in[0] )->getValue();
        BoxBound *boxBound = new BoxBound;
        boxBound->size->setValue( Vec3f( boxSide, boxSide, boxSide ) );
        value = boxBound;
      }
    };


    /// Constructor.
    IBFVStreamSurface( Inst<    SFNode >  _metadata = 0,
            Inst< SFBound >                 _bound = 0,
	    Inst< SFBool >		_enableTexture = new SFBool(true));

    /// Renders the IBFVStreamSurface with OpenGL.
    virtual void render();

    // Traverse the scenegraph.
    //virtual void traverseSG( TraverseInfo &ti );


    /// the volume to visualize
    auto_ptr< SFVectorDataNode > vectorVolume;

    /// the source of the seed
    auto_ptr< SFVec3f > source;

    /// the width of the IBFVStreamSurface in number of streamlines (should
    /// be even)
    auto_ptr< SFInt32 > width;

    /// the length of the IBFVStreamSurface in number of steps
    auto_ptr< SFInt32 > length;

    /// the space between two seedlines
    auto_ptr< SFFloat > interspace;

    auto_ptr< SFBound > bound;

    auto_ptr< SFFloat > radius;

    //auto_ptr< SFBool > solid;

    auto_ptr< SFFloat > step;

    auto_ptr< SFBool > enableTexture;

    auto_ptr< SFBool > wireFrame;

    /// the kind of seed wanted.
    /// 0 = curved seed line
    /// 1 = straight seed line
    /// 2 = straight seed line using seedOrientation if != 0
    auto_ptr< SFInt32 > seedType;

    auto_ptr< SFVec3f > seedOrientation;

    auto_ptr< SFBool > display;

    /// The H3DNodeDatabase for this node.
    static H3DNodeDatabase database;

  protected:
    /*
    GLUquadricObj* gl_quadric;
    */

    /// Compute the next seed-line point adjacent to \p seedPoint, stepping one
    /// \c interspace in the direction selected by \p direction (+1 to one side,
    /// -1 to the other). The exact offset direction depends on \c seedType.
    Vec3f seedNeighbour(Vec3f seedPoint, int direction);

    /// Fill the first row of \c surfacePoints with the seed line, fanning out
    /// from the \c source point in both directions according to \c seedType.
    void calculateSeed(void);

    /// Flat width-by-length grid of stream-surface vertices, addressed as
    /// surfacePoints[row*width + column] (row = integration step,
    /// column = stream line).
    std::vector<Vec3f> surfacePoints;

    /// Issue glVertex for the surface vertex at integration step \p stepIndex
    /// and stream-line index \p streamLineIndex.
    void drawPoint(int stepIndex, int streamLineIndex);

    /// Cached seed-line orientation, set while building a straight seed line
    /// (seedType == 1) so that both halves of the line share one direction.
    Vec3f cachedSeedOrientation;

    /// The animated IBFV flow texture draped over the surface.
    IBFVFBOTexture flowTexture;

    /// OpenGL texture name of \c flowTexture, fetched lazily on first render.
    GLuint flowTextureId;
  };
}

#endif

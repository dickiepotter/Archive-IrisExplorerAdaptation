/// \file IBFVStreamSurface.cpp
/// \brief CPP file for IBFVStreamSurface, X3D scene-graph node.
///
/// See IBFVStreamSurface.h for the high-level algorithm. This file implements:
///   - seed-line construction (calculateSeed / seedNeighbour) for the three
///     seedType modes,
///   - forward integration of the vector field to grow the surface, and
///   - the wireframe and lit/textured rendering paths (render).

#include "IBFVStreamSurface.h"
#include <Scene.h>
#include <iostream>
//#include "texture_debug.h"

using namespace VHTK;


// Add this node to the H3DNodeDatabase system. The literal "IBFVStreamSurface"
// is the type name X3D scenes use to instantiate this node and must be kept.
H3DNodeDatabase IBFVStreamSurface::database(
                                 "IBFVStreamSurface",
                                 &(newInstance<IBFVStreamSurface>),
                                 typeid( IBFVStreamSurface ),
                                 &X3DGeometryNode::database );

// Register each X3D-visible field. These names are addressed by X3D scenes and
// by state.py (e.g. SS.seedType, SS.display) and must not be renamed.
namespace IBFVStreamSurfaceInternals {
  FIELDDB_ELEMENT( IBFVStreamSurface, vectorVolume, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, source, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, width, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, length, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, interspace, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, step, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, enableTexture, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, wireFrame, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, seedType, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, seedOrientation, INPUT_OUTPUT );
  FIELDDB_ELEMENT( IBFVStreamSurface, display, INPUT_OUTPUT );
}


IBFVStreamSurface::IBFVStreamSurface(
               Inst<    SFNode >  _metadata,
               Inst< SFBound >                 _bound,
	       Inst< SFBool >		_enableTexture) :
  X3DGeometryNode( _metadata, _bound ),
  vectorVolume(new SFVectorDataNode),
  source(new SFVec3f),
  width(new SFInt32),
  length(new SFInt32),
  interspace(new SFFloat),
  bound(_bound),
  radius(new SFFloat),
  //solid(new SFBool),
  step(new SFFloat),
  enableTexture(_enableTexture),
  wireFrame(new SFBool(false)),
  seedType(new SFInt32(0)),
  seedOrientation(new SFVec3f(Vec3f(0,0,0))),
  display(new SFBool(true)),
  flowTexture(),
  flowTextureId(0){

  type_name = "IBFVStreamSurface";
  database.initFields( this );

  // Texturing only makes sense for a displayed, solid (non-wireframe) surface.
  if(wireFrame->getValue() || !(display->getValue()) ){
	  enableTexture->setValue(false);
  }

//  flowTextureId = flowTexture.getTextureID();

/*
  radius->setValue( 3 );
  solid->setValue( true );

  radius->route( bound );
  radius->route( displayList );
  solid->route( displayList );
*/
  // Re-evaluate the display list every frame so the animated texture updates.
  Scene::time->route(displayList);
}

// Return the seed-line point adjacent to seedPoint, one interspace away, in the
// half indicated by direction (+1 / -1). The offset direction depends on the
// seedType mode (taken modulo 3):
//   0 = curved seed line: offset is perpendicular to the local flow, so the
//       seed line bends to follow the field.
//   1 = straight seed line: the perpendicular is computed once at the source
//       and cached (cachedSeedOrientation) so the whole line stays straight.
//   2 = straight seed line along an explicit seedOrientation vector.
Vec3f IBFVStreamSurface::seedNeighbour(Vec3f seedPoint, int direction)
{
	Vec3f offsetDirection, neighbourPoint;
	Vec3f flowAtPoint = vectorVolume->getCastedValue()->getVector(seedPoint);
	int seedMode = seedType->getValue() % 3;
	if(seedMode == 0){
		// Perpendicular to the local flow (rotate the xy components by 90 deg),
		// recomputed at every point so the line follows the field's curvature.
		offsetDirection.x =  direction * flowAtPoint.y;
		offsetDirection.y = -direction * flowAtPoint.x;
		offsetDirection.z = 0;
		offsetDirection.normalizeSafe();
	}else if((seedMode == 1) && (seedPoint == source->getValue()) ){
		// First step of a straight line: compute the perpendicular at the
		// source once and cache it so both halves share the same direction.
		offsetDirection.x =  flowAtPoint.y;
		offsetDirection.y = - flowAtPoint.x;
		offsetDirection.z = 0;
		offsetDirection.normalizeSafe();
		cachedSeedOrientation = offsetDirection;
		offsetDirection = direction * offsetDirection;
	}else{ // either using provided seedOrientation or cachedSeedOrientation set
		//on the preceding case
		if(seedMode == 1){
			offsetDirection = direction * cachedSeedOrientation;
		}else{
			offsetDirection=seedOrientation->getValue();
			offsetDirection = direction * offsetDirection;
		}
	}

	neighbourPoint = seedPoint + interspace->getValue() * offsetDirection ;
	return neighbourPoint;
}

void IBFVStreamSurface::drawPoint(int stepIndex, int streamLineIndex)
{
	int streamLineCount = width->getValue();
	Vec3f vertex = surfacePoints[stepIndex*streamLineCount+streamLineIndex];
	glVertex3f(vertex.x, vertex.y, vertex.z);
}

// Build the seed line (the first row of the surface grid) by placing the source
// point in the middle and fanning outward to both sides via seedNeighbour().
void IBFVStreamSurface::calculateSeed(void)
{
	int streamLineCount = width->getValue();
	int middleIndex = (streamLineCount-1)/2;
	Vec3f orientation = seedOrientation->getValue();
	Vec3f sourcePoint = source->getValue();
	Vec3f flowAtSource = vectorVolume->getCastedValue()->getVector(sourcePoint);

	H3DFloat orientationLengthSqr = orientation.lengthSqr();
	if((orientationLengthSqr != 0)&& (orientationLengthSqr != 1)){ // if orientation != 0 0 0 and not yet normalized
		orientation.normalize();
		seedOrientation->setValue(orientation);
	}

	// Place the source at the centre of the seed line.
	surfacePoints[middleIndex] = sourcePoint;

	// Fan out to the left (towards index 0) then to the right (towards width-1),
	// each new point one interspace from its already-placed neighbour.
	for(int streamLineIndex=middleIndex-1; streamLineIndex>=0; streamLineIndex--){
		surfacePoints[streamLineIndex] = seedNeighbour(surfacePoints[streamLineIndex+1], -1);
	}
	for(int streamLineIndex=middleIndex+1; streamLineIndex<streamLineCount; streamLineIndex++){
		surfacePoints[streamLineIndex] = seedNeighbour(surfacePoints[streamLineIndex-1], 1);
	}

}

void IBFVStreamSurface::render()
{
	if(! display->getValue() ){
		return;
	}
	// Work on the texture matrix with a clean identity, restored at the end.
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
	glDisable(GL_TEXTURE_3D);
	glEnable(GL_TEXTURE_2D);

	// Advance the animated IBFV texture by one frame and grab its id once.
	if(enableTexture->getValue()){
		flowTexture.render();
		if(!flowTextureId){
			flowTextureId = flowTexture.getTextureID();
		}
	}

	int streamLineCount = width->getValue();
	int stepCount = length->getValue();
	// TODO: make this a member of the class
	// The texture's lower half is unused, so we map surface texture coordinates
	// into the upper half only: v runs from textureVOffset (0.5) up to 1.
	GLfloat textureVOffset = 0.5;
	GLfloat textureUStep = 1./( (float) (streamLineCount-1));
	GLfloat textureVStep = (1-textureVOffset)/( (float) (stepCount-1)); // we only use the upper half of the texture

	surfacePoints.reserve(streamLineCount*stepCount);

	calculateSeed();

	// Grow the surface: integrate the vector field forward one Euler step at a
	// time. Each point in row i is its predecessor in row i-1 advanced by
	// step * (field at that predecessor).
	for(int stepIndex=1; stepIndex<stepCount; stepIndex++)
		for(int streamLineIndex=0; streamLineIndex<streamLineCount; streamLineIndex++){
			Vec3f previousPoint = surfacePoints[(stepIndex-1)*streamLineCount +streamLineIndex];
			Vec3f flowAtPoint = vectorVolume->getCastedValue()->getVector(previousPoint);
			surfacePoints[stepIndex*streamLineCount+streamLineIndex] = previousPoint + step->getValue() * flowAtPoint;
		}


	// now the points are created, we do the real rendering
	if(wireFrame->getValue()){
		// Wireframe path: draw, per stream line, the segments to the next step
		// (along the line) and to the neighbouring stream line (across the surface).
		glDisable(GL_TEXTURE_2D);
		for(int streamLineIndex=0; streamLineIndex<streamLineCount-1; streamLineIndex++){
			glBegin(GL_LINES);
			for(int stepIndex=0; stepIndex<stepCount-1; stepIndex++){
				drawPoint(stepIndex,streamLineIndex);
				drawPoint(stepIndex+1,streamLineIndex);
				drawPoint(stepIndex,streamLineIndex);
				drawPoint(stepIndex,streamLineIndex+1);
			}
			glEnd();
		}
	}else{

		// Solid path: enable lighting with a near-white material so the IBFV
		// texture (modulated against it) carries the flow appearance.
		//glColor4f(1,1,1,1);
		glEnable(GL_LIGHTING);
		GLfloat white[] = { 0.85, 0.85, 0.85, 0};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, white);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, white);
		//	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, white);
		//	glColor4f(1,1,1,1);
		glShadeModel(GL_SMOOTH);
		if(enableTexture->getValue()){
			// Bind the animated flow texture and modulate it with the lighting.
			glBindTexture(GL_TEXTURE_2D, flowTextureId);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			/*
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			   */
			//dumpTexStates();
			if(glIsTexture(flowTextureId) != GL_TRUE){
				std::cerr << "not a texture !" << std::endl;
			}
		}else{
			glDisable(GL_TEXTURE_2D);
		}
	}
	checkError();
	checkError("0");
	//debug code following
	/*
	GLubyte * img = new GLubyte[128*256*4];
	for(int i=0; i<128*256*4; i+=4){
		img[i]   = 255; // red
		img[i+1] = 255; // green
		img[i+2] = 0; // blue
		img[i+3] = 255; // alpha
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	checkError("1");


	glBegin(GL_QUADS);
//		glColor4f(0,0,1,1);
		glTexCoord2f(0,0);
		glVertex3f(0,0,0);
//
//		glColor4f(0,1,0,1);
		glTexCoord2f(0,1);
		glVertex3f(0,0.2,0);
//
//		glColor4f(0,1,1,1);
		glTexCoord2f(1,1);
		glVertex3f(0.2,0.2,0);
//
//		glColor4f(1,0,0,1);
		glTexCoord2f(1,0);
		glVertex3f(0.2,0,0);
	glEnd();
	*/
	//debug code above

	checkError("11");
	if(!wireFrame->getValue()){
		// Solid surface as a series of quad strips, one between each pair of
		// adjacent stream lines, with texture coordinates spanning the upper
		// half of the IBFV texture along the integration direction.
		for(int streamLineIndex=0; streamLineIndex<streamLineCount-1; streamLineIndex++){
			glBegin(GL_QUAD_STRIP);
			for(int stepIndex=0; stepIndex<stepCount-1; stepIndex++){
				glTexCoord2f(streamLineIndex*textureUStep, textureVOffset + stepIndex*textureVStep);
				drawPoint(stepIndex,streamLineIndex);
				glTexCoord2f((streamLineIndex+1)*textureUStep, textureVOffset + stepIndex*textureVStep);
				drawPoint(stepIndex,streamLineIndex+1);
			}
			glEnd();
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_LIGHTING);
	}
	checkError("12");
	glMatrixMode(GL_TEXTURE);
	checkError("13");
	glPopMatrix();
	checkError("14");


//  glPushMatrix();


//  glPopMatrix();
}

/*
void Sphere::traverseSG( TraverseInfo &ti ) {
  if( ti.hapticsEnabled() && ti.getCurrentSurface() ) {
//    ti.addHapticShapeToAll( new HapticSphere( radius->getValue(),
//                                              solid->getValue(),
//                                              this,
//                                              ti.getCurrentSurface(),
//                                              ti.getAccForwardMatrix() ) );
  }
}
*/

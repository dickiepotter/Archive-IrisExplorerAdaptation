/// \file IBFVFBOTexture.cpp
/// \brief Implementation of IBFVFBOTexture: the GPU feedback loop that produces
///        an animated Image-Based Flow Visualisation (IBFV) texture.
///
/// See IBFVFBOTexture.h for a description of the IBFV algorithm. In short, each
/// render() call:
///   1. directs OpenGL output into the IBFV texture via the FBO,
///   2. advects last frame's texture by drawing it onto a mesh whose geometry
///      is shifted by texSpeed in the flow direction,
///   3. blends a fresh, semi-transparent noise pattern on top to keep detail
///      alive, then
///   4. restores normal rendering, leaving the updated texture ready for both
///      display and the next frame.

#include "IBFVFBOTexture.h"
#include "texture_debug.h"
#include <GL/gl.h>
#include <iostream>

// ===========================================================================
// Nested FBO helper: switch rendering between the off-screen texture and the
// main window.
// ===========================================================================

void IBFVFBOTexture::FBO::checkStatus(char *id)
{
	// A framebuffer must be "complete" before it can be rendered into; this
	// reports any incompleteness for debugging.
	GLenum framebufferStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if(framebufferStatus != GL_FRAMEBUFFER_COMPLETE_EXT){
		//fprintf(stderr, "WARNING: fb not complete ! %d\n", framebufferStatus);
		std::cerr << "WARNING: fb not complete ! ";
		if(id)
			std::cerr  << id << " , ";
		std::cerr << framebufferStatus << std::endl;
	}else{
		if(id)
			std::cerr  << id << ": OK" << std::endl;
	}
}

IBFVFBOTexture::FBO::FBO(GLuint texture, GLuint _width, GLuint _height):
	renderingToTexture(GL_FALSE),
	savedMainDrawBuffer(0),
	width(_width),
	height(_height)
{
	// GLEW must be initialised before any of the *EXT framebuffer entry points
	// can be called.
	GLenum glewInitStatus;
	glewInitStatus = glewInit();
	if(glewInitStatus != GLEW_OK){
		std::cerr << "glewInit() failed: " << glewGetErrorString(glewInitStatus) << std::endl;
	}
	glEnable(GL_TEXTURE_2D);

	// Create the framebuffer object and attach the supplied texture as its
	// colour buffer, so that drawing into the FBO writes into the texture.
	glGenFramebuffersEXT(1, &framebufferId);
	checkError("after gen fb");
	std::cerr << "fb: " << std::hex << framebufferId << std::dec << std::endl;
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferId);
	checkError("after bind fb");
	glBindTexture(GL_TEXTURE_2D, texture);
	checkError("after bind texture");
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture, 0);
	checkError("after texture attach");
	checkStatus("after fbo init");

	// Leave the default framebuffer bound; toTexture() will rebind ours.
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void IBFVFBOTexture::FBO::toTexture(void)
{
/*	if(renderingToTexture){
		return;
	}
*/
	// Remember the main window's draw buffer and viewport so toMain() can put
	// them back exactly as they were.
	glGetIntegerv(GL_DRAW_BUFFER, &savedMainDrawBuffer);
	glGetIntegerv(GL_VIEWPORT, savedMainViewport);


	// Direct all subsequent drawing into our texture-backed framebuffer.
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebufferId);
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

	glViewport(0, 0, width, height);

	// Reset all three matrix stacks (pushing the existing state so toMain() can
	// restore it) and install a projection that maps the unit square [0,1]^2 in
	// normalised device coordinates: translate by (-1,-1) then scale by (2,2).
	// This lets the advection mesh, drawn in [0,1] space, fill the texture.
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-1, -1, 0);
	glScalef(2, 2, 1);

	gluPerspective(0,1,0,100);
	renderingToTexture = GL_TRUE;
}

void IBFVFBOTexture::FBO::toMain(void)
{
/*	if(!renderingToTexture){
		return;
	}
*/
	// Restore the main-window viewport and pop each matrix stack back to the
	// state saved in toTexture().
	glViewport(savedMainViewport[0], savedMainViewport[1], savedMainViewport[2], savedMainViewport[3]);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// Rebind the default framebuffer and restore the main draw buffer.
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glDrawBuffer(savedMainDrawBuffer);
	renderingToTexture = GL_FALSE;
}

IBFVFBOTexture::FBO::~FBO(void)
{
	/* suppress the framebuffer */
}


// ===========================================================================
// IBFVFBOTexture
// ===========================================================================

// TODO: add initialisation for all those members, with nice default vals
IBFVFBOTexture::IBFVFBOTexture(GLuint _width, GLuint _height, GLuint _npat,
		GLuint _alpha, GLuint _randomTexSize, GLuint _nMesh,
		GLfloat _texSpeed):
	currentFrameIndex(0),
	numNoisePatterns(_npat),
	noiseAlpha(_alpha),
	randomTexSize(_randomTexSize),
	meshResolution(_nMesh),
	// Spacing between adjacent mesh vertices in texture space.
	meshSpacing(((GLfloat)(1.0/(_nMesh-1.0)))),
	texSpeed(_texSpeed),
	initialized(GL_FALSE),
	width(_width),
	height(_height)
{
}

void IBFVFBOTexture::init(void)
{
	glEnable(GL_TEXTURE_2D);

	// Allocate names for both textures: [0] = IBFV/flow texture, [1] = random
	// noise source texture.
	glGenTextures(2, ibfvAndRandomTextureIds);

	// Configure the random noise texture: tiled (REPEAT) with linear filtering.
	glBindTexture(GL_TEXTURE_2D, ibfvAndRandomTextureIds[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	// Configure the IBFV texture identically, then allocate its (empty) storage.
	glBindTexture(GL_TEXTURE_2D, ibfvAndRandomTextureIds[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	// Wrap the IBFV texture in an FBO so we can render into it, then build the
	// animated noise patterns.
	framebuffer = new FBO(ibfvAndRandomTextureIds[0], width, height);
	makeNoisePatterns();
	initialized=GL_TRUE;
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}


// Build the family of noise patterns used to keep the advected image sharp.
// Each of the numNoisePatterns display lists uploads one phase-shifted noise
// image; render() cycles through them frame by frame.
//
// TODO: check if we can delete the arrays at the end of the func and still
// have the lists work (in the example C code the arrays are on the stack,
// so there should be no problem).
void IBFVFBOTexture::makeNoisePatterns(void)
{
	GLuint patternIndex, texelColumn, texelRow, intensityThreshold;

	// Binary look-up table: maps the lower half of the 0-255 range to black and
	// the upper half to white, producing crisp two-tone noise.
	int binaryThresholdLut[256];

	// Per-texel random phase offset, so each texel animates at a different
	// point in the black/white cycle. Allocated as a 2D array of size
	// randomTexSize x randomTexSize.
	//int phase[randomTexSize][randomTexSize];
	int **perTexelPhase = new int *[randomTexSize];
	for(patternIndex=0; patternIndex<randomTexSize; patternIndex++){
		perTexelPhase[patternIndex] = new int[randomTexSize];
	}

	// RGBA pixel buffer for one pattern image, laid out row-major.
	//GLubyte pat[randomTexSize][randomTexSize][4];
	GLubyte *patternPixels = new GLubyte[randomTexSize*randomTexSize*4];
/*	GLubyte ***pat = new GLubyte**[randomTexSize];
	for(i=0; i<randomTexSize; i++){
		pat[i] = new GLubyte*[randomTexSize];
		for(j=0; j<randomTexSize; j++){
			pat[i][j] = new GLubyte[4];
		}
	}
	*/


	// Reserve a contiguous block of display-list names, one per pattern.
	noisePatternListBase = glGenLists(numNoisePatterns);


	// Initialise the binary threshold LUT and a random phase for every texel.
	for (patternIndex = 0; patternIndex < 256; patternIndex++) binaryThresholdLut[patternIndex] = patternIndex < 127 ? 0 : 255;
	for (texelColumn = 0; texelColumn < randomTexSize; texelColumn++)
		for (texelRow = 0; texelRow < randomTexSize; texelRow++) perTexelPhase[texelColumn][texelRow] = rand() % 256;

	for (patternIndex = 0; patternIndex < numNoisePatterns; patternIndex++) {
		// Advance the global phase for this pattern so successive patterns step
		// the noise through one full black/white cycle across the animation.
		intensityThreshold = patternIndex*256/numNoisePatterns;
		//std::cerr << "k=" << patternIndex << std::endl;
		for (texelColumn = 0; texelColumn < randomTexSize; texelColumn++){
			for (texelRow = 0; texelRow < randomTexSize; texelRow++) {
				GLuint pixelOffset = (texelColumn*randomTexSize + texelRow)*4;
				/*
				pat[i][j][0] = lut[(t + phase[i][j]) % 255];
				pat[i][j][1] =
					pat[i][j][2] = 0;
				pat[i][j][3] = alpha ;
				*/
				// Grey value for this texel: threshold the (phase + global
				// offset) through the binary LUT; write it to R, G and B.
				patternPixels[pixelOffset] =
				patternPixels[pixelOffset+1] =
					patternPixels[pixelOffset+2] = binaryThresholdLut[(intensityThreshold + perTexelPhase[texelColumn][texelRow]) % 255];
				patternPixels[pixelOffset+3] = noiseAlpha ;
				if(texelColumn==(randomTexSize-1)){
				}
			}
		}

		// Compile a display list that uploads this pattern as the noise texture.
		glEnable(GL_TEXTURE_2D);
		//std::cerr << 1 << std::endl;
		glNewList(noisePatternListBase + patternIndex, GL_COMPILE);
		//std::cerr << 2 << " randomTexSize=" << randomTexSize << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, randomTexSize, randomTexSize, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, patternPixels);
		//std::cerr << 3 << std::endl;
		glEndList();
		glDisable(GL_TEXTURE_2D);
		//std::cerr << 4 << std::endl;
	}

}

void IBFVFBOTexture::render(void)
{
	// Lazily create GL resources on the first frame, once a context exists.
	if(!initialized){
		init();
	}
//	std::cerr << "." ;
	glEnable(GL_TEXTURE_2D);
//	dumpTexStates();

	// Redirect rendering into the IBFV texture for the advection + blend pass.
	framebuffer->toTexture();
	framebuffer->checkStatus();


	GLuint meshColumn, meshRow;
	float columnLeft, columnRight, rowBottom;


	// --- Pass 1: advection -------------------------------------------------
	// Draw last frame's IBFV texture back onto a regular mesh, but shift every
	// vertex by +texSpeed in y. Texture coordinates stay aligned to the unshifted
	// grid, so the sampled image is displaced relative to the geometry, making
	// the pattern appear to flow.
	glBindTexture(GL_TEXTURE_2D, ibfvAndRandomTextureIds[0]);
	checkError("render(): after bind tex");
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	checkError("render(): after texenv");
	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	checkError("render(): after tex param 1");
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	checkError("render(): after tex param 2");
	*/
	for (meshColumn = 0; meshColumn < meshResolution-1; meshColumn++) {
		columnLeft = meshSpacing*meshColumn; columnRight = columnLeft + meshSpacing;
                glBegin(GL_QUAD_STRIP);
		for (meshRow = 0; meshRow < meshResolution; meshRow++) {
			rowBottom = meshSpacing*meshRow;
			glTexCoord2f(columnLeft, rowBottom);
			glVertex2f(columnLeft, rowBottom+texSpeed);

			glTexCoord2f(columnRight, rowBottom);
			glVertex2f(columnRight, rowBottom+texSpeed);
		}
		glEnd();
	}
	checkError("render(): after drawing");



	// --- Pass 2: inject fresh noise ---------------------------------------
	// Blend a low-opacity noise pattern over the advected image. Selecting a
	// different pattern each frame (currentFrameIndex % numNoisePatterns)
	// animates the injected detail and prevents the image washing out.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, ibfvAndRandomTextureIds[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	*/
	glCallList(currentFrameIndex % numNoisePatterns + noisePatternListBase);
	glBegin(GL_QUAD_STRIP);
	glTexCoord2f(0.0,  0.0);  glVertex2f(0.0, 0.0);
	glTexCoord2f(0.0,  1); glVertex2f(0.0, 0.1);
	glTexCoord2f(1, 0.0);  glVertex2f(1.0, 0.0);
	glTexCoord2f(1, 1); glVertex2f(1.0, 0.1);
	glEnd();
	glDisable(GL_BLEND);
	checkError("render(): after random tex blending");

	glBindTexture(GL_TEXTURE_2D, 0);
	//glClear(GL_COLOR_BUFFER_BIT);

	/*
	glClearColor(0,1,0,1);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0,0,0,0);
	*/

	// Restore normal rendering; the IBFV texture now holds this frame's result.
	framebuffer->toMain();
	currentFrameIndex++;
//	glDisable(GL_TEXTURE_2D);
}

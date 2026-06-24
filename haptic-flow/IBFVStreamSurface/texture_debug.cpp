/// \file texture_debug.cpp
/// \brief Implementation of the OpenGL diagnostic helpers declared in
///        texture_debug.h.

#include "texture_debug.h"
#include <iostream>
#include <GL/gl.h>
#include <GL/glut.h>

// ---------------------------------------------------------------------------
// Parallel lookup tables used by dumpTexStates().
//
// states[] holds the GLenum tokens we want to query with glGetIntegerv(), and
// names[] holds the matching human-readable string for each token so the dump
// is self-labelling. The two arrays must stay index-aligned and the same
// length (43). These tables are intentionally exhaustive for texture/texgen
// state; their contents are preserved verbatim.
// ---------------------------------------------------------------------------
GLuint states[43] = {
GL_TEXTURE_ENV,
GL_TEXTURE_ENV_MODE,
GL_TEXTURE_1D,
GL_TEXTURE_2D,
GL_TEXTURE_WRAP_S,
GL_TEXTURE_WRAP_T,
GL_TEXTURE_MAG_FILTER,
GL_TEXTURE_MIN_FILTER,
GL_TEXTURE_ENV_COLOR,
GL_TEXTURE_GEN_S,
GL_TEXTURE_GEN_T,
GL_TEXTURE_GEN_MODE,
GL_TEXTURE_BORDER_COLOR,
GL_TEXTURE_WIDTH,
GL_TEXTURE_HEIGHT,
GL_TEXTURE_BORDER,
GL_TEXTURE_COMPONENTS,
GL_TEXTURE_RED_SIZE,
GL_TEXTURE_GREEN_SIZE,
GL_TEXTURE_BLUE_SIZE,
GL_TEXTURE_ALPHA_SIZE,
GL_TEXTURE_LUMINANCE_SIZE,
GL_TEXTURE_INTENSITY_SIZE,
GL_NEAREST_MIPMAP_NEAREST,
GL_NEAREST_MIPMAP_LINEAR,
GL_LINEAR_MIPMAP_NEAREST,
GL_LINEAR_MIPMAP_LINEAR,
GL_OBJECT_LINEAR,
GL_OBJECT_PLANE,
GL_EYE_LINEAR,
GL_EYE_PLANE,
GL_SPHERE_MAP,
GL_DECAL,
GL_MODULATE,
GL_NEAREST,
GL_REPEAT,
GL_CLAMP,
GL_S,
GL_T,
GL_R,
GL_Q,
GL_TEXTURE_GEN_R,
GL_TEXTURE_GEN_Q
};

char * names[43] = {
"GL_TEXTURE_ENV",
"GL_TEXTURE_ENV_MODE",
"GL_TEXTURE_1D",
"GL_TEXTURE_2D",
"GL_TEXTURE_WRAP_S",
"GL_TEXTURE_WRAP_T",
"GL_TEXTURE_MAG_FILTER",
"GL_TEXTURE_MIN_FILTER",
"GL_TEXTURE_ENV_COLOR",
"GL_TEXTURE_GEN_S",
"GL_TEXTURE_GEN_T",
"GL_TEXTURE_GEN_MODE",
"GL_TEXTURE_BORDER_COLOR",
"GL_TEXTURE_WIDTH",
"GL_TEXTURE_HEIGHT",
"GL_TEXTURE_BORDER",
"GL_TEXTURE_COMPONENTS",
"GL_TEXTURE_RED_SIZE",
"GL_TEXTURE_GREEN_SIZE",
"GL_TEXTURE_BLUE_SIZE",
"GL_TEXTURE_ALPHA_SIZE",
"GL_TEXTURE_LUMINANCE_SIZE",
"GL_TEXTURE_INTENSITY_SIZE",
"GL_NEAREST_MIPMAP_NEAREST",
"GL_NEAREST_MIPMAP_LINEAR",
"GL_LINEAR_MIPMAP_NEAREST",
"GL_LINEAR_MIPMAP_LINEAR",
"GL_OBJECT_LINEAR",
"GL_OBJECT_PLANE",
"GL_EYE_LINEAR",
"GL_EYE_PLANE",
"GL_SPHERE_MAP",
"GL_DECAL",
"GL_MODULATE",
"GL_NEAREST",
"GL_REPEAT",
"GL_CLAMP",
"GL_S",
"GL_T",
"GL_R",
"GL_Q",
"GL_TEXTURE_GEN_R",
"GL_TEXTURE_GEN_Q"
};

/// Counts how many times dumpTexStates() has been called. It is used to
/// rate-limit the dump: we skip the first 20 invocations and only emit on the
/// 21st, then never again. This keeps a per-frame caller from flooding stderr
/// while still capturing the state once the scene has settled.
int dumped=0;

void dumpTexStates(void)
{
	GLint queriedStateValue;

	// Only emit on exactly the 21st call (dumped == 20). Earlier calls just
	// bump the counter and return; later calls also fall through the guard
	// because the counter keeps incrementing past 20.
	if(dumped!=20){
		dumped++;
		return;
	}
	dumped++;

	std::cerr << "States:" << std::endl;
	std::cerr << std::hex;
	// Query and print each token alongside its name.
	for(GLuint stateIndex=0; stateIndex<43 ; stateIndex++){
		glGetIntegerv(states[stateIndex], &queriedStateValue);
		std::cerr << names[stateIndex] << ": " << queriedStateValue << std::endl;
	}
	// The active texture-environment mode needs a dedicated query.
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &queriedStateValue);
	std::cerr << "GL_TEXTURE_ENV_MODE: " << queriedStateValue << std::endl;
	std::cerr << std::dec;
}

void checkError(char *id)
{
	// glGetError() returns GL_NO_ERROR when the error flag is clear; otherwise
	// it returns (and clears) the most recent error code.
	GLenum glErrorCode = glGetError();
	if(glErrorCode!=GL_NO_ERROR){
		if(id) std::cerr << id << ": ";
		std::cerr << "err: " << gluErrorString(glErrorCode) << std::endl;
	}
}

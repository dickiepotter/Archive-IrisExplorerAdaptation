/// \file IBFVFBOTexture.h
/// \brief Declares IBFVFBOTexture, which generates an animated Image-Based
///        Flow Visualisation (IBFV) texture entirely on the GPU.
///
/// Image-Based Flow Visualisation (van Wijk, SIGGRAPH 2002) animates a dense
/// noise texture so that it appears to flow along a vector field. The core
/// idea is a feedback loop on a single texture, evaluated once per frame:
///
///   1. Take last frame's texture and "advect" it: draw it onto a mesh whose
///      vertices are displaced slightly in the flow direction. Because the
///      texture coordinates stay put while the geometry shifts, the image
///      content appears to move downstream.
///   2. Blend a fresh, low-opacity noise pattern on top. This continually
///      injects new high-frequency detail so the advected image never washes
///      out into a uniform blur. Cycling through a sequence of noise patterns
///      (Npat of them) produces the characteristic shimmering animation.
///
/// Here that feedback loop is implemented with a Framebuffer Object (FBO):
/// rather than reading the visible back buffer, we render directly into an
/// off-screen 2D texture (textures[0]) and reuse it as the input next frame.
/// This class owns that texture, the random noise texture, the display-list
/// noise patterns, and a small nested FBO helper that switches rendering
/// between the off-screen texture and the main window.
///
/// IMPORTANT: The class name IBFVFBOTexture, the nested class name FBO, and the
/// public methods render(), getTextureID() and init() are referenced from
/// other translation units and must not be renamed.
#ifndef _IBFVFBOTEXTURE_H
#define _IBFVFBOTEXTURE_H
#include "ibfvss_api.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>


class IBFVSS_API IBFVFBOTexture {
	public:
		/// Construct the generator. Nothing touches OpenGL yet -- all GL
		/// resources are created lazily in init(), which must run after a GL
		/// context exists.
		///
		/// \param _width         width  (in texels) of the IBFV texture.
		/// \param _height        height (in texels) of the IBFV texture.
		/// \param _npat          number of distinct noise patterns to cycle
		///                       through for the animation.
		/// \param _alpha         opacity (0-255) of the injected noise; lower
		///                       values give longer, smoother streaks.
		/// \param _randomTexSize edge length of the square random noise texture
		///                       (called NPN in van Wijk's reference code).
		/// \param _nMesh         resolution of the advection mesh.
		/// \param _texSpeed      how far texture coordinates are advected per
		///                       frame, i.e. the apparent flow speed.
		IBFVFBOTexture(GLuint _width=128, GLuint _height=256,
				GLuint _npat=64, GLuint _alpha=255,
				GLuint _randomTexSize=512, GLuint _nMesh=30,
				GLfloat _texSpeed=0.02);
		~IBFVFBOTexture(void) { }

		/// \return the OpenGL texture name of the animated IBFV texture so that
		///         callers can bind it onto their own geometry.
		GLuint getTextureID(void){ return ibfvAndRandomTextureIds[0]; }

		/// Advance the animation by one frame: advect the previous texture and
		/// blend in fresh noise, rendering the result back into the IBFV
		/// texture via the FBO. Calls init() automatically on first use.
		void render(void);

		/// Allocate all OpenGL resources (textures, FBO, noise patterns). Must
		/// be called once a GL context is current; render() will call it on the
		/// first frame if it has not already run.
		void init(void);

	protected:
		// ibfvAndRandomTextureIds[0]: the animated IBFV (flow) texture.
		// ibfvAndRandomTextureIds[1]: the static random noise source texture.
		GLuint ibfvAndRandomTextureIds[2];

		/// Index of the current animation frame; used both to pick the noise
		/// pattern for this frame and as a free-running counter.
		GLuint currentFrameIndex;

		/// Build the \c numNoisePatterns display lists, each of which uploads a
		/// phase-shifted noise image. Cycling through them animates the flow.
		void makeNoisePatterns(void);

		/// Base name of the contiguous block of display lists holding the noise
		/// patterns (one list per pattern, starting at this name).
		GLuint noisePatternListBase;

		/// Number of distinct noise patterns (van Wijk's "Npat").
		GLuint numNoisePatterns;

		/// Opacity (0-255) applied to the injected noise.
		GLuint noiseAlpha;

		/// Edge length of the square random noise texture (van Wijk's "NPN").
		GLuint randomTexSize;

		/// Resolution (vertices per side) of the advection mesh.
		GLuint meshResolution;

		/// Spacing between adjacent advection-mesh vertices in texture space
		/// (1 / (meshResolution - 1)).
		GLfloat meshSpacing;

		/// Per-frame texture-coordinate advection distance (apparent speed).
		GLfloat texSpeed;

		/// True once init() has allocated the GL resources.
		GLboolean initialized;

		/// Dimensions of the IBFV texture in texels.
		GLuint width;
		GLuint height;

		/// \brief Helper that redirects OpenGL rendering into the IBFV texture
		///        (off-screen) and back to the main window.
		///
		/// Wraps a single OpenGL Framebuffer Object whose colour attachment is
		/// the IBFV texture. toTexture() saves the current draw buffer/viewport
		/// and matrices, then binds the FBO and sets up a 1:1 orthographic-style
		/// projection so the advection mesh maps directly onto the texture.
		/// toMain() restores everything and unbinds the FBO.
		///
		/// IMPORTANT: the nested class name FBO and its methods toTexture(),
		/// toMain() and checkStatus() are part of this library's internal API.
		class FBO {
			public:
				/// Create an FBO whose colour attachment is \p texture (which
				/// must already have at least GL_TEXTURE_MIN_FILTER set).
				/// \p _width / \p _height are the texture dimensions, used as
				/// the off-screen viewport size.
				FBO(GLuint texture, GLuint _width, GLuint _height) ;
				~FBO(void);

				/// Begin rendering into the attached texture: save main-window
				/// state, bind the FBO, and install the render-to-texture
				/// viewport and projection.
				void toTexture(void);

				/// Stop rendering into the texture: restore the saved main
				/// viewport/projection and rebind the default framebuffer.
				void toMain(void);

				/// Report (to stderr) whether the FBO is complete. The optional
				/// \p id labels the message at the call site.
				void checkStatus(char *id=NULL);
			protected:
				/// OpenGL name of the framebuffer object.
				GLuint framebufferId;

				/// True while rendering is currently directed to the texture.
				GLboolean renderingToTexture;

				/// Saved draw buffer of the main window (restored by toMain()).
				GLint savedMainDrawBuffer;

				/// Saved viewport of the main window (x, y, w, h).
				GLint savedMainViewport[4];

				/// Off-screen viewport dimensions (== attached texture size).
				GLuint width;
				GLuint height;
		};

		/// The framebuffer-object helper bound to the IBFV texture.
		FBO *framebuffer;
};

#endif //_IBFVFBOTEXTURE_H

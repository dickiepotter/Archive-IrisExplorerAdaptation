/// \file testIBFVFBOTexture.cpp
/// \brief Stand-alone GLUT harness for visually testing IBFVFBOTexture in
///        isolation, outside the H3D scene graph.
///
/// It creates a window, builds one IBFVFBOTexture, and every frame advances the
/// animation (tex->render()) and draws the resulting flow texture on a rotated
/// quad. Press q or Esc to quit. This is a developer test program; it is not
/// part of the H3D node build.

#include "IBFVFBOTexture.h"
#include <GL/glut.h>
#include <iostream>

/// The flow-texture generator under test.
IBFVFBOTexture * flowTexture;

/// Cached OpenGL texture name of the flow texture, fetched after init().
GLint flowTextureId;

/// One-time OpenGL setup: a simple perspective projection and a 2D-texture
/// enabled state, with the colour buffer cleared.
void initGL(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 4.0/3.0, 0.1, 100.0);
	glViewport(0,0,800,600);
	glClearDepth(1);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

}

/// GLUT display/idle callback: step the IBFV animation, then draw the flow
/// texture on a diamond-shaped quad so the animation is visible.
void display(void)
{
	// Advance the IBFV animation one frame (renders into the flow texture).
	flowTexture->render();
	glClearColor(0,0,0.5,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-0.5, -0.5, -2);
	//glColor3f(1,0,0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, flowTextureId);

	// Draw a diamond quad sampling the upper half of the flow texture.
	glBegin(GL_QUADS);
	glTexCoord2f(0,0.5);
	glVertex3f(0.5, 0, 0);
	glTexCoord2f(0,1);
	glVertex3f(1, 0.5, 0);
	glTexCoord2f(1,1);
	glVertex3f(0.5, 1, 0);
	glTexCoord2f(1,0.5);
	glVertex3f(0, 0.5, 0);
	glEnd();


	/*
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex3f(0, 0, 0);
	glTexCoord2f(0,1); glVertex3f(0, 1, 0);
	glTexCoord2f(1,1); glVertex3f(1, 1, 0);
	glTexCoord2f(1,0); glVertex3f(1, 0, 0);
	glEnd();
	*/
//	glBindTexture(GL_TEXTURE_2D, 0);
	glutSwapBuffers();

}

/// Keyboard callback: quit on q, Q or Escape.
void kbmanager(unsigned char key, int x, int y)
{
	switch(key){
		case 'q':
		case 'Q':
		case '\033': // escape key
			exit(0);
	}
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(kbmanager);
	initGL();

	// Build the generator and create its GL resources now that a context exists.
	flowTexture = new IBFVFBOTexture;
	flowTexture->init();
	flowTextureId = flowTexture -> getTextureID();
	std::cerr << "tex_id = " << flowTextureId << std::endl;
	glutMainLoop();

}

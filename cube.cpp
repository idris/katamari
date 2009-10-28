#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "cube.h"

using namespace std;


Cube::Cube() {
}

void Cube::read(ifstream *inFile) {
	*inFile >> angle >> center[0] >> center[1] >> center[2] >> radius >> height >> color[0] >> color[1] >> color[2];

	height = height;
	radius = radius;
	center[0] = center[0];
	center[1] = center[1];
	center[2] = center[2];

	length = width = radius * 2;
}

void Cube::draw() {
	GLdouble shadowMatrix[4][4];
	GLdouble lightx=1.0, lighty=1.0, lightz=2.0;
	shadowMatrix[0][0] = 1.0;
	shadowMatrix[1][1] = 1.0;
	shadowMatrix[2][2] = 0.0;
	shadowMatrix[3][3] = 1.0;
	shadowMatrix[2][0] = (0.0-lightx) / lightz;
	shadowMatrix[2][1] = (0.0-lighty) / lightz;

	// draw the shadow
	glDisable(GL_LIGHTING);
	glColor3f(0.3f, 0.1f, 0.1f);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);
	glPushMatrix();
	glMultMatrixd((GLdouble *)shadowMatrix);
	_draw();
	glPopMatrix();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_LIGHTING);
	
	// draw the object
	glColor3fv(color);
	_draw();
}

void Cube::_draw() {
	glPushMatrix();
	glTranslated(center[0], center[1], center[2]);
	glRotated(angle, 0.0, 0.0, 1.0);
	gluSphere(gluNewQuadric(), 1.2*radius, 40, 20);
	glTranslated(0.0 - width/2, 0.0 - length/2, 0.0 - height/2);

	// front
	glBegin(GL_QUADS);
	glNormal3d(0.0, -1.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(width, 0.0, 0.0);
	glVertex3d(width, 0.0, height);
	glVertex3d(0.0, 0.0, height);
	glEnd();

	// back
	glBegin(GL_QUADS);
	glNormal3d(0.0, 1.0, 0.0);
	glVertex3d(0.0, length, 0.0);
	glVertex3d(0.0, length, height);
	glVertex3d(width, length, height);
	glVertex3d(width, length, 0.0);
	glEnd();

	// left
	glBegin(GL_QUADS);
	glNormal3d(-1.0, 0.0, 0.0);
	glVertex3d(0.0, length, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, 0.0, height);
	glVertex3d(0.0, length, height);
	glEnd();

	// right
	glBegin(GL_QUADS);
	glNormal3d(1.0, 0.0, 0.0);
	glVertex3d(width, 0.0, 0.0);
	glVertex3d(width, length, 0.0);
	glVertex3d(width, length, height);
	glVertex3d(width, 0.0, height);
	glEnd();

	// top
	glBegin(GL_QUADS);
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(0.0, 0.0, height);
	glVertex3d(width, 0.0, height);
	glVertex3d(width, length, height);
	glVertex3d(0.0, length, height);
	glEnd();

	// bottom
	glBegin(GL_QUADS);
	glNormal3d(0.0, 0.0, -1.0);
	glVertex3d(width, length, 0.0);
	glVertex3d(width, 0.0, 0.0);
	glVertex3d(0.0, 0.0, 0.0);
	glVertex3d(0.0, length, 0.0);
	glEnd();

	glPopMatrix();
}
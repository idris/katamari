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

#include <math.h>
#include <iostream>

#include "common.h"
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
	glColor3fv(color);

	glPushMatrix();
	glTranslated(center[0], center[1], center[2]);
	glRotated(angle, 0.0, 0.0, 1.0);
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
	glVertex3d(0.0, length, height);
	glVertex3d(width, length, height);
	glVertex3d(width, length, 0.0);
	glVertex3d(0.0, length, 0.0);
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
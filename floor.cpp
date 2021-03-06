#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

#include <math.h>

#include "floor.h"

#include <iostream>
using namespace std;
Floor::Floor() {
	width = length = 50000;
//	color[0] = color[1] = color[2] = 1.0f;
	color[0] = 0.7f;
	color[1] = 0.3f;
	color[2] = 0.3f;
}

void Floor::draw() {
	glColor3fv(color);
	glBegin(GL_QUADS);
	glNormal3d(0.0, 0.0, 1.0);
	glVertex3d(0.0 - width/2, 0.0 - length/2, 0.0);
	glVertex3d(width/2, 0.0 - length/2, 0.0);
	glVertex3d(width/2, length/2, 0.0);
	glVertex3d(0.0 - width/2, length/2, 0.0);
	glEnd();
}
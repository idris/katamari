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

#include "common.h"
#include "floor.h"

#include <iostream>
using namespace std;
Floor::Floor() {
	width = length = 1000;
//	color[0] = color[1] = color[2] = 1.0f;
	color[0] = 0.7;
	color[1] = 0.3;
	color[2] = 0.3;
}

void Floor::draw() {
	glColor3fv(color);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0 - width/2, 0.0 - length/2, 0.0);
	glVertex3f(width/2, 0.0 - length/2, 0.0);
	glVertex3f(width/2, length/2, 0.0);
	glVertex3f(0.0 - width/2, length/2, 0.0);
	glEnd();
}
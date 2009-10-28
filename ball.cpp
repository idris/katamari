#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif
#include <math.h>

#include "common.h"
#include "ball.h"

#include "Quaternion.h"
#include "Vector3D.h"

#include <sys/timeb.h>

#include <iostream>
using namespace std;

void getMultMatrix(Quaternion *q, GLdouble *m) {
	double *qm = (double*)(q->getRotationMatrix().m);

	m[0] = qm[0];
	m[1] = qm[3];
	m[2] = qm[6];
	m[3] = 0;
	m[4] = qm[1];
	m[5] = qm[4];
	m[6] = qm[7];
	m[7] = 0;
	m[8] = qm[2];
	m[9] = qm[5];
	m[10] = qm[8];
	m[11] = 0;
	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

void multiplyMatrices(GLdouble *m1, GLdouble *m2, GLdouble *m) {
	int i,j,k;
	for(i=0;i<4;i++) {
		for(j=0;j<4;j++) {
			for(k=0;k<4;k++) {
				m[(i*4)+j] += m1[(i*4)+k] * m2[(k*4)+j];
			}
		}
	}
}


Ball::Ball() {
	init();
}

void Ball::init() {
	color[0] = 0.0;
	color[1] = 0.3;
	color[2] = 0.6;
	dx = 0.0;
	dy = 0.0;
	Quaternion q(0.0, *(new Vector3D(0.0,0.0,0.0)));
	rotation = (GLdouble*)calloc(sizeof(GLdouble), 16);
	getMultMatrix(&q, rotation);

	speed = 1;

	radius = 50.0;
	width = radius * 2;
	length = radius * 2;
	height = radius * 2;

	center[0] = center[1] = 0.0;
	center[2] = height/2;
}

void Ball::step() {
	struct timeb lastTime;
	memcpy(&lastTime, &time, sizeof(struct timeb));
	ftime(&time);

	double dt = time.millitm - lastTime.millitm;
	if(dt < 0) {
		dt += 1000;
	}

	dt *= speed;

	double s = sin(camera_angle * M_PI/180);
	double c = cos(camera_angle * M_PI/180);

	double ddx=0, ddy=0;

	if(upKey) {
		ddy = 1;
	}
	if(rightKey) {
		ddx = 1;
	}
	if(downKey) {
		ddy = -1;
	}
	if(leftKey) {
		ddx = -1;
	}

	ddx *= 0.0006 * dt;
	ddy *= 0.0006 * dt;

	dx += c * ddx - s * ddy;
	dy += s * ddx + c * ddy;

	dx *= 0.9995;
	dy *= 0.9995;

	double l = sqrt(dx*dx + dy*dy);
	if(l != 0) {
		Quaternion q(90.0 * (l / (2*M_PI*radius)), *(new Vector3D(dy/l,0.0 - dx/l,0.0)));
		GLdouble qm[16];
		GLdouble *m = (GLdouble*)calloc(sizeof(GLdouble), 16);
		getMultMatrix(&q, qm);
		multiplyMatrices(rotation, qm, m);
		free(rotation);
		rotation = m;

		center[0] += dx * l;
		center[1] += dy * l;
	}

	checkCollisions();
}

void Ball::draw() {
	step();

	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glMultMatrixd(rotation);
	glColor3f(0.3, 0.3, 0.3);
	gluSphere(gluNewQuadric(), 1.2*radius, 40, 20);
	glTranslatef(0.0 - width/2, 0.0 - length/2, 0.0 - height/2);

	glColor3fv(color);

	// front
	glBegin(GL_QUADS);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(width, 0.0f, 0.0f);
	glVertex3f(width, 0.0f, height);
	glVertex3f(0.0f, 0.0f, height);
	glEnd();

	// back
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, length, height);
	glVertex3f(width, length, height);
	glVertex3f(width, length, 0.0f);
	glVertex3f(0.0f, length, 0.0f);
	glEnd();

	// left
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, length, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, height);
	glVertex3f(0.0f, length, height);
	glEnd();

	// right
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(width, 0.0f, 0.0f);
	glVertex3f(width, length, 0.0f);
	glVertex3f(width, length, height);
	glVertex3f(width, 0.0f, height);
	glEnd();

	// top
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, height);
	glVertex3f(width, 0.0f, height);
	glVertex3f(width, length, height);
	glVertex3f(0.0f, length, height);
	glEnd();

	// bottom
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(width, length, 0.0f);
	glVertex3f(width, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, length, 0.0f);
	glEnd();

	glPopMatrix();
}

void Ball::faster() {
	speed += 0.1;
}

void Ball::slower() {
	if(speed > 0.1)
		speed -= 0.1;
}

bool Ball::checkCollision(Cube *object) {
	double normal[3];
	double oldDx = dx;
	double oldDy = dy;
	double x1 = center[0];
	double y1 = center[1];
	double x2 = object->center[0];
	double y2 = object->center[1];
	double objectRadius = 1.2 * object->radius;
	double v[2];
	double v2[2];

	if(sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)) <= radius + objectRadius) {
		normal[0] = x1 - x2;
		normal[1] = y1 - y2;

		double l = sqrt(normal[0]*normal[0] + normal[1]*normal[1]);

		normal[0] /= l;
		normal[1] /= l;

		v[0] = dx * abs(normal[0]);
		v[1] = dy * abs(normal[1]);

		v2[0] = dx - v[0];
		v2[1] = dy - v[1];

		dx = v2[0] - 0.9*v[0];
		dy = v2[1] - 0.9*v[1];

		// get out of the collision zone
		center[0] += 5.0 * dx;
		center[1] += 5.0 * dy;
		x1 = center[0];
		y1 = center[1];

		if(sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1)) <= radius + objectRadius) {
			// we are still in a collision. this is a problem. get out.
			// this usually happens when we side-swipe an object
			dx = -oldDx;
			dy = -oldDy;
			center[0] += 5.0 * dx;
			center[1] += 5.0 * dy;
//			x1 = center[0];
//			y1 = center[1];
		}
		return true;
	}

	return false;
}

void Ball::checkCollisions() {
	int i;
	for(i=0;i<numObjects;i++) {
		checkCollision(&objects[i]);
	}
}
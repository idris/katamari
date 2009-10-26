#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#include "unimotion.h"
#else
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

bool use_accelerometer = true;
int sms_type = 0;

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
	reset();
#ifdef __APPLE__
	sms_type = detect_sms();
#endif
}

void Ball::reset() {
	color[0] = 0.0;
	color[1] = 0.3;
	color[2] = 0.6;
	dx = 0.0;
	dy = 0.0;
	quat = new Quaternion(0.0, *(new Vector3D(0.0,0.0,0.0)));
	rotation = (GLdouble*)calloc(sizeof(GLdouble), 16);
	getMultMatrix(quat, rotation);

	radius = 5.0;
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

	double s = sin(camera_angle * M_PI/180);
	double c = cos(camera_angle * M_PI/180);

	double ddx=0, ddy=0;

	if(upKey) {
		ddy = 0.0005 * dt;
	}
	if(rightKey) {
		ddx = 0.0005 * dt;
	}
	if(downKey) {
		ddy = -0.0005 * dt;
	}
	if(leftKey) {
		ddx = -0.0005 * dt;
	}

	cout << camera_angle << endl;
	cout << s << " " << c << endl;

	dx += c * ddx - s * ddy;
	dy += s * ddx + c * ddy;

	dx *= 0.9995;
	dy *= 0.9995;

	double l = sqrt(dx*dx + dy*dy);
	if(l != 0) {
		Quaternion q(l, *(new Vector3D(dy/l,0.0 - dx/l,0.0)));
		GLdouble qm[16];
		GLdouble *m = (GLdouble*)calloc(sizeof(GLdouble), 16);
		getMultMatrix(&q, qm);
		multiplyMatrices(rotation, qm, m);
		free(rotation);
		rotation = m;

		center[0] += dx * l;
		center[1] += dy * l;
	}
}

void Ball::draw() {
	step();

	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glMultMatrixd(rotation);
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
	glVertex3f(0.0f, length, 0.0f);
	glVertex3f(width, length, 0.0f);
	glVertex3f(width, length, height);
	glVertex3f(0.0f, length, height);
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
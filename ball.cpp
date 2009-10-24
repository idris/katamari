#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
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


Ball::Ball() {
	reset();
}

void Ball::reset() {
	x = 0.5;
	y = 0.5;
	dx = 0.0030;
	dy = -0.0030;
	width = 0.08;
	height = 0.08;
	angle = 45;
	spin = -1;
	speed = 1;
	angleX = 0;
	angleY = 0;

	radius = 10.0;
	width = 10.0;
	length = 10.0;
	height = 10.0;

	center[0] = center[1] = 0.0;
	center[2] = height/2;
}

void Ball::draw() {
	struct timeb lastTime;
	memcpy(&lastTime, &time, sizeof(struct timeb));
	ftime(&time);

	double dt = time.millitm -  lastTime.millitm;
	if(dt < 0) {
		dt += 1000;
	}
	cout << time.time << "." << time.millitm << " diff: " << dt << endl;


	Quaternion qx(angleX += (dx * dt * 50), *(new Vector3D(1.0,0.0,0.0)));
	Quaternion qy(angleY += (dy * dt * 50), *(new Vector3D(0.0,1.0,0.0)));
	GLdouble mx[16];
	GLdouble my[16];

	getMultMatrix(&qx, mx);
	getMultMatrix(&qy, my);


	glColor3f(0.0, 0.3, 0.6);
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
	glMultMatrixd(mx);
	glMultMatrixd(my);
	glTranslatef(0.0 - width/2, 0.0 - length/2, 0.0 - height/2);

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

void Ball::step() {
	x += dx;
	y += dy;

	float theta = angle * M_PI/180;
	float add = width/2 + (width/2 * sin(theta)) + (width/2 * cos(theta));
	float subtract = width/2 - (width/2 * sin(theta)) - (width/2 * cos(theta));

	float top = y + add;
	float right = x + add;
	float bottom = y + subtract;
	float left = x + subtract;

	if(top >= 1) {
		dy = -dy;
		bounce();
	} else if(bottom <= 0) {
		dy = -dy;
		bounce();
	}

	if(right >= 1) {
		dx = -dx;
		bounce();
	} else if(left <= 0 ||
			  // check for collision with the paddle
			  (paddle.x + paddle.width >= left && paddle.y + paddle.height >= bottom && paddle.y <= top)) {
		dx = -dx;
		bounce();
	}

	angle += -spin;
	if(angle < 0) {
		angle += 90;
	} else if(angle >= 90) {
		angle -= 90;
	}

//	y = 0;
//	x = 0;
}

void Ball::bounce() {
	if(angle < 45) {
		spin = 2 * angle/45;
	} else if(angle > 45) {
		spin = 2 * (45-angle)/45;
	}
}

void Ball::slower() {
	dx /= speed;
	dy /= speed;
	spin /= speed;
	speed -= 0.2;
	dx *= speed;
	dy *= speed;
	spin *= speed;
}

void Ball::faster() {
	dx /= speed;
	dy /= speed;
	spin /= speed;
	speed += 0.2;
	dx *= speed;
	dy *= speed;
	spin *= speed;
}
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
/*				cout << "before: m[" << i << "][" << j << "] = " << m[(i*4)+j] << endl;
				cout << "m[" << i << "][" << j << "] += m1[" << i << "][" << k << "] * m2[" << k << "][" << j << "]" << endl;
				cout << "after: m[" << i << "][" << j << "] = " << m[(i*4)+j] << endl;
*/			}
		}
	}
/*
	cout << "m:[" << endl;
	for(i=0;i<4;i++) {
		cout << m[(i*4)+0] << " " << m[(i*4)+1] << " " << m[(i*4)+2] << " " << m[(i*4)+3] << endl;
	}
	cout << "]" << endl;

	cout << "m1:[" << endl;
	for(i=0;i<4;i++) {
		cout << m1[(i*4)+0] << " " << m1[(i*4)+1] << " " << m1[(i*4)+2] << " " << m1[(i*4)+3] << endl;
	}
	cout << "]" << endl;

	cout << "m2:[" << endl;
	for(i=0;i<4;i++) {
		cout << m2[(i*4)+0] << " " << m2[(i*4)+1] << " " << m2[(i*4)+2] << " " << m2[(i*4)+3] << endl;
	}
	cout << "]" << endl;

	exit(0);
*/
}


Ball::Ball() {
	reset();
#ifdef __APPLE__
	sms_type = detect_sms();
#endif
}

void Ball::reset() {
	x = 0.5;
	y = 0.5;
	dx = 0.0;
	dy = 0.0;
	width = 0.08;
	height = 0.08;
	angle = 0.0;
	spin = -1;
	speed = 1;
	angleX = 0;
	angleY = 0;
	accelX = 0;
	accelY = 0;
	xPercent = 0.0;
	yPercent = 0.0;
	dxPercent = 0.0;
	dyPercent = 0.0;
	quat = new Quaternion(0.0, *(new Vector3D(0.0,0.0,0.0)));
	rotation = (GLdouble*)calloc(sizeof(GLdouble), 16);
	getMultMatrix(quat, rotation);

	radius = 10.0;
	width = 10.0;
	length = 10.0;
	height = 10.0;

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

	if(upKey) {
		dy += 0.001 * dt;
	}
	if(rightKey) {
		dx += 0.001 * dt;
	}
	if(downKey) {
		dy -= 0.001 * dt;
	}
	if(leftKey) {
		dx -= 0.001 * dt;
	}

	dy *= 0.999;
	dx *= 0.999;

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

//	cout << angle << " " << xPercent << " " << yPercent << endl;

//	double l = sqrt(xPercent*xPercent + yPercent*yPercent);
//	double l = 1;

//	Quaternion qx(angle, *(new Vector3D(xPercent/l,0.0 - yPercent/l,0.0)));
//	qx += quat;
//	quat = qx;
//	Quaternion qx(angleX, *(new Vector3D(1.0,0.0,0.0)));

//	qx.normalize();
//	Quaternion qy(angleY, *(new Vector3D(0.0,-1.0,0.0)));

//	Quaternion sum = qy * qx;
//	sum.normalize();
//	GLdouble mx[16];
//	GLdouble my[16];
//	GLdouble msum[16];

//	getMultMatrix(quat, mx);
//	getMultMatrix(&qy, my);
//	getMultMatrix(&sum, msum);


	glColor3f(0.0, 0.3, 0.6);
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
//	glMultMatrixd(msum);
//	if(l != 0) glMultMatrixd(mx);
	glMultMatrixd(rotation);
//	glMultMatrixd(my);
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

/*
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
*/

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
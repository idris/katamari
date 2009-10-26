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

//	cout << time.time << "." << time.millitm << " diff: " << dt << endl;
/*
	double jerkX = 0;
	double jerkY = 0;

	if(leftKey) {
		jerkX = -1;
	}
	if(rightKey) {
		jerkX = 1;
	}
	if(upKey) {
		jerkY = 1;
	}
	if(downKey) {
		jerkY = -1;
	}

//	accelX += jerkY / 1000;
//	accelY += jerkX / 1000;

//	accelX *= 0.5;
//	accelY *= 0.5;

	dx += jerkY / 1000;
	dy += jerkX / 1000;

	dx *= 0.999;
	dy *= 0.999;

	if(jerkY == 0 && abs(dx) < 0.01) dx = 0;
	if(jerkX == 0 && abs(dy) < 0.01) dy = 0;

	angleX += dx * dt;
	angleY += dy * dt;

//	angle += (dx + dy) * dt;
	double dr = sqrt(dx*dx + dy*dy);
//	angle += dr/radius;
*/
/*	if(angleX >= 360) angleX -= 360;
	else if(angleX <= -360) angleX += 360;
	if(angleY >= 360) angleY -= 360;
	else if(angleY <= -360) angleY += 360;
*/
/*
	if(abs(angleX) + abs(angleY) == 0) {
		xPercent = yPercent = 0;
	} else {
		xPercent = abs(dx) / dr;
		yPercent = abs(dy) / dr;
	}
*/
//	center[0] += (dy * dt)/360 * (2 * M_PI * radius);
//	center[1] += (dx * dt)/360 * (2 * M_PI * radius);


#ifdef __APPLE__
	double x, y, z;
	if(false && sms_type > 0) {
		read_sms_real(sms_type, &x, &y, &z);
//		cout << x << " " << y << " " << z;
//		xPercent = y;
		xPercent = 0;
		yPercent = x;
	}
#endif

/*
	if(upKey) {
		xPercent += 0.1;
	}
	if(downKey) xPercent -= 0.1;
	if(leftKey) yPercent -= 0.1;
	if(rightKey) {
		yPercent += 0.001;
	}
*/

	double dAngle = 0;

	if(upKey) {
		dy += 0.01 * dt;
		xPercent += 0.001 * dt;
		if(!rightKey && !leftKey && yPercent > 0)
			yPercent -= 0.001 * dt;
		dAngle = 0.1 * dt;
	}
	if(rightKey) {
		dx += 0.01 * dt;
		yPercent += 0.001 * dt;
		if(!upKey && !downKey && xPercent > 0)
			xPercent -= 0.001 * dt;
		dAngle = 0.1 * dt;
	}
	if(downKey) {
		dy -= 0.01 * dt;
		yPercent += 0.001 * dt;
		if(!rightKey && !leftKey && xPercent < 0)
			xPercent += 0.001 * dt;
		dAngle = -0.1 * dt;
	}
	if(leftKey) {
		dx -= 0.01 * dt;
		xPercent -= 0.001 * dt;
		if(!upKey && !downKey)
			yPercent -= 0.001 * dt;
		dAngle = -0.1 * dt;
	}

//	dAngle += 2.0 * abs(dAngle);
	angle += dAngle;

	if(xPercent < 0) {
		xPercent = 0.0;
	} else if(xPercent > 1) {
		xPercent = 1.0;
	} else if(xPercent < -1) {
		xPercent = -1.0;
	}
	if(yPercent < 0) {
		yPercent = 0.0;
	} else if(yPercent > 1) {
		yPercent = 1.0;
	} else if(yPercent < -1) {
		yPercent = -1.0;
	}

	// normalize x and y percents:
	double nx, ny;
	double l = sqrt(xPercent * xPercent + yPercent * yPercent);
	if(l > 0) {
		nx = xPercent / l;
		ny = yPercent / l;
//		center[0] += (ny * dAngle)/360 * (2 * M_PI * radius);
//		center[1] += (nx * dAngle)/360 * (2 * M_PI * radius);
	}

	dx *= 0.999;
	dy *= 0.999;
	double s = sqrt(dx*dx + dy*dy);
	angle += s;

	if(s != 0) {
		dyPercent += dx/s;
		dxPercent += dy/s;
	}

	dxPercent *= 0.99;
	dyPercent *= 0.99;

//	cout << s << endl;
	yPercent += dyPercent;
	xPercent += dxPercent;

	double ss = sqrt(yPercent*yPercent + xPercent*xPercent);

	if(s != 0) {
//		cout << dx / ss << endl;
		center[0] += (yPercent * dx/ss)/360 * (2 * M_PI * radius);
		center[1] += (xPercent * dy/ss)/360 * (2 * M_PI * radius);
	}


//	angle += 0.1 * dt;
//	if(yPercent > 0 && rightKey) angle += 0.1 * dt;
//	if(upKey || rightKey || downKey || leftKey) angle += 0.1 * dt;
//	if(angle != 0) angle += 0.1 * dt;
//	if(upKey || rightKey) angle += 0.1;
//	if(leftKey || downKey) angle -= 0.1;
	if(angle >= 360) angle -= 360;
	if(angle < 0) angle += 360;

	if(s != 0) {
		cout << angle << " " << dx/s << " " << dy/s << endl;
		Quaternion q(s*s, *(new Vector3D(dy/s,0.0 - dx/s,0.0)));
		*quat += q;
		quat->normalize();
	}
}

void Ball::draw() {
	step();

//	cout << angle << " " << xPercent << " " << yPercent << endl;

	double l = sqrt(xPercent*xPercent + yPercent*yPercent);
//	double l = 1;

//	Quaternion qx(angle, *(new Vector3D(xPercent/l,0.0 - yPercent/l,0.0)));
//	qx += quat;
//	quat = qx;
//	Quaternion qx(angleX, *(new Vector3D(1.0,0.0,0.0)));

//	qx.normalize();
//	Quaternion qy(angleY, *(new Vector3D(0.0,-1.0,0.0)));

//	Quaternion sum = qy * qx;
//	sum.normalize();
	GLdouble mx[16];
//	GLdouble my[16];
//	GLdouble msum[16];

	getMultMatrix(quat, mx);
//	getMultMatrix(&qy, my);
//	getMultMatrix(&sum, msum);


	glColor3f(0.0, 0.3, 0.6);
	glPushMatrix();
	glTranslatef(center[0], center[1], center[2]);
//	glMultMatrixd(msum);
	if(l != 0) glMultMatrixd(mx);
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
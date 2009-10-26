#ifndef _BALL_H
#define _BALL_H

#include <sys/timeb.h>
#include "Quaternion.h"

class Ball {
	float radius;

	GLfloat v[8][3];
	float x;
	float y;
	float dx;
	float dy;
	float width;
	float length;
	float height;
	float angle;
	float angleX;
	float angleY;
	double accelX;
	double accelY;
	double xPercent;
	double yPercent;
	double dxPercent;
	double dyPercent;
	Quaternion *quat;
	GLdouble *rotation;

	float spin;
	float speed;
	struct timeb time;
	void bounce();
public:
	float center[3];

	Ball();
	void reset();
	void draw();
	void step();
	void slower();
	void faster();
};

#endif
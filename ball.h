#ifndef _BALL_H
#define _BALL_H

#include <sys/timeb.h>

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
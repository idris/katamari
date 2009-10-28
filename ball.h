#ifndef _BALL_H
#define _BALL_H

#include <sys/timeb.h>
#include "Quaternion.h"

class Ball {
	float radius;
	float dx;
	float dy;
	float width;
	float length;
	float height;
	GLfloat color[3];
	GLdouble *rotation;
	struct timeb time;
	float speed;

	void checkCollisions();
	bool checkCollision(Cube *object);

public:
	float center[3];

	Ball();
	void init();
	void step();
	void draw();
	void faster();
	void slower();
};

#endif
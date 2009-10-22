#ifndef _BALL_H
#define _BALL_H

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
	float spin;
	float speed;
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
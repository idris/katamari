#ifndef _PADDLE_H
#define _PADDLE_H

class Paddle {
	float speed;
	float dy;
public:
	float direction;
	float x;
	float y;
	float width;
	float height;

	Paddle();
	void reset();
	void draw();
	void step();
	void up();
	void down();
	void stop();
	void slower();
	void faster();
};

#endif
#include "common.h"
#include "paddle.h"

#include <OpenGL/glu.h>
#include <OpenGL/gl.h>

Paddle::Paddle() {
	reset();
}

void Paddle::reset() {
	x = 0.01;
	y = 0.1;
	width = 0.03;
	height = 0.2;
	direction = 0;
	dy = 0.006;
	speed = 1;
}

void Paddle::draw() {
	glColor3fv(BLACK_RGB);
	glRectf(x, y, x+width, y+height);
}

void Paddle::step() {
	y += direction * dy;
	if(y >= 1 - height) y = 1 - height;
	else if(y <= 0) y = 0;
}

void Paddle::up() {
	direction = 1;
}

void Paddle::down() {
	direction = -1;
}

void Paddle::stop() {
	direction = 0;
}

void Paddle::slower() {
	dy /= speed;
	speed -= 0.2;
	dy *= speed;
}

void Paddle::faster() {
	dy /= speed;
	speed += 0.2;
	dy *= speed;
}
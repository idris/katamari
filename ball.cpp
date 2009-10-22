#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#include <math.h>

#include "common.h"
#include "ball.h"

#include <iostream>
using namespace std;


Ball::Ball() {
	reset();
}

void Ball::reset() {
	x = 0.5;
	y = 0.5;
	dx = 0.0030;
	dy = -0.0020;
	width = 0.08;
	height = 0.08;
	angle = 45;
	spin = -1;
	speed = 1;

	radius = 10.0;
	width = 10.0;
	length = 10.0;
	height = 10.0;

	center[0] = center[1] = 0.0;
	center[2] = height/2;
}

void Ball::draw() {
	glColor3f(0.0, 0.3, 0.6);

	glPushMatrix();
	glTranslatef(center[0], center[1], center[3]);
//	glRotatef(45.0, 0.0f, 0.0f, 1.0f);
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
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#include <math.h>

#include "common.h"
#include "ball.h"

#include <iostream>
using namespace std;


GLfloat n[6][3] = {  /* Normals for the 6 faces of a cube. */
	{-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
	{0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };

GLint faces[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
	{0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
	{4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };


Ball::Ball() {
	reset();
}

void Ball::reset() {
	radius = 1.0f;
	center[0] = center[1] = center[2] = 0.5f;

	v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
	v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
	v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
	v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
	v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
	v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

	x = 0.5;
	y = 0.5;
	dx = 0.0030;
	dy = -0.0020;
	width = 0.08;
	height = 0.08;
	angle = 45;
	spin = -1;
	speed = 1;
}

void Ball::draw() {
	glColor3fv(RED_RGB);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();


	glColor3fv(BLUE_RGB);
	glBegin(GL_QUADS);
	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();


	glColor3fv(GREEN_RGB);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 1.0f, 1.0f);
	glEnd();

	
//	int i;
//
//	for (i = 0; i < 6; i++) {
//		glBegin(GL_QUADS);
//		glNormal3fv(&n[i][0]);
//		glVertex3fv(&v[faces[i][0]][0]);
//		glVertex3fv(&v[faces[i][1]][0]);
//		glVertex3fv(&v[faces[i][2]][0]);
//		glVertex3fv(&v[faces[i][3]][0]);
//		glEnd();
//	}

//	glColor3fv(BLUE_RGB);
//
//	glTranslatef(x + (width/2), y + (height/2), 0);
//	glRotatef(angle, 0.0, 0.0, 1.0);
//	glTranslatef(-(x + width/2), -(y + height/2), 0);
//
//	glRectf(x, y, x+width, y+height);
//
//	glLoadIdentity();
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
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

#include "common.h"
#include "cube.h"
#include "ball.h"
#include "floor.h"

GLfloat RED_RGB[] = {1.0, 0.0, 0.0};
GLfloat GREEN_RGB[] = {0.0, 1.0, 0.0};
GLfloat BLUE_RGB[] = {0.0, 0.0, 1.0};
GLfloat BLACK_RGB[] = {0.0, 0.0, 0.0};
GLfloat WHITE_RGB[] = {1.0, 1.0, 1.0};

int numObjects = 0;
Cube *objects;

Ball ball;
Floor theFloor;
Sky sky;

bool leftKey = false;
bool rightKey = false;
bool upKey = false;
bool downKey = false;

GLdouble shadowMatrix[4][4];

GLdouble camera_angle = 0.0;


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
			}
		}
	}
}

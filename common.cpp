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
#include "cloth.h"



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
Cloth cloth(304,200,14,10);

GLdouble camera_distance = 500.0;
GLdouble camera_height_angle = 10.0;

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

// adapted from http://labprograms.wordpress.com/2009/07/22/matrix-vector-multiplication-using-friend-function/
GLdouble* matrixTimesVector(GLdouble *m1, GLdouble *v1) {
	GLdouble *m = (GLdouble*)calloc(sizeof(GLdouble), 16);
	GLdouble *ans = (GLdouble*)calloc(sizeof(GLdouble), 3);
	int i, j;

	m[0] = m1[0];
	m[4] = m1[1];
	m[8] = m1[2];
	m[12] = m1[3];
	m[1] = m1[4];
	m[5] = m1[5];
	m[9] = m1[6];
	m[13] = m1[7];
	m[2] = m1[8];
	m[6] = m1[9];
	m[10] = m1[10];
	m[14] = m1[11];
	m[3] = m1[12];
	m[7] = m1[13];
	m[11] = m1[14];
	m[15] = m1[15];

//	cout<<"\nThe resultant matrix...\n";
	for(i=0;i<3;i++)
	{
		ans[i]=0;
		for(j=0;j<3;j++)
			ans[i] += m[(i*4)+j] * v1[j];
//		cout<<ans[i]<<"\t";
	}
	return ans;
}
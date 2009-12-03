#include "cube.h"
#include "ball.h"
#include "floor.h"
#include "sky.h"
#include "cloth.h"

extern GLfloat RED_RGB[];
extern GLfloat GREEN_RGB[];
extern GLfloat BLUE_RGB[];
extern GLfloat BLACK_RGB[];
extern GLfloat WHITE_RGB[];

extern int numObjects;
extern Cube *objects;

extern Ball ball;
extern Floor theFloor;
extern Sky sky;
extern Cloth cloth;

extern GLdouble camera_distance;
extern GLdouble camera_height_angle;

extern bool leftKey;
extern bool rightKey;
extern bool upKey;
extern bool downKey;

extern GLdouble shadowMatrix[4][4];

extern GLdouble camera_angle;


void getMultMatrix(Quaternion *q, GLdouble *m);

void multiplyMatrices(GLdouble *m1, GLdouble *m2, GLdouble *m);

GLdouble* matrixTimesVector(GLdouble *m1, GLdouble *v1);
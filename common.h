#include "cube.h"
#include "ball.h"
#include "floor.h"
#include "sky.h"

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

extern bool leftKey;
extern bool rightKey;
extern bool upKey;
extern bool downKey;

extern GLdouble shadowMatrix[4][4];

extern GLdouble camera_angle;


void getMultMatrix(Quaternion *q, GLdouble *m);

void multiplyMatrices(GLdouble *m1, GLdouble *m2, GLdouble *m);
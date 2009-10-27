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

GLdouble camera_angle = 0.0;
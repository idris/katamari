#include <OpenGL/glu.h>
#include <OpenGL/gl.h>

#include "common.h"
#include "cube.h"
#include "ball.h"
#include "floor.h"

GLfloat RED_RGB[] = {1.0, 0.0, 0.0};
GLfloat GREEN_RGB[] = {0.0, 1.0, 0.0};
GLfloat BLUE_RGB[] = {0.0, 0.0, 1.0};
GLfloat BLACK_RGB[] = {0.0, 0.0, 0.0};
GLfloat WHITE_RGB[] = {1.0, 1.0, 1.0};

Cube *objects;

Ball ball;
Floor theFloor;
Sky sky;
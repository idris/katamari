#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <cmath>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#include "unimotion.h"
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "common.h"
#include "ball.h"
#include "paddle.h"

using namespace std;

GLint TIMER_DELAY = 5;

GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, -1.0, 1.0, 0.0};  /* Infinite light location. */

int sms_type;

void myReshape(int w, int h) {
//    glViewport (0, 0, w, h);
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
//    glMatrixMode(GL_MODELVIEW);
//    glutPostRedisplay();
}

void display(void) {
	GLdouble xyCameraRadius;
	GLdouble camera_height;
	GLdouble camera_distance = 4.0;
	GLdouble camera_angle = 30.0;
	GLdouble camera_height_angle = 30.0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	xyCameraRadius = cos(camera_height_angle * M_PI/180) * camera_distance;
	camera_height = ball.center[2] + sin(camera_height_angle * M_PI/180) * camera_distance;

	// camera stuff
	glLoadIdentity();
	gluLookAt(ball.center[0] + sin(camera_angle * M_PI/180) * xyCameraRadius, ball.center[1] - cos(camera_angle * M_PI/180) * xyCameraRadius, camera_height, ball.center[0], ball.center[1], ball.center[2], 0.0, 0.0, 1.0);
//	gluLookAt(ball.center[0], -2.0, camera_height, ball.center[0], ball.center[1], ball.center[2], 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluPerspective(<#GLdouble fovy#>, <#GLdouble aspect#>, <#GLdouble zNear#>, <#GLdouble zFar#>)
	gluPerspective(60.0, 1.0, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);

	// draw the floor
	glColor3fv(WHITE_RGB);
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-50.0f, -50.0f, 0.0f);
	glVertex3f(50.0f, -50.0f, 0.0f);
	glVertex3f(50.0f, 50.0f, 0.0f);
	glVertex3f(-50.0f, 50.0f, 0.0f);
	glEnd();


	ball.draw();

    glutSwapBuffers();
}

void myTimer(int id) {
	double x, y, z;
	read_sms_real(sms_type, &x, &y, &z);
	paddle.direction = y * 10.0f;
//	paddle.dy = 0.2f;
//	if(y > 0) {
//		paddle.down();
//	} else {
//		paddle.up();
//	}

	paddle.step();
	ball.step();
	
	glutPostRedisplay();
    glutTimerFunc(TIMER_DELAY, myTimer, 0);
}

void myKeyboard(unsigned char c, int x, int y) {
    switch (c) {
        case 'q':
            exit(0);
            return;
		case 'r':
			ball.reset();
			paddle.reset();
			return;
		case '-':
		case '_':
			ball.slower();
			paddle.slower();
			return;
		case '+':
		case '=':
			ball.faster();
			paddle.faster();
			return;
        default:
            cout << "Hit q to quit.  All other characters ignored" << endl;
            break;
    }
}
void mySpecial(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			paddle.up();
			break;
		case GLUT_KEY_DOWN:
			paddle.down();
			break;
		default:
			break;
	}
}

void mySpecialUp(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			paddle.stop();
			break;
		case GLUT_KEY_DOWN:
			paddle.stop();
			break;
		default:
			break;
	}
}

int main(int argc, char** argv)
{
	sms_type = detect_sms();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(argv[0]);

    glutDisplayFunc(display);
    glutReshapeFunc(myReshape);
//    glutKeyboardFunc(myKeyboard);
//	glutSpecialFunc(mySpecial);
//	glutSpecialUpFunc(mySpecialUp);
//	glutIdleFunc(display);
//    glutTimerFunc(TIMER_DELAY, myTimer, 0);


	/* initialize 3d environment */
//	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHTING);
//	glMatrixMode(GL_PROJECTION);
//	gluPerspective(40.0, 1.0, -9.0, 50.0);
//	gluPerspective(40.0, 1.0, 1.0, 10.0);
//	glMatrixMode(GL_MODELVIEW);
//	gluLookAt(-10.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
//	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
//	glTranslatef(0.0, 0.0, -1.0);
//	glRotatef(60, 1.0, 0.0, 0.0);
//	glRotatef(-20, 0.0, 0.0, 1.0);

//	glClearColor(0.3, 0.3, 0.3, 0.0);


	glEnable(GL_DEPTH_TEST);

	//setup lighting
	GLfloat globalambient[] = {0.5,0.5,0.5,1.0};
	GLfloat sun_direction[] = {5.0, -5.0, 5.0, 0.0};
	GLfloat sun_intensity[] = {0.8, 0.8, 0.8, 1.0};
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalambient);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_intensity);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glutMainLoop();

    return 0;
}

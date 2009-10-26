#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <cmath>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "common.h"

using namespace std;

GLint TIMER_DELAY = 10;

GLdouble camera_distance = 50.0;
GLdouble camera_height_angle = 20.0;

GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, -1.0, 1.0, 0.0};  /* Infinite light location. */


int numObjects = 0;

void reshape(int w, int h) {
	sky.init();
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
	gluPerspective(60.0, 1.0, 0.5, 500.0);
	glMatrixMode(GL_MODELVIEW);

	theFloor.draw();
	sky.draw();

	for(int i=0;i<numObjects;i++) {
		objects[i].draw();
	}

	ball.draw();

    glutSwapBuffers();
}

void myIdle() {
	glutPostRedisplay();
}

void myTimer(int id) {
	double x, y, z;

//	cout << x << " " << y << " " << z << endl;


//	paddle.direction = y * 10.0f;
//	paddle.dy = 0.2f;
//	if(y > 0) {
//		paddle.down();
//	} else {
//		paddle.up();
//	}

//	paddle.step();
//	ball.step();

	glutPostRedisplay();
    glutTimerFunc(TIMER_DELAY, myTimer, 0);
}

void myKeyboard(unsigned char c, int x, int y) {
    switch (c) {
        case 'q':
            exit(0);
            return;
		case 'e':
			camera_angle -= 2;
			return;
		case 'r':
			camera_angle += 2;
			return;
		case 'd':
			camera_height_angle -= 2;
			return;
		case 'f':
			camera_height_angle += 2;
			return;
		case 'c':
			if(camera_distance > 1)
				camera_distance -= 1;
			return;
		case 'v':
			camera_distance += 1;
			return;
		case '-':
		case '_':
//			ball.slower();
//			paddle.slower();
			return;
		case '+':
		case '=':
//			ball.faster();
//			paddle.faster();
			return;
/*		case 'a':
			use_accelerometer = !use_accelerometer;
			return;
*/        default:
            cout << "Hit q to quit.  All other characters ignored" << endl;
            return;
    }
}

void mySpecial(int key, int x, int y) {
	float s = sin(camera_angle * M_PI/180);
	float c = cos(camera_angle * M_PI/180);
	switch (key) {
		case GLUT_KEY_UP:
			upKey = true;
//			ball.center[0] -= s;
//			ball.center[1] += c;
			break;
		case GLUT_KEY_DOWN:
			downKey = true;
//			ball.center[0] += s;
//			ball.center[1] -= c;
			break;
		case GLUT_KEY_LEFT:
			leftKey = true;
//			ball.center[0] -= c;
//			ball.center[1] -= s;
			break;
		case GLUT_KEY_RIGHT:
			rightKey = true;
//			ball.center[0] += c;
//			ball.center[1] += s;
			break;
		default:
			break;
	}
}

void mySpecialUp(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			upKey = false;
//			ball.center[1] += 1;
			break;
		case GLUT_KEY_DOWN:
			downKey = false;
//			ball.center[1] -= 1;
			break;
		case GLUT_KEY_LEFT:
			leftKey = false;
//			ball.center[0] -= 1;
			break;
		case GLUT_KEY_RIGHT:
			rightKey = false;
//			ball.center[0] += 1;
			break;
		default:
			break;
	}
}

void readObjects() {
	ifstream inFile;
	inFile.open("objects.txt");
	if(!inFile) {
		cerr << "Unable to open objects.txt" << endl;
		return;
	}

	inFile >> numObjects;

	objects = new Cube[numObjects];

	char type[10];

	int i = 0;
	while(!inFile.eof()) {
		inFile >> type;
		if(strcmp("CUBE", type) == 0) {
			objects[i].read(&inFile);
		} else {
			cerr << "Unknown object type: " << type << endl;
		}
		i++;
	}

	inFile.close();
}

int main(int argc, char** argv) {
	readObjects();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(0, 0);
    glutCreateWindow(argv[0]);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(myKeyboard);
	glutSpecialFunc(mySpecial);
	glutSpecialUpFunc(mySpecialUp);
	glutIdleFunc(myIdle);
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
//	GLfloat globalposition[] = {0.5, 0.5, 200.0};
	GLfloat globalambient[] = {0.6, 0.6, 0.6, 1.0};
//	GLfloat globaldiffuse[] = {5.0, 5.0, 5.0, 1.0};
	GLfloat sun_direction[] = {1.0, -1.0, 2.0, 0.0};
	GLfloat sun_intensity[] = {0.5, 0.5, 0.5, 1.0};
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalambient);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_intensity);
//	glEnable(GL_LIGHT1);
//	glLightfv(GL_LIGHT1, GL_AMBIENT, globalambient);
//	glLightfv(GL_LIGHT1, GL_DIFFUSE, globaldiffuse);
//	glLightfv(GL_LIGHT1, GL_POSITION, globalposition);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glutMainLoop();

    return 0;
}

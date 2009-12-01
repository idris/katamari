#include <math.h>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#include "common.h"

using namespace std;

GLint TIMER_DELAY = 10;

bool pause_cloth = false;
GLdouble cloth_flip[4][4];

GLdouble camera_distance = 500.0;
GLdouble camera_height_angle = 10.0;

GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, -1.0, 1.0, 0.0};  /* Infinite light location. */

int viewWidth, viewHeight;

void reshape(int w, int h) {
	sky.init();
	glViewport (0, 0, w, h);
	viewWidth = w;
	viewHeight = h;
}

void display(void) {
	GLdouble xyCameraRadius;
	GLdouble camera_height;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	xyCameraRadius = cos(camera_height_angle * M_PI/180) * camera_distance;
	camera_height = 50.0 + ball.center[2] + sin(camera_height_angle * M_PI/180) * camera_distance;

	// camera stuff
	glLoadIdentity();
	gluLookAt(ball.center[0] + sin(camera_angle * M_PI/180) * xyCameraRadius, ball.center[1] - cos(camera_angle * M_PI/180) * xyCameraRadius, camera_height, ball.center[0], ball.center[1], ball.center[2], 0.0, 0.0, 1.0);
//	gluLookAt(ball.center[0], -2.0, camera_height, ball.center[0], ball.center[1], ball.center[2], 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluPerspective(<#GLdouble fovy#>, <#GLdouble aspect#>, <#GLdouble zNear#>, <#GLdouble zFar#>)
	gluPerspective(45.0, (GLdouble)viewWidth/viewHeight, 50.0, 50000.0);
	glMatrixMode(GL_MODELVIEW);

	theFloor.draw();
	sky.draw();

	for(int i=0;i<numObjects;i++) {
		objects[i].draw();
	}

	ball.draw();

	if(!pause_cloth) {
		cloth.addForce(Vec3(0.0,-1.0,0.0)*(0.5*0.5)); // add gravity each frame, pointing down
//		cloth.windForce(Vec3(0.1,0.0,0.05)*(0.05*0.005)); // generate some wind each frame
		cloth.timeStep(); // calculate the particle positions of the next frame
		Vec3 center(ball.center[0] - 100, ball.center[2] - 200.0, ball.center[1] - 100);
		cloth.ballCollision(center, 1.2 * ball.radius);
	}

	glDisable(GL_CULL_FACE);
	glPushMatrix();

	glDisable(GL_LIGHTING);
	glColor3f(0.3f, 0.1f, 0.1f);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -1.0f);
	glPushMatrix();
	glMultMatrixd((GLdouble *)shadowMatrix);
	cloth.drawShadow();
	glPopMatrix();
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_LIGHTING);

	glTranslated(100.0, 100.0, 200.0);
	glMultMatrixd((GLdouble *)cloth_flip);

	cloth.drawShaded();

	glPopMatrix();
	glEnable(GL_CULL_FACE);

    glutSwapBuffers();
}

void myIdle() {
	glutPostRedisplay();
}

void myTimer(int id) {
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
			if(camera_angle < 0) camera_angle += 360;
			return;
		case 'r':
			camera_angle += 2;
			if(camera_angle > 360) camera_angle -= 360;
			return;
		case 'd':
			if(camera_height_angle > -10)
				camera_height_angle -= 2;
			return;
		case 'f':
			if(camera_height_angle < 80)
				camera_height_angle += 2;
			return;
		case 'c':
			if(camera_distance > 1)
				camera_distance -= 10;
			return;
		case 'v':
			camera_distance += 10;
			return;
		case 'p':
			pause_cloth = !pause_cloth;
			return;
		case '-':
		case '_':
			ball.slower();
			return;
		case '+':
		case '=':
			ball.faster();
			return;
        default:
            cout << "Invalid key pressed. Hit q to quit." << endl;
            return;
    }
}

void mySpecial(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			upKey = true;
			break;
		case GLUT_KEY_DOWN:
			downKey = true;
			break;
		case GLUT_KEY_LEFT:
			leftKey = true;
			break;
		case GLUT_KEY_RIGHT:
			rightKey = true;
			break;
		default:
			break;
	}
}

void mySpecialUp(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			upKey = false;
			break;
		case GLUT_KEY_DOWN:
			downKey = false;
			break;
		case GLUT_KEY_LEFT:
			leftKey = false;
			break;
		case GLUT_KEY_RIGHT:
			rightKey = false;
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
	GLdouble lightx=1.0, lighty=1.0, lightz=2.0;
	shadowMatrix[0][0] = 1.0;
	shadowMatrix[1][1] = 1.0;
	shadowMatrix[2][2] = 0.0;
	shadowMatrix[3][3] = 1.0;
	shadowMatrix[2][0] = (0.0-lightx) / lightz;
	shadowMatrix[2][1] = (0.0-lighty) / lightz;

	cloth_flip[0][0] = 1;
	cloth_flip[0][1] = 0;
	cloth_flip[0][2] = 0;
	cloth_flip[0][3] = 0;
	
	cloth_flip[1][0] = 0;
	cloth_flip[1][1] = 0;
	cloth_flip[1][2] = 1;
	cloth_flip[1][3] = 0;
	
	cloth_flip[2][0] = 0;
	cloth_flip[2][1] = 1;
	cloth_flip[2][2] = 0;
	cloth_flip[2][3] = 0;
	
	cloth_flip[3][0] = 0;
	cloth_flip[3][1] = 0;
	cloth_flip[3][2] = 0;
	cloth_flip[3][3] = 1;
	

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
    glutTimerFunc(TIMER_DELAY, myTimer, 0);


	glEnable(GL_DEPTH_TEST);

	//setup lighting
	GLfloat globalambient[] = {0.45, 0.45, 0.45, 1.0};
	GLfloat sun_direction[] = {lightx, lighty, lightz, 0.0};
	GLfloat sun_intensity[] = {0.6, 0.6, 0.6, 1.0};
	glEnable(GL_LIGHTING);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalambient);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_intensity);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    glutMainLoop();

    return 0;
}

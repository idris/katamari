#ifndef _CUBE_H
#define _CUBE_H

#include <fstream>
using namespace std;

class Cube {
	GLdouble angle; // Rotation angle (with respect to z-axis) CCW in degrees.
	GLdouble height; // The side length of the cube
	GLfloat color[3];

	GLdouble length;
	GLdouble width;

	void _draw();

public:
	GLdouble center[3]; // Coordinates [x,y,z] of the center of the object
	GLdouble radius; // Half the side length of the cube

	Cube();
	void read(ifstream *inFile);
	void draw();
};

#endif
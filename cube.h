#ifndef _CUBE_H
#define _CUBE_H

#include <fstream>
using namespace std;

class Cube {
	float angle; // Rotation angle (with respect to z-axis) CCW in degrees.
	float center[3]; // Coordinates [x,y,z] of the center of the object
	float radius; // Half the side length of the cube
	float height; // The side length of the cube
	float color[3];

	float length;
	float width;

public:
	Cube();
	void read(ifstream *inFile);
	void draw();
};

#endif
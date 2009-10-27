#ifndef _FLOOR_H
#define _FLOOR_H

class Floor {
	GLdouble width;
	GLdouble length;
	GLfloat color[3];
public:
	Floor();
	void draw();
};

#endif
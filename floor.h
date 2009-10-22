#ifndef _FLOOR_H
#define _FLOOR_H

class Floor {
	float width;
	float length;
	GLfloat color[3];
public:
	Floor();
	void draw();
};

#endif
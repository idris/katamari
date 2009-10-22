#ifndef _SKY_H
#define _SKY_H

#include "RGBpixmap.h"

class Sky {
	float width;
	float length;
	float hwidth;
	float hlength;
	float height;
	RGBpixmap pixmap;
	GLuint textureId;
public:
	Sky();
	void draw();
	void init();
};

#endif
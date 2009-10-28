#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

#include "RGBpixmap.h"

#include "common.h"
#include "sky.h"


using namespace std;

Sky::Sky() {
	width = length = 50000;
	hwidth = width / 2;
	hlength = length / 2;
	height = hwidth;
}

void Sky::init() {
	if (!pixmap.readBMPFile("skybox-clouds.bmp", true, true)) {
		cerr << "File skybox-clouds.bmp cannot be read or illegal format" << endl;
	}

	glGenTextures(1, &textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // store pixels by byte
	glBindTexture(GL_TEXTURE_2D, textureId);            // select current texture (0)
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(                               // initialize texture
				 GL_TEXTURE_2D,                          // texture is 2-d
				 0,                                      // resolution level 0
				 GL_RGB,                                 // internal format
				 pixmap.nCols,                         // image width
				 pixmap.nRows,                         // image height
				 0,                                      // no border
				 GL_RGB,                                 // my format
				 GL_UNSIGNED_BYTE,                       // my type
				 pixmap.pixel);                        // the pixels
}

void Sky::draw() {
	glDisable(GL_LIGHTING);
	glColor3fv(WHITE_RGB);
	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, textureId);
//	glMaterialfv(GL_BACK, GL_AMBIENT, WHITE_RGB);

	// front
	glBegin(GL_QUADS);
	glNormal3f(0.0, -1.0, 0.0);
	glTexCoord2f(0.25, 1.0);
	glVertex3d(0.0 - hwidth, hlength, 0.0);
	glTexCoord2f(0.75, 1.0);
	glVertex3d(hwidth, hlength, 0.0);
	glTexCoord2f(0.75, 0.75);
	glVertex3d(hwidth, hlength, height);
	glTexCoord2f(0.25, 0.75);
	glVertex3d(0.0 - hwidth, hlength, height);
	glEnd();

	// right
	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0.0, 0.0);
	glTexCoord2f(1.0, 0.75);
	glVertex3d(hwidth, hlength, 0.0);
	glTexCoord2f(1.0, 0.25);
	glVertex3d(hwidth, 0.0 - hlength, 0.0);
	glTexCoord2f(0.75, 0.25);
	glVertex3d(hwidth, 0.0 - hlength, height);
	glTexCoord2f(0.75, 0.75);
	glVertex3d(hwidth, hlength, height);
	glEnd();

	// left
	glBegin(GL_QUADS);
	glNormal3f(1.0, 0.0, 0.0);
	glTexCoord2f(0.0, 0.25);
	glVertex3d(0.0 - hwidth, 0.0 - hlength, 0.0);
	glTexCoord2f(0.0, 0.75);
	glVertex3d(0.0 - hwidth, hlength, 0.0);
	glTexCoord2f(0.25, 0.75);
	glVertex3d(0.0 - hwidth, hlength, height);
	glTexCoord2f(0.25, 0.25);
	glVertex3d(0.0 - hwidth, 0.0 - hlength, height);
	glEnd();

	// back
	glBegin(GL_QUADS);
	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(0.75, 0.0);
	glVertex3d(hwidth, 0.0 - hlength, 0.0);
	glTexCoord2f(0.25, 0.0);
	glVertex3d(0.0 - hwidth, 0.0 - hlength, 0.0);
	glTexCoord2f(0.25, 0.25);
	glVertex3d(0.0 - hwidth, 0.0 - hlength, height);
	glTexCoord2f(0.75, 0.25);
	glVertex3d(hwidth, 0.0 - hlength, height);
	glEnd();

	// top
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(0.25, 0.25);
	glVertex3d(0.0 - hwidth, 0.0 - hlength, height);
	glTexCoord2f(0.25, 0.75);
	glVertex3d(0.0 - hwidth, hlength, height);
	glTexCoord2f(0.75, 0.75);
	glVertex3d(hwidth, hlength, height);
	glTexCoord2f(0.75, 0.25);
	glVertex3d(hwidth, 0.0 - hlength, height);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	glEnable(GL_LIGHTING);
}
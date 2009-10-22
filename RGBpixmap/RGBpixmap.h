//----------------------------------------------------------------------
//      File:           RGBpixmap.h
//      Description:    Pixmap storage object
//      Programmer:     Dave Mount
//      Date:           April 2004
//      Source:         Adapted from the code from F. S. Hill's book,
//                      "Computer Graphics Using OpenGL", Prentice Hall,
//                      2000.
//----------------------------------------------------------------------
//      See the file ReadMe.txt for further information.
//
//      To use this with OpenGL, define the compiler variable "OGL".
//----------------------------------------------------------------------

#ifndef RGB_PIXMAP
#define RGB_PIXMAP

#include <cstdlib>                      // standard includes
#include <fstream>                      // C++ file I/O
#include <iostream>                     // C++ I/O
#include <string>                       // STL strings
#ifdef OGL                              // for OpenGL use only
 #include <GL/glut.h>                   // glut/OpenGL includes
#endif

//----------------------------------------------------------------------
//  Types and constants
//----------------------------------------------------------------------
typedef unsigned char   RGB_uchar;      // unsigned type aliases
typedef unsigned short  RGB_ushort;
typedef unsigned long   RGB_ulong;

const int RGB_MAX = 255;                // maximum RGB value

using namespace std;                    // make std:: accessible

//----------------------------------------------------------------------
//  RGBpixel
//      Stores a single RGB value
//----------------------------------------------------------------------

class RGBpixel {
public:
    RGB_uchar   r, g, b;
//
    RGBpixel() { r = g = b = 0;  }              // default constructor

    RGBpixel(const RGBpixel& p)                 // copy constructor
    { r = p.r; g = p.g; b = p.b;  }
                                                // construct from components
    RGBpixel(RGB_uchar rr, RGB_uchar gg, RGB_uchar bb)
    { r = rr; g = gg; b = bb; }
                                                // set values
    void set(RGB_uchar rr, RGB_uchar gg, RGB_uchar bb)
    { r = rr; g = gg; b = bb; }
};

//----------------------------------------------------------------------
//  RGBpixmap
//      RGB pixel array
//----------------------------------------------------------------------

class RGBpixmap {
protected:
                                                // error function
    void RGBerror(const string& msg, bool fatal) const;
    RGB_ulong round2Power2(RGB_ulong n);        // round up to power of 2
    RGB_ushort getShort();                      // read a short int
    RGB_ulong getLong();                        // read a long int
    void putShort(const RGB_ushort ip) const;   // write a short int
    void putLong(const RGB_ulong ip) const;     // write a long int
public:
    RGBpixel*   pixel;                          // array of pixels
    int         nRows, nCols;                   // dimensions
    ifstream*   bmpIn;                          // input file
    ofstream*   bmpOut;                         // output file
//
    RGBpixmap()                                 // default constructor
    {  nRows = nCols = 0;  pixel = NULL;  bmpIn = NULL; bmpOut = NULL; }

    RGBpixmap(int rows, int cols)               // constructor
    {
        nRows = rows;  nCols = cols;
        pixel = new RGBpixel[rows*cols];
        bmpIn = NULL;  bmpOut = NULL;
    }

    void freeIt();                              // deallocate everything
    // ~RGBpixmap() { freeIt(); }               // destructor (disabled)

#ifdef OGL                                      // for OpenGL only
    void draw() const;                          // draw pixmap to raster pos
#endif
                                                // set pixel color
    void setPixel(int col, int row, const RGBpixel& color)
    {
        if (col >= 0 && col < nCols && row >= 0 && row < nRows)
            pixel[col + nCols*row] = color;
    }

    RGBpixel getPixel(int col, int row)         // return pixel color
        { return pixel[col  + nCols*row]; }

                                                // read from file
    bool readBMPFile(const string& fname, bool glPad=false,
                    bool verbose=false);
                                                // write to file
    bool writeBMPFile(const string& fname);
};

#endif

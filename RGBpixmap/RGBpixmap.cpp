//----------------------------------------------------------------------
//      File:           RGBpixmap.cpp
//      Description:    Pixmap storage object
//      Programmer:     Dave Mount
//      Date:           April 2004
//      Source:         Adapted from the code from F. S. Hill's book,
//                      "Computer Graphics Using OpenGL", Prentice Hall,
//                      2000.
//
//      See the file RGBpixmap.h for information.
//
//      Beware: This procedure has not been carefully tested.  Use at
//      your own risk.  Report errors to Dave Mount (mount@cs.umd.edu).
//----------------------------------------------------------------------

#include "RGBpixmap.h"                   // pixel map definitions

//----------------------------------------------------------------------
//  BMP header structure
//      A .bmp file has a header and info section.  These are combined
//      in the following structure.  I'm not sure what all the values
//      mean, so the comments indicate by best guess.  The header info
//      structure below is restricted to work with 24-bit, uncompressed
//      images.
//
//      After this header comes numRows, each containing numCols of RGB
//      pixel byte values (output in the order BGR).  Each row is padded
//      to the next higher multiple of 4 bytes.
//
//      TODO: For symmetry, perhaps readBMP should use this same
//      structure.
//----------------------------------------------------------------------
typedef struct {                // bmp header (and info) structure
                                // header section
    char            id[2];              // must be "BM"
    RGB_ulong       fileSize;           // file size in bytes
    RGB_ushort      reserved[2];        // reserved (always 0)
    RGB_ulong       totHeadSize;        // total head size (assume 54L=0x36)
                                // info section
    RGB_ulong       headerSize;         // header size (assume 40L=0x28)
    RGB_ulong       numCols;            // number of columns in image
    RGB_ulong       numRows;            // number of rows in image
    RGB_ushort      planes;             // number of planes (assume 1)
    RGB_ushort      bitsPerPixel;       // bits per pixel (assume 24)
    RGB_ulong       compression;        // compression (assume 0L=none)
    RGB_ulong       imageSize;          // total pixels in image
    RGB_ulong       xPelsPerMeter;      // x pixels/meter (assume 2925L=0x0b6d)
    RGB_ulong       yPelsPerMeter;      // y pixels/meter (assume 2925L=0x0b6d)
    RGB_ulong       numLUTentry;        // number of LUT used (assume 0L)
    RGB_ulong       impColors;          // number of LUT important (assume 0L)
} BMPHead;

//----------------------------------------------------------------------
//  Error utility
//----------------------------------------------------------------------
void RGBpixmap::RGBerror(const string& msg, bool fatal) const
{
    if (fatal) {
        std::cerr << "Error: " << msg << std::endl;
										// close files
        if (bmpIn != NULL) bmpIn->close();
        if (bmpOut != NULL) bmpOut->close();
    }
    else {
        std::cerr << "Warning: " << msg << std::endl;
    }
}

//----------------------------------------------------------------------
//  Deallocation
//  TODO: There was a mysterious problem with this function, and so
//  it has been not been called in the destructor.  I need to track
//  this down.
//----------------------------------------------------------------------
void RGBpixmap::freeIt() {                      // deallocate everything
    delete [] pixel;
    nRows = nCols = 0;
    if (bmpIn != NULL) {
        bmpIn->close();
        delete bmpIn;
    }
}

//----------------------------------------------------------------------
//  round2Power2
//      Rounds the argument up to the next higher power of 2.  The
//      method is to divide n-1 by 2 until 0.  If it takes k divides to
//      get to zero, then the result returned in 2^k.
//----------------------------------------------------------------------
RGB_ulong RGBpixmap::round2Power2(RGB_ulong n)
{
    RGB_ulong res = 1;
    n -= 1;
    while (n > 0) {
        n >>= 1;  res <<= 1;
    }
    return res;
}

//----------------------------------------------------------------------
//  getShort, getLong
//      Read short (long) from file.  Note that BMP format stores values
//      in little-endian form (lowest byte first).
//----------------------------------------------------------------------
RGB_ushort RGBpixmap::getShort()
{
    char c;
    RGB_ushort ip;
    bmpIn->get(c);  ip = RGB_ushort(c);         // store lower order byte
    bmpIn->get(c);  ip |= RGB_ushort(c) << 8;   // store higher order byte
    return ip;
}

RGB_ulong RGBpixmap::getLong()
{
    char c;
    RGB_ulong ip;
    bmpIn->get(c);  ip = RGB_uchar(c);          // store lower order byte
    bmpIn->get(c);  ip |= RGB_uchar(c) << 8;
    bmpIn->get(c);  ip |= RGB_uchar(c) << 16;
    bmpIn->get(c);  ip |= RGB_uchar(c) << 24;   // store highest order byte
    return ip;
}

//----------------------------------------------------------------------
//  readBMPfile
//      Reads a .bmp file and stores in RGBpixmap.  Since OpenGL
//      requires that pixel arrays be a power of 2 in size, there is
//      an option which pads the size of the array up to the next higher
//      power of 2.
//----------------------------------------------------------------------
bool RGBpixmap::readBMPFile(const string& fname, bool glPad, bool verbose)
{
    BMPHead h;                                  // file header
                                                // open file for reading
    bmpIn = new ifstream(fname.c_str(), ios::in|ios::binary);
    if (bmpIn == NULL || !(*bmpIn)) {
        std::cerr << "File: " << fname << std::endl;
        RGBerror("Cannot open file", true);
        return false;
    }
    else if (verbose) {
        std::cerr << "Opening bmp file " << fname << std::endl;
    }
    bmpIn->get(h.id[0]);                        // read .bmp magic number
    bmpIn->get(h.id[1]);
    if (h.id[0] != 'B' || h.id[1] != 'M') {
        RGBerror("Illegal magic number.  May not be a .bmp file", true);
    return false;
    }
    h.fileSize          = getLong();    // read file size
    h.reserved[0]       = getShort();   // (ignore)
    h.reserved[1]       = getShort();   // (ignore)
    h.totHeadSize       = getLong();    // offset to image (unreliable)
    h.headerSize        = getLong();    // always 40
    h.numCols           = getLong();    // number of columns in image
    h.numRows           = getLong();    // number of rows in image
    h.planes            = getShort();   // number of planes (1)
    h.bitsPerPixel      = getShort();   // (assume 24)
    if (h.bitsPerPixel != 24) {         // check bits per pixel
        RGBerror("We only support uncompressed 24-bit/pixel images", true);
        return false;
    }
    h.compression       = getLong();    // (assume 0)
    h.imageSize         = getLong();    // total bytes in image
    h.xPelsPerMeter     = getLong();    // (ignored)
    h.yPelsPerMeter     = getLong();    // (ignored)
    h.numLUTentry       = getLong();    // (ignored)
    h.impColors         = getLong();    // (ignored)
    //------------------------------------------------------------------
    //  Note: Compiler may complain about many unused variables.
    //------------------------------------------------------------------

    int nBytesInRow = ((3 * h.numCols + 3)/4) * 4;  // round to mult of 4
    int numPadBytes = nBytesInRow - 3 * h.numCols;  // unused bytes in row

    if (glPad) {                                // pad to power of 2?
        nRows = round2Power2(h.numRows);        // set class's members
        nCols = round2Power2(h.numCols);
    }
    else {
        nRows = h.numRows;                      // set class's members
        nCols = h.numCols;
    }
    if (verbose) {
        std::cerr << "BMP file information: \n"
            << "  file-name: "  << fname << "\n"
            << "  file-size: "  << h.fileSize << "\n"
            << "  columns: "    << h.numCols << "\n"
            << "  rows: "       << h.numRows << "\n"
            << "  bits/pixel: " << h.bitsPerPixel << "\n";
        if (glPad) {
            std::cerr << "  Rounding size to " << nRows
                      << " x " << nCols << "\n";
        }
    }

    pixel = new RGBpixel[nRows * nCols];        // allocate array
    if (!pixel) {                               // cannot allocate
        RGBerror("Cannot allocate storage for image array", true);
        return false;
    }
                                                // read pixel values
    for (RGB_ulong row = 0; row < h.numRows; row++) {
        RGB_ulong index = row * nCols;
        for (RGB_ulong col = 0; col < h.numCols; col++) {
            char r, g, b;
            bmpIn->get(b);                      // read RGB (reversed)
            bmpIn->get(g);
            bmpIn->get(r);
                                                // store in array 
            pixel[index++].set(RGB_uchar(r), RGB_uchar(g), RGB_uchar(b));
        }
        for (int k = 0; k < numPadBytes; k++) {
            char pad;
            bmpIn->get(pad);                    // skip padding bytes
        }
    }
    bmpIn->close();                             // close the file
    return true;                                // return good status
}

//----------------------------------------------------------------------
//  putShort, putLong
//      Write short (2 bytes) or long (4 bytes) from file.  Note that BMP
//      format stores values in little-endian form (lowest byte first).
//----------------------------------------------------------------------

void RGBpixmap::putShort(const RGB_ushort ip) const
{
    bmpOut->put(char(ip & 0x00FF));             // output lowest to highest
    bmpOut->put(char((ip >> 8) & 0x00FF));
}

void RGBpixmap::putLong(const RGB_ulong ip) const
{
    bmpOut->put(char(ip & 0x00FF));             // output lowest to highest
    bmpOut->put(char((ip >> 8) & 0x00FF));
    bmpOut->put(char((ip >> 16) & 0x00FF));
    bmpOut->put(char((ip >> 24) & 0x00FF));
}

//----------------------------------------------------------------------
//  writeBMPfile
//      Writes pixmap as a .bmp file.  Returns false if an error
//      occured.
//
//      We set up a header structure, output it, and then output the
//      pixels.  Note that we do not simply output the header structure,
//      because on some systems bytes may be output in different orders.
//----------------------------------------------------------------------

bool RGBpixmap::writeBMPFile(const string& fname) {
                                                // open file for writing
    bmpOut = new ofstream(fname.c_str(), ios::out|ios::binary);
    if (bmpOut == NULL || !(*bmpOut)) {         // cannot open?
        std::cerr << "File: " << fname << endl;
        RGBerror("Cannot open file for writing", true);
        return false;
    }

    int bytesPerLine = ((3*nCols + 3)/4)*4;     // round nCols to mult of 4
    int numPadBytes = bytesPerLine - 3*nCols;   // unused bytes per row

    BMPHead h;                                  // file header
    h.id[0] = 'B';  h.id[1] = 'M';              // magic number ("BM")
    h.reserved[0] = h.reserved[1] = 0;          // reserved entries (0)
    h.totHeadSize       = 54L;                  // total header+info size
    h.headerSize        = 40L;                  // just header size
                                                // total file size
    h.fileSize          = h.totHeadSize + (long) bytesPerLine*nRows;
    h.numRows           = nRows;                // number of rows and cols
    h.numCols           = nCols;
    h.planes            = 1;                    // number of planes (1)
    h.bitsPerPixel      = 24;                   // bits per pixel (24)
    h.compression       = 0L;                   // compression (0)
    h.imageSize         = (long) 3*nCols*nRows; // total pixels in image
    h.xPelsPerMeter     = 2925L;                // x pixels/meter (2925L)
    h.yPelsPerMeter     = 2925L;                // y pixels/meter (2925L)
    h.numLUTentry       = 0L;                   // LUT used (0L)
    h.impColors         = 0L;                   // LUT important (0L)

    bmpOut->put(h.id[0]);   bmpOut->put(h.id[1]);       // output magic number
    putLong(h.fileSize);                        // output rest of header
    putShort(h.reserved[0]);
    putShort(h.reserved[1]);
    putLong(h.totHeadSize);
    putLong(h.headerSize);
    putLong(h.numCols);
    putLong(h.numRows);
    putShort(h.planes);
    putShort(h.bitsPerPixel);
    putLong(h.compression);
    putLong(h.imageSize);
    putLong(h.xPelsPerMeter);
    putLong(h.yPelsPerMeter);
    putLong(h.numLUTentry);
    putLong(h.impColors);

    for (int row = 0; row < nRows; row++) {     // write pixel values
        int index = row * nCols;
        for (int col = 0; col < nCols; col++) {
            RGBpixel &p = pixel[index++];       // the pixel
            bmpOut->put(p.b);                   // write RGB
            bmpOut->put(p.g);
            bmpOut->put(p.r);
        }
        for (int k = 0; k < numPadBytes; k++) { // output padding bytes
            bmpOut->put(char(0));
        }
    }
    bmpOut->close();                            // close the file
    return true;                                // return good status
}

//----------------------------------------------------------------------
//  draw pixmap to current raster position (OpenGL)
//----------------------------------------------------------------------
#ifdef OGL                                      // for OpenGL only
void draw() const                               // draw pixmap to raster pos
{
    if (nRows == 0 || nCols == 0) return;       // nothing there
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);      // align to single byte
    glDrawPixels(nCols, nRows, GL_RGB, GL_UNSIGNED_BYTE, pixel);
}
#endif

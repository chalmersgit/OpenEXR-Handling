/*
Author: Andrew Chalmers

Basic I/O for OpenEXR images. 

TODO:
  - Tiled images
  - Cropped images
  - Deep scanline I/O
  - Multithreaded I/O
  - Currently assumes alpha

Resources:
 - http://www.openexr.com/TechnicalIntroduction.pdf
 - http://www.openexr.com/ReadingAndWritingImageFiles.pdf
 - https://github.com/openexr/openexr/blob/master/OpenEXR/IlmImfExamples/generalInterfaceExamples.cpp
*/

#include "imageio.h"

// EXR
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <half.h>
#include <ImfArray.h>

using namespace Imath;

imageio::imageio(void) {

}

imageio::~imageio(void) {

}

/*
 * Helper functions
 */
void imageio::insert(FrameBuffer& frameBuffer, char const* name, const PixelType type, char* base, const int xStride, const int yStride){
  frameBuffer.insert(name, Slice(type, base, xStride, yStride, 1, 1, 0.0));
}

// help for writing
void imageio::setHeader(Header &header, const PixelType type){
    header.channels().insert ("R", Channel (type));
    header.channels().insert ("G", Channel (type));
    header.channels().insert ("B", Channel (type));
    header.channels().insert ("A", Channel (type));
}

void imageio::writeData(OutputFile &oFile, FrameBuffer &frameBuffer, const int height){
    oFile.setFrameBuffer(frameBuffer);
    oFile.writePixels (height);
}

// Help for reading
void imageio::setup_readFile(InputFile &iFile, Box2i &dw, int &width, int &height, bool &hasAlpha){
  dw = iFile.header().dataWindow();
  width = dw.max.x - dw.min.x + 1;
  height = dw.max.y - dw.min.y + 1;
  hasAlpha = true;       
}

void imageio::close_readFile(InputFile &iFile, FrameBuffer &frameBuffer, Box2i &dw){
  iFile.setFrameBuffer(frameBuffer);
  iFile.readPixels(dw.min.y, dw.max.y);
}

/*
 * ------------------ end Helper functions
 */

/*
 * Writing EXR Image
 */
void imageio::WriteEXR_Rgba(
  const char *fileName, 
  const Rgba *pixels, 
  const int width, 
  const int height){
  try {
    RgbaOutputFile file (fileName, width, height, WRITE_RGBA);
    file.setFrameBuffer (pixels, 1, width);
    file.writePixels (height);
  } catch (const std::exception &e) {
    fprintf(stderr, "Unable to write image file \"%s\": %s", fileName, e.what());
  }
}

void imageio::writeEXR_float(
  const char *fileName, 
  const float *frgba, 
  const int width, const int height){
  try {
    Header header(width, height);
    setHeader(header, FLOAT);

    OutputFile file (fileName, header);
    FrameBuffer frameBuffer;

    const int yStride4f = width * xStride4f;

    insert(frameBuffer,"R",FLOAT,(char *)frgba,           xStride4f, yStride4f);
    insert(frameBuffer,"G",FLOAT,(char *)frgba+offsetf,   xStride4f, yStride4f);
    insert(frameBuffer,"B",FLOAT,(char *)frgba+2*offsetf, xStride4f, yStride4f);
    insert(frameBuffer,"A",FLOAT,(char *)frgba+3*offsetf, xStride4f, yStride4f);

    writeData(file, frameBuffer, height);
  } catch (const std::exception &e) {
    fprintf(stderr, "Unable to write image file \"%s\": %s", fileName, e.what());
  }
}

void imageio::writeEXR_float(
  const char *fileName, 
  const float *rPixels, 
  const float *gPixels, 
  const float *bPixels, 
  const float *aPixels, 
  const int width, 
  const int height){
  try {
    Header header (width, height);
    setHeader(header, FLOAT);

    OutputFile file (fileName, header);
    FrameBuffer frameBuffer;

    const int yStride1f = width * xStride1f;

    insert(frameBuffer,"R",FLOAT,(char *)rPixels, xStride1f, yStride1f);
    insert(frameBuffer,"G",FLOAT,(char *)gPixels, xStride1f, yStride1f);
    insert(frameBuffer,"B",FLOAT,(char *)bPixels, xStride1f, yStride1f);
    insert(frameBuffer,"A",FLOAT,(char *)aPixels, xStride1f, yStride1f);

    writeData(file, frameBuffer, height);
  } catch (const std::exception &e) {
    fprintf(stderr, "Unable to write image file \"%s\": %s", fileName, e.what());
  }
}

void imageio::writeEXR_half(
  const char *fileName, 
  const half *rgba, 
  const int width, 
  const int height){
  try {
    Header header (width, height);
    setHeader(header, HALF);

    OutputFile file (fileName, header);
    FrameBuffer frameBuffer;

    const int yStride4h = width * xStride4h;
    
    insert(frameBuffer,"R",HALF,(char *)rgba,           xStride4h, yStride4h);
    insert(frameBuffer,"G",HALF,(char *)rgba+offseth,   xStride4h, yStride4h);
    insert(frameBuffer,"B",HALF,(char *)rgba+2*offseth, xStride4h, yStride4h);
    insert(frameBuffer,"A",HALF,(char *)rgba+3*offseth, xStride4h, yStride4h);

    writeData(file, frameBuffer, height);
  } catch (const std::exception &e) {
    fprintf(stderr, "Unable to write image file \"%s\": %s", fileName, e.what());
  }
}

// EXR doesn't support double (?). 
// But if you were handling doubles, this will convert and output as float for you.
void imageio::writeEXR_double(
    const char *fileName, 
    const double *rPixelsD, 
    const double *gPixelsD, 
    const double *bPixelsD, 
    const int width, 
    const int height){
  try {
    float* rPixels = new float[width*height];
    float* gPixels = new float[width*height];
    float* bPixels = new float[width*height];
    float* aPixels = new float[width*height];

    for(int i = 0; i < width*height; ++i){
      rPixels[i] = (float)rPixels[i];
      gPixels[i] = (float)gPixels[i];
      bPixels[i] = (float)bPixels[i];
      aPixels[i] = 1.0f;
    }

    writeEXR_float(fileName, rPixels, gPixels, bPixels, aPixels, width, height);

    delete[] rPixels;
    delete[] gPixels;
    delete[] bPixels;
    delete[] aPixels;
  } catch (const std::exception &e) {
    fprintf(stderr, "Unable to write image file \"%s\": %s", fileName, e.what());
  }
}

/*
 * ------------------ end Writing EXR Image
 */

/*
 * Reading EXR Image
 */
void imageio::ReadEXR_Array2D(const char *fileName, Array2D<Rgba> &pixels, int &width, int &height){
  try {
    RgbaInputFile file (fileName);
    Box2i dw = file.dataWindow();
    width = dw.max.x - dw.min.x + 1;
    height = dw.max.y - dw.min.y + 1;
    pixels.resizeErase (height, width);
    file.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels (dw.min.y, dw.max.y);
  } catch (const std::exception &e) {
    fprintf(stderr, "Unable to read image file \"%s\": %s", fileName, e.what());
  }
}

void imageio::ReadEXR_float(const char *fileName, float *&rgba, int &width, int &height, bool &hasAlpha){
  try {
    InputFile file(fileName);
    Box2i dw;
    setup_readFile(file, dw, width, height, hasAlpha);
    
    FrameBuffer frameBuffer;
    rgba = new float[channels4 * width * height];
    const int yStride4f = width * xStride4f;
    
    insert(frameBuffer,"R",FLOAT,(char *)rgba,          xStride4f, yStride4f);
    insert(frameBuffer,"G",FLOAT,(char *)rgba+offsetf,  xStride4f, yStride4f);
    insert(frameBuffer,"B",FLOAT,(char *)rgba+2*offsetf,xStride4f, yStride4f);
    insert(frameBuffer,"A",FLOAT,(char *)rgba+3*offsetf,xStride4f, yStride4f);

    close_readFile(file, frameBuffer, dw);
  } catch (const std::exception &e) {
    fprintf(stderr, "Unable to read image file \"%s\": %s", fileName, e.what());
  }
}

void imageio::ReadEXR_float(const char *fileName, float *&rPixels, float *&gPixels, float *&bPixels, float *&aPixels, int &width, int &height, bool &hasAlpha){
  try {
    InputFile file(fileName);
    Box2i dw;
    setup_readFile(file, dw, width, height, hasAlpha);
    
    FrameBuffer frameBuffer;

    rPixels = new float[width * height];
    gPixels = new float[width * height];
    bPixels = new float[width * height];
    aPixels = new float[width * height];
    
    const int yStride1f = width * xStride1f;
    insert(frameBuffer,"R",FLOAT,(char *)rPixels, xStride1f, yStride1f);
    insert(frameBuffer,"G",FLOAT,(char *)gPixels, xStride1f, yStride1f);
    insert(frameBuffer,"B",FLOAT,(char *)bPixels, xStride1f, yStride1f);
    insert(frameBuffer,"A",FLOAT,(char *)aPixels, xStride1f, yStride1f);                                 
    
    close_readFile(file, frameBuffer, dw);
  } catch (const std::exception &e) {
      fprintf(stderr, "Unable to read image file \"%s\": %s", fileName, e.what());
  }
}

void imageio::ReadEXR_half(const char *fileName, half *&hrgba, int &width, int &height, bool &hasAlpha){
  try {
    InputFile file(fileName);
    Box2i dw;
    setup_readFile(file, dw, width, height, hasAlpha);
    
    FrameBuffer frameBuffer;
    hrgba = new half[channels4 * width * height];
    
    const int yStride4h = width * xStride4h;    
    
    insert(frameBuffer,"R",HALF,(char *)hrgba,           xStride4h, yStride4h);
    insert(frameBuffer,"G",HALF,(char *)hrgba+offseth,   xStride4h, yStride4h);
    insert(frameBuffer,"B",HALF,(char *)hrgba+2*offseth, xStride4h, yStride4h);
    insert(frameBuffer,"A",HALF,(char *)hrgba+3*offseth, xStride4h, yStride4h);

    close_readFile(file, frameBuffer, dw);
  } catch (const std::exception &e) {
    fprintf(stderr, "Unable to read image file \"%s\": %s", fileName, e.what());
  }
}

/*
 * ------------------ end Reading EXR Image
 */

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


#pragma once

// OpenEXR
#include <ImfOutputFile.h>
#include <ImfInputFile.h>
#include <ImfRgbaFile.h>

using namespace Imf;

class imageio{
private:
  static void insert(FrameBuffer& frameBuffer, char const* name, const PixelType type, char* base, const int xStride, const int yStride);
  
  static void setup_readFile(InputFile &iFile, Imath::Box2i &dw, int &width, int &height, bool &hasAlpha);
  static void close_readFile(InputFile &iFile, FrameBuffer &frameBuffer, Imath::Box2i &dw);

  static const int offsetf = sizeof(float);
  static const int offseth = sizeof(half);

  static const int channels4 = 4;
  static const int xStride4f = channels4 * offsetf;
  static const int xStride4h = channels4 * offseth;

  static const int channels1 = 1;
  static const int xStride1f = channels1 * offsetf;
  static const int xStride1h = channels1 * offseth;

public:
  imageio(void);
  ~imageio(void);

  static void WriteEXR_Rgba(const char *fileName, const Rgba *pixels, const int width, const int height);
  static void writeEXR_float(const char *fileName, const float *frgba, const int width, const int height);
  static void writeEXR_float(const char *fileName, const float *rPixels, const float *gPixels, const float *bPixels, const float *aPixels, const int width, const int height);
  static void writeEXR_half(const char *fileName, const half *rgba, const int width, const int height);
  static void writeEXR_double(const char *fileName, const double *rPixels, const double *gPixels, const double *bPixels, const int width, const int height);

  static void ReadEXR_Array2D(const char *fileName, Array2D<Rgba> &pixels, int &width, int &height);
  static void ReadEXR_float(const char *fileName, float *&rgba, int &width, int &height, bool &hasAlpha);
  static void ReadEXR_float(const char *fileName, float *&rPixels, float *&gPixels, float *&bPixels, float *&aPixels, int &width, int &height, bool &hasAlpha);
  static void ReadEXR_half(const char *fileName, half *&hrgba, int &width, int &height, bool &hasAlpha);
};





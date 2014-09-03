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


// Standard
#include <stdio.h>
#include <stdlib.h>

// EXR
#include <ImfArray.h>

// My includes
#include "imageio.h"

// Namespaces
using namespace std;

void doExample(const char *fileName){
	// Setup
	int width;
	int height;
	bool hasAlpha;

	Array2D<Rgba> pixels;

	float* rPixels = new float[width*height];
	float* gPixels = new float[width*height];
	float* bPixels = new float[width*height];
	float* aPixels = new float[width*height];

	float* fPixels;

	half* hPixels;

	// Read
	cout << "Reading " << fileName << endl;
	imageio::ReadEXR_Array2D(fileName, pixels, width, height);
	imageio::ReadEXR_float(fileName, fPixels, width, height, hasAlpha);
	imageio::ReadEXR_float(fileName, rPixels, gPixels, bPixels, aPixels, width, height, hasAlpha);
	imageio::ReadEXR_half(fileName, hPixels, width, height, hasAlpha);

	// Write
	cout <<  "Writing" << endl;	
	imageio::WriteEXR_Rgba("original_rgba.exr", pixels[0], width, height);
	imageio::writeEXR_float("original_float1.exr", fPixels, width, height);
	imageio::writeEXR_float("original_float2.exr", rPixels, gPixels, bPixels, aPixels, width, height);
	imageio::writeEXR_half("original_half.exr", hPixels, width, height);
	
	cout << "Complete" << endl;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage:\n");
    printf("./exrExample [filename.exr]\n");
    exit(EXIT_FAILURE);
  }

  doExample(argv[1]);

  return EXIT_SUCCESS;
}



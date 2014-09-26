// Routines for creating mask images given a resolution map.
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin

#ifndef IMAGE_H
#define IMAGE_H

#include <vector>

namespace SVIS
{

// Grayscale image -- user is responsible for allocating pixels
struct Image
{
    unsigned width;
    unsigned height;
    // When accessing pixels in the image, coordinates are always
    // shifted 'scale' number of bits to the right.
    unsigned scale;
    unsigned char *pixels;
};

// Grayscale image -- pixels are deallocted upon destruction
struct AutoImage
{
    unsigned width;
    unsigned height;
    // When accessing pixels in the image, coordinates are always
    // shifted 'scale' number of bits to the right.
    unsigned scale;
    std::vector<unsigned char> pixels;
};

} // namespace SVIS

#endif // IMAGE_H

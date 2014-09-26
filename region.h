// Manipulate rectangular regions.
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Thu Aug 17 19:31:24 CDT 2006

#ifndef REGION_H
#define REGION_H

#include "image.h"

namespace SVIS
{

// A rectangular region
struct Region
{
    unsigned x1, y1; // Top-left
    unsigned x2, y2; // Bottom-right, non-inclusive
};

// Find non-zero regions in an 8 bit image.
//
// Return 0 if no region can be found, 1 otherwise.
//
// Starting at pixel 'start_x', 'start_y', divide the image into 'width' by
// 'height' blocks.
//
// Starting at the top left block, work left to right, top to bottom, until
// you find the first block that contains a non-zero value.  The top-left
// pixel in this block marks the beginning of the region.  The end of
// the region is found when the next all-zero block on this row is
// encountered.  If all of the remaining blocks on this row are non-zero,
// then the end of the region is the end of the row.
//
// The returned region will never more than 'height' pixels tall.
//
// The returned region can be less than 'height' pixels tall if it
// is adjacent to the bottom edge of the image.
//
// The returned region will be a multiple of 'width' pixels wide,
// unless it is adjacent to the right edge of the image.
//
// To find all the regions in an image, call this routine consecutively
// until it can't find any more, like this:
//
//     struct Image i;
//     struct Region r;
//     int sx, sy, w, h;
//
//     sx = sy = 0;
//
//     ... setup the image, i
//     ... set w and h
//
//     while (FindNonzero (&i, sx, sy, w, h, &r))
//     {
//         ... set sx, and sy to continue searching just
//         ... past the region we found
//         sx = r.x2;
//         sy = r.y1;
//     }
bool FindNonzero (const AutoImage &image,
    unsigned start_x,
    unsigned start_y,
    unsigned width,
    unsigned height,
    Region *region);

// Count the non-zero regions in an 8 bit image.
// Return the number of regions found.
unsigned TotalNonzero (const AutoImage &image,
    unsigned start_x,
    unsigned start_y,
    unsigned width,
    unsigned height);

} // namespace SVI

#endif // REGION_H

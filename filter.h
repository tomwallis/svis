// Filter, reduce, expand, and blend routines
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#ifndef FILTER_H
#define FILTER_H

#include "image.h"

namespace SVIS
{

struct Rect
{
    int x1, y1;
    int x2, y2; // Non-inclusive
};

void Reduce2x2 (const Image *src, Image *dest);
void Reduce3x3 (const Image *src, Image *dest);

// Use ExpandEven on an image that was reduced by an even-tap filter.
void ExpandEven (const Image *src, Image *dest);

// Use ExpandOdd on an image that was reduced by an odd-tap filter.
void ExpandOdd (const Image *src, Image *dest);

// Blend src and dest together and store result in dest.
// Only blend over the src rect region if one is specified.
// mask_offset_x and _y specify where the mask's top left
// pixel is relative to the src's top left pixel.
void Blend (const Image *src,
    Image *dest,
    const AutoImage *mask,
    const Rect *rect,
    int mask_offset_x,
    int mask_offset_y);

} // namespace SVIS

#endif // FILTER_H

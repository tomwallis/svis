// Routines for creating mask images given a resolution map.
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue Aug 15 18:28:05 CDT 2006

#ifndef MASK_H
#define MASK_H

#include <cmath>
#include "image.h"

namespace SVIS
{

// Compute sigma for the given level
inline double Sigma (unsigned level)
{
    return 0.248 / (1 << level);
}

// Compute critical frequency for the given level
inline double CriticalFrequency (unsigned level, double criterion)
{
    return Sigma (level) * sqrt (2.0) * sqrt (-log (criterion));
}

// Create a resolution map.
void CreateResmap (unsigned width,
    unsigned height,
    std::vector<unsigned char> &pixels,
    double halfres,
    double resmap_fov_deg);

// Create a mask given a resolution map and level.
void CreateMask (AutoImage &mask,
    const AutoImage &resmap,
    unsigned level);

// Determine which pixels may be eliminated from the mask
void GetCropParams (unsigned width,
    unsigned height,
    const std::vector<unsigned char> &pixels,
    unsigned &crop_x, unsigned &crop_y,
    unsigned &new_width, unsigned &new_height);

// Remove the edges of a mask that are zero-- keeping it centered.
void CropMask (AutoImage &mask, unsigned crop_x, unsigned crop_y, unsigned crop_width, unsigned crop_height);

} // namespace SVIS

#endif // MASK_H

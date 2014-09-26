// Calculations involving pixel/eccentricity/resolution/arc conversions.
//
// Copyright (C) 2003-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/06/25

#ifndef ECC_H
#define ECC_H

namespace SVIS
{

// Get field of view in degrees given a distance of an object and
// its width
double FieldOfView (double distance, double width);

// Given an arbitrary criterion in the range (0.0, 1.0), calculate the resolution
// in cycles per pixel at pixel (x, y).  The fovea is at pixel (0, 0).
//
// The criterion, 'c', represents an output contrast and lies in the range (0.0, 1.0).
//
// 'frequency_decay_constant' and 'half_resolution' units are in pixels.
double Resolution (int x,
    int y,
    double c,
    double frequency_decay_constant,
    double half_resolution);

// Get the appropriate value for distance to be used in reverse foveation.
double ReverseD (double c,
    double frequency_decay_constant,
    double half_resolution,
    double display_diameter);

// Get the reverse resolution, making the visual field's resolution uniform.
//
// 'd' is the value returned from ReverseD().
double ReverseResolution (int x,
    int y,
    double c,
    double frequency_decay_constant,
    double half_resolution,
    double d);

} // namespace SVIS

#endif // ECC_H

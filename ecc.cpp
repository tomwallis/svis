// Eccentricity calulations
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include "ecc.h"
#include <cmath>

namespace SVIS
{

double FieldOfView (double distance,
    double width)
{
    const double PI = 2 * asin (1.0);
    return 2 * (180.0 / PI) * atan ((width / 2) / distance);
}

double Resolution (int x,
    int y,
    double c,
    double frequency_decay_constant,
    double half_resolution)
{
    double eccentricity = sqrt (static_cast<double> (x * x + y * y));
    return -log (c) / frequency_decay_constant * half_resolution / (half_resolution + eccentricity);
}

double ReverseD (double c,
    double frequency_decay_constant,
    double half_resolution,
    double display_width_pix)
{
    return display_width_pix - (2 * half_resolution * log (c)) / frequency_decay_constant;
}

double ReverseResolution (int x,
    int y,
    double c,
    double frequency_decay_constant,
    double half_resolution,
    double d)
{
    double eccentricity = sqrt (static_cast<double> (x * x + y * y));
    if (eccentricity >= d)
        eccentricity = d - 1;
    return (half_resolution / (frequency_decay_constant * (eccentricity - d))) * log (c);
}

} // namespace SVIS

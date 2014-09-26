// Benchmark eccentricity routines
//
// Copyright (C) 2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue Aug 15 09:33:20 CDT 2006

#include <ctime>
#include "ecc.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace SVIS;

void benchmark1 ()
{
    // Setup fovea params
    const unsigned distance_cm = 45;
    const unsigned width_cm = 40;
    const unsigned width_pixels = 640;
    // Convert to pixels
    const double FREQUENCY_DECAY_CONSTANT_PIXELS =
        0.1 * width_pixels / FieldOfView (distance_cm, width_cm);
    const double HALF_RESOLUTION_PIXELS =
        2.3 * width_pixels / FieldOfView (distance_cm, width_cm);
    double cpp;

    // Time it
    size_t count = 0;
    time_t t = clock ();
    while (static_cast<double> (clock () - t) / CLOCKS_PER_SEC < 1.0)
    {
        double c = 0.3;
        int x = 0;
        int y = 0;

        cpp = Resolution (x, y, c, FREQUENCY_DECAY_CONSTANT_PIXELS, HALF_RESOLUTION_PIXELS);

        ++count;
    }
    cout << count << "Hz" << endl;
    cpp = 0.0; // Get rid of unused variable warnings
}

int main (int argc, char *argv[])
{
    try
    {
        benchmark1 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

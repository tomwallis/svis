// Test eccentricity routines
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include "ecc.h"
#include <iostream>
#include "verify.h"

using namespace SVIS;
using namespace std;

void test1 ()
{
    VERIFY (FieldOfView (1, 1) < 54);
    VERIFY (FieldOfView (1, 1) > 53);

    double half_resolution_pixels = 2.3 * 640 / FieldOfView (45, 40);
    double frequency_decay_constant_pixels = 0.1 * 640 / FieldOfView (45, 40);
    double cpp;
    double c = 0.3;
    int x = 0;
    int y = 0;

    cpp = Resolution (x, y, c, frequency_decay_constant_pixels, half_resolution_pixels);
    // For c = 0.300, resolution at (0, 0) = 0.902 cycles/pixel
    VERIFY (static_cast<int> (cpp * 100 + 0.5) == 90);

    x = 100;
    y = 100;
    cpp = Resolution (x, y, c, frequency_decay_constant_pixels, half_resolution_pixels);
    // For c = 0.300, resolution at (100, 100) = 0.161 cycles/pixel
    VERIFY (static_cast<int> (cpp * 100 + 0.5) == 16);

    x = 500;
    y = 500;
    cpp = Resolution (x, y, c, frequency_decay_constant_pixels, half_resolution_pixels);
    // For c = 0.300, resolution at (500, 500) = 0.038 cycles/pixel
    VERIFY (static_cast<int> (cpp * 100 + 0.5) == 4);
}

int main ()
{
    try
    {
        test1 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

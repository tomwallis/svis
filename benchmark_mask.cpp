// Benchmark mask routines
//
// Copyright (C) 2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue Aug 15 18:27:13 CDT 2006

#include <ctime>
#include "ecc.h"
#include "mask.h"
#include <iostream>
#include <stdexcept>

using namespace std;
using namespace SVIS;

void benchmark1 ()
{
    // Create a resmap
    SVIS::AutoImage resmap = { 320, 240, 0 };

    const double halfres = 1.0;
    const double fov = 45.0;
    CreateResmap (resmap.width, resmap.height, resmap.pixels, halfres, fov);

    // The top of the pyramid does not get blended, so you need 1 less mask than pyramid levels.
    const unsigned LEVELS = 5;
    vector<SVIS::AutoImage> masks (LEVELS - 1);

    // Time it
    size_t count = 0;
    time_t t = clock ();
    while (static_cast<double> (clock () - t) / CLOCKS_PER_SEC < 1.0)
    {
        // Compute mask image for each level.
        for (unsigned n = 0; n < LEVELS - 1; n++)
        {
            SVIS::CreateMask (masks[n], resmap, n);

            // Crop the mask image
            unsigned crop_x;
            unsigned crop_y;
            unsigned crop_width;
            unsigned crop_height;

            // Where should the image be cropped?
            SVIS::GetCropParams (masks[n].width, masks[n].height, masks[n].pixels, crop_x, crop_y, crop_width, crop_height);

            //cout << crop_x << ", " << crop_y << " -- ";
            //cout << crop_width << ", " << crop_height << endl;

            // Don't crop if you don't need to
            if (crop_width != masks[n].width || crop_height != masks[n].height)
                SVIS::CropMask (masks[n], crop_x, crop_y, crop_width, crop_height);
        }
        ++count;
    }
    cout << count << "Hz" << endl;
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

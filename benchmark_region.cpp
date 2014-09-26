// Benchmark region routines
//
// Copyright (C) 2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Thu Aug 17 19:38:32 CDT 2006

#include <algorithm>
#include <ctime>
#include <iostream>
#include "region.h"
#include <stdexcept>
#include <vector>

using namespace std;
using namespace SVIS;

bool Rand ()
{
    return !(rand () % 100);
}

void benchmark1 ()
{
    // Create an image with random pixels set
    const unsigned W = 1024;
    const unsigned H = 768;
    SVIS::AutoImage i;
    i.width = W;
    i.height = H;
    i.scale = 0;
    i.pixels.resize (W * H);

    // Set some
    generate (i.pixels.begin (), i.pixels.end (), Rand);

    // Time it
    size_t count = 0;
    time_t t = clock ();
    while (static_cast<double> (clock () - t) / CLOCKS_PER_SEC < 1.0)
    {
        const unsigned region_w = 32;
        const unsigned region_h = 8;

        // Find out how many there are
        int total = TotalNonzero (i, 0, 0, region_w, region_h);

        int start_x = 0, start_y = 0;
        Region r;

        // Get all the regions.
        for (int n = 0; n < total; n++)
        {
            if (!FindNonzero (i, start_x, start_y, region_w, region_h, &r))
                throw runtime_error ("FindRegion() found less regions than TotalNonzero()");

            if (r.x2 <= r.x1)
                throw runtime_error ("Invalid return value");
            if (r.y2 <= r.y1)
                throw runtime_error ("Invalid return value");

            // Mark the region on the image with a gray rectangle.
            for (unsigned bmy = r.y1; bmy < r.y2; bmy++)
            {
                i.pixels[bmy * i.width + r.x1] = 192;
                i.pixels[bmy * i.width + r.x2 - 1] = 192;
            }
            for (unsigned bmx = r.x1; bmx < r.x2; bmx++)
            {
                i.pixels[(r.y1) * i.width + bmx] = 192;
                i.pixels[(r.y2 - 1) * i.width + bmx] = 192;
            }

            // Start searching one pixel past the top right of the last block.
            start_x = r.x2; // right
            start_y = r.y1; // top
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

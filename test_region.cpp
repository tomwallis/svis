// Manipulate rectangular regions.
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Thu Aug 17 19:31:50 CDT 2006

#include <algorithm>
#include <functional>
#include <iostream>
#include "verify.h"
#include "pnm_util.h"
#include "region.h"
#include <stdexcept>
#include <vector>

using namespace std;
using namespace SVIS;

void test1 ()
{
    const unsigned W = 256;
    const unsigned H = 256;
    AutoImage i;
    i.width = W;
    i.height = H;
    i.scale = 0;
    i.pixels.resize (W * H);

    // Find the pixel with some weird sized blocks.
    int sizes[] = { 1, 7, 10, 23, 29, 1001, 1000023 };

    // Do all combinations of the block sizes.
    // It shouldn't find anything.
    for (unsigned w = 0; w < sizeof (sizes) / sizeof (int); w++)
    {
        for (unsigned h = 0; h < sizeof (sizes) / sizeof (int); h++)
        {
            //cout << sizes[w] << "X" << sizes[h] << endl;
            if (TotalNonzero (i, 0, 0, sizes[w], sizes[h]) != 0)
                throw runtime_error ("Failed");
        }
    }
}

void test2 ()
{
    const unsigned W = 256;
    const unsigned H = 256;
    AutoImage i;
    i.width = W;
    i.height = H;
    i.scale = 0;
    i.pixels.resize (W * H);

    // Find the pixel with some weird sized blocks.
    int sizes[] = { 1, 7, 10, 23, 29, 1001, 1000023 };

    // Set the pixel in the exact center of the image to a value.
    i.pixels[i.width * i.height / 2 + i.width / 2] = 0xFF;

    for (unsigned w = 0; w < sizeof (sizes) / sizeof (int); w++)
    {
        for (unsigned h = 0; h < sizeof (sizes) / sizeof (int); h++)
        {
            //cout << sizes[w] << "X" << sizes[h] << endl;
            if (TotalNonzero (i, 0, 0, sizes[w], sizes[h]) != 1)
                throw runtime_error ("Failed");
        }
    }

    // Other small sizes.
    int sizes2[] = { 1, 2, 3, 11, 17, 32, 64, 67 };

    /* Set the corners to a value. */
    i.pixels[0] = 0xFF; // top left
    i.pixels[i.height - 1] = 0xFF; // top right
    i.pixels[i.width * (i.height - 1)] = 0xFF; // bottom left
    i.pixels[i.width * i.height - 1] = 0xFF; // bottom right

    for (unsigned w = 0; w < sizeof (sizes2) / sizeof (int); w++)
    {
        for (unsigned h = 0; h < sizeof (sizes2) / sizeof (int); h++)
        {
            //cout << sizes2[w] << "X" << sizes2[h] << endl;
            if (TotalNonzero (i, 0, 0, sizes2[w], sizes2[h]) != 5)
                throw runtime_error ("Failed");
        }
    }

    for (unsigned w = 0; w < sizeof (sizes2) / sizeof (int); w++)
    {
        for (unsigned h = 0; h < sizeof (sizes2) / sizeof (int); h++)
        {
            //cout << sizes2[w] << "X" << sizes2[h] << endl;
            if (TotalNonzero (i, 0, 0, sizes2[w], sizes2[h]) != 5)
                throw runtime_error ("Failed");
        }
    }
}

void test3 ()
{
    PNM::Image src_image;

    Load (src_image, "src.pgm");
    VERIFY (src_image.GetPixelDepth () == 1);

    SVIS::AutoImage i;
    i.width = src_image.GetWidth ();
    i.height = src_image.GetHeight ();
    i.scale = 0;
    i.pixels = src_image.GetPixels ();

    // Make all pixel values 0 except for 128's
    // MS's compiler, version 14.00.50727.42, chokes during runtime on
    // the following code:
    //replace_copy_if (&i.pixels[0],
    //    &i.pixels[i.width * i.height],
    //    &i.pixels[0],
    //    bind2nd (not_equal_to<unsigned char> (), 128),
    //    0);
    for (unsigned j = 0; j < i.width * i.height; ++j)
        if (i.pixels[j] != 128)
            i.pixels[j] = 0;

    const unsigned region_w = 32;
    const unsigned region_h = 8;

    // Find out how many there are
    unsigned total = TotalNonzero (i, 0, 0, region_w, region_h);

    // cout << total << " regions found." << endl;

    unsigned start_x = 0, start_y = 0;
    Region r;

    // Get all the regions.
    for (unsigned n = 0; n < total; n++)
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

    // Total+1 calls should fail.
    if (FindNonzero (i, start_x, start_y, region_w, region_h, &r))
        throw runtime_error ("FindRegion() found more regions than TotalRegions()");
}

int main ()
{
    try
    {
        test1 ();
        test2 ();
        test3 ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

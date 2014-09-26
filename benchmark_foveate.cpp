// Benchmark foveation routines
//
// Copyright (C) 2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Fri Aug 18 13:32:43 CDT 2006

#include <algorithm>
#include <ctime>
#include <iostream>
#include "foveate.h"
#include "pnm.h"
#include <stdexcept>
#include <vector>

using namespace std;
using namespace SVIS;

void benchmark1 ()
{
    // Create the source and dest
    const unsigned W = 640;
    const unsigned H = 480;
    const unsigned LEVELS = 5;
    std::vector<unsigned char> src_pixels (W * H);
    PNM::Image dest_image (W, H, 1);
    SVIS::Image src = { W, H, 0, &src_pixels[0] };
    SVIS::Image dest = { W, H, 0, const_cast<unsigned char *> (&dest_image.GetPixels ()[0]) };

    // Create a resolution map
    //cout << "Creating resmap..." << endl;
    SVIS::AutoImage resmap;
    resmap.width = W * 2;
    resmap.height = H * 2;
    resmap.scale = 0;
    CreateResmap (resmap.width, resmap.height, resmap.pixels, 2.3, 45);

    // Create the masks -- The top level of the pyramid is not
    // blended, so for N levels, you need N-1 masks.
    FoveationMasks masks;
    //cout << "Creating resmap masks..." << endl;
    masks.Create (resmap, LEVELS - 1);

    // Create the pyramids.  Image encoding and decoding will take
    // place within the allocated pyramids.
    FoveationPyramid src_p;
    FoveationPyramid dest_p;
    src_p.Create (src, LEVELS);
    dest_p.Create (dest, LEVELS);

    //cout << "Benchmarking normal resmap..." << endl;

    // Time it
    size_t count = 0;
    time_t t = clock ();
    while (static_cast<double> (clock () - t) / CLOCKS_PER_SEC < 1.0)
    {
        src_p.Reduce ();
        FoveationEncode (src_p, masks,
            (rand () % (W * 3)) - W * 3 / 2,
            (rand () % (H * 3)) - H * 3 / 2);
        FoveationDecode (src_p, masks, dest_p);
        ++count;
    }

    cout << count << "Hz" << endl;

    // Make a random resmap
    generate (resmap.pixels.begin (), resmap.pixels.end (), rand);
    //cout << "Creating resmap masks..." << endl;
    masks.Create (resmap, LEVELS - 1);

    //cout << "Benchmarking random resmap..." << endl;

    // Time it
    count = 0;
    t = clock ();
    while (static_cast<double> ((clock () - t) / CLOCKS_PER_SEC) < 1.0)
    {
        src_p.Reduce ();
        FoveationEncode (src_p, masks,
            (rand () % (W * 3)) - W * 3 / 2,
            (rand () % (H * 3)) - H * 3 / 2);
        FoveationDecode (src_p, masks, dest_p);
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

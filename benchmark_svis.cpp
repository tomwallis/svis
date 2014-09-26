// Benchmark SVIS routines
//
// Copyright (C) 2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Fri Aug 18 13:32:43 CDT 2006

#include <algorithm>
#include <ctime>
#include <iostream>
#include "svis.h"
//#include "pnm_util.h"
#include <stdexcept>
#include <vector>

using namespace std;
using namespace SVIS;

void benchmark1 ()
{
    // Read an image
    //PNM::Image src;
    //Load (src, "src.pgm");

    // Allocate the encoded image
    //PNM::Image dest (src.GetWidth (), src.GetHeight (), 1);
    const unsigned W = 640;
    const unsigned H = 480;
    vector<unsigned char> src (W * H);
    vector<unsigned char> dest (W * H);
    generate (src.begin (), src.end (), rand);

    // Setup the codec
    CODEC codec (W, H, &src[0], &dest[0]);
    vector<unsigned char> resmap;
    CreateResmap (W * 2, // resmap width
        H * 2, // resmap height
        resmap, // resmap pixels
        2.3, // halfres
        45.0); // field of view
    codec.SetResmap (W * 2, H * 2, resmap);

    int x = W / 2;
    int y = H / 2;
    codec.Reduce ();
    codec.Encode (x, y);

    // Time it
    size_t count = 0;
    time_t t = clock ();
    while (static_cast<double> (clock () - t) / CLOCKS_PER_SEC < 1.0)
    {
        // Encode/Decode
        codec.Reduce ();
        x = rand () % (W * 2) - W;
        y = rand () % (H * 2) - H;
        codec.Encode (x, y);
        codec.Decode ();
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

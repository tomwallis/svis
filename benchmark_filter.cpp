// Benchmark filter routines
//
// Copyright (C) 2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue Aug 15 09:33:20 CDT 2006

#include <ctime>
#include "filter.h"
#include <iostream>
#include <stdexcept>

using namespace std;

void benchmark1 ()
{
    // Setup image buffers
    const unsigned W = 256;
    const unsigned H = 256;

    SVIS::Image src = { W, H, 0, new unsigned char [W * H] };
    SVIS::Image dest = { W, H, 1, new unsigned char [(W / 2) * (H / 2)] };
    SVIS::Image final = { W, H, 0, new unsigned char [W * H] };

    size_t count = 0;
    time_t t = clock ();
    while (static_cast<double> (clock () - t) / CLOCKS_PER_SEC < 1.0)
    {
        SVIS::Reduce3x3 (&src, &dest);
        SVIS::ExpandOdd (&dest, &final);
        ++count;
    }

    cout << count << "Hz" << endl;

    // Free image buffers
    delete [] src.pixels;
    delete [] dest.pixels;
    delete [] final.pixels;
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

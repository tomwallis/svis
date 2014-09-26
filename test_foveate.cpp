// Test foveation routines
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include <cassert>
#include "foveate.h"
#include <iostream>
#include "verify.h"
#include "pnm_util.h"
#include <stdexcept>

using namespace std;
using namespace SVIS;

Image GetImage (const PNM::Image &p, unsigned w, unsigned h, unsigned scale)
{
    VERIFY (p.GetWidth () == w >> scale);
    VERIFY (p.GetHeight () == h >> scale);
    Image i;
    i.width = w;
    i.height = h;
    i.scale = scale;
    // An image may be empty, so avoid referencing pixels[0] when this
    // is true...
    if (p.GetSize ())
        i.pixels = const_cast<unsigned char *> (&p.GetPixels ()[0]);
    else
        i.pixels = 0;
    return i;
}

void test1 ()
{
    // Setup the bitmaps.
    PNM::Image src_image;
    PNM::Image dest_image;

    Load (src_image, "src.pgm");
    const unsigned W = src_image.GetWidth ();
    const unsigned H = src_image.GetHeight ();
    VERIFY (src_image.GetPixelDepth () == 1);
    dest_image.SetSize (W, H, 1);

    // Setup src and dest to point to the allocated bitmaps.
    Image src = GetImage (src_image, W, H, 0);
    Image dest = GetImage (dest_image, W, H, 0);

    // Create a resolution map
    SVIS::AutoImage resmap = { W * 2, H * 2, 0 };
    //cout << "Creating resmap..." << endl;
    CreateResmap (resmap.width, resmap.height, resmap.pixels, 2.3, 45);

    // Create the pyramids.  Image encoding and decoding will take
    // place within the allocated pyramids.
    const unsigned LEVELS = 5;
    FoveationPyramid src_p;
    FoveationPyramid dest_p;
    src_p.Create (src, LEVELS);
    dest_p.Create (dest, LEVELS);

    // Create the masks -- The top level of the pyramid is not
    // blended, so for N levels, you need N-1 masks.
    FoveationMasks masks;
    //cout << "Creating resmap masks..." << endl;
    masks.Create (resmap, LEVELS - 1);

    // First reduce the source.  You may have to do this only once if
    // the fixation point changes, but the source stays the same
    // (i.e.: a still image).  Or, you may have to do it on every
    // fixation if the source is changing (e.g: video, framegrabber,
    // ... etc.).
    src_p.Reduce ();

    // Fixation params
    vector<int> x;
    vector<int> y;

    // Foveate at a number of locations
    x.push_back (W / 2); y.push_back (H / 2);
    x.push_back (0); y.push_back (0);
    x.push_back (W); y.push_back (0);
    x.push_back (W); y.push_back (H);
    x.push_back (0); y.push_back (H);
    x.push_back (W * -3); y.push_back (H * -3);
    x.push_back (W * 3); y.push_back (H * 3);
    x.push_back (W + 100); y.push_back (-100);
    x.push_back (100); y.push_back (H - 100);

    assert (x.size () == y.size ());

    for (unsigned i = 0; i < x.size (); ++i)
    {
        // Foveate
        //cout << "Encoding..." << endl;
        FoveationEncode (src_p, masks, x[i], y[i]);
        FoveationDecode (src_p, masks, dest_p);

        // Write it out
        stringstream ss1;
        ss1 << "tmp_foveate_" << x[i] << "_" << y[i] << ".pgm";
        Save (dest_image, ss1.str ());

        // What if the src pyramid's base scale does not start at 0?
        // E.g.: What if src's are U and V portions of a YUV image.

        // Let's use level 2 of the src pyramid as our source image instead--
        // it will be downsampled by a factor of 4.

        FoveationPyramid src_p2;
        src_p2.Create (src_p.images[2], LEVELS);

        // We have to create a new dest buffer and pyramid.
        PNM::Image dest_image2 (src.width / 4, src.height / 4, 1);

        // Dest has the same dimensions as the source.
        SVIS::Image dest2 = GetImage (dest_image2, src.width, src.height, 2);

        // Setup the destination pyramid
        FoveationPyramid dest_p2;
        dest_p2.Create (dest2, LEVELS);

        // Encode
        //
        // Note that fixation coords are at scale=0, not scale=2
        src_p2.Reduce ();
        FoveationEncode (src_p2, masks, x[i], y[i]);
        FoveationDecode (src_p2, masks, dest_p2);

        // Save it
        stringstream ss2;
        ss2 << "tmp_foveate_downsampled_" << x[i] << "_" << y[i] << ".pgm";
        Save (dest_image2, ss2.str ());
    }
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



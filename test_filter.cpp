// Test filter routines
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include "filter.h"
#include "pnm_util.h"
#include "verify.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace SVIS;
using namespace std;

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

AutoImage GetAutoImage (const PNM::Image &p, unsigned w, unsigned h, unsigned scale)
{
    VERIFY (p.GetWidth () == w >> scale);
    VERIFY (p.GetHeight () == h >> scale);
    AutoImage i;
    i.width = w;
    i.height = h;
    i.scale = scale;
    i.pixels = p.GetPixels ();
    return i;
}

void DoReduceTest1 (void reduce_function (const Image*, Image*), const string &suffix)
{
    // Setup the bitmaps.
    PNM::Image src_image;
    PNM::Image dest_image;

    Load (src_image, "src.pgm");
    VERIFY (src_image.GetPixelDepth () == 1);
    dest_image.SetSize (src_image.GetWidth () / 2, src_image.GetHeight () / 2, 1);

    Image src;
    Image dest;

    // Setup src and dest to point to the allocated bitmaps.
    src = GetImage (src_image, src_image.GetWidth (), src_image.GetHeight (), 0);
    dest = GetImage (dest_image, src_image.GetWidth (), src_image.GetHeight (), 1);

    reduce_function (&src, &dest);

    // Save dest.
    string fn ("tmp_filter_reduced");
    fn += suffix;
    fn += ".pgm";
    Save (dest_image, fn);
}

void DoReduceTest2 (void reduce_function (const Image*, Image*))
{
    // Do some random reduces.
    for (int pass = 0; pass < 1000; pass++)
    {
        unsigned w = rand () % 128;
        unsigned h = rand () % 128;
        unsigned scale = rand () % 10;
        int pixel = rand () % 256;

        PNM::Image src_image;
        PNM::Image dest_image;
        src_image.SetSize (w >> scale, h >> scale, 1);
        dest_image.SetSize (w >> (scale + 1), h >> (scale + 1), 1);

        Image src = GetImage (src_image, w, h, scale);
        Image dest = GetImage (dest_image, w, h, scale + 1);

        // Set src pixels.
        vector<unsigned char> pixels (src_image.GetSize (), pixel);
        src_image.SetPixels (pixels);

        reduce_function (&src, &dest);

        // All the dest pixels should be set.
        for (unsigned y = 0; y < (dest.height >> dest.scale); y++)
            for (unsigned x = 0; x < (dest.width >> dest.scale); x++)
                VERIFY (dest.pixels[y * (dest.width >> dest.scale) + x] == pixel);
    }
}

void DoExpandTest1 (void expand_function (const Image*, Image*), const string &suffix)
{
    // Setup the bitmaps.
    PNM::Image src_image;
    PNM::Image dest_image;

    Load (src_image, "src.pgm");
    VERIFY (src_image.GetPixelDepth () == 1);
    dest_image.SetSize (src_image.GetWidth () * 2, src_image.GetHeight () * 2, 1);

    // Setup src and dest to point to the allocated bitmaps.
    Image src = GetImage (src_image, dest_image.GetWidth (), dest_image.GetHeight (), 1);
    Image dest = GetImage (dest_image, dest_image.GetWidth (), dest_image.GetHeight (), 0);

    expand_function (&src, &dest);

    // Save dest.
    string fn ("tmp_filter_expanded");
    fn += suffix;
    fn += ".pgm";
    Save (dest_image, fn);

    vector<unsigned char> src_pixels (src_image.GetSize (), 0xFF);
    vector<unsigned char> dest_pixels (dest_image.GetSize (), 0x00);
    // Set src pixels.
    src_image.SetPixels (src_pixels);
    // Clear dest pixels.
    dest_image.SetPixels (dest_pixels);

    expand_function (&src, &dest);

    // All the dest pixels should be set.
    for (unsigned y = 0; y < (dest.height >> dest.scale); y++)
        for (unsigned x = 0; x < (dest.width >> dest.scale); x++)
            VERIFY (dest.pixels[y * (dest.width >> dest.scale) + x] == 0xFF);
}


void DoExpandTest2 (void expand_function (const Image*, Image*))
{
    Image src;
    Image dest;

    // Do some random expands.
    for (unsigned pass = 0; pass < 1000; pass++)
    {
        unsigned w = rand () % 128;
        unsigned h = rand () % 128;
        unsigned scale = rand () % 10;
        int pixel = rand () % 256;

        PNM::Image src_image;
        PNM::Image dest_image;
        src_image.SetSize (w >> (scale + 1), h >> (scale + 1), 1);
        dest_image.SetSize (w >> scale, h >> scale, 1);

        src = GetImage (src_image, w, h, scale + 1);
        dest = GetImage (dest_image, w, h, scale);

        // Set src pixels.
        vector<unsigned char> pixels (src_image.GetSize (), pixel);
        src_image.SetPixels (pixels);

        expand_function (&src, &dest);

        // If the src is too small, the dest should not have changed.
        if (((src.height >> src.scale) < 2) || ((src.width >> src.scale) < 2))
        {
            // None of the dest pixels should be set.
            for (unsigned y = 0; y < (dest.height >> dest.scale); y++)
                for (unsigned x = 0; x < (dest.width >> dest.scale); x++)
                    VERIFY (dest.pixels[y * (dest.width >> dest.scale) + x] == 0);
        }
        else
        {
            // All the dest pixels should be set.
            for (unsigned y = 0; y < (dest.height >> dest.scale); y++)
                for (unsigned x = 0; x < (dest.width >> dest.scale); x++)
                    VERIFY (dest.pixels[y * (dest.width >> dest.scale) + x] == pixel);
        }
    }
}

void DoBlendTest1 ()
{
    // Make MASK_W a multiple of 4 so you don't get into trouble with bitmaps.
    const int MASK_W = 200;
    const int MASK_H = 200;

    // Setup the bitmaps.
    PNM::Image src_image;
    PNM::Image dest_image;
    PNM::Image mask_image;

    Load (src_image, "src.pgm");
    VERIFY (src_image.GetPixelDepth () == 1);
    dest_image.SetSize (src_image.GetWidth (), src_image.GetHeight (), 1);
    mask_image.SetSize (MASK_W, MASK_H, 1);

    Image src;
    Image dest;
    AutoImage mask;

    // Setup src and dest to point to the allocated bitmaps.
    src = GetImage (src_image, src_image.GetWidth (), src_image.GetHeight (), 0);
    dest = GetImage (dest_image, dest_image.GetWidth (), dest_image.GetHeight (), 0);
    mask = GetAutoImage (mask_image, mask_image.GetWidth (), mask_image.GetHeight (), 0);

    // Mask will be a circle with K pixel ramped edges.
    const int K = 40;
    for (int y = 0; y < MASK_H; y++)
    {
        for (int x = 0; x < MASK_W; x++)
        {
            int x2 = x - MASK_W/2;
            int y2 = y - MASK_H/2;
            int dist = static_cast<int> (sqrt (static_cast<double> (x2*x2 + y2*y2)) + 1);
            if (dist > MASK_W/2)
                mask.pixels[y * MASK_W + x] = 0;
            else if (dist > MASK_W/2 - K)
                mask.pixels[y * MASK_W + x] = (unsigned char) ((MASK_W/2 - dist) * 255 / K); // K pixel ramp
            else
                mask.pixels[y * MASK_W + x] = 255;
        }
    }

    // Save the mask.
    string fn ("tmp_filter_mask1.pgm");
    mask_image.SetPixels (mask.pixels);
    Save (mask_image, fn);

    // Center the mask.
    int mask_x = src.width/2 - MASK_W/2;
    int mask_y = src.height/2 - MASK_H/2;

    Blend (&src, &dest, &mask, 0, mask_x, mask_y);

    // Save dest.
    fn = "tmp_filter_blended1.pgm";
    Save (dest_image, fn);

    // Clear dest
    dest_image.SetPixels (0);

    // Do the same thing in a blend region = quadrant 4.
    Rect r;
    r.x1 = src.width/2;
    r.y1 = src.height/2;
    r.x2 = src.width;
    r.y2 = src.height;
    Blend (&src, &dest, &mask, &r, mask_x, mask_y);

    // Save dest.
    fn = "tmp_filter_quarter_blended.pgm";
    Save (dest_image, fn);

    // Do some random blend coordinates.
    for (int pass = 0; pass < 100; pass++)
        Blend (&src, &dest, &mask, 0, rand () - RAND_MAX/2, rand () - RAND_MAX/2);
}

void DoBlendTest2 ()
{
    const int MASK_W = 64;
    const int MASK_H = 7;

    // Setup the bitmaps.
    PNM::Image src_image;
    PNM::Image dest_image;
    PNM::Image mask_image;

    Load (src_image, "src.pgm");
    VERIFY (src_image.GetPixelDepth () == 1);
    dest_image.SetSize (src_image.GetWidth (), src_image.GetHeight (), 1);
    mask_image.SetSize (MASK_W, MASK_H, 1);

    Image src = GetImage (src_image, src_image.GetWidth (), src_image.GetHeight (), 0);
    Image dest = GetImage (dest_image, dest_image.GetWidth (), dest_image.GetHeight (), 0);
    // Mask is at a different scale than the image
    AutoImage mask = GetAutoImage (mask_image, mask_image.GetWidth ()*8, mask_image.GetHeight ()*8, 3);

    // Mask will be 1 pixel wide vertical stripes.
    for (int y = 0; y < MASK_H; y++)
        for (int x = 0; x < MASK_W; x++)
            if (x & 1)
                mask.pixels[y * MASK_W + x] = 0;
            else
                mask.pixels[y * MASK_W + x] = 255;

    // Save the mask.
    string fn ("tmp_filter_mask2.pgm");
    mask_image.SetPixels (mask.pixels);
    Save (mask_image, fn);

    // Center the mask.
    int mask_x = src.width/2 - mask.width/2;
    int mask_y = src.height/2 - mask.height/2;

    // Blend it.
    Blend (&src, &dest, &mask, 0, mask_x, mask_y);

    // Save dest.
    fn = "tmp_filter_blended2.pgm";
    Save (dest_image, fn);
}

void DoBlendTest3 ()
{
    const int MASK_W = 64;
    const int MASK_H = 19;

    // Setup the bitmaps.
    PNM::Image src_image;
    PNM::Image dest_image;
    PNM::Image mask_image;

    Load (src_image, "src.pgm");
    VERIFY (src_image.GetPixelDepth () == 1);
    dest_image.SetSize (src_image.GetWidth (), src_image.GetHeight (), 1);
    mask_image.SetSize (MASK_W, MASK_H, 1);

    Image src = GetImage (src_image, src_image.GetWidth (), src_image.GetHeight (), 0);
    Image dest = GetImage (dest_image, dest_image.GetWidth (), dest_image.GetHeight (), 0);
    AutoImage mask = GetAutoImage (mask_image, mask_image.GetWidth ()*8, mask_image.GetHeight ()*8, 3);

    // Mask will be a horizontal ramp.
    for (int y = 0; y < MASK_H; y++)
        for (int x = 0; x < MASK_W; x++)
            mask.pixels[y * MASK_W + x] = x * 255 / MASK_W;

    // Save the mask.
    string fn ("tmp_filter_mask3.pgm");
    mask_image.SetPixels (mask.pixels);
    Save (mask_image, fn);

    // Center the mask.
    int mask_x = src.width/2 - mask.width/2;
    int mask_y = src.height/2 - mask.height/2;

    // Blend it.
    Blend (&src, &dest, &mask, 0, mask_x, mask_y);

    // Save dest.
    fn = "tmp_filter_blended3.pgm";
    Save (dest_image, fn);
}

int main ()
{
    try
    {
        DoReduceTest1 (&Reduce2x2, "_2x2");
        DoReduceTest1 (&Reduce3x3, "_3x3");
        DoReduceTest2 (&Reduce2x2);
        DoReduceTest2 (&Reduce3x3);
        DoExpandTest1 (&ExpandEven, "_even");
        DoExpandTest1 (&ExpandOdd, "_odd");
        DoExpandTest2 (&ExpandEven);
        DoExpandTest2 (&ExpandOdd);
        DoBlendTest1 ();
        DoBlendTest2 ();
        DoBlendTest3 ();
        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

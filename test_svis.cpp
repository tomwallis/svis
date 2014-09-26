// Space Variant Imaging System
//
// Copyright (C) 2003-2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue Aug 22 12:00:49 CDT 2006

#include "verify.h"
#include "pnm_util.h"
#include "svis.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
using namespace SVIS;

void test1 ()
{
    // Read an image
    PNM::Image src;
    Load (src, "src.pgm");
    VERIFY (src.GetPixelDepth () == 1);

    // Allocate the encoded image
    PNM::Image dest (src.GetWidth (), src.GetHeight (), 1);

    // Setup the codec
    CODEC *codec = new CODEC (src.GetWidth (),
        src.GetHeight (),
        src.GetPixelsAddress (),
        dest.GetPixelsAddress ());
    VERIFY (codec->GetWidth () == src.GetWidth ());
    VERIFY (codec->GetHeight () == src.GetHeight ());
    VERIFY (codec->GetImageSize () == src.GetWidth () * src.GetHeight ());
    vector<unsigned char> pixels;
    CreateResmap (src.GetWidth () * 2, // resmap width
        src.GetHeight () * 2, // resmap height
        pixels,
        2.3, // halfres
        45.0); // field of view
    codec->SetResmap (src.GetWidth () * 2,
        src.GetHeight () * 2,
        pixels);

    // Fixation params
    vector<int> x;
    vector<int> y;

    const unsigned W = src.GetWidth ();
    const unsigned H = src.GetHeight ();

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

    // You only have to call Reduce once because the input never
    // changes...
    codec->Reduce ();

    for (unsigned i = 0; i < x.size (); ++i)
    {
        codec->Encode (x[i], y[i]);
        codec->Decode ();

        // Save it
        stringstream ss;
        ss << "tmp_svis_" << x[i] << "_" << y[i] << ".pgm";
        Save (dest, ss.str ());
    }
}

void test2 ()
{
    // Read an image
    PNM::Image src;
    Load (src, "src.pgm");
    VERIFY (src.GetPixelDepth () == 1);

    // Allocate the encoded image
    PNM::Image dest (src.GetWidth (), src.GetHeight (), 1);

    // Setup a codec
    CODEC codec (src.GetWidth (),
        src.GetHeight (),
        src.GetPixelsAddress (),
        dest.GetPixelsAddress ());
    // Allocate a resmap
    vector<unsigned char> pixels;
    CreateResmap (src.GetWidth () * 2, // resmap width
        src.GetHeight () * 2, // resmap height
        pixels, // resmap pixels
        2.3, // halfres
        45.0); // field of view
    // And set it
    codec.SetResmap (src.GetWidth () * 2,
        src.GetHeight () * 2,
        pixels);

    // Get the masks associated with the resmap
    vector<unsigned char> mp;
    PNM::Image mask;

    // Write masks associated with this resmap
    for (unsigned i = 0; i < codec.PyramidLevels () - 1; ++i)
    {
        unsigned w, h;
        codec.GetMask (i, w, h, mp);
        //clog << "i: " << i << endl;
        //clog << "w: " << w << endl;
        //clog << "h: " << h << endl;
        //clog << "sz: " << mp.size () << endl;
        if (w == 0 || h == 0)
            continue;
        mask.SetSize (w, h, 1);
        mask.SetPixels (mp);
        // Save it
        stringstream ss;
        ss << "tmp_svis_mask_" << i << ".pgm";
        Save (mask, ss.str ());
    }

    // The function throws if you try to get a mask that doesn't exist
    bool caught = false;
    try
    {
        unsigned w, h;
        codec.GetMask (codec.PyramidLevels () - 1, w, h, mp);
    }
    catch (...)
    {
        caught = true;
    }

    VERIFY (caught);
}

void test3 ()
{
    // Read an image
    PNM::Image src;
    Load (src, "src.pgm");
    VERIFY (src.GetPixelDepth () == 1);

    // Allocate the encoded image
    PNM::Image dest (src.GetWidth (), src.GetHeight (), 1);

    // Setup a codec
    CODEC codec (src.GetWidth (),
        src.GetHeight (),
        src.GetPixelsAddress (),
        dest.GetPixelsAddress ());
    // Allocate a resmap
    vector<unsigned char> pixels;
    CreateResmap (src.GetWidth () * 2, // resmap width
        src.GetHeight () * 2, // resmap height
        pixels, // resmap pixels
        2.3, // halfres
        45.0); // field of view
    // And set it
    codec.SetResmap (src.GetWidth () * 2,
        src.GetHeight () * 2,
        pixels);

    // Reduce Pyramid
    codec.Reduce ();

    // Get the reduced images
    unsigned riw, rih;
    vector<unsigned char> rip;
    PNM::Image ri;

    // Write images of reduced pyramid
    for (unsigned i = 0; i < codec.PyramidLevels (); ++i)
    {
        codec.GetReducedImage (i, riw, rih, rip);
        ri.SetSize (riw, rih, 1);
        ri.SetPixels (rip);
        // Save it
        stringstream ss;
        ss << "tmp_svis_reduced_" << i << ".pgm";
        Save (ri, ss.str ());
    }

    // The function throws if you try to get an image that doesn't exist
    bool caught = false;
    try
    {
        codec.GetReducedImage (codec.PyramidLevels (), riw, rih, rip);
    }
    catch (...)
    {
        caught = true;
    }

    VERIFY (caught);

    // Fixation params
    vector<int> x;
    vector<int> y;

    const int W = src.GetWidth ();
    const int H = src.GetHeight ();

    // Foveate at a number of locations
    x.push_back (W / 2); y.push_back (H / 2);
    x.push_back (W); y.push_back (H);
    x.push_back (W + W/2); y.push_back (H + H/2);
    x.push_back (-W); y.push_back (-H);
    x.push_back (W/2); y.push_back (-H*10);
    for (unsigned i = 0; i < x.size (); ++i)
    {
        // Encode Pyramid
        codec.Encode (x[i], y[i]);

        // Get the Encoded images

        // Write images of decoded pyramid
        for (unsigned j = 0; j < codec.PyramidLevels (); ++j)
        {
            vector<unsigned> eix;
            vector<unsigned> eiy;
            vector<unsigned> eiw;
            vector<unsigned> eih;
            vector<vector<unsigned char> > eip;
            PNM::Image ei;
            codec.GetEncodedImageBlocks (j, eix, eiy, eiw, eih, eip);
            VERIFY (eix.size () == eiy.size ());
            VERIFY (eix.size () == eiw.size ());
            VERIFY (eix.size () == eih.size ());
            VERIFY (eix.size () == eip.size ());
            for (unsigned k = 0; k < eix.size (); ++k)
            {
                ei.SetSize (eiw[k], eih[k], 1);
                ei.SetPixels (eip[k]);
                // Save it
                stringstream ss;
                ss << "tmp_svis_pass_" << i
                    << "_encoded_level_" << j
                    << "_block_" << k
                    << "_" << eix[k]
                    << "_" << eiy[k]
                    << "_" << eiw[k]
                    << "_" << eih[k]
                    << ".pgm";
                Save (ei, ss.str ());
            }
        }
    }

    // The function throws if you try to get an image that doesn't exist
    caught = false;
    try
    {
        vector<unsigned> eix;
        vector<unsigned> eiy;
        vector<unsigned> eiw;
        vector<unsigned> eih;
        vector<vector<unsigned char> > eip;
        codec.GetEncodedImageBlocks (codec.PyramidLevels (), eix, eiy, eiw, eih, eip);
    }
    catch (...)
    {
        caught = true;
    }

    // Decode image
    codec.Decode ();

    // Get the Decoded images
    unsigned diw, dih;
    vector<unsigned char> dip;
    PNM::Image di;

    // Write images of decoded pyramid
    for (unsigned i = 0; i < codec.PyramidLevels (); ++i)
    {
        codec.GetDecodedImage (i, diw, dih, dip);
        di.SetSize (diw, dih, 1);
        di.SetPixels (dip);
        // Save it
        stringstream ss;
        ss << "tmp_svis_decoded_" << i << ".pgm";
        Save (di, ss.str ());
    }

    // The function throws if you try to get an image that doesn't exist
    caught = false;
    try
    {
        codec.GetDecodedImage (codec.PyramidLevels (), diw, dih, dip);
    }
    catch (...)
    {
        caught = true;
    }
}

void test4 ()
{
    vector<unsigned char> src (240, 320);
    vector<unsigned char> dest (240, 320);
    CODEC *codec = new CODEC (240, 320, &src[0], &dest[0]);
    delete codec;
}

int main ()
{
    try
    {
        test1 ();
        test2 ();
        test3 ();
        test4 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

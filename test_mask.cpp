// Test mask routines
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include "ecc.h"
#include "mask.h"
#include "verify.h"
#include "pnm_util.h"
#include <sstream>
#include <stdexcept>

using namespace std;

void test1 ()
{
    const unsigned levels = 5;
    const double criterion = 0.5;

    for (unsigned n = 0; n < levels; n++)
    {
        double cf = SVIS::CriticalFrequency (n, criterion);
        //clog << "cf[" << n << "]=" << cf << endl;
        // When criterion = 0.25:
        //VERIFY (n != 0 || static_cast<int> (cf * 1000 + 0.5) == 413);
        //VERIFY (n != 1 || static_cast<int> (cf * 1000 + 0.5) == 206);
        //VERIFY (n != 2 || static_cast<int> (cf * 1000 + 0.5) == 103);
        //VERIFY (n != 3 || static_cast<int> (cf * 1000 + 0.5) == 52);
        //VERIFY (n != 4 || static_cast<int> (cf * 1000 + 0.5) == 26);
        // When criterion = 0.5:
        VERIFY (n != 0 || static_cast<int> (cf * 1000 + 0.5) == 292);
        VERIFY (n != 1 || static_cast<int> (cf * 1000 + 0.5) == 146);
        VERIFY (n != 2 || static_cast<int> (cf * 1000 + 0.5) == 73);
        VERIFY (n != 3 || static_cast<int> (cf * 1000 + 0.5) == 36);
        VERIFY (n != 4 || static_cast<int> (cf * 1000 + 0.5) == 18);
        double sigma = SVIS::Sigma (n);
        // sigma[0] = 0.248
        // sigma[1] = 0.124
        // sigma[2] = 0.062
        // sigma[3] = 0.031
        // sigma[4] = 0.0155
        //clog << "sigma[" << n << "]=" << sigma << endl;
        VERIFY (n != 0 || static_cast<int> (sigma * 1000 + 0.5) == 248);
        VERIFY (n != 1 || static_cast<int> (sigma * 1000 + 0.5) == 124);
        VERIFY (n != 2 || static_cast<int> (sigma * 1000 + 0.5) == 62);
        VERIFY (n != 3 || static_cast<int> (sigma * 1000 + 0.5) == 31);
        VERIFY (n != 4 || static_cast<int> (sigma * 10000 + 0.5) == 155);
    }
}

void test2 ()
{
    const unsigned W = 640;
    const unsigned H = 480;
    SVIS::AutoImage resmap = { W, H, 0 };

    const double halfres = 1.0;
    const double fov = 45.0;
    SVIS::CreateResmap (resmap.width, resmap.height, resmap.pixels, halfres, fov);

    PNM::Image img;
    img.SetSize (W, H, 1);
    img.SetPixels (resmap.pixels);

    Save (img, "tmp_mask_resmap.pgm");

    // The top of the pyramid does not get blended, so you need 1 less mask than pyramid levels.
    const unsigned LEVELS = 5;
    vector<SVIS::AutoImage> masks (LEVELS - 1);

    // Compute mask image for each level.
    for (unsigned n = 0; n < LEVELS - 1; n++)
    {
        SVIS::CreateMask (masks[n], resmap, n);
        img.SetSize (masks[n].width, masks[n].height, 1);
        img.SetPixels (masks[n].pixels);

        stringstream ss;
        ss << "tmp_mask_mask" << n << ".pgm";
        Save (img, ss.str ());
    }

    // Crop the mask images
    for (unsigned n = 0; n < LEVELS - 1; n++)
    {
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

        img.SetSize (masks[n].width, masks[n].height, 1);
        img.SetPixels (masks[n].pixels);

        stringstream ss;
        ss << "tmp_mask_cropped" << n << ".pgm";
        Save (img, ss.str ());
    }
}

int main ()
{
    try
    {
        test1 ();
        test2 ();

        return 0;
    }
    catch (const exception &e)
    {
        cerr << e.what () << endl;
        return -1;
    }
}

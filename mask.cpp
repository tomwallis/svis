// Mask calulations
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include <algorithm>
#include <cassert>
#include <cmath>
#include "ecc.h"
#include "mask.h"
#include <stdexcept>
#include <vector>

namespace SVIS
{

unsigned char BlendingFunction (unsigned level, unsigned char pixel)
{
    const double CRITERION = 0.5;
    // Put pixel in the range [0.0, c0]
    const double w = pixel * CriticalFrequency (0, CRITERION) / 255;

    // If it's below the frequency range for this level, make it 0.
    if (w < CriticalFrequency (level + 1, CRITERION))
        return 0;
    // If it's above the frequency range for this level, make it 255.
    else if (w >= CriticalFrequency (level, CRITERION))
        return 255;
    else
    {
        double sigma_n0 = Sigma (level) * sqrt (2.0);
        double sigma_n1 = Sigma (level + 1) * sqrt (2.0);
        double t0 = exp (-(w / sigma_n0) * (w / sigma_n0));
        double t1 = exp (-(w / sigma_n1) * (w / sigma_n1));
        double r =  (CRITERION - t1) / (t0 - t1);
        assert (r >= 0);
        if (r > 1.0) // Clamp to 255
            return 255;
        else
            return static_cast<unsigned char> (r * 255);
    }
}

void CreateResmap (unsigned width,
    unsigned height,
    std::vector<unsigned char> &pixels,
    double halfres,
    double resmap_fov_deg)
{
    pixels.resize (width * height);

    // Convert from degrees to pixels
    double half_res_pix = halfres * width / resmap_fov_deg;

    for (unsigned y = 0; y < height; y++)
    {
        unsigned y_offset = y - height / 2;
        for (unsigned x = 0; x < width; x++)
        {
            unsigned x_offset = x - width / 2;
            double eccentricity = sqrt (static_cast<double> (x_offset * x_offset + y_offset * y_offset));
            double res = half_res_pix / (half_res_pix + eccentricity);
            pixels[y * width + x] = static_cast<unsigned char> (res * 255);
        }
    }
}

void CreateMask (AutoImage &mask, const AutoImage &resmap, unsigned level)
{
    // Level must be between 1 and 16.
    if (level > 16)
        throw std::runtime_error ("Invalid 'level' parameter");

    // Fill in the mask values.
    mask.width = resmap.width;
    mask.height = resmap.height;
    mask.scale = resmap.scale;

    const unsigned SZ = resmap.width * resmap.height;

    // Allocate the mask pixels.
    mask.pixels.resize (SZ);

    // Precompute the blending function mask values.
    std::vector<unsigned char> blending_function (256);
    for (unsigned i = 0; i < 256; ++i)
        blending_function[i] = BlendingFunction (level, i);

    // Fill the mask pixels with blending function values.
    for (unsigned i = 0; i < SZ; ++i)
    {
        assert (i < mask.pixels.size ());
        assert (i < resmap.pixels.size ());

        // Get its new interpolated value and set it in the mask
        assert (resmap.pixels[i] < blending_function.size ());
        mask.pixels[i] = blending_function[resmap.pixels[i]];
    }
}

void GetCropParams (unsigned width,
    unsigned height,
    const std::vector<unsigned char> &pixels,
    unsigned &crop_x, unsigned &crop_y,
    unsigned &new_width, unsigned &new_height)
{
    unsigned min_x = width;
    unsigned max_x = 0;
    unsigned min_y = height;
    unsigned max_y = 0;

    for (unsigned y = 0; y < height; y++)
    {
        for (unsigned x = 0; x < width; x++)
        {
            unsigned p = pixels[y * width + x];

            if (p > 0)
            {
                if (x < min_x)
                    min_x = x;
                if (x > max_x)
                    max_x = x;
                if (y < min_y)
                    min_y = y;
                if (y > max_y)
                    max_y = y;
            }
        }
    }

    // Make max_x and max_y non-inclusive.
    ++max_x;
    ++max_y;

    // If no non-zero pixels were found, boundaries are a zero sized region.
    if (min_x >= max_x)
        min_x = max_x = 0;
    if (min_y >= max_y)
        min_y = max_y = 0;

    crop_x = min_x;
    crop_y = min_y;
    new_width = max_x - min_x;
    new_height = max_y - min_y;
}

void CropMask (AutoImage &mask, unsigned crop_x, unsigned crop_y, unsigned crop_width, unsigned crop_height)
{
    assert (crop_width * crop_height <= mask.width * mask.height);

    // The cropped pixels...
    std::vector<unsigned char> crop_pixels (crop_width * crop_height);

    // Copy src to dest.
    for (unsigned y = 0; y < crop_height; y++)
        //std::copy (&mask.pixels[(y + crop_y) * mask.width + crop_x],
        //    &mask.pixels[(y + crop_y) * mask.width + crop_x + crop_width],
        //    &crop_pixels[y * crop_width]);
        // Melchi Michel 20071017
        std::copy (&mask.pixels[0] + (y + crop_y) * mask.width + crop_x,
            &mask.pixels[0] + (y + crop_y) * mask.width + crop_x + crop_width,
            &crop_pixels[0] + y * crop_width);

    // Set the new values for the cropped image.
    mask.width = crop_width;
    mask.height = crop_height;
    mask.pixels = crop_pixels;

    return;
}

} // namespace SVIS

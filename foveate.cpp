// Foveation routines
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include <algorithm>
#include <cassert>
#include "foveate.h"
#include <stdexcept>

using namespace std;

namespace SVIS
{

void FoveationPyramid::Create (Image &base, unsigned levels)
{
    // Levels must be between 1 and 16.
    if (levels < 1 || levels > 16)
        throw runtime_error ("Invalid 'levels' parameter");

    // Allocate the vector of images.
    images.resize (levels);

    // Allocate the vector of regions.
    regions.resize (levels);

    this->levels = levels;
    this->fixation_x = base.width / 2;
    this->fixation_y = base.height / 2;

    // Allocate the image buffers and setup pointers.
    buffers.resize (levels);
    for (unsigned n = 0; n < levels; n++)
    {
        images[n].width = buffers[n].width = base.width;
        images[n].height = buffers[n].height = base.height;
        images[n].scale = buffers[n].scale = base.scale + n;

        if (n == 0)
        {
            // If this is level 0, point the image to the base...
            assert (base.pixels);
            images[0].pixels = &base.pixels[0];
        }
        else
        {
            // ... otherwise, resize the buffer according to its scale,
            // and point the image to the buffer
            unsigned size = (buffers[n].width >> buffers[n].scale) *
                (buffers[n].height >> buffers[n].scale);
            // Make sure the image has at least one pixel so that we may address pixels[0]
            if (!size)
                size = 1;
            buffers[n].pixels.resize (size);
            images[n].pixels = &buffers[n].pixels[0];
        }
    }
}

void FoveationMasks::Create (const AutoImage &resmap, unsigned levels)
{
    // Levels must be between 1 and 16.
    if (levels < 1 || levels > 16)
        throw runtime_error ("Invalid 'levels' parameter");

    this->levels = levels;

    // Allocate vectors
    masks.resize (levels);
    center_xs.resize (levels);
    center_ys.resize (levels);
    regions.resize (levels);

    // Create the masks, crop them, and save their offsets in the offsets arrays.
    for (unsigned n = 0; n < levels; n++)
    {
        CreateMask (masks[n], resmap, n);
        unsigned crop_x;
        unsigned crop_y;
        unsigned crop_width;
        unsigned crop_height;

        // Where should the image be cropped?
        SVIS::GetCropParams (masks[n].width, masks[n].height, masks[n].pixels, crop_x, crop_y, crop_width, crop_height);

        // Don't crop if you don't need to
        if (crop_width != masks[n].width || crop_height != masks[n].height)
            SVIS::CropMask (masks[n], crop_x, crop_y, crop_width, crop_height);

        // Where is the center of the mask, relative to its top left corner?
        center_xs[n] = resmap.width / 2 - crop_x;
        center_ys[n] = resmap.height / 2 - crop_y;
    }

    // Find non-zero regions in the masks
    for (unsigned n = 0; n < levels; n++)
    {
        const unsigned REGION_WIDTH = 32;
        const unsigned REGION_HEIGHT = 32;

        // Find out how many there are
        unsigned total = TotalNonzero (masks[n], 0, 0, REGION_WIDTH << n, REGION_HEIGHT << n);
        unsigned start_x = 0, start_y = 0;

        // Allocate them
        regions[n].resize (total);

        // Get all the regions.
        for (unsigned region = 0; region < total; region++)
        {
            Region r;
            bool found = FindNonzero (masks[n],
                start_x,
                start_y,
                REGION_WIDTH << n,
                REGION_HEIGHT << n,
                &r);

            assert (found);

            if (!found)
                continue;

            assert (r.x2 > r.x1);
            assert (r.y2 > r.y1);

            // Save it
            regions[n][region] = r;

            // Start searching one pixel past the top right of the last block.
            start_x = r.x2; // right
            start_y = r.y1; // top
        }
    }
}

void FoveationPyramid::Reduce ()
{
    // Reduce the images.
    assert (images.size () > 0);
    for (unsigned n = 0; n < images.size () - 1; n++)
        Reduce3x3 (&images[n], &images[n + 1]);
}

void FoveationEncode (FoveationPyramid &p, const FoveationMasks &m, int x, int y)
{
    // Compute the regions relative to x, y.

    // For each region in each mask level, convert the region to
    // coordinates relative to the pyramid image.
    for (unsigned n = 0; n < p.levels - 1; ++n)
    {
        int mask_offset_x = x - m.center_xs[n];
        int mask_offset_y = y - m.center_ys[n];

        // If this is the first time we are calling this routine, we need
        // to allocate space in the pyramid for the regions
        if (p.regions[n].size () != m.regions[n].size ())
            p.regions[n].resize (m.regions[n].size ());

        for (unsigned r = 0; r < m.regions[n].size (); ++r)
        {
            // The results are signed
            int x1, y1, x2, y2;

            // Convert from mask coordinates.
            x1 = mask_offset_x + m.regions[n][r].x1;
            y1 = mask_offset_y + m.regions[n][r].y1;
            x2 = mask_offset_x + m.regions[n][r].x2;
            y2 = mask_offset_y + m.regions[n][r].y2;

            // Make coords multiples of the image scale.
            x1 >>= p.images[n].scale;
            x1 <<= p.images[n].scale;
            y1 >>= p.images[n].scale;
            y1 <<= p.images[n].scale;
            x2 >>= p.images[n].scale;
            x2 <<= p.images[n].scale;
            y2 >>= p.images[n].scale;
            y2 <<= p.images[n].scale;

            // Clip the region to the image boundaries.
            if (x1 < 0)
                x1 = 0;
            if (x1 > static_cast<int> (p.images[n].width))
                x1 = p.images[n].width;
            if (y1 < 0)
                y1 = 0;
            if (y1 > static_cast<int> (p.images[n].height))
                y1 = p.images[n].height;
            if (x2 < 0)
                x2 = 0;
            if (x2 > static_cast<int> (p.images[n].width))
                x2 = p.images[n].width;
            if (y2 < 0)
                y2 = 0;
            if (y2 > static_cast<int> (p.images[n].height))
                y2 = p.images[n].height;

            // Now set the unsigned rect
            p.regions[n][r].x1 = x1;
            p.regions[n][r].y1 = y1;
            p.regions[n][r].x2 = x2;
            p.regions[n][r].y2 = y2;
        }
    }

    // Always set the top level of the pyramid to include the entire
    // image
    unsigned top = p.levels - 1;
    p.regions[top].resize (1);
    p.regions[top][0].x1 = 0;
    p.regions[top][0].y1 = 0;
    p.regions[top][0].x2 = p.images[top].width;
    p.regions[top][0].y2 = p.images[top].height;

    // Set the fixation point.
    p.fixation_x = x;
    p.fixation_y = y;
}

void FoveationDecode (const FoveationPyramid &src, const FoveationMasks &masks, FoveationPyramid &dest)
{
    // Make sure the pyramid bases are the same dimension.
    if (src.levels != dest.levels ||
        src.images[0].width != dest.images[0].width ||
        src.images[0].height != dest.images[0].height ||
        src.images[0].scale != dest.images[0].scale)
        throw runtime_error ("Pyramid dimensions must be equal");

    assert (src.levels > 0);
    unsigned top = src.levels - 1;

    // Make sure the pyramid tops are the same dimension.
    if (src.images[top].width != dest.images[top].width ||
        src.images[top].height != dest.images[top].height ||
        src.images[top].scale != dest.images[top].scale)
        throw runtime_error ("Pyramid dimensions must be equal");

    unsigned top_size = (src.images[top].width >> src.images[top].scale) *
        (src.images[top].height >> src.images[top].scale);

    // Copy top level of src to dest.
    copy (&src.images[top].pixels[0],
        &src.images[top].pixels[top_size],
        &dest.images[top].pixels[0]);

    // Set the fixation point.
    dest.fixation_x = src.fixation_x;
    dest.fixation_y = src.fixation_y;

    // Expand and copy over regions as you go, starting with the top
    // and working down...
    for (unsigned n = dest.levels - 1; n > 0; --n)
    {
        int mask_offset_x = dest.fixation_x - masks.center_xs[n - 1];
        int mask_offset_y = dest.fixation_y - masks.center_ys[n - 1];

        // Upsample and interpolate
        ExpandOdd (&dest.images[n], &dest.images[n - 1]);

        // Now blend the regions
        for (unsigned r = 0; r < masks.regions[n - 1].size (); r++)
        {
            Rect rect;
            rect.x1 = src.regions[n - 1][r].x1;
            rect.y1 = src.regions[n - 1][r].y1;
            rect.x2 = src.regions[n - 1][r].x2;
            rect.y2 = src.regions[n - 1][r].y2;

            // It is possible that the region has a zero dimension.
            // In this case, do not blend.
            assert ((rect.x2 - rect.x1) * (rect.y2 - rect.y1) >= 0);

            if ((rect.x2 - rect.x1) * (rect.y2 - rect.y1) == 0)
                continue;

            // Do the blending
            Blend (&src.images[n - 1],
                &dest.images[n - 1],
                &masks.masks[n - 1],
                &rect,
                mask_offset_x,
                mask_offset_y);
        }
    }
}

} // namespace SVIS

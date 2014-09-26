// Calculations involving pixel/eccentricity/resolution/arc conversions.
//
// Copyright (C) 2003-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include "region.h"

namespace SVIS
{

bool FindNonzero (const AutoImage &image, unsigned start_x, unsigned start_y, unsigned width, unsigned height, struct Region *region)
{
    if (!region)
        return 0;
    if (width < 1)
        return 0;
    if (height < 1)
        return 0;

    // Region widths and heights will always be multiples of width and height.
    for (unsigned y = start_y; y < image.height; y += height)
    {
        bool first_block_found = false;
        for (unsigned x = start_x; x < image.width; x += width)
        {
            // (x,y) is the top left corner of the block that we are searching.

            bool all_zero = true; // Assume that this will be an all-zero block.

            // Scan this block to see if it contains any non-zero values.
            for (unsigned offset_y = 0; offset_y < height; offset_y++)
            {
                unsigned total_y = y + offset_y;

                if (total_y >= image.height)
                    break; // We have reached the bottom edge of the image.
                for (unsigned offset_x = 0; offset_x < width; offset_x++)
                {
                    unsigned total_x = x + offset_x;

                    if (total_x >= image.width)
                        break; // We have reached to right edge of the image.
                    if (image.pixels[total_y * image.width + total_x] != 0)
                    {
                        all_zero = 0;
                        break; // Found a non-zero pixel, we can stop searching the scanline.
                    }
                }
                if (!all_zero)
                    break; // Found a non-zero pixel, we can stop searching the block.
            }

            // We have just scanned a block.
            if (!all_zero)
            {
                // It was a non-zero block.
                if (!first_block_found)
                {
                    // This is the first one we have found so far.
                    first_block_found = 1;
                    // Mark the upper left corner of the region.
                    region->x1 = x;
                    region->y1 = y;
                }

                // Whether or not is was the first block, mark the lower right corner of the region.
                // Note that non-zero regions do NOT go across scanlines.
                region->x2 = x + width;;
                region->y2 = y + height;

                // Clip to image dimensions.
                if (region->x2 > image.width)
                    region->x2 = image.width;
                if (region->y2 > image.height)
                    region->y2 = image.height;
            }
            else
            {
                // It was an all-zero block.
                if (first_block_found)
                    return 1; // Success: we have found a block that ends in the middle of a scanline.
            }
        }

        if (first_block_found)
            return 1; // Success: we have found a block that ends at the end of a scanline.

        // On the next row of scanlines, we need to back up all the way to the left edge of the image.
        start_x = 0;
    }

    return 0; // Failed: there were no non-zero blocks found.
}

unsigned TotalNonzero (const AutoImage &image, unsigned x, unsigned y, unsigned width, unsigned height)
{
    struct Region region = { 0, 0, 0, 0 };
    unsigned start_x = 0;
    unsigned start_y = 0;
    unsigned total = 0;

    while (FindNonzero (image, start_x, start_y, width, height, &region))
    {
        ++total;
        // Next time we call FindRegion(), start in the top right corner of the last found region.
        // If the corner is against the right edge, that's OK, it will wrap around.
        start_x = region.x2; // right
        start_y = region.y1; // top
    }

    return total;
}

} // namespace SVIS

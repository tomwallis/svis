// Filter, reduce, expand, and blend routines
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp 2001/05/17

#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>

#include "filter.h"

using std::runtime_error;

namespace SVIS
{

/*
static void FixRect (int width, int height, Rect *rect)
{
    // Fix regions that lie outside the image boundaries.
    if (rect->x1 < 0)
        rect->x1 = 0;
    if (rect->x1 > width)
        rect->x1 = width;
    if (rect->y1 < 0)
        rect->y1 = 0;
    if (rect->y1 > height)
        rect->y1 = height;
    if (rect->x2 < 0)
        rect->x2 = 0;
    if (rect->x2 > width)
        rect->x2 = width;
    if (rect->y2 < 0)
        rect->y2 = 0;
    if (rect->y2 > height)
        rect->y2 = height;
}
*/

void Reduce2x2 (const Image *src, Image *dest)
{
    int x;
    int y;
    int src_width;
    int src_height;
    int dest_width;
    int dest_height;
    unsigned char *src_p1, *src_p2, *dest_p;
    unsigned int p1, p2, p3, p4;

    // Make sure we have valid pointers and that
    // src and dest have the same dimensions.
    // Destination's scale must be 1/2 source's scale.
    if (!src || !dest)
        throw runtime_error ("Reduce2x2: Invalid parameters");
    if (src->width != dest->width || src->height != dest->height ||
        src->scale + 1 != dest->scale)
        throw runtime_error ("Reduce2x2: Invalid dimensions");

    src_width = src->width >> src->scale;
    src_height = src->height >> src->scale;
    dest_width = dest->width >> dest->scale;
    dest_height = dest->height >> dest->scale;

    for (y = 0; y < src_height; y += 2)
    {
        if (y / 2 >= dest_height)
            break;
        else
            dest_p = &dest->pixels[y / 2 * dest_width];

        src_p1 = &src->pixels[y * src_width];

        if (y + 1 >= src_height)
            src_p2 = src_p1; // Clamp the edge.
        else
            src_p2 = &src->pixels[(y + 1) * src_width];

        for (x = 0; x < src_width; x += 2)
        {
            if (x / 2 >= dest_width)
                break;

            p1 = src_p1[x];
            p3 = src_p2[x];

            if (x + 1 >= src_width)
            {
                p2 = p1; // Clamp the edge.
                p4 = p3;
            }
            else
            {
                p2 = src_p1[x + 1];
                p4 = src_p2[x + 1];
            }
            // Add 2 for round-off error.
            dest_p[x / 2] = (p1 + p2 + p3 + p4 + 2) / 4;
        }
    }
}

void Reduce3x3 (const Image *src, Image *dest)
{
    int x;
    int y;
    int src_width;
    int src_height;
    int dest_width;
    int dest_height;
    unsigned char *src_p1, *src_p2, *src_p3, *dest_p;
    unsigned int p1, p2, p3, p4, p5, p6, p7, p8, p9;

    // Make sure we have valid pointers and that
    // src and dest have the same dimensions.
    // Destination's scale must be 1/2 source's scale.
    if (!src || !dest)
        throw runtime_error ("Reduce3x3: Invalid parameters");
    if (src->width != dest->width || src->height != dest->height ||
        src->scale + 1 != dest->scale)
        throw runtime_error ("Reduce3x3: Invalid dimensions");

    src_width = src->width >> src->scale;
    src_height = src->height >> src->scale;
    dest_width = dest->width >> dest->scale;
    dest_height = dest->height >> dest->scale;

    for (y = 0; y < src_height; y += 2)
    {
        if (y / 2 >= dest_height)
            break;
        else
            dest_p = &dest->pixels[y / 2 * dest_width];

        src_p1 = &src->pixels[y * src_width];

        if (y + 1 >= src_height)
            src_p2 = src_p1; // Clamp the edge.
        else
            src_p2 = &src->pixels[(y + 1) * src_width];

        if (y + 2 >= src_height)
            src_p3 = src_p2; // Clamp the edge.
        else
            src_p3 = &src->pixels[(y + 2) * src_width];

        for (x = 0; x < src_width; x += 2)
        {
            if (x / 2 >= dest_width)
                break;

            p1 = src_p1[x];
            p4 = src_p2[x];
            p7 = src_p3[x];

            if (x + 1 >= src_width)
            {
                p2 = p1; // Clamp the edge.
                p5 = p4;
                p8 = p7;
            }
            else
            {
                p2 = src_p1[x + 1];
                p5 = src_p2[x + 1];
                p8 = src_p3[x + 1];
            }

            if (x + 2 >= src_width)
            {
                p3 = p2; // Clamp the edge.
                p6 = p5;
                p9 = p8;
            }
            else
            {
                p3 = src_p1[x + 2];
                p6 = src_p2[x + 2];
                p9 = src_p3[x + 2];
            }

            // This is a Gaussian-like filter weighted like this:
            //
            //  1/4 1/2 1/4
            //  1/2   1 1/2
            //  1/4 1/2 1/4
            dest_p[x / 2] = (p1 + p2*2 + p3 + p4*2 + p5*4 + p6*2 + p7 + p8*2 + p9 + 8) >> 4;
        }
    }
}

// Slow helper functions to get/set pixels.  Note that x and y are already converted to i's scale.
static unsigned char GetPixel (const Image *i, int x, int y)
{
    int w, h;

    w = i->width >> i->scale;
    h = i->height >> i->scale;

    assert (w && h);

    if (x >= w)
        x = w - 1;

    if (y >= h)
        y = h - 1;

    return i->pixels[y * w + x];
}

static void SetPixel (Image *i, int x, int y, unsigned char p)
{
    int w, h;

    w = i->width >> i->scale;
    h = i->height >> i->scale;

    assert (w && h);

    if (x >= w)
        x = w - 1;

    if (y >= h)
        y = h - 1;

    i->pixels[y * w + x] = p;
}

void ExpandEven (const Image *src, Image *dest)
{
    int x;
    int y;
    int src_width;
    int src_height;
    int dest_width;
    int dest_height;
    unsigned char *src_p1, *src_p2;
    unsigned char *dest_p1, *dest_p2;
    unsigned int p1, p2, p3, p4;
    unsigned char p;

    // Make sure we have valid pointers and that
    // src and dest have the same dimensions.
    // Source's scale must be 1/2 destination's scale.
    if (!src || !dest)
        throw runtime_error ("ExpandEven: Invalid parameters");
    if (src->width != dest->width || src->height != dest->height ||
        src->scale != dest->scale + 1)
        throw runtime_error ("ExpandEven: Invalid dimensions");

    src_width = src->width >> src->scale;
    src_height = src->height >> src->scale;
    dest_width = dest->width >> dest->scale;
    dest_height = dest->height >> dest->scale;

    // If the source is too small, don't expand it.
    if (src_width < 2 || src_height < 2)
        return;

    // Do the center part of the image.
    for (y = 1; y < dest_height - 2; y += 2)
    {
        src_p1 = &src->pixels[(y / 2) * src_width];
        src_p2 = &src->pixels[(y / 2 + 1) * src_width];
        dest_p1 = &dest->pixels[y * dest_width];
        dest_p2 = &dest->pixels[(y + 1) * dest_width];

        for (x = 1; x < dest_width - 2; x += 2)
        {
            p1 = src_p1[x / 2];
            p2 = src_p1[x / 2 + 1];
            p3 = src_p2[x / 2];
            p4 = src_p2[x / 2 + 1];
            dest_p1[x    ] = (p1 * 2 + p2 + p3 + 2) / 4;
            dest_p1[x + 1] = (p2 * 2 + p1 + p4 + 2) / 4;
            dest_p2[x    ] = (p3 * 2 + p1 + p4 + 2) / 4;
            dest_p2[x + 1] = (p4 * 2 + p2 + p3 + 2) / 4;
        }
    }

    // Fix the edges.
    for (y = 0; y < dest_height; y++)
    {
        // Left
        p = GetPixel (dest, 1, y);
        SetPixel (dest, 0, y, p);

        // Right
        // If width is an odd number, there is an extra column to fix on the right.
        if (dest_width & 1)
        {
            p = GetPixel (dest, dest_width - 3, y);
            SetPixel (dest, dest_width - 2, y, p);
        }
        p = GetPixel (dest, dest_width - 2, y);
        SetPixel (dest, dest_width - 1, y, p);
    }

    for (x = 0; x < dest_width; x++)
    {
        // Top
        if (dest_height > 1)
        {
            p = GetPixel (dest, x, 1);
            SetPixel (dest, x, 0, p);
        }

        // Bottom
        // If height is an odd number, there is an extra row to fix on the bottom.
        if (dest_height & 1)
        {
            p = GetPixel (dest, x, dest_height - 3);
            SetPixel (dest, x, dest_height - 2, p);
        }
        p = GetPixel (dest, x, dest_height - 2);
        SetPixel (dest, x, dest_height - 1, p);
    }
}

void ExpandOdd (const Image *src, Image *dest)
{
    int x;
    int y;
    int src_width;
    int src_height;
    int dest_width;
    int dest_height;
    unsigned char *src_p1, *src_p2;
    unsigned char *dest_p0, *dest_p1, *dest_p2;
    unsigned char p;

    // Make sure we have valid pointers and that
    // src and dest have the same dimensions.
    // Source's scale must be 1/2 destination's scale.
    if (!src || !dest)
        throw runtime_error ("ExpandOdd: Invalid parameters");
    if (src->width != dest->width || src->height != dest->height ||
        src->scale != dest->scale + 1)
        throw runtime_error ("ExpandOdd: Invalid dimensions");

    src_width = src->width >> src->scale;
    src_height = src->height >> src->scale;
    dest_width = dest->width >> dest->scale;
    dest_height = dest->height >> dest->scale;

    // If the source is too small, don't expand it.
    if (src_width < 2 || src_height < 2)
        return;

    // First, copy edge pixels from the source to the dest.
    for (y = 0; y < dest_height; y++)
    {
        // Left
        p = GetPixel (src, 0, y / 2);
        SetPixel (dest, 0, y, p);

        // Right
        p = GetPixel (src, (dest_width - 1) / 2, y / 2);
        SetPixel (dest, dest_width - 1, y, p);
    }

    for (x = 0; x < dest_width; x++)
    {
        // Top
        p = GetPixel (src, x / 2, 0);
        SetPixel (dest, x, 0, p);

        // Bottom
        p = GetPixel (src, x / 2, (dest_height - 1) / 2);
        SetPixel (dest, x, dest_height - 1, p);
    }

    // Do the center part of the image.
    for (y = 1; y < dest_height - 2; y += 2)
    {
        src_p1 = &src->pixels[(y / 2) * src_width];
        src_p2 = &src->pixels[(y / 2 + 1) * src_width];
        dest_p0 = &dest->pixels[(y - 1) * dest_width];
        dest_p1 = &dest->pixels[y * dest_width];
        dest_p2 = &dest->pixels[(y + 1) * dest_width];

        for (x = 1; x < dest_width - 2; x += 2)
        {
            // This way of upsampling produces artifacts that are
            // typical of bilinear interpolation.
            dest_p1[x    ] = src_p1[x / 2];
            dest_p2[x + 1] = (src_p1[x / 2] + src_p1[(x + 1) / 2] + src_p2[x / 2] + src_p2[(x + 1) / 2] + 2) / 4;
            dest_p1[x + 1] = (src_p1[x / 2] + src_p1[(x + 1) / 2] + 1) / 2;
            dest_p2[x    ] = (src_p1[x / 2] + src_p2[x / 2] + 1) / 2;

            // This way of upsampling mitigates the artifacts, but it
            // is a total kludge and introduces other types of high
            // frequency artifacts, so don't use it.
            /*
            dest_p2[x + 1] = (src_p1[x / 2] + src_p1[(x + 1) / 2] + src_p2[x / 2] + src_p2[(x + 1) / 2] + 2) / 4;
            dest_p1[x + 1] = (src_p1[x / 2] + src_p1[(x + 1) / 2] + dest_p0[x + 1] + dest_p2[x + 1] + 2) / 4;
            dest_p2[x    ] = (src_p1[x / 2] + src_p2[x / 2] + dest_p2[x - 1] + dest_p2[x + 1] + 2) / 4;
            dest_p1[x    ] = (dest_p0[x] + dest_p1[x - 1] + dest_p1[x + 1] + dest_p2[x + 1] + 2) / 4;
            */
        }
    }

    // Fix the edges.
    for (y = 0; y < dest_height; y++)
    {
        // Left
        p = GetPixel (dest, 1, y);
        SetPixel (dest, 0, y, p);

        // Right
        // If width is an odd number, there is an extra column to fix on the right.
        if (dest_width & 1)
        {
            p = GetPixel (dest, dest_width - 3, y);
            SetPixel (dest, dest_width - 2, y, p);
        }
        p = GetPixel (dest, dest_width - 2, y);
        SetPixel (dest, dest_width - 1, y, p);
    }

    for (x = 0; x < dest_width; x++)
    {
        // Top
        if (dest_height > 1)
        {
            p = GetPixel (dest, x, 1);
            SetPixel (dest, x, 0, p);
        }

        // Bottom
        // If height is an odd number, there is an extra row to fix on the bottom.
        if (dest_height & 1)
        {
            p = GetPixel (dest, x, dest_height - 3);
            SetPixel (dest, x, dest_height - 2, p);
        }
        p = GetPixel (dest, x, dest_height - 2);
        SetPixel (dest, x, dest_height - 1, p);
    }
}

void Blend (const Image *src,
    Image *dest,
    const AutoImage *mask,
    const Rect *rect,
    int mask_offset_x,
    int mask_offset_y)
{
    // Make sure we have valid pointers and that
    // src and dest have the same dimensions.
    if (!src || !dest || !mask)
        throw runtime_error ("Blend: Invalid parameters");
    if (src->width != dest->width || src->height != dest->height ||
        src->scale != dest->scale)
        throw runtime_error ("Blend: Invalid dimensions");

    unsigned x1;
    unsigned y1;
    unsigned x2;
    unsigned y2;

    if (rect)
    {
        x1 = rect->x1;
        y1 = rect->y1;
        x2 = rect->x2;
        y2 = rect->y2;
    }
    else
    {
        // Default is the entire src.
        x1 = 0;
        y1 = 0;
        x2 = src->width;
        y2 = src->height;
    }

    assert (x1 <= dest->width);
    assert (y1 <= dest->height);
    assert (x2 <= dest->width);
    assert (y2 <= dest->height);

    // x and y are at normal resolution, i.e., scale = 0.  Ultimately,
    // only pixels at dest's resolution need to be processed, so we will
    // skip over the ones that aren't important.
    assert (dest->scale < 32);
    int inc = 1 << dest->scale;

    // Do the blending.
    for (unsigned y = y1; y < y2; y += inc)
    {
        unsigned char *src_p;
        unsigned char *dest_p;
        const unsigned char *mask_p;
        unsigned src_y, src_width;
        int mask_y, mask_width;

        // Convert to src coordinates.
        // Dest uses same coordinates as src.
        assert (src->scale < 32);
        src_y = (y >> src->scale);
        src_width = (src->width >> src->scale);

        // Check y.  It may go past the end of the buffer because of truncation.
        if (src_y >= (src->height >> src->scale))
            break;

        // Convert to mask coordinates.
        assert (src->scale < 32);
        mask_y = (y >> mask->scale) - (mask_offset_y >> mask->scale);
        mask_width = (mask->width >> mask->scale);

        // Get scanline pointers.
        src_p = &src->pixels[src_y * src_width];
        dest_p = &dest->pixels[src_y * src_width];

        // If the mask coordinate falls outside the boundary, don't blend.
        if (mask_y < 0)
            continue;
        if (static_cast<unsigned> (mask_y) >= (mask->height >> mask->scale))
            break;

        mask_p = &mask->pixels[mask_y * mask_width];

        for (unsigned x = x1; x < x2; x += inc)
        {
            unsigned src_x;
            int mask_x;
            int m, p1, p2, p;

            // Convert to src coordinates.
            // Dest uses same coordinates as src.
            src_x = (x >> src->scale);

            // Check x.  It may go past the end of the buffer because of truncation.
            if (src_x >= (src->width >> src->scale))
                break;

            // Convert to mask coordinates.
            mask_x = (x >> mask->scale) - (mask_offset_x >> mask->scale);

            // If it falls outside of the mask, don't blend.
            if (mask_x < 0)
                continue;
            if (static_cast<unsigned> (mask_x) >= (mask->width >> mask->scale))
                break;

            // Blend the pixel.
            m = mask_p[mask_x];
            p1 = src_p[src_x] * m;
            p2 = dest_p[src_x] * (255 - m);
            p = (p1 + p2) / 255;
            assert (p >= 0 && p <= 255);
            dest_p[src_x] = p;
        }
    }
}

} // namespace SVIS

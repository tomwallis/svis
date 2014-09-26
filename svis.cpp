// Space Variant Imaging System
//
// Copyright (C) 2003-2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Mon Aug 21 18:55:48 CDT 2006

#include "image.h"
#include "foveate.h"
#include "mask.h"
#include "svis.h"

#include <cassert>
#include <cmath>
#include <cstring>
#include <vector>
#include <stdexcept>

using namespace std;

namespace SVIS
{

// The CODEC implementation
struct CODEC::CODECImpl
{
    AutoImage resmap;
    FoveationMasks masks;
    FoveationPyramid src_pyramid;
    FoveationPyramid dest_pyramid;
};

CODEC::CODEC (unsigned width,
    unsigned height,
    unsigned char *src,
    unsigned char *dest,
    unsigned pyramid_levels) :
    pyramid_levels (pyramid_levels),
    pimpl (new CODECImpl)
{
    if (!src)
        throw runtime_error ("The src image pointer is not valid");
    if (!dest)
        throw runtime_error ("The dest image pointer is not valid");

    // Setup src_image and dest_image to point to the allocated bitmaps.
    Image src_image = { width, height, 0, src };
    Image dest_image = { width, height, 0, dest };

    // Create the pyramids.
    pimpl->src_pyramid.Create (src_image, pyramid_levels);
    pimpl->dest_pyramid.Create (dest_image, pyramid_levels);
}

CODEC::~CODEC ()
{
}

unsigned CODEC::GetImageSize () const
{
    return GetWidth () * GetHeight ();
}

unsigned CODEC::GetWidth () const
{
    assert (pimpl->src_pyramid.images.size () > 0);
    assert (pimpl->dest_pyramid.images.size () > 0);
    // src and dest should be equal
    assert (pimpl->src_pyramid.images[0].width ==
        pimpl->dest_pyramid.images[0].width);
    return pimpl->src_pyramid.images[0].width;
}

unsigned CODEC::GetHeight () const
{
    assert (pimpl->src_pyramid.images.size () > 0);
    assert (pimpl->dest_pyramid.images.size () > 0);
    // src and dest should be equal
    assert (pimpl->src_pyramid.images[0].height ==
        pimpl->dest_pyramid.images[0].height);
    return pimpl->src_pyramid.images[0].height;
}

void CODEC::SetSrcImage (unsigned char *p)
{
    assert (pimpl->src_pyramid.images.size () > 0);
    pimpl->src_pyramid.images[0].pixels = p;
}

void CODEC::SetDestImage (unsigned char *p)
{
    assert (pimpl->dest_pyramid.images.size () > 0);
    pimpl->dest_pyramid.images[0].pixels = p;
}

void CODEC::SetResmap (unsigned width,
    unsigned height,
    const vector<unsigned char> &pixels) const
{
    if (pixels.size () != width * height)
        throw runtime_error ("Incorrect pixel vector size");

    // Copy the resolution map
    pimpl->resmap.width = width;
    pimpl->resmap.height = height;
    pimpl->resmap.pixels = pixels;
    pimpl->resmap.scale = 0;

    // Create the masks from the resmap
    //
    // The top level of the pyramid is not blended, so for N levels,
    // you need N-1 masks.
    pimpl->masks.Create (pimpl->resmap, pyramid_levels - 1);
}

void CODEC::GetMask (unsigned level,
    unsigned &width,
    unsigned &height,
    vector<unsigned char> &pixels) const
{
    if (level >= pimpl->masks.levels)
        throw runtime_error ("Incorrect level parameter");
    width = pimpl->masks.masks[level].width;
    height = pimpl->masks.masks[level].height;
    pixels = pimpl->masks.masks[level].pixels;
}

void CODEC::Reduce ()
{
    assert (pimpl->src_pyramid.images.size () > 0);
    if (!pimpl->src_pyramid.images[0].pixels)
        throw runtime_error ("The source image has not been set");
    pimpl->src_pyramid.Reduce ();
}

void CODEC::GetReducedImage (unsigned level,
    unsigned &width,
    unsigned &height,
    vector<unsigned char> &pixels) const
{
    if (level >= pimpl->src_pyramid.levels)
        throw runtime_error ("Incorrect level parameter");
    // Scale the images down accordingly
    width = (pimpl->src_pyramid.images[level].width
        >> pimpl->src_pyramid.images[level].scale);
    height = (pimpl->src_pyramid.images[level].height
        >> pimpl->src_pyramid.images[level].scale);
    // Now copy the pixels
    pixels.resize (width * height);
    pixels.assign (&pimpl->src_pyramid.images[level].pixels[0],
        &pimpl->src_pyramid.images[level].pixels[width * height]);
}

void CODEC::Encode (int x, int y)
{
    if (pimpl->resmap.pixels.empty ())
        throw runtime_error ("A resolution map has not been set");
    FoveationEncode (pimpl->src_pyramid,
        pimpl->masks,
        x, y);
}

void CODEC::GetEncodedImageBlocks (unsigned level,
    vector<unsigned> &x,
    vector<unsigned> &y,
    vector<unsigned> &width,
    vector<unsigned> &height,
    vector<vector<unsigned char> > &pixels) const
{
    assert (pimpl->src_pyramid.regions.size () == pimpl->src_pyramid.levels);
    if (level >= pimpl->src_pyramid.levels)
        throw runtime_error ("Incorrect level parameter");
    // Start out empty
    x.clear ();
    y.clear ();
    width.clear ();
    height.clear ();
    pixels.clear ();
    // Get params for this pyramid image
    unsigned iwidth = pimpl->src_pyramid.images[level].width >> level;
#ifndef NDEBUG
    unsigned iheight = pimpl->src_pyramid.images[level].height >> level;
    unsigned iscale = pimpl->src_pyramid.images[level].scale;
#endif
    unsigned char *ipixels = pimpl->src_pyramid.images[level].pixels;
    vector<Region> &iregions = pimpl->src_pyramid.regions[level];
    // The scale should be redundant
    assert (iscale == level);
    // Create a temp vector for holding pixels
    vector<unsigned char> tp;
    // Copy the regions
    for (unsigned i = 0; i < iregions.size (); ++i)
    {
        Region r = iregions[i];
        assert (r.x1 <= r.x2);
        assert (r.y1 <= r.y2);
        // Convert to pyramid level's scale
        unsigned tw = (r.x2 >> level) - (r.x1 >> level);
        unsigned th = (r.y2 >> level) - (r.y1 >> level);
        // A region may be empty because it got cropped
        // -- that's OK, just keep going
        if (tw == 0 || th == 0)
            continue;
        unsigned tx = (r.x1 >> level);
        unsigned ty = (r.y1 >> level);
        // Size the pixels
        tp.resize (tw * th);
        // Copy the pixels row by row
        for (unsigned j = 0; j < th; ++j)
        {
            unsigned src_begin = (ty + j) * iwidth + tx;
            unsigned src_end = (ty + j) * iwidth + tx + tw;
            unsigned dest_begin = j * tw;
            // Make sure end() is in bounds
            assert (src_end <= iwidth * iheight);
            assert (dest_begin + tw <= tp.size ());
            // Do the copy
            copy (&ipixels[src_begin],
                &ipixels[src_end],
                &tp[dest_begin]);
        }
        // Save all the params in the given vectors
        x.push_back (tx);
        y.push_back (ty);
        width.push_back (tw);
        height.push_back (th);
        pixels.push_back (tp);
    }
}

void CODEC::Decode ()
{
    if (pimpl->resmap.pixels.empty ())
        throw runtime_error ("A resolution map has not been set");
    assert (pimpl->dest_pyramid.images.size () > 0);
    if (!pimpl->dest_pyramid.images[0].pixels)
        throw runtime_error ("The destination image has not been set");
    FoveationDecode (pimpl->src_pyramid,
        pimpl->masks,
        pimpl->dest_pyramid);
}

void CODEC::GetDecodedImage (unsigned level,
    unsigned &width,
    unsigned &height,
    vector<unsigned char> &pixels) const
{
    if (level >= pimpl->dest_pyramid.levels)
        throw runtime_error ("Incorrect level parameter");
    // Scale the images down accordingly
    width = (pimpl->dest_pyramid.images[level].width
        >> pimpl->dest_pyramid.images[level].scale);
    height = (pimpl->dest_pyramid.images[level].height
        >> pimpl->dest_pyramid.images[level].scale);
    // Now copy the pixels
    pixels.resize (width * height);
    pixels.assign (&pimpl->dest_pyramid.images[level].pixels[0],
        &pimpl->dest_pyramid.images[level].pixels[width * height]);
}

} // namespace SVIS

// Routines for foveating and unfoveating images.
//
// Copyright (C) 2001-2006
// Center for Vision and Image Sciences
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Fri Aug 18 13:35:10 CDT 2006

#ifndef FOVEATE_H
#define FOVEATE_H

#include "ecc.h"
#include "filter.h"
#include "mask.h"
#include "region.h"
#include "image.h"
#include <vector>

namespace SVIS
{

struct FoveationPyramid
{
    void Create (Image &base, unsigned levels);
    void Reduce ();
    unsigned levels;
    std::vector<Image> images;
    int fixation_x;
    int fixation_y;
    std::vector<std::vector<Region> > regions;
    FoveationPyramid () { }
    ~FoveationPyramid () { }

    private:
    // This member provides temporary storage that gets deallocated
    // automatically when the object is destroyed.
    std::vector<AutoImage> buffers;
    // Disable copying
    FoveationPyramid (const FoveationPyramid &);
    FoveationPyramid &operator= (const FoveationPyramid &);
};

struct FoveationMasks
{
    void Create (const AutoImage &resmap, unsigned levels);
    unsigned levels;
    std::vector<AutoImage> masks;
    // Center x and y values tell where the center of the mask lies
    // relative to some arbitrary point, like, for example, the center of
    // the retina.
    std::vector<int> center_xs;
    std::vector<int> center_ys;
    std::vector<std::vector<Region> > regions;
};

// Encode a pyramid given its masks and the fixation point
void FoveationEncode (FoveationPyramid &p, const FoveationMasks &masks, int x, int y);

// Decode a pyramid given its masks and a place to decode it into
void FoveationDecode (const FoveationPyramid &src, const FoveationMasks &masks, FoveationPyramid &dest);

} // namespace SVIS

#endif /* FOVEATE_H */

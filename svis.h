// Space Variant Imaging System
//
// Copyright (C) 2003-2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Mon Aug 21 18:26:52 CDT 2006

#ifndef SVIS_H
#define SVIS_H

#include <memory>
#include <vector>

namespace SVIS
{

// Create a resolution map.
void CreateResmap (unsigned width,
    unsigned height,
    std::vector<unsigned char> &pixels,
    double halfres,
    double resmap_fov_deg);

// A Codec encodes and decodes grayscale images.
class CODEC
{
    public:
    // Create a grayscale codec for encoding 'src_image' to
    // 'dest_image'.
    CODEC (unsigned width,
        unsigned height,
        unsigned char *src_image,
        unsigned char *dest_image,
        unsigned pyramid_levels = 5);
    ~CODEC ();
    // Return dimensions specified in ctor
    unsigned GetImageSize () const;
    unsigned GetWidth () const;
    unsigned GetHeight () const;
    // The image buffer in 'p' must match the size specified in the
    // constructor, i.e. 'width' * 'height'.
    void SetSrcImage (unsigned char *p);
    void SetDestImage (unsigned char *p);
    unsigned PyramidLevels () { return pyramid_levels; }


    // Set the resolution map used to encode the image
    void SetResmap (unsigned width,
        unsigned height,
        const std::vector<unsigned char> &pixels) const;
    void GetMask (unsigned level,
        unsigned &width,
        unsigned &height,
        std::vector<unsigned char> &pixels) const;

    // Encode/decode routines
    void Reduce ();
    void GetReducedImage (unsigned level,
        unsigned &width,
        unsigned &height,
        std::vector<unsigned char> &pixels) const;
    void Encode (int x, int y);
    void GetEncodedImageBlocks (unsigned level,
        std::vector<unsigned> &x,
        std::vector<unsigned> &y,
        std::vector<unsigned> &width,
        std::vector<unsigned> &height,
        std::vector<std::vector<unsigned char> > &pixels) const;
    void Decode ();
    void GetDecodedImage (unsigned level,
        unsigned &width,
        unsigned &height,
        std::vector<unsigned char> &pixels) const;

    private:
    unsigned pyramid_levels;
    struct CODECImpl;
    std::auto_ptr<CODECImpl> pimpl;
};

} // namespace SVIS

#endif // SVIS_H

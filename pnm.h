// Portable network graphics
//
// Copyright (C) 2005 Jeff Perry
// Center for Perceptual Systems, University of Texas at Austin
//
// jsp Tue Dec 20 14:19:11 CST 2005

#ifndef PNM_H
#define PNM_H

#include <cassert>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace PNM
{

class Image
{
    public:
    Image () :
        width (0),
        height (0),
        depth (1)
    {
    }
    Image (unsigned w, unsigned h, unsigned d)
    {
        SetSize (w, h, d);
    }
    ~Image ()
    {
    }
    unsigned GetSize () const
    {
        return width * height * depth;
    }
    void SetSize (unsigned w, unsigned h, unsigned d)
    {
        width = w;
        height = h;
        depth = d;
        pixels.resize (width * height * depth);
    }
    unsigned GetWidth () const
    {
        return width;
    }
    void SetWidth (unsigned w)
    {
        width = w;
        pixels.resize (width * height * depth);
    }
    unsigned GetHeight () const
    {
        return height;
    }
    void SetHeight (unsigned h)
    {
        height = h;
        pixels.resize (width * height * depth);
    }
    unsigned GetPixelDepth () const
    {
        return depth;
    }
    void SetPixelDepth (unsigned d)
    {
        depth = d;
        pixels.resize (width * height * depth);
    }
    std::string GetComments () const
    {
        return comments;
    }
    void SetComments (const std::string &c)
    {
        comments = c;
    }
    const std::vector<unsigned char> &GetPixels () const
    {
        return pixels;
    }
    unsigned char *GetPixelsAddress ()
    {
        return &pixels[0];
    }
    unsigned char GetPixel (unsigned x, unsigned y, unsigned d)
    {
        size_t i = (y * width + x) * depth + d;
        assert (i < pixels.size ());
        return pixels[i];
    }
    void SetPixel (unsigned x, unsigned y, unsigned d, unsigned char p)
    {
        size_t i = (y * width + x) * depth + d;
        assert (i < pixels.size ());
        pixels[i] = p;
    }
    // Set all pixels in the image to 'p'
    void SetPixels (unsigned char p)
    {
        pixels.clear (); // probably won't dealloc
        pixels.resize (GetSize (), p); // set them all to 'p'
    }
    void SetPixels (const std::vector<unsigned char> &p)
    {
        if (p.size () != width * height * depth)
            throw std::runtime_error ("Incorrect pixel buffer size");
        pixels = p;
    }
    // Relational operators
    friend bool operator== (const Image &i1, const Image &i2)
    {
        return i1.width == i2.width &&
            i1.height == i2.height &&
            i1.comments == i2.comments &&
            (memcmp (&i1.pixels[0], &i2.pixels[0], i1.width * i1.height * i1.depth) == 0);
    }
    // I/O
    friend std::istream& operator>> (std::istream &s, Image &i)
    {
        char ch;

        s >> ch;

        if (ch != 'P')
            throw std::runtime_error ( "Invalid PNM magic number");

        s >> ch;

        switch (ch)
        {
            case '5':
            i.depth = 1;
            break;
            case '6':
            i.depth = 3;
            break;
            default:
            throw std::runtime_error ("Unsupported PNM file type: Only PGM and PPM are supported");
        }

        i.GetComment (s);
        s >> i.width;

        i.GetComment (s);
        s >> i.height;

        unsigned maxval;

        s >> maxval;

        if (maxval > 255)
            throw std::runtime_error ("Only 8 bpp greyscale or 24 bpp RGB is supported");

        // Read a single WS
        s.get (ch);

        if (s.eof ())
            throw std::runtime_error ("Unexpected EOF");

        if (!s)
            throw std::runtime_error ("Error reading from stream");

        i.pixels.resize (i.width * i.height * i.depth);

        const std::streamsize sz =
            static_cast<std::streamsize> (i.pixels.size ());
        s.read (reinterpret_cast<char *> (&i.pixels[0]), sz);

        if (s.eof ())
            throw std::runtime_error ("Unexpected EOF");

        if (!s)
            throw std::runtime_error ("Error reading from stream");

        return s;
    }
    friend std::ostream& operator<< (std::ostream &s, const Image &i)
    {
        s << 'P';

        switch (i.depth)
        {
            case 1:
            s << '5';
            break;
            case 3:
            s << '6';
            break;
            default:
            throw std::runtime_error ("Unknown PNM pixel depth");
        }

        s << '\n';
        s << i.comments;
        s << '\n';
        s << i.width;
        s << ' ';
        s << i.height;
        s << '\n';
        s << 255;
        s << '\n';

        const std::streamsize sz =
            static_cast<std::streamsize> (i.pixels.size ());
        s.write (reinterpret_cast<const char *> (&i.pixels[0]), sz);

        if (!s)
            throw std::runtime_error ("Error writing to stream");

        return s;
    }
    void GetComment (std::istream &s)
    {
        // Ignore whitespace
        s >> std::ws;

        // A '#' marks a comment line
        while (s.peek () == '#')
        {
            const unsigned LINE_LENGTH = 256;
            char line[LINE_LENGTH];
            s.getline (line, LINE_LENGTH);
            comments += line;
            comments += "\n";
            // Ignore whitespace
            s >> std::ws;
        }
    }

    private:
    unsigned width;
    unsigned height;
    unsigned depth;
    std::string comments;
    std::vector<unsigned char> pixels;
};

} // namespace PNM

#endif

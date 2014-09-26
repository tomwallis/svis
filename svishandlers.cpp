// Space Variant Imaging System MATLAB MEX Handler File
//
// Copyright (C) 2003-2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Wed Sep  6 10:46:03 CDT 2006

#include <algorithm>
#include <cassert>
#include <map>
#include "mex.h"
#include "svis.h"
#include <vector>
#include "version.h"
#include <stdexcept>

using namespace std;
using namespace SVIS;

// A Helper for coupling an SVIS codec to the decoded image buffer
class MatlabCODEC
{
    public:
    MatlabCODEC (unsigned width,
        unsigned height,
        unsigned pyramid_levels)
    {
        // Allocate space for the src/dest images
        original.resize (width * height);
        decoded.resize (width * height);
        // Allocate the codec
        codec = new CODEC (width, height, &original[0], &decoded[0], pyramid_levels);
    }
    ~MatlabCODEC ()
    {
        // Deallocate codec
        delete codec;
    }
    void SetSrcImage (unsigned char *src, size_t sz)
    {
        assert (src);
        assert (sz == original.size ());
        copy (src, src + sz, &original[0]);
        codec->Reduce ();
    }
    void GetDestImage (unsigned char *dest, size_t sz)
    {
        assert (dest);
        assert (sz == decoded.size ());
        copy (decoded.begin (), decoded.end (), dest);
    }
    CODEC *SVISCODEC ()
    {
        assert (codec);
        return codec;
    }
    private:
    CODEC *codec;
    vector<unsigned char> original;
    vector<unsigned char> decoded;
};

// MatlabCODEC pointers will be stored in a global container
static vector<MatlabCODEC *> codecs;
static const size_t LEVELS = 8; // pyramid levels in CODEC
static bool g_init = false;
static bool g_debug = false;

void svisinit (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs > 0)
        g_debug = static_cast<bool> (*mxGetPr (prhs[0]) != 0.0);
    else
        g_debug = false;

    if (g_debug)
    {
        mexPrintf ("-svisinit\n");
        mexPrintf ("Space Variant Imaging System, version %d.%d\n", MAJOR_VERSION, MINOR_VERSION);
        mexPrintf ("Copyright (C) 2003-2006\n");
        mexPrintf ("Center for Perceptual Systems\n");
        mexPrintf ("University of Texas at Austin\n");
    }

    for (size_t i = 0; i < codecs.size (); ++i)
    {
        if (codecs[i])
        {
            delete codecs[i];
            codecs[i] = 0;
        }
    }
    // Clear it
    codecs.clear ();
    g_init = true;
}

void svisrelease (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (g_debug)
        mexPrintf ("-svisrelease\n");

    svisinit (0, 0, 0, 0);
    g_init = false;
}

void sviscodec (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (!g_init)
        mexErrMsgTxt ("You must call svisinit");

    // Src image
    int ndims = mxGetNumberOfDimensions (prhs[0]);

    if (ndims != 2)
        mexErrMsgTxt ("Input 1 must be a 2D image");

    const int *dims = mxGetDimensions (prhs[0]);

    size_t rows = dims[0];
    size_t cols = dims[1];

    unsigned char *src = static_cast <unsigned char *> (mxGetData (prhs[0]));

    if (g_debug)
        mexPrintf ("-sviscodec, rows=%d,cols=%d,src=%p\n", rows, cols, src);

    try
    {
        MatlabCODEC *c = new MatlabCODEC (rows, cols, LEVELS);
        codecs.push_back (c);
        codecs.back ()->SetSrcImage (src, rows * cols);
    }
    catch (const exception &e)
    {
        mexErrMsgTxt (e.what ());
    }

    // Create a scalar and put a handle into it
    const int d[2] = { 1, 1 };
    plhs[0] = mxCreateNumericArray (2, d, mxDOUBLE_CLASS, mxREAL);
    *mxGetPr (plhs[0]) = static_cast<double> (codecs.size ());
}

void svissetresmap (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (!g_init)
        mexErrMsgTxt ("You must call svisinit");

    size_t c = static_cast<size_t> (*mxGetPr (prhs[0]));

    if (c < 1 || c > codecs.size ())
        mexErrMsgTxt ("Invalid value for 'C'");

    --c; // Make it zero based

    int ndims = mxGetNumberOfDimensions (prhs[1]);

    if (ndims != 2)
        mexErrMsgTxt ("Input 2 must be a 2D resmap");

    const int *dims = mxGetDimensions (prhs[1]);

    size_t rows = dims[0];
    size_t cols = dims[1];
    unsigned char *resmap = static_cast <unsigned char *> (mxGetData (prhs[1]));

    if (g_debug)
        mexPrintf ("-svissetresmap: c=%d,rows=%d,cols=%d,resmap=%p\n", c+1, rows, cols, resmap);

    try
    {
        // This copy is inefficient, but you will also have to create
        // some large masks from the resmap...
        vector<unsigned char> v (&resmap[0], &resmap[rows * cols]);
        codecs[c]->SVISCODEC ()->SetResmap (rows, cols, v);
    }
    catch (const exception &e)
    {
        mexErrMsgTxt (e.what ());
    }
}

void svissetsrc (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (!g_init)
        mexErrMsgTxt ("You must call svisinit");

    size_t c = static_cast<size_t> (*mxGetPr (prhs[0]));

    if (c < 1 || c > codecs.size ())
        mexErrMsgTxt ("Invalid value for 'C'");

    --c; // Make it zero based

    int ndims = mxGetNumberOfDimensions (prhs[1]);

    if (ndims != 2)
        mexErrMsgTxt ("Input 2 must be a 2D image");

    const int *dims = mxGetDimensions (prhs[1]);

    size_t rows = dims[0];
    size_t cols = dims[1];
    unsigned char *src = static_cast <unsigned char *> (mxGetData (prhs[1]));

    if (g_debug)
        mexPrintf ("-svissetsrc: c=%d,rows=%d,cols=%d,src=%p\n", c+1, rows, cols, src);

    if (codecs[c]->SVISCODEC ()->GetImageSize () != rows * cols)
        mexErrMsgTxt ("The input image is not the correct size");

    try
    {
        codecs[c]->SetSrcImage (src, rows * cols);
    }
    catch (const exception &e)
    {
        mexErrMsgTxt (e.what ());
    }
}

void svisencode (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (!g_init)
        mexErrMsgTxt ("You must call svisinit");

    size_t c = static_cast<size_t> (*mxGetPr (prhs[0]));
    int row = static_cast<int> (*mxGetPr (prhs[1]));
    int col = static_cast<int> (*mxGetPr (prhs[2]));

    if (c < 1 || c > codecs.size ())
        mexErrMsgTxt ("Invalid value for 'C'");

    --c; // Make it zero based

    if (g_debug)
        mexPrintf ("-svisencode: c=%d,row=%d,col=%d\n", c+1, row, col);

    try
    {
        codecs[c]->SVISCODEC ()->Encode (row, col);
        codecs[c]->SVISCODEC ()->Decode ();

        // Create a matrix and copy the decoded image to it
        const int dims[2] = { codecs[c]->SVISCODEC ()->GetWidth (),
            codecs[c]->SVISCODEC ()->GetHeight () };
        plhs[0] = mxCreateNumericArray (2, dims, mxUINT8_CLASS, mxREAL);
        codecs[c]->GetDestImage (
            static_cast<unsigned char *> (mxGetData (plhs[0])),
            static_cast<size_t> (dims[0] * dims[1]));
    }
    catch (const exception &e)
    {
        mexErrMsgTxt (e.what ());
    }
}

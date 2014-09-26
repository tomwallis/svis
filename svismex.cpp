// Mexgen generated this file on Wed Jun  8 12:23:58 2011
// DO NOT EDIT!

#include "mex.h"

void svisinit (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

void svisinit_mexgen (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Check inputs
    if (nrhs < 0)
        mexErrMsgTxt ("This function requires at least 0 arguments.");

    // Check outputs
    if (nlhs > 1)
        mexErrMsgTxt ("Too many return values were specified.");

    // Check input types

    svisinit (nlhs, plhs, nrhs, prhs);

    // Check output types
}

void svisrelease (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

void svisrelease_mexgen (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Check inputs
    if (nrhs != 0)
        mexErrMsgTxt ("This function requires 0 arguments.");

    // Check outputs
    if (nlhs > 1)
        mexErrMsgTxt ("Too many return values were specified.");

    // Check input types
    svisrelease (nlhs, plhs, nrhs, prhs);

    // Check output types
}

void sviscodec (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

void sviscodec_mexgen (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Check inputs
    if (nrhs != 1)
        mexErrMsgTxt ("This function requires 1 arguments.");

    // Check outputs
    if (nlhs > 1)
        mexErrMsgTxt ("Too many return values were specified.");

    // Check input types
    if (!mxIsUint8 (prhs[0]))
        mexErrMsgTxt ("Input 1 must be uint8.");

    sviscodec (nlhs, plhs, nrhs, prhs);

    // Check output types
    if (!plhs[0])
        mexErrMsgTxt ("Output 1 was not allocated.");
    if (!mxIsDouble (plhs[0]))
        mexErrMsgTxt ("Output 1 must be double.");
    if (mxIsComplex (plhs[0]))
        mexErrMsgTxt ("Output 1 may not be complex.");

}

void svissetresmap (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

void svissetresmap_mexgen (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Check inputs
    if (nrhs != 2)
        mexErrMsgTxt ("This function requires 2 arguments.");

    // Check outputs
    if (nlhs > 1)
        mexErrMsgTxt ("Too many return values were specified.");

    // Check input types
    if (!mxIsDouble (prhs[0]))
        mexErrMsgTxt ("Input 1 must be double.");
    if (mxIsComplex (prhs[0]))
        mexErrMsgTxt ("Input 1 may not be complex.");

    if (!mxIsUint8 (prhs[1]))
        mexErrMsgTxt ("Input 2 must be uint8.");

    svissetresmap (nlhs, plhs, nrhs, prhs);

    // Check output types
}

void svissetsrc (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

void svissetsrc_mexgen (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Check inputs
    if (nrhs != 2)
        mexErrMsgTxt ("This function requires 2 arguments.");

    // Check outputs
    if (nlhs > 1)
        mexErrMsgTxt ("Too many return values were specified.");

    // Check input types
    if (!mxIsDouble (prhs[0]))
        mexErrMsgTxt ("Input 1 must be double.");
    if (mxIsComplex (prhs[0]))
        mexErrMsgTxt ("Input 1 may not be complex.");

    if (!mxIsUint8 (prhs[1]))
        mexErrMsgTxt ("Input 2 must be uint8.");

    svissetsrc (nlhs, plhs, nrhs, prhs);

    // Check output types
}

void svisencode (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

void svisencode_mexgen (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Check inputs
    if (nrhs != 3)
        mexErrMsgTxt ("This function requires 3 arguments.");

    // Check outputs
    if (nlhs > 1)
        mexErrMsgTxt ("Too many return values were specified.");

    // Check input types
    if (!mxIsDouble (prhs[0]))
        mexErrMsgTxt ("Input 1 must be double.");
    if (mxIsComplex (prhs[0]))
        mexErrMsgTxt ("Input 1 may not be complex.");

    if (!mxIsDouble (prhs[1]))
        mexErrMsgTxt ("Input 2 must be double.");
    if (mxIsComplex (prhs[1]))
        mexErrMsgTxt ("Input 2 may not be complex.");

    if (!mxIsDouble (prhs[2]))
        mexErrMsgTxt ("Input 3 must be double.");
    if (mxIsComplex (prhs[2]))
        mexErrMsgTxt ("Input 3 may not be complex.");

    svisencode (nlhs, plhs, nrhs, prhs);

    // Check output types
    if (!plhs[0])
        mexErrMsgTxt ("Output 1 was not allocated.");
    if (!mxIsUint8 (plhs[0]))
        mexErrMsgTxt ("Output 1 must be uint8.");

}

typedef void (*MEXFUNCTION) (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

MEXFUNCTION functions[] =
{
    svisinit_mexgen,
    svisrelease_mexgen,
    sviscodec_mexgen,
    svissetresmap_mexgen,
    svissetsrc_mexgen,
    svisencode_mexgen,
};

static const int MAX_FUNCTIONS = sizeof (functions) / sizeof (void (*) ());

void mexFunction (int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if (nrhs < 1)
        mexErrMsgTxt ("Incorrect number of arguments.\n");

    int mrows = mxGetM (prhs[0]);
    int ncols = mxGetN (prhs[0]);

    if (!mxIsDouble (prhs[0]) || mxIsComplex (prhs[0]) || !(mrows == 1 && ncols == 1))
        mexErrMsgTxt ("Input must be a noncomplex scalar double.\n");

    int findex = (int) (*mxGetPr (prhs[0]));

    if (findex < 0 || findex >= MAX_FUNCTIONS)
        mexErrMsgTxt ("Invalid function index.\n");

    functions[findex] (nlhs, plhs, nrhs - 1, prhs + 1);
}

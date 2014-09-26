// PNM Utilities
//
// Copyright (C) 2003-2006
// Center for Perceptual Systems
// University of Texas at Austin
//
// jsp Tue Aug 22 17:12:45 CDT 2006


#ifndef PNM_UTIL_H

#include <fstream>
#include <iostream>
#include "pnm.h"
#include <stdexcept>
#include <string>

using namespace std;

inline void Load (PNM::Image &i, const std::string &fn)
{
    //std::cout << "Reading " << fn << std::endl;
    std::ifstream ifs (fn.c_str (), std::ios::binary);
    if (!ifs)
        throw std::runtime_error ("Could not open file for reading");
    ifs >> i;
    //std::cout << i.GetWidth () << " ";
    //std::cout << i.GetHeight () << " ";
    //std::cout << i.GetPixelDepth () << std::endl;
    //std::cout << i.GetComments () << std::endl;
}

inline void Save (const PNM::Image &i, const std::string &fn)
{
    std::cout << "Writing " << fn << "... ";
    std::ofstream ofs (fn.c_str (), ios::binary);
    if (!ofs)
        throw std::runtime_error ("Could not open file for writing");
    ofs << i;

    std::cout << "<<< Visually inspect this file" << std::endl;
}

#endif // PNM_UTIL_H

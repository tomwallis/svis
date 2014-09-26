// Verify that a statement is true at runtime
//
// Copyright (C) 2004 Jeff Perry
// Center for Perceptual Systems, University of Texas at Austin
//
// jsp created Tue Aug 17 17:20:11 CDT 2004

#ifndef VERIFY_H
#define VERIFY_H

#include <sstream>
#include <stdexcept>

inline void Verify (const char *e, const char *file, unsigned line)
{
    std::stringstream s;
    s << "verification failed in " << file << ", line " << line << ": " << e;
    throw std::runtime_error (s.str ());
}

#define VERIFY(e) (void)((e) || (Verify (#e, __FILE__, __LINE__), 0))

#endif // VERIFY_H

#ifndef GUARD_DIM_TPP
#define GUARD_DIM_TPP

#include <ostream>

#include "Errors.hpp"

/*** Dim members ***/
// constructers
Dim::Dim(int height, int width, int frames)
    : h(height), w(width), f(frames)
{
    check();
}

// check validity
void Dim::check() const
{
    if (h < 1 || w < 1 || f < 1) error("non-positive dimensions");
}

// helper functions
std::ostream& operator<<(std::ostream& os, const Dim& dim) {
    os << dim.height() << " " << dim.width() << " " << dim.frames();
    return os;
}

#endif

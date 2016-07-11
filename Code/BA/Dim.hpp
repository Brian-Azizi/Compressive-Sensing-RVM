#ifndef GUARD_DIM_HPP
#define GUARD_DIM_HPP

#include <ostream>
#include <string>

/***Class for dimensions of signals ***/
class Dim {
public:
    // constructers
    Dim() : h(1), w(1), f(1) { check(); }
    Dim(int height, int width, int frames = 1);

    // print member data
    int height() const { return h; }
    int width() const { return w; }
    int frames() const { return f; }
    int size() const { return h*w*f; }

    // some relational operators
    bool operator==(const Dim&) const;
    bool operator!=(const Dim&) const;
    // check validity
    void check() const;
private:
    int h, w, f;
};

// helper functions
std::ostream& operator<<(std::ostream& os, const Dim& dim);
Dim dim_from_Signal_file(const std::string& fileName);

// include implementation file
#include "Dim.tpp"

#endif

#ifndef GUARD_DIM_TPP
#define GUARD_DIM_TPP

#include <ostream>
#include <fstream>
#include <string>

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

bool Dim::operator==(const Dim& arg) const
{
    return (h == arg.h && w == arg.w && f == arg.f);
}

bool Dim::operator!=(const Dim& arg) const
{
    return !(*this == arg);
}

// helper functions
std::ostream& operator<<(std::ostream& os, const Dim& dim) {
    os << dim.height() << " " << dim.width() << " " << dim.frames();
    return os;
}

Dim dim_from_Signal_file(const std::string& inputFile) // get dimensions from an input file
{
    std::ifstream in(inputFile.c_str());
    if(!in) error("couldn't open file ", inputFile);
    
    std::string line;
    double entry;
    
    int rows = 0;
    int cols = 0;
    int frames = 0;
    int prevCols = 0; // for checking if all rows have same #cols
    int prevFramesCols = 0; // for checking if all frames have same width
    int prevRows = 0;		// for checking all frames have same #rows
    bool firstRow = true;
    bool firstFrame = true;
    bool lastLineEmpty = true;

    while(std::getline(in, line)) {
       	if (line.empty()) {
	    if (!lastLineEmpty) { // we've finished a frame
		if (!firstFrame) {
		    if (prevRows != rows) error("frames do not have consistent height in file ", inputFile);
		    if (prevFramesCols != cols) error("frames do not have consistent width in file ", inputFile);
		} else firstFrame = false;
		prevRows = rows;
		prevFramesCols = cols;		
	    }
	    lastLineEmpty = true;
	    continue;
	} else if (lastLineEmpty) { // we've hit a frames
	    frames++;
	    rows = 0;
	    firstRow = true;
	    lastLineEmpty = false;
	}
	++rows;
	std::istringstream os(line);
	cols = 0;
	while(os >> entry) {
	    ++cols;
	}
	    
	// check that frame has consistent width
	if (firstRow) firstRow = false;
	else {
	    if (cols != prevCols) 
		error("rows must have same number of entries in file ", inputFile);
	}
	prevCols = cols;	
    }

    Dim ret(rows, cols, frames);

    return ret;
}

#endif

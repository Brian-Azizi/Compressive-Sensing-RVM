#ifndef GUARD_SIGNAL_TPP
#define GUARD_SIGNAL_TPP

#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <random>
#include <stdexcept>

#include "Errors.hpp"
#include "Mask.hpp"
#include "SignalBasis.hpp"
#include "Dim.hpp"
#include "SignalSettings.hpp"
#include "LapackFunctions.hpp"

/*** Signal members ***/

template <typename T> Signal<T>::Signal(int height, int width, int frames, bool init)
    : m_height(height), m_width(width), m_frames(frames),
      m_data(new T[height*width*frames]) 
{
    check();
    if(init)
	for (int i = 0; i < height*width*frames; ++i) m_data[i] = 0;
}
template <typename T> Signal<T>::Signal(int height, int width, bool init)
    : m_height(height), m_width(width), m_frames(1),
      m_data(new T[height*width])
{
    check();
    if(init)
	for (int i = 0; i < height*width; ++i) m_data[i] = 0;
}
template <typename T> Signal<T>::Signal(int height, bool init)
    : m_height(height), m_width(1), m_frames(1),
      m_data(new T[height])
{
    check();
    if(init)
	for (int i = 0; i < height; ++i) m_data[i] = 0;
}
template <typename T> Signal<T>::Signal() 
    : m_height(0), m_width(0), m_frames(0),
      m_data(0) 
{
}
template <typename T> Signal<T>::Signal(const Dim& dim, bool init)
    : m_height(dim.height()), m_width(dim.width()), m_frames(dim.frames()),
      m_data(new T[dim.size()])
{
    check();
    if(init) {
	int sz = dim.size();
	for (int i = 0; i < sz; ++i) m_data[i] = 0;
    }
}

template <typename T> Signal<T>::Signal(const Signal<T>& arg)
    : m_height(arg.m_height), m_width(arg.m_width), m_frames(arg.m_frames),
      m_data(new T[arg.m_height*arg.m_width*arg.m_frames]) 
{
    for (int i = 0; i < m_height*m_width*m_frames; ++i) m_data[i] = arg.m_data[i]; 
}
template <typename T> template<typename V>
Signal<T>::Signal(const Signal<V>& arg)
    : m_height(arg.height()), m_width(arg.width()), m_frames(arg.frames()),
      m_data(new T[arg.height()*arg.width()*arg.frames()]) 
{
    for (int i = 0; i < m_height*m_width*m_frames; ++i) 
	m_data[i] = static_cast<T>(arg.data()[i]);
}
template <typename T> Signal<T>& Signal<T>::operator=(const Signal<T>& arg) {
    int sz = arg.size();
    T* p = new T[sz];
    for (int i = 0; i < sz; ++i) p[i] = arg.m_data[i];
    delete[] m_data;  
    m_data = p;
    m_height = arg.m_height;
    m_width = arg.m_width;
    m_frames = arg.m_frames;
    return *this;
}

template<class T> T& Signal<T>::operator() (int i, int j, int k)
{
    return m_data[i*m_width + j + k*m_height*m_width]; 
}
template<class T> T Signal<T>::operator() (int i, int j, int k) const
{
    return m_data[i*m_width + j + k*m_height*m_width]; 
}
template<class T> T& Signal<T>::operator() (int i, int j) 
{
    if (m_frames != 1) error("index unspecified");
    return this->operator()(i, j, 0);
}
template<class T> T Signal<T>::operator() (int i, int j) const 
{
    if (m_frames != 1) error("index unspecified");
    return this->operator()(i, j, 0);
}
template<class T> T& Signal<T>::operator() (int i) 
{
    if (m_width != 1 || m_frames != 1) error("unspecified index");
    return this->operator()(i,0,0);
}
template<class T> T Signal<T>::operator() (int i) const 
{
    if (m_width != 1 || m_frames != 1) error("unspecified index");
    return this->operator()(i,0,0);
}

template <typename T> void Signal<T>::check() const
{
    if (m_height < 1 || m_width < 1 || m_frames < 1)
	error("Bad Signal: ", "non-positive dimensions");
    if (m_data == 0) 
	error("Bad Signal: ", "no data");
    return;
}

template <typename T> void Signal<T>::read(const std::string& inputFile) {
    std::ifstream in(inputFile.c_str());
    if (!in) error("can't open input file ", inputFile);
    
    T number; 
    int idx = 0;
    
    while (in >> number) {
	m_data[idx] = number;
	++idx;
    }
    if (idx != this->size()) error("size of Signal does not match size of input file ", inputFile);
    in.close();
    
    return;
}

template <typename T>
void Signal<T>::putPatch(const Signal<T>& arg, int row, int col, int slice)
{
    if (row < 0 || col < 0 || slice < 0) error("negative indeces");
    if (row + arg.height() > m_height||
	col + arg.width() > m_width ||
	slice + arg.frames() > m_frames) error("patch does not fit inside signal");
   
    int h = arg.height(), w = arg.width(), f = arg.frames();
    for (int k = 0; k < f; ++k)
	for (int i = 0; i < h; ++i)
	    for (int j = 0; j < w; ++j)
		this->operator()(row+i, col+j, slice+k) = arg(i,j,k);
    return;
}

template <typename T>
void Signal<T>::putPatch(const Signal<T>& arg, int row, int col)
{
    if (m_frames != 1) error("unspecified index");
    return this->putPatch(arg, row, col, 0);
}

template <typename T>
void Signal<T>::putPatch(const Signal<T>& arg, int row)
{
    if (m_width != 1) error("unspecified index");
    return this->putPatch(arg, row, 0);
}


template <typename T>
Signal<T> Signal<T>::getPatch(int hIdx, int wIdx, int fIdx,
			      int pH, int pW, int pF) const
{
    if (hIdx < 0 || wIdx < 0 || fIdx < 0) error("negative indeces");
    if (hIdx + pH > m_height ||
	wIdx + pW > m_width ||
	fIdx + pF > m_frames) error("out-of-range access");

    Signal<T> patch(pH, pW, pF, false);
    for(int k = 0; k < pF; ++k) {
	for (int i = 0; i < pH; ++i) {
	    for (int j = 0; j < pW; ++j) {
		patch(i,j,k) = this->operator()(hIdx + i, 
						wIdx + j, fIdx + k);
	    }
	}
    }
    
    return patch;
} 

template <typename T>
Signal<T> Signal<T>::getPatch(int hIdx, int wIdx, int pH, int pW) const 
{
    if (m_frames != 1) error("missing arguments for dimension 3");
    return getPatch(hIdx, wIdx, 0, pH, pW, 1);
}

template <typename T>
Signal<T> Signal<T>::getPatch(int hIdx, int pH) const
{
    if (m_width != 1) error("missing arguments for dimension 2");
    return getPatch(hIdx, 0, pH, 1);
}

template <typename T>
Signal<T> Signal<T>::getSlice(int startRow, int endRow, int startCol, int endCol, int startFrame, int endFrame) const
{
    int patchHeight = endRow - startRow;
    int patchWidth = endCol - startCol;
    int patchFrames = endFrame - startFrame;
    return getPatch(startRow, startCol, startFrame, patchHeight, patchWidth, patchFrames);
}

template <typename T>
Signal<T> Signal<T>::getSlice(int startRow, int endRow, int startCol, int endCol) const
{
    int patchHeight = endRow - startRow;
    int patchWidth = endCol - startCol;    
    return getPatch(startRow, startCol, patchHeight, patchWidth);    
}

template <typename T>
Signal<T> Signal<T>::getSlice(int startRow, int endRow) const
{
    int patchHeight = endRow - startRow;
    return getPatch(startRow, patchHeight);
}

template <class T>
void Signal<T>::fill(const Signal<T>& filler, const Signal<bool>& mask)
{
    // check dimensions match:
    if (m_height != filler.height()
	|| m_width != filler.width()
	|| m_frames != filler.frames() ) error("filler dimensions don't match");
    if (m_height != mask.height()
	|| m_width != mask.width()
	|| m_frames != mask.frames() ) error("mask dimensions don't match");
    
    int sz = this->size();
    for (int i = 0; i < sz; ++i)
	if (mask.data()[i])
	    m_data[i] = filler.data()[i];

    return;
}

template <class T>
void Signal<T>::fill(const Signal<T>& filler)
{
    // check dimensions match:
    if (m_height != filler.height()
	|| m_width != filler.width()
	|| m_frames != filler.frames() ) error("filler dimensions don't match");
    
    int sz = this->size();
    for (int i = 0; i < sz; ++i)
	    m_data[i] = filler.data()[i];

    return;
}

template <class T> 
void Signal<T>::fill(T num)
{
    int sz = this->size();
    for (int i = 0; i < sz; ++i) m_data[i] = num;
    return;
}

template <class T>
void Signal<T>::reshape(int h, int w, int f)
{
    if(this->size() != h*w*f) error("number of elements must not change");
    
    m_height = h;
    m_width = w;
    m_frames = f;
    
    check();
    return;
}

template<class T>
void Signal<T>::reshape(const Dim& dim)
{
    return reshape(dim.height(),dim.width(),dim.frames());
}

template <typename T>
Signal<T> Signal<T>::operator*(T factor) const
{
    Signal<T> ret(this->dim(),false);
    int sz = this->size();
    for (int i = 0; i < sz; ++i) ret.data()[i] = factor * this->data()[i];
    return ret;
}

template<class T>
Signal<T> Signal<T>::operator-() const
{
    Signal<T> ret(this->dim(),false);
    int sz = this->size();
    for (int i = 0; i < sz; ++i) ret.data()[i] = - this->data()[i];
    return ret;
}

// output vector containing indeces for a given raw array index.
template<class T>
Signal<int> Signal<T>::index(int idx) {
    if (idx < 0 || idx >= size()) error("Signal.index: argument out of range");
    Signal<int> ret(3);
    int i, j, k;

    k = 0;
    while (k*m_width*m_height <= idx) ++k;
    --k;			// k done
    
    idx -= k*m_width*m_height;
    i = 0;
    while (i*m_width <= idx) ++i;
    --i;			// i done

    j = idx - i*m_width;	// j done

    ret(0) = i;
    ret(1) = j;
    ret(2) = k;
    
    return ret;
}



/*** Signal helpers ***/
template <typename T, typename V>
Signal<V> operator*(V factor, const Signal<T>& A)
{
    return A.operator*(factor);
}

template <typename T>
Signal<T> vectorize(const Signal<T>& arg)
{
    Signal<T> vec(arg.size(),false);
    for (int i = 0; i < arg.size(); ++i) 
	vec.data()[i] = arg.data()[i];
    
    return vec;
}

template <typename T>
Signal<T> transpose(const Signal<T>& arg)
{
    if (arg.frames() != 1) error("can only take transpose of 2D signal");
    
    Signal<T> ret(arg.width(), arg.height(),false);
    for (int i = 0; i < arg.height(); ++i)
	for (int j = 0; j < arg.width(); ++j)
	    ret(j,i) = arg(i,j);

    return ret;
}

template <typename T> std::ostream& operator<<(std::ostream& os, const Signal<T>& s) 
{
    int h = s.height();
    int w = s.width();
    int f = s.frames();
    for (int k = 0; k < f; ++k) {
	for (int i = 0; i < h; ++i) {
	    for (int j = 0; j < w; ++j) {
		os << s(i,j,k) << " ";
	    }
	    os << "\n";
	}
	if(k!=f-1) os << "\n";
    }   
    return os;
}



template<typename T>
Signal<T> kronecker(const Signal<T>& A, const Signal<T>& B)
{
    if (A.frames() != 1) error("A needs to be at most 2D");
    if (B.frames() != 1) error("B needs to be at most 2D");

    Signal<T> C(A.height()*B.height(), A.width()*B.width());
    for (int i = 0; i < A.height(); ++i) 
	for (int j = 0; j < A.width(); ++j)
	    for (int p = 0; p < B.height(); ++p) 
		for (int q = 0; q < B.width(); ++q) 
		    C(i*B.height()+p,j*B.width()+q) = A(i,j) * B(p,q);
    return C;
}

template<typename T>
Signal<T> matMult(const Signal<T>& A, const Signal<T>& B, double alpha) // does NOT modify A and B. const would be incompatible with blas call
{
    if (A.frames() != 1 || B.frames() != 1) 
	error("multiplication not defined for 3D signals");
    if (A.width() != B.height())
	error("inner matrix dimensions must agree");
    Signal<T> ret(A.height(), B.width(),false);
    blasMatrixMultiplication(A.height(), A.width(), A.data(), B.height(), B.width(),
			     B.data(), ret.height(), ret.width(), ret.data(), alpha);

    /* Old    */
    // for (int i = 0; i < A.height(); ++i)
    // 	for (int j = 0; j < B.width(); ++j)
    // 	    for (int k = 0; k < A.width(); ++k)
    // 		ret(i,j) += A(i,k) * B(k,j);

    return ret;
}

template<typename T>
Signal<T> add(const Signal<T>& A, const Signal<T>& B)
{
    if (A.dim() != B.dim()) 
	error("ADD: Signals must have the same dimensions");
    
    Signal<T> ret(A.dim(),false);
    int sz = A.size();
    for (int i = 0; i < sz; ++i)
	ret.data()[i] = A.data()[i] + B.data()[i];

    return ret;
}


template <typename T>
Signal<T> getTargets (const Signal<T>& corrSignal,
		      const Signal<bool>& sensed)
{
    Signal<T> target(countSensed(sensed));
    int currentIdx = 0;
    int sz = corrSignal.size();
    for (int i = 0; i < sz; ++i) {
	if (sensed(i)) {	// Assumes Signals are vectorized
	    target(currentIdx) = corrSignal(i);
	    ++currentIdx;
	}
    }
    return target;
}


template <typename T>
Signal<T> getDesignMatrix(const Signal<T>& orig,
			  const Signal<bool>& sensed)
{
    int currentIdx = 0;
    int h = orig.height();
    int w = orig.width();
    Signal<T> designMatrix(countSensed(sensed), w);
    
    for (int i = 0; i < h; ++i) {
	if (sensed(i)) {	// assumes vectorized
	    for (int j = 0; j < w; ++j) {		
		designMatrix(currentIdx, j) = orig(i,j); // assumes orig is 2D
	    }
	    ++currentIdx;
	}
    }

    return designMatrix;
}

template<typename T>
Signal<T> reshape(Signal<T> orig, int h, int w, int f) 
{
    int sz = h*w*f;
    if (orig.size() != sz) error("number of elements must not change");

    Signal<T> ret(h, w, f, false);
    for (int i = 0; i < sz; ++i) ret.data()[i] = orig.data()[i];
    return ret;
}

template<typename T>
Signal<T> reshape(Signal<T> orig, Dim dim) 
{
    return reshape(orig, dim.height(), dim.width(), dim.frames());
}

Signal<double> read(const std::string& inputFile, int numFrames)
{
    if (numFrames < 1) error("numFrames must be positive");
    std::ifstream in(inputFile.c_str());
    if(!in) error("couldn't open file ", inputFile);
    
    std::string line;
    double entry;
    
    int rows = 0;
    int cols = 0;
    int prevCols = 0;		// for checking if all rows have same #cols
    bool firstLine = true;
    
    while(std::getline(in, line)) {
	++rows;

	std::istringstream os(line);
	cols = 0;
	while(os >> entry) {
	    ++cols;
	}
	if (firstLine) firstLine = false;
	else 
	    if (cols != prevCols) 
		error("rows must have same number of entries in file ", inputFile);
	
	prevCols = cols;
    }

    // initialize Signal and go back to beginning of the file
    if (rows % numFrames != 0) {
	std::cerr << "numFrames does not divide number of rows in file "
		  << inputFile << ": Setting numFrames == 1 (default)\n\n";
	numFrames = 1;
    }
    
    rows /= numFrames;

    Signal<double> ret(rows,cols, numFrames,false);
    in.clear();
    in.seekg(0, std::ios::beg);

    int idx = 0;
    while(in >> entry) {
	ret.data()[idx] = entry;
	++idx;
    }

    return ret;
}

double norm(const Signal<double>& A)
{
    if (A.width() != 1 || A.frames() != 1) error("can only take norm of a vector");

    double p = 0;    
    int length = A.height();
    for (int i = 0; i < length; ++i) p += A(i)*A(i);
	
    return std::sqrt(p);
}

double dot(const Signal<double>& a, const Signal<double>& b)
{
    if (a.width() != 1 || a.frames() != 1 || b.width() != 1 || b.frames() != 1
	|| a.height() != b.height()) error("arguments must be vectors of the same length");
    double ret = blasDot(a.height(), a.data(), b.height(), b.data());
    // int length = a.height();
    // for (int i = 0; i < length; ++i) ret += a(i)*b(i);

    return ret;
}


Signal<double> readSignal(const std::string& inputFile) // reads a signal from a file. Assumes frames are seperated by empty lines
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

    Signal<double> ret(rows, cols, frames,false);
    in.clear();
    in.seekg(0, std::ios::beg);

    int idx = 0;
    while(in >> entry) {
    	ret.data()[idx] = entry;
    	++idx;
    }

    return ret;
}


// returns inverse of an NxN matrix A
// uses Lapack
Signal<double> inverse(const Signal<double>& A)
{
    int N = A.height();
    if (A.dim() != Dim(N,N,1)) error("input has invalid dimensions");

    Signal<double> invA = A;
    
    int errorCode;
    lapackInverse(invA.data(), N, errorCode);
    //if(errorCode != 0) error("Inverse: matrix inversion failed! Error code", errorCode);

    return invA;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Generate a random matrix by drawing Bernoulli Samples
Signal<double> bernoulliSamples(const Dim& dim, double p, double min, double max)
{
    if (p < 0 || p > 1) error("Bernoulli parameter must be in [0,1]");

    Signal<double> ret(dim, false);
    for (int i = 0; i < dim.size(); ++i) {
	double rnd = rand()/ (double) RAND_MAX;
	if (rnd < p) ret.data()[i] = min;
	else ret.data()[i] = max;
    }
    return ret;
}
Signal<double> eye(const Dim& dim)
{
    if (dim.frames() != 1 || dim.height() != dim.width()) error("eye: dimensions must be for square matrices");

    Signal<double> ret(dim);	// init to zero;
    for (int i = 0; i < ret.size(); i += (ret.width()+1))
	ret.data()[i] = 1;
	
    return ret;
}
Signal<double> eye(int N)
{
    return eye(Dim(N,N));
}
// ADD SUPPORT FOR CUSTOM SEED
Signal<double> gaussianSamples(const Dim& dim, double mean, double stddev)
{
    std::random_device rd;
    std::mt19937 g(rd());
    //std::mt19937 g(0);

    std::normal_distribution<double> d(mean, stddev);
    
    Signal<double> ret(dim, false);
    for (int i = 0; i < ret.size(); ++i)
	ret.data()[i] = d(g);

    return ret;
}

Signal<double> getSensingMatrix(int size, Sensor::mode setting)
{
    Signal<double> ret(size,size);
    switch(setting) {
    case Sensor::mask:
	return eye(size);
    case Sensor::gaussian:
	return gaussianSamples(Dim(size,size), 0, 1/std::sqrt(size));
    case Sensor::bernoulli:
	return bernoulliSamples(Dim(size,size), 0.5, -1/std::sqrt(size), 1/std::sqrt(size));
    default:
	error("Sensing Matrix: invalid setting");
	return Signal<double>();
    }
}


// Mask Helper Functions

int countSensed(const Signal<bool>& sensed)
{
    int count = 0; 
    int sz = sensed.size();
    for (int i = 0; i < sz; ++i) 
	if (sensed.data()[i]) ++count;

    return count;	
}


template <typename T> 
Signal<T> applyMask(const Signal<T>& orig, Signal<bool>& sensed, const Mask& mask)
{
    if (orig.height() != sensed.height() || 
	orig.width() != sensed.width() ||
	orig.frames() != sensed.frames())
	error("Signal and Mask must have same dimensions");

    int h = orig.height();
    int w = orig.width();
    int f = orig.frames();
    double randNum;
    Signal<bool> filterBlock(h,w,f); // initialized to bool(0) == false
    Signal<T> maskSignal(h,w,f);     // initialized to 0

    for (int i = 0; i < h; ++i) {
	for (int j = 0; j < w; ++j) {
	    for (int k = 0; k < f; ++k) {
		randNum = ((double) rand()) / RAND_MAX; //between 0 and 1
		if (randNum > mask.percentage()/100) filterBlock(i,j,k) = false;
		else filterBlock(i,j,k) = true;
	    }
	}
    }

    for (int k = 0; k < f; ++k) {
	for (int i = 0; i < h; ++i) {
	    for (int j = 0; j < w; ++j) {
		switch (mask.setting()) {
		case Mask::uniform:
		    sensed(i,j,k) = filterBlock(i,j,k);
		    break;
		case Mask::timeRays:
		    sensed(i,j,k) = filterBlock(i,j,0);
		    break;
		case Mask::verticalFlicker:
		    sensed(i,j,k) = filterBlock(0,j,k);
		    break;
		case Mask::horizontalFlicker:
		    sensed(i,j,k) = filterBlock(i,0,k);
		    break;
		case Mask::missingFrames:
		    sensed(i,j,k) = filterBlock(0,0,k);
		    break;
		case Mask::verticalLines:
		    sensed(i,j,k) = filterBlock(0,j,0);
		    break;
		case Mask::horizontalLines:
		    sensed(i,j,k) = filterBlock(i,0,0);
		    break;
		}

		if (sensed(i,j,k)) maskSignal(i,j,k) = orig(i,j,k);
		else maskSignal(i,j,k) = 0;
	    }
	}
    }

    return maskSignal;
}

template<class T>
Signal<T> applyMask(const Signal<T>& orig, const Signal<bool>& mask)
{
    if (orig.dim() != mask.dim()) error("orig and mask must have the same dimensions");

    Signal<T> ret(orig.dim());	// initialized to zero
    for (int i = 0; i < orig.size(); ++i) 
	if(mask.data()[i])
	    ret.data()[i] = orig.data()[i]; // else 0

    return ret;
}

/* Quick and dirty attempt at using db2 wavelets
// Signal<double> haarPhiMatrixTranspose(int rows) //db2 phi matrix
// {
//     if (rows < 1) error("need positive number of rows");
//     if (rows%2 != 0) error("number of rows must be even");
    
//     Signal<double> phiT(rows, rows/2); // default initialized to zero
//     double h1 = (1+std::sqrt(3))/(4*std::sqrt(2));
//     double h2 = (3+std::sqrt(3))/(4*std::sqrt(2));
//     double h3 = (3-std::sqrt(3))/(4*std::sqrt(2));
//     double h4 = (1-std::sqrt(3))/(4*std::sqrt(2));

//     for (int i = 0; i < rows; ++i)
// 	for (int j = 0; j < rows/2-1; ++j) {
// 	    if (i == 2*j) phiT(i,j) = h1;
// 	    if (i == 2*j+1) phiT(i,j) = h2;
// 	    if (i == 2*j+2) phiT(i,j) = h3;
// 	    if (i == 2*j+3) phiT(i,j) = h4;
// 	}
    
//     phiT(0,rows/2-1) = h3;
//     phiT(1,rows/2-1) = h4;
//     phiT(rows-2,rows/2-1) = h1;
//     phiT(rows-1,rows/2-1) = h2;
    
//     return phiT;
// }

// Signal<double> haarPsiMatrixTranspose(int rows) //db2 psi matrix
// {
//     if (rows < 1) error("need positive number of rows");
//     if (rows%2 != 0) error("number of rows must be even");
    
//     Signal<double> psiT(rows, rows/2); // default initialized to zero
//     double g4 = -(1+std::sqrt(3))/(4*std::sqrt(2));
//     double g3 = (3+std::sqrt(3))/(4*std::sqrt(2));
//     double g2 = -(3-std::sqrt(3))/(4*std::sqrt(2));
//     double g1 = (1-std::sqrt(3))/(4*std::sqrt(2));

//     for (int i = 0; i < rows; ++i) {
// 	for (int j = 0; j < rows/2-1; ++j) {
// 	    if (i == 2*j) psiT(i,j) = g1;
// 	    if (i == 2*j + 1) psiT(i,j) = g2;
// 	    if (i == 2*j + 2) psiT(i,j) = g3;
// 	    if (i == 2*j + 3) psiT(i,j) = g4;
// 	}
//     }	    
//     psiT(0,rows/2-1) = g3;
//     psiT(1,rows/2-1) = g4;
//     psiT(rows-2,rows/2-1) = g1;
//     psiT(rows-1,rows/2-1) = g2;

//     return psiT;
// }
*/

Signal<double> haarPhiMatrixTranspose(int rows)
{
    if (rows < 1) error("need positive number of rows");
    if (rows%2 != 0) error("number of rows must be even");
    
    Signal<double> phiT(rows, rows/2); // default initialized to zero
    const double irt2 = 1/std::sqrt(2);

    for (int i = 0; i < rows; ++i)
	for (int j = 0; j < rows/2; ++j)
	    if (i == 2*j || i == 2*j + 1) phiT(i,j) = irt2;

    return phiT;
}

Signal<double> haarPsiMatrixTranspose(int rows)
{
    if (rows < 1) error("need positive number of rows");
    if (rows%2 != 0) error("number of rows must be even");
    
    Signal<double> psiT(rows, rows/2); // default initialized to zero
    const double irt2 = 1/std::sqrt(2);

    for (int i = 0; i < rows; ++i) {
	for (int j = 0; j < rows/2; ++j) {
	    if (i == 2*j) psiT(i,j) = irt2;
	    else if (i == 2*j + 1) psiT(i,j) = -irt2;
	}
    }	    

    return psiT;
}

Signal<double> generateLL(int scale, int currentScale, int h, int w, 
			  Signal<double> rPreFactor, Signal<double> cPreFactor)
{
    int const dimDivider = pow(2, scale-1);
    int const currentDimDivider = pow(2, currentScale);
    int const smallH = h/currentDimDivider;
    int const smallW = w/currentDimDivider;
    int const smallestH = h/dimDivider;
    int const smallestW = w/dimDivider;
    
    Signal<double> LL(h*w, smallH*smallW);

    if (currentScale == scale) {
	LL = kronecker(cPreFactor, rPreFactor);
	return LL;
    }

    if (currentScale < scale) {
	Signal<double> new_rPhiT = haarPhiMatrixTranspose(smallH);
	Signal<double> new_rPsiT = haarPsiMatrixTranspose(smallH);
	Signal<double> new_cPhiT = haarPhiMatrixTranspose(smallW);
	Signal<double> new_cPsiT = haarPsiMatrixTranspose(smallW);
	
	Signal<double> rPre_Phi = matMult(rPreFactor,new_rPhiT);
	Signal<double> rPre_Psi = matMult(rPreFactor,new_rPsiT);
	Signal<double> cPre_Phi = matMult(cPreFactor,new_cPhiT);
	Signal<double> cPre_Psi = matMult(cPreFactor,new_cPsiT);
		
	int full_dim1 = h*w;
	int full_dim2 = smallH * smallW / 4;
	Signal<double> tempFull(full_dim1, full_dim2);
	Signal<double> new_LL(full_dim1, full_dim2);

	tempFull = kronecker(cPre_Phi, rPre_Psi); // new LH
	LL.putPatch(tempFull, 0, full_dim2);
	tempFull = kronecker(cPre_Psi, rPre_Phi); // new HL
	LL.putPatch(tempFull, 0, 2*full_dim2);
	tempFull = kronecker(cPre_Psi, rPre_Psi); // new HH
	LL.putPatch(tempFull, 0, 3*full_dim2);

	// Generate Higher scales recursively
	new_LL = generateLL(scale, currentScale+1, h, w, rPre_Phi, cPre_Phi);
	LL.putPatch(new_LL, 0, 0);
	return LL;
    }

    if (currentScale > scale) error("impossible");
}

Signal<double> haarBasis2D(int h, int w, int scale)
{
    Signal<double> rPhiT(h, h/2);	// filters
    Signal<double> rPsiT(h, h/2);
    Signal<double> cPhiT(w, w/2);
    Signal<double> cPsiT(w, w/2);
    
    int sizeFull = h*w;		
    Signal<double> tmp(sizeFull, sizeFull/4);    
    Signal<double> LL(sizeFull, sizeFull/4);

    rPhiT = haarPhiMatrixTranspose(h); // generate filters
    rPsiT = haarPsiMatrixTranspose(h);
    cPhiT = haarPhiMatrixTranspose(w);
    cPsiT = haarPsiMatrixTranspose(w);
    
    Signal<double> basis(sizeFull, sizeFull);
    tmp = kronecker(cPhiT, rPsiT); // LH
    basis.putPatch(tmp,0,sizeFull/4);
    tmp = kronecker(cPsiT, rPhiT); // HL
    basis.putPatch(tmp,0, 2*sizeFull/4);
    tmp = kronecker(cPsiT, rPsiT); // HH
    basis.putPatch(tmp,0, 3*sizeFull/4);

    int startScale = 1;
    tmp = generateLL(scale, startScale, h, w, rPhiT, cPhiT);
    basis.putPatch(tmp, 0, 0);	// LL

    return basis;
}	 

Signal<double> generateLLL(int scale, int currentScale, int h, int w, int f,
			   Signal<double> rPreFactor, Signal<double> cPreFactor,
			   Signal<double> sPreFactor)
{
    int const dimDivider = pow(2, scale-1);
    int const currentDimDivider = pow(2, currentScale);
    int const smallH = h/currentDimDivider;
    int const smallW = w/currentDimDivider;
    int const smallF = f/currentDimDivider;
    int const smallestH = h/dimDivider;
    int const smallestW = w/dimDivider;
    int const smallestF = f/dimDivider;
    
    Signal<double> LLL(h*w*f, smallH*smallW*smallF);

    if (currentScale == scale) {
	int CR_dim1 = w*h, CR_dim2 = smallestW*smallestH /4;
	int full_dim1 = f*w*h;
	int full_dim2 = smallestF*smallestW*smallestH / 8;
	Signal<double> tempCR(CR_dim1, CR_dim2);
	tempCR = kronecker(cPreFactor, rPreFactor);
	LLL = kronecker(sPreFactor, tempCR);

	return LLL;
    }

    if (currentScale < scale) {


	Signal<double> new_rPhiT = haarPhiMatrixTranspose(smallH);
	Signal<double> new_rPsiT = haarPsiMatrixTranspose(smallH);
	Signal<double> new_cPhiT = haarPhiMatrixTranspose(smallW);
	Signal<double> new_cPsiT = haarPsiMatrixTranspose(smallW);
	Signal<double> new_sPhiT = haarPhiMatrixTranspose(smallF);
	Signal<double> new_sPsiT = haarPsiMatrixTranspose(smallF);

	Signal<double> rPre_Phi = matMult(rPreFactor,new_rPhiT);
	Signal<double> rPre_Psi = matMult(rPreFactor,new_rPsiT);
	Signal<double> cPre_Phi = matMult(cPreFactor,new_cPhiT);
 	Signal<double> cPre_Psi = matMult(cPreFactor,new_cPsiT);
	Signal<double> sPre_Phi = matMult(sPreFactor,new_sPhiT);
	Signal<double> sPre_Psi = matMult(sPreFactor,new_sPsiT);

	int CR_dim1 = h*w;
	int CR_dim2 = smallH * smallW / 4;
	int full_dim1 = h*w*f;
	int full_dim2 = smallH * smallW * smallF / 8;
	Signal<double> tempCR(CR_dim1, CR_dim2);
	Signal<double> tempFull(full_dim1, full_dim2);
	Signal<double> new_LLL(full_dim1, full_dim2);

	tempCR = kronecker(cPre_Phi, rPre_Psi); // new LLH
	tempFull = kronecker(sPre_Phi, tempCR);
	LLL.putPatch(tempFull, 0, full_dim2);
	tempCR = kronecker(cPre_Psi, rPre_Phi); // new LHL
	tempFull = kronecker(sPre_Phi, tempCR);
	LLL.putPatch(tempFull, 0, 2*full_dim2);
	tempCR = kronecker(cPre_Psi, rPre_Psi); // new LHH
	tempFull = kronecker(sPre_Phi, tempCR);
	LLL.putPatch(tempFull, 0, 3*full_dim2);

	tempCR = kronecker(cPre_Phi, rPre_Phi); // new HLL
	tempFull = kronecker(sPre_Psi, tempCR);
	LLL.putPatch(tempFull, 0, 4*full_dim2);
	tempCR = kronecker(cPre_Phi, rPre_Psi); // new HLH
	tempFull = kronecker(sPre_Psi, tempCR);
	LLL.putPatch(tempFull, 0, 5*full_dim2);
	tempCR = kronecker(cPre_Psi, rPre_Phi); // new HHL
	tempFull = kronecker(sPre_Psi, tempCR);
	LLL.putPatch(tempFull, 0, 6*full_dim2);
	tempCR = kronecker(cPre_Psi, rPre_Psi); // new HHH
	tempFull = kronecker(sPre_Psi, tempCR);
	LLL.putPatch(tempFull, 0, 7*full_dim2);

	// Generate Higher scales recursively
	new_LLL = generateLLL(scale, currentScale+1, h, w, f, rPre_Phi, cPre_Phi, sPre_Phi);
	LLL.putPatch(new_LLL, 0, 0);

	return LLL;
    }

    if (currentScale > scale) error("impossible");
}


Signal<double> haarBasis(int h, int w, int f, int scale)
{
    if (f == 1) 
	return haarBasis2D(h, w, scale);

    Signal<double> rPhiT(h, h/2);	// filters
    Signal<double> rPsiT(h, h/2);
    Signal<double> cPhiT(w, w/2);
    Signal<double> cPsiT(w, w/2);
    Signal<double> sPhiT(f, f/2);
    Signal<double> sPsiT(f, f/2);
    
    int sizeFull = h*w*f, sizeCR = w*h;
    Signal<double> tmp(sizeFull, sizeFull/8);    
    Signal<double> LLL(sizeFull, sizeFull/8);
    Signal<double> tmpCR(sizeCR, sizeCR/4);

    rPhiT = haarPhiMatrixTranspose(h); // generate filters
    rPsiT = haarPsiMatrixTranspose(h);
    cPhiT = haarPhiMatrixTranspose(w);
    cPsiT = haarPsiMatrixTranspose(w);
    sPhiT = haarPhiMatrixTranspose(f);
    sPsiT = haarPsiMatrixTranspose(f);

    Signal<double> basis(sizeFull, sizeFull);
    tmpCR = kronecker(cPhiT, rPsiT); // LLH
    tmp = kronecker(sPhiT, tmpCR);
    basis.putPatch(tmp,0,sizeFull/8);
    tmpCR = kronecker(cPsiT, rPhiT); // LHL
    tmp = kronecker(sPhiT, tmpCR);
    basis.putPatch(tmp,0, 2*sizeFull/8);
    tmpCR = kronecker(cPsiT, rPsiT); // LHH
    tmp = kronecker(sPhiT, tmpCR);
    basis.putPatch(tmp,0, 3*sizeFull/8);

    
    tmpCR = kronecker(cPhiT, rPhiT); // HLL
    tmp = kronecker(sPsiT, tmpCR);   
    basis.putPatch(tmp, 0, 4*sizeFull/8);
    tmpCR = kronecker(cPhiT, rPsiT); // HLH
    tmp = kronecker(sPsiT, tmpCR);
    basis.putPatch(tmp, 0, 5*sizeFull/8);
    tmpCR = kronecker(cPsiT, rPhiT); // HHL
    tmp = kronecker(sPsiT, tmpCR);
    basis.putPatch(tmp, 0, 6*sizeFull/8);
    tmpCR = kronecker(cPsiT, rPsiT); // HHH
    tmp = kronecker(sPsiT, tmpCR);
    basis.putPatch(tmp, 0, 7*sizeFull/8);
		  
    int startScale = 1;		
    tmp = generateLLL(scale, startScale, h, w, f, rPhiT, cPhiT, sPhiT);
    basis.putPatch(tmp, 0, 0);	// LL

    return basis;
}	 
 
Signal<double> dctMatrix(int dim)
{
    double M = (double) dim;
    Signal<double> dct(dim, dim);
    for (int p = 0; p < dim; ++p) {
	for (int q = 0; q < dim; ++q) {
	    if (p == 0) dct(p,q) = 1.0/std::sqrt(M);
	    else dct(p,q) = std::sqrt(2.0/M)*std::cos(M_PI*(2.0*q+1)*p/(2.0*M));
	}
    }
    return dct;
}

Signal<double> dctBasis2D(int h, int w)
{
    Signal<double> basis(w*h, w*h);
    Signal<double> basisTran(w*h, w*h);
    Signal<double> rowDCT = dctMatrix(h);
    Signal<double> colDCT = dctMatrix(w);

    basisTran = kronecker(colDCT, rowDCT);
    basis = transpose(basisTran);
    return basis;
}

Signal<double> dctBasis(int h, int w, int f)
{
    if (f == 1) 
	return dctBasis2D(h, w);
    
    int dictionarySize = h*w*f;
    //    Signal<double> basis(dictionarySize, dictionarySize);

    Signal<double> rowDCT = dctMatrix(h);
    Signal<double> colDCT = dctMatrix(w);
    Signal<double> sliceDCT = dctMatrix(f);
    
    // Signal<double> tmp(h*w, h*w);
    // Signal<double> basisTran(dictionarySize, dictionarySize);    
    Signal<double> tmp = kronecker(colDCT, rowDCT);
    Signal<double> basisTran = kronecker(sliceDCT, tmp);
    
    Signal<double> basis = transpose(basisTran);
    return basis;
}



Signal<double> getBasis(int height, int width, int frames, SignalBasis::mode basisMode, int scale)
{
    Signal<double> ret(height*width*frames, height*width*frames);
    switch (basisMode) {
    case SignalBasis::haar:
	ret = haarBasis(height, width, frames, scale);
	return ret;
    case SignalBasis::dct:
	ret = dctBasis(height, width, frames);
	return ret;
    }
}

Signal<double> getBasis(Dim dim, SignalBasis::mode basisMode, int scale)
{
    return getBasis(dim.height(), dim.width(), dim.frames(), basisMode, scale);
}


/* NOT YET IMPLEMENTED */
// Signal<double> haarBasisDirect2D(int h, int w, int scale)
// {
//     Signal<double> ret(h*w,h*w);
//     // Haar coefficients
//     double h0 = 1/std::sqrt(2);
//     double h1 = h0;
//     double g0 = h0;
//     double g1 = -h0;
    
//     Signal<double> haarHH(h,w,false);
//     haarHH.fill(h0*h1);
//     Signal<double> haarHG = haarHH;
//     Signal<double> haarGH = haarHH;
//     Signal<double> haarGG = haarHH;

//     for (int i = h/2; i < h; ++i)
// 	for (int j = 0; j < w; ++j)
// 	    haarHG(i,j) = - haarHH(i,j);
    
//     for (int j = w/2; j < w; ++j)
// 	for (int i = 0; i < h; ++i)
// 	    haarGH(i,j) = - haarHH(i,j);

//     for (int j = w/2; j < w; ++j)
// 	for (int i = 0; i < h/2; ++i)
// 	    haarGG(i,j) = -haarHH(i,j);
//     for (int j = 0; j < w/2; ++j)
// 	for (int i = h/2; i < h; ++i)
// 	    haarGG(i,j) = -haarHH(i,j);
    
	 
//     Signal<double> wavelet(h,w); // container holding the 2D wavelet
    
    


//     return ret;
// }    

// Signal<double> haarBasisDirect(int h, int w, int f, int scale)
// {
//     if (f == 1)
// 	return haarBasisDirect2D(h,w,scale);

//     return Signal<double>(h*w*f,h*w*f);
// }


#endif

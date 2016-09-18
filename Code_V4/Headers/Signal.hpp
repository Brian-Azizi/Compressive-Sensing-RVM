#ifndef GUARD_SIGNAL_HPP
#define GUARD_SIGNAL_HPP

#include <iostream>

#include "Dim.hpp"
#include "Errors.hpp"
#include "Mask.hpp"
#include "SignalBasis.hpp"
#include "Sensor.hpp"

/*** Main Signal class that is heavily used throughout the program.
     Supports 1D,2D and 3D signal, ***/

template <typename T> class Signal{
private:
    T* m_data; 			// Underlying array
    int m_height, m_width, m_frames; // Dimensions
public:
    // constructers (if init is true, initialize to zero.
    Signal(int height, int width, int frames, bool init=true);
    Signal(int height, int width, bool init=true);
    Signal(int height, bool init=true);
    Signal();
    Signal(const Dim& dim, bool init=true);

    // copy constructers
    Signal(const Signal&);
    template <typename V> Signal(const Signal<V>&);
    Signal& operator=(const Signal& arg);

    // destructer
    ~Signal() {	delete[] m_data; }

    // print member data
    int height() const { return m_height; }
    int width() const {	return m_width; }
    int frames() const { return m_frames; }
    int size() const { return m_height*m_width*m_frames; }
    Dim dim() const { return Dim(m_height, m_width, m_frames); }
    T* const data() const{ return m_data; } // Warning: allows access to raw data even if Signal was declared const!

    // access to member data
    T* data() {	return m_data; } // Careful: exposes internal data, Does not protect against modification of pointer
    T& operator() (int i, int j, int k);
    T operator() (int i, int j, int k) const;
    T& operator() (int i, int j);
    T operator() (int i, int j) const;
    T& operator() (int i);
    T operator() (int i) const;

    /* fill Signal with data */
    // fills all the entries that are not covered by mask with the data in filler
    void fill(const Signal<T>& filler, const Signal<bool>& mask); 
    void fill(const Signal<T>& filler); // same as above when every entry of mask == true.
    void fill(T filler);		// fill every entry with the same scalar 
    
    /* return sub blocks (slicing for output) */
    // using start index and size of block
    Signal<T> getPatch(int startRow, int startCol, int startSlice, int blockHeight, int blockWidth, int blockFrames) const; // 3D
    Signal<T> getPatch(int startRow, int startCol, int blockHeight, int blockWidth) const; // 2D
    Signal<T> getPatch(int startRow, int blockHeight) const; // 1D

    // using start index and end index
    Signal<T> getSlice(int startRow, int endRow, int startCol, int endCol, int startFrame, int endFrame) const;
    Signal<T> getSlice(int startRow, int endRow, int startCol, int endCol) const;
    Signal<T> getSlice(int startRow, int endRow) const;

    // replace sub blocks (slicing for input)
    void putPatch(const Signal<T>& arg, int startingRow, int startingCol, int startingSlice);
    void putPatch(const Signal<T>& arg, int startingRow, int startingCol);
    void putPatch(const Signal<T>& arg, int startingRow);
    
    // reshape signal
    void reshape(int height, int width, int frames = 1);
    void reshape(const Dim& dim);
    
    // post multiply by scalar
    Signal<T> operator*(T factor) const;
    
    // unary minus
    Signal<T> operator-() const;
    
    // check validity of member data
    void check() const;

    // replace data with input data read from a file
    void read(const std::string& fileName);

    // output vector containing indeces for a given position in the underlying array.
    Signal<int> index(int pos);
};


/* Mask */
// Generates a mask as specified in argument 'mask'. Masked signal is stored in argument 'sensed'. Return the masked signal
template <typename T> Signal<T> applyMask(const Signal<T>& orig, Signal<bool>& sensed, const Mask& mask);
// Apply an external mask
template <typename T> Signal<T> applyMask(const Signal<T>& orig, const Signal<bool>& mask);
int countSensed(const Signal<bool>& sensed);


/* RVM */
// Deletes rows corresponding to 'false' entries in 'sensed' arguement
template <typename T> Signal<T> getTargets (const Signal<T>& corrSignal, const Signal<bool>& sensed);
template <typename T> Signal<T> getDesignMatrix(const Signal<T>& orig, const Signal<bool>& sensed);


/* Sensor */
Signal<double> bernoulliSamples(const Dim& dim, double prob = 0.5, double min = 0, double max = 1);
Signal<double> eye(const Dim& dim); // identity matrix 
Signal<double> eye(int N); // identity matrix 
Signal<double> gaussianSamples(const Dim& dim, double mean = 0, double stddev = 1);
Signal<double> getSensingMatrix(int size, Sensor::mode mode); // Interface function

// Helpers
template <typename T> std::ostream& operator<<(std::ostream& os, const Signal<T>& s);
template <typename T> Signal<T> reshape(Signal<T> orig, int h, int w, int f = 1);
template <typename T> Signal<T> reshape(Signal<T> orig, Dim dim);
Signal<double> read(const std::string& inputFile, int numFrames = 1); // reads a signal from a file. Explicitly provide number of frames
Signal<double> readSignal(const std::string& inputFile); // reads a signal from a file. Assumes frames are seperated by empty lines


// Linear algebra (for vectors (1D signals) and matrices (2D signals))
template <typename T, typename V> Signal<V> operator*(V factor, const Signal<T>& A);
template <typename T> Signal<T> vectorize(const Signal<T>& arg);
template <typename T> Signal<T> transpose(const Signal<T>& arg);
template<typename T> Signal<T> kronecker(const Signal<T>& A, const Signal<T>& B);
template<typename T> Signal<T> matMult(const Signal<T>& A, const Signal<T>& B, double alpha = 1); // returns alpha*(A*B)
template<typename T> Signal<T> add(const Signal<T>& A, const Signal<T>& B);
double norm(const Signal<double>& A); // norm of vector
double dot(const Signal<double>& a, const Signal<double>& b); // dot product of 2 vectors

Signal<double> inverse(const Signal<double>& A); // output inverse of matrix A


/* Signal Basis */
Signal<double> haarPhiMatrixTranspose(int rows);
Signal<double> haarPsiMatrixTranspose(int rows);
// Internal helpers used by haarBasis and haarBasis2D to recursively compute the parts of the matrix which correspond to the higher scales
Signal<double> generateLL(int scale, int currentScale, int h, int w, Signal<double> rPreFactor, Signal<double> cPreFactor);
Signal<double> generateLLL(int scale, int currentScale, int h, int w, int f, Signal<double> rPreFactor, Signal<double> cPreFactor, Signal<double> sPreFactor);
// outputs 2D haar basis matrix (used by haarBasis(...))
Signal<double> haarBasis2D(int height, int width, int scale);
Signal<double> haarBasis(int height, int width, int frames, int scale); // Outputs 3D haar basis matrix (2D if f==1)
// DCT basis matrix 
Signal<double> dctMatrix(int dim); // 1D
Signal<double> dctBasis2D(int height, int wwidth); // 2D 
Signal<double> dctBasis(int height, int width, int frames); // 3D
// Interface for getting basis matrix basis on the basisMode setting
Signal<double> getBasis(int height, int width, int frames, SignalBasis::mode basisMode, int scale = 1);
Signal<double> getBasis(Dim dim, SignalBasis::mode basisMode, int scale = 1);


Signal<double> haarBasisDirect(int height, int width, int frames, int scale); // Compute Haar Basis matrix directly 
Signal<double> haarBasisDirect2D(int height, int width, int scale);
double haarMother(double);
double haarFather(double);
double haarPhi(int s, int k, double x);
double haarPsi(int s, int k, double x);
double haar2D(int h, int w, int i, int j, int s, double x, double y);
double haar3D(int h, int w, int f, int i, int j, int k, int scale, double x, double y, double z);

Signal<double> gaussianBasis(int height, int width, int frames, double concentration);
Signal<double> gaussianRBF3D(int h, int w, int f, int i, int j, int k, double concentration);

// Implementation
#include "Signal.tpp"


#endif

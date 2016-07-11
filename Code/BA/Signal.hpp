#ifndef GUARD_SIGNAL_HPP
#define GUARD_SIGNAL_HPP

#include <iostream>

#include "Dim.hpp"
#include "Errors.hpp"
#include "Corrupter.hpp"
#include "SignalBasis.hpp"
#include "SignalSettings.hpp"

/*** Signal class ***/
template <typename T> class Signal{
private:
    T* m_data;
    int m_height, m_width, m_frames;
public:
    // constructers
    Signal(int height, int width, int frames);
    Signal(int height, int width);
    Signal(int height);
    Signal();
    Signal(const Dim& dim);

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
    const T* const data() const{ return m_data; }

    // access to member data
    T* data() {	return m_data; }
    T& operator() (int i, int j, int k);
    T operator() (int i, int j, int k) const;
    T& operator() (int i, int j);
    T operator() (int i, int j) const;
    T& operator() (int i);
    T operator() (int i) const;

    // fill Signal with data
    void fill(Signal<T> filler, Signal<bool> mask);
    void fill(Signal<T> filler);
    void fill(T filler);
    
    // return sub blocks (slicing for output)
    Signal<T> getPatch(int startRow, int startCol, int startSlice, int patchHeight, int patchWidth, int patchFrames) const;
    Signal<T> getPatch(int startRow, int startCol, int patchHeight, int patchWidth) const;
    Signal<T> getPatch(int startRow, int patchHeight) const;

    Signal<T> getSlice(int startRow, int endRow, int startCol, int endCol, int startFrame, int endFrame) const;
    Signal<T> getSlice(int startRow, int endRow, int startCol, int endCol) const;
    Signal<T> getSlice(int startRow, int endRow) const;

    // replace sub blocks (slicing for input)
    void putPatch(const Signal<T>& arg, int startingRow, int startingCol, int startingSlice);
    void putPatch(const Signal<T>& arg, int startingRow, int startingCol);
    void putPatch(const Signal<T>& arg, int startingRow);
    
    // reshape signal
    void reshape(int height, int width, int frames);
    void reshape(const Dim& dim);
    
    // post multiply by scalar
    Signal<T> operator*(T factor) const;

    // check validity of member data
    void check();

    // replace data with input data read from a file (use the helper function readSignal instead)
    void read(const std::string& fileName);

};


// helper functions
template <typename T, typename V> Signal<V> operator*(V factor, const Signal<T>& A);
template <typename T> Signal<T> vectorize(const Signal<T>& arg);
template <typename T> Signal<T> transpose(const Signal<T>& arg);
template <typename T> std::ostream& operator<<(std::ostream& os, const Signal<T>& s);

template <typename T> Signal<T> corruptSignal(const Signal<T>& orig, Signal<bool>& sensed, const Corrupter& corr);
template <typename T> Signal<T> corruptSignal(const Signal<T>& orig, const Signal<bool>& mask);
Signal<double> haarPhiMatrixTranspose(int rows);
Signal<double> haarPsiMatrixTranspose(int rows);
template<typename T> Signal<T> kronecker(const Signal<T>& A, const Signal<T>& B);
template<typename T> Signal<T> matMult(const Signal<T>& A, const Signal<T>& B);
Signal<double> generateLL(int scale, int currentScale, int h, int w, Signal<double> rPreFactor, Signal<double> cPreFactor);
Signal<double> haarBasis2D(int h, int w, int scale);
Signal<double> generateLLL(int scale, int currentScale, int h, int w, int f, Signal<double> rPreFactor, Signal<double> cPreFactor, Signal<double> sPreFactor);
Signal<double> haarBasis(int h, int w, int f, int scale);
Signal<double> dctMatrix(int dim);
Signal<double> dctBasis2D(int h, int w);
Signal<double> dctBasis(int h, int w, int f);
Signal<double> getBasis(int height, int width, int frames, SignalBasis::mode basisMode, int scale = 1);
Signal<double> getBasis(Dim dim, SignalBasis::mode basisMode, int scale = 1);
int countSensed(const Signal<bool>& sensed);
template <typename T> Signal<T> getTargets (const Signal<T>& corrSignal, const Signal<bool>& sensed);
template <typename T> Signal<T> getDesignMatrix(const Signal<T>& orig, const Signal<bool>& sensed);
template <typename T> Signal<T> reshape(Signal<T> orig, int h, int w, int f = 1);
template <typename T> Signal<T> reshape(Signal<T> orig, Dim dim);
Signal<double> read(const std::string& inputFile, int numFrames = 1);
double norm(const Signal<double> A);
double dot(const Signal<double> a, const Signal<double> b);
Signal<double> cholesky(const Signal<double>& A);
Signal<double> inversed(const Signal<double>& A);
Signal<double> readSignal(const std::string& inputFile); // reads a signal from a file. Assumes frames are seperated by empty lines
template<class T> void outputSignal(const Signal<T>& S, const std::string& label, const SignalSettings& cfg);


// Implementation
#include "Signal.tpp"


#endif

#ifndef GUARD_SIGNAL_HPP
#define GUARD_SIGNAL_HPP

#include <iostream>

// Error helper functions: simply throws a runtime_error
void error(const std::string& s);
void error(const std::string& s1, const std::string& s2);
void error(const std::string& s1, int i);


// simple class for holding settings for Signal masks
class Corrupter{
public:
    enum corruptionMode {uniform, timeRays, verticalFlicker, horizontalFlicker, missingFrames, verticalLines, horizontalLines};

    Corrupter(double p, corruptionMode m); // Constructers
    Corrupter(corruptionMode m, double p);
    Corrupter();

    double percentage() const { return m_perc; } // printers
    corruptionMode setting() const { return m_setting; }
    std::string settingString() const;

    void setPercentage(double perc); // setters
    void setSetting(corruptionMode setting) { m_setting = setting; }
private:
    corruptionMode m_setting;
    double m_perc;
};

std::string modeToString(const Corrupter::corruptionMode& mode);


/*** Not settings ***/
enum basisFunctionMode {haar, dct};


/***Class for dimensions of signals ***/

class SigDim {
private:
    int h, w, f;
public:
    SigDim(int height, int width, int frames = 1) 
	: h(height), w(width), f(frames) {
	if (height < 1 || width < 1 || frames < 1) error("non-positive dimensions");
    }
    int height() const { return h; }
    int width() const { return w; }
    int frames() const { return f; }
    int size() const { return h*w*f; }
};
    
std::ostream& operator<<(std::ostream& os, const SigDim& dim);


/*** Signal class ***/

template <typename T> class Signal{
private:
    T* m_data;
    int m_height, m_width, m_frames;
public:
    Signal(int height, int width, int frames);
    Signal(int height, int width);
    Signal(int height);
    Signal();
    Signal(const SigDim& dim);
    Signal(const Signal&);
    template <typename V> Signal(const Signal<V>&);
    Signal& operator=(const Signal& arg);
    ~Signal() {	delete[] m_data; }
    int height() const { return m_height; }
    int width() const {	return m_width; }
    int frames() const { return m_frames; }
    int size() const { return m_height*m_width*m_frames; }
    SigDim dim() const { return SigDim(m_height, m_width, m_frames); }
    const T* const data() const{ return m_data; }
    T* data() {	return m_data; }
    T& operator() (int i, int j, int k);
    T operator() (int i, int j, int k) const;
    T& operator() (int i, int j);
    T operator() (int i, int j) const;
    T& operator() (int i);
    T operator() (int i) const;
    void check();
    void read(const std::string& fileName);
    void putPatch(const Signal<T>& arg, int startingRow, int startingCol, int startingSlice);
    void putPatch(const Signal<T>& arg, int startingRow, int startingCol);
    void putPatch(const Signal<T>& arg, int startingRow);
    Signal<T> getPatch(int startRow, int startCol, int startSlice, int patchHeight, int patchWidth, int patchFrames) const;
    Signal<T> getPatch(int startRow, int startCol, int patchHeight, int patchWidth) const;
    Signal<T> getPatch(int startRow, int patchHeight) const;
    void fill(Signal<T> filler, Signal<bool> mask);
    void fill(Signal<T> filler);
    void fill(T filler);
    void reshape(int height, int width, int frames);
    void reshape(const SigDim& dim);
    Signal<T> operator*(T factor) const;
};

template <typename T, typename V> Signal<V> operator*(V factor, const Signal<T>& A);
template <typename T> Signal<T> vectorize(const Signal<T>& arg);
template <typename T> Signal<T> transpose(const Signal<T>& arg);
template <typename T> std::ostream& operator<<(std::ostream& os, const Signal<T>& s);
template <typename T> Signal<T> corruptSignal(const Signal<T>& orig, Signal<bool>& sensed, const Corrupter& corr);
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
Signal<double> getBasis(int height, int width, int frames, basisFunctionMode basisMode, int scale = 1);
Signal<double> getBasis(SigDim dim, basisFunctionMode basisMode, int scale = 1);
int countSensed(const Signal<bool>& sensed);
template <typename T> Signal<T> getTargets (const Signal<T>& corrSignal, const Signal<bool>& sensed);
template <typename T> Signal<T> getDesignMatrix(const Signal<T>& orig, const Signal<bool>& sensed);
template <typename T> Signal<T> reshape(Signal<T> orig, int h, int w, int f = 1);
template <typename T> Signal<T> reshape(Signal<T> orig, SigDim dim);
Signal<double> read(const std::string& inputFile, int numFrames = 1);
double norm(const Signal<double> A);
double dot(const Signal<double> a, const Signal<double> b);
Signal<double> cholesky(const Signal<double>& A);
Signal<double> inversed(const Signal<double>& A);
Signal<double> readSignal(const std::string& inputFile); // reads a signal from a file. Assumes frames are seperated by empty lines
template<class T> void outputSignal(const Signal<T>& S, std::string label); // not finished yet


// Implementation
#include "Signal.tpp"


#endif

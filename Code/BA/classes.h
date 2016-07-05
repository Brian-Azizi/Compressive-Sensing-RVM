void error(const std::string& s) {
    throw std::runtime_error(s);
}
void error(const std::string& s1, const std::string& s2) {
    error(s1 + s2);
}
void error(const std::string& s1, int i) {
    std::ostringstream os;
    os << s1 << ": " << i;
    error(os.str());
}

static const std::string settingsString[] = {"uniform", "timeRays", "verticalFlicker", "horizontalFlicker", "missingFrames", "verticalLines", "horizontalLines"};

struct Corrupter{
    enum corruptionMode {uniform, timeRays, verticalFlicker, horizontalFlicker, missingFrames, verticalLines, horizontalLines};
    corruptionMode setting;
    std::string getString() {return settingsString[setting];}
    double perc;
    Corrupter(double p, corruptionMode m) {
	perc = p;
	setting = m;
    }
    Corrupter(corruptionMode m, double p) {
	perc = p;
	setting = m;
    }
};

/*** Not settings ***/
enum basisFunctionMode {haar, dct};
std::string basisFunctionStrings[] = {"haar", "dct"};

int const startScale = 1;	// Keep it at 1. Code doesn't work well at all if we skip 1.
/*** ***/

struct SigDim {
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
    
std::ostream& operator<<(std::ostream& os, const SigDim& dim) {
    os << dim.height() << " " << dim.width() << " " << dim.frames();
    return os;
}

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
    ~Signal() {
	delete[] m_data; }
    int height() const {
	return m_height; }
    int width() const {
	return m_width; }
    int frames() const {
	return m_frames; }
    int size() const {
	return m_height*m_width*m_frames; }
    const T* const  data() const{
	return m_data; }
    T* data() {
	return m_data; }
    T& operator() (int i, int j, int k) {
	return m_data[i*m_width + j + k*m_height*m_width]; }
    T operator() (int i, int j, int k) const {
	return m_data[i*m_width + j + k*m_height*m_width]; }
    T& operator() (int i, int j) {
	if (m_frames != 1) error("index unspecified");
	return this->operator()(i, j, 0);
    }
    T operator() (int i, int j) const {
	if (m_frames != 1) error("index unspecified");
	return this->operator()(i, j, 0);
    }
    T& operator() (int i) {
	if (m_width != 1 || m_frames != 1) error("unspecified index");
	return this->operator()(i,0,0);
    }
    T operator() (int i) const {
	if (m_width != 1 || m_frames != 1) error("unspecified index");
	return this->operator()(i,0,0);
    }
    void check();
    void read(const std::string& fileName);
    void putPatch(const Signal<T>& arg, int startingRow, 
		  int startingCol, int startingSlice);
    void putPatch(const Signal<T>& arg, int startingRow,
		  int startingCol);
    void putPatch(const Signal<T>& arg, int startingRow);
    Signal<T> getPatch(int startRow, int startCol, int startSlice,
		       int patchHeight, int patchWidth, int patchFrames) const;
    Signal<T> getPatch(int startRow, int startCol,
		       int patchHeight, int patchWidth) const;
    Signal<T> getPatch(int startRow, int patchHeight) const;
    void fill(Signal<T> filler, Signal<bool> mask);
    void fill(Signal<T> filler);
    void fill(T filler);
    SigDim dim() const { return SigDim(m_height, m_width, m_frames); }
    void reshape(int height, int width, int frames);
    void reshape(const SigDim& dim);
};

template <typename T> Signal<T>::Signal(int height, int width, int frames)
    : m_height(height), m_width(width), m_frames(frames),
      m_data(new T[height*width*frames]) 
{
    check();
    for (int i = 0; i < height*width*frames; ++i) m_data[i] = 0;
}
template <typename T> Signal<T>::Signal(int height, int width)
    : m_height(height), m_width(width), m_frames(1),
      m_data(new T[height*width])
{
    check();
    for (int i = 0; i < height*width; ++i) m_data[i] = 0;
}
template <typename T> Signal<T>::Signal(int height)
    : m_height(height), m_width(1), m_frames(1),
      m_data(new T[height])
{
    check();
    for (int i = 0; i < height; ++i) m_data[i] = 0;
}
template <typename T> Signal<T>::Signal() 
    : m_height(0), m_width(0), m_frames(0),
      m_data(0) {}

template <typename T> Signal<T>::Signal(const SigDim& dim)
    : m_height(dim.height()), m_width(dim.width()), m_frames(dim.frames()),
      m_data(new T[dim.size()])
{
    check();
    int sz = dim.size();
    for (int i = 0; i < sz; ++i) m_data[i] = 0;
}

template <typename T> Signal<T>::Signal(const Signal<T>& arg)
    : m_height(arg.m_height), m_width(arg.m_width), m_frames(arg.m_frames),
      m_data(new T[arg.m_height*arg.m_width*arg.m_frames]) 
{
    for (int i = 0; i < m_height*m_width*m_frames; ++i) m_data[i] = arg.m_data[i];
    check();
}

template <typename T> template<typename V>
Signal<T>::Signal(const Signal<V>& arg)
    : m_height(arg.height()), m_width(arg.width()), m_frames(arg.frames()),
      m_data(new T[arg.height()*arg.width()*arg.frames()]) 
{
    for (int i = 0; i < m_height*m_width*m_frames; ++i) 
	m_data[i] = static_cast<T>(arg.data()[i]);
    check();
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
    check();
    return *this;
}


template <typename T> std::ostream& operator<<(std::ostream& os, const Signal<T>& s) {
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
    }   
    return os;
}

template <typename T> void Signal<T>::check() {
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
Signal<T> transpose(const Signal<T>& arg)
{
    if (arg.frames() != 1) error("can only take transpose of 2D signal");
    
    Signal<T> ret(arg.width(), arg.height());
    for (int i = 0; i < arg.height(); ++i)
	for (int j = 0; j < arg.width(); ++j)
	    ret(j,i) = arg(i,j);

    return ret;
}

template <typename T>
Signal<T> Signal<T>::getPatch(int hIdx, int wIdx, int fIdx,
			      int pH, int pW, int pF) const
{
    if (hIdx < 0 || wIdx < 0 || fIdx < 0) error("negative indeces");
    if (hIdx + pH > m_height ||
	wIdx + pW > m_width ||
	fIdx + pF > m_frames) error("out-of-range access");

    Signal<T> patch(pH, pW, pF);
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
Signal<T> Signal<T>::getPatch(int hIdx, int wIdx, 
			      int pH, int pW) const 
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
Signal<T> vectorize(const Signal<T>& arg)
{
    Signal<T> vec(arg.size());
    for (int i = 0; i < arg.size(); ++i) 
	vec.data()[i] = arg.data()[i];
    
    return vec;
}

template <class T>
void Signal<T>::fill(Signal<T> filler, Signal<bool> mask)
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
void Signal<T>::fill(Signal<T> filler)
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
void Signal<T>::reshape(int h, int w, int f = 1)
{
    if(this->size() != h*w*f) error("number of elements must not change");
    
    m_height = h;
    m_width = w;
    m_frames = f;
    
    check();
    return;
}

template<class T>
void Signal<T>::reshape(const SigDim& dim)
{
    return reshape(dim.height(),dim.width(),dim.frames());
}

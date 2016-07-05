template <typename T> class RVM 
{
private:
    double m_sigma;		// std deviation
    double m_deltaML_thresh;
    bool m_printProgress;
    const static double zeroFactor = 1e-12;
public:
    RVM() 
	: m_sigma(1), m_deltaML_thresh(1),
	m_printProgress(true) { };
    void setSigma(double sigma);
    void setDeltaML(double deltaML);
    void setPrintProgress(bool print);
};

template<class T>
void RVM<T>::setSigma(double sigma) 
{
    if (sigma < zeroFactor) error("sigma must be positive");
    m_sigma = sigma;
}

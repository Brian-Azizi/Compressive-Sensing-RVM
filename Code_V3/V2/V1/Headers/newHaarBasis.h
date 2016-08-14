template <class T>
void newHaarBasis(T **BASIS, int h, int w, int f, int scale)
{
    /*** Set up the prefactors for the individual elements of the triple low pass fileter ***/    
    int const preFactorDimDivider =  2^(scale-1); 

    T **rowPreFactor = new T*[h];
    for (int i = 0; i < h; ++i) {
	rowPreFactor[i] = new T*[h/preFactorDimDivider];
    }
    T **colPreFactor = new T*[w];
    for (int i = 0; i < w; ++i) {
	colPreFactor[i] = new T*[w/preFactorDimDivider];
    }
    T **slicePreFactor = new T*[f];
    for (int i = 0; i < f; ++i) {
	slicePreFactor[i] = new T*[f/preFactorDimDivider];
    }

    int const filterDimDivider1 = 2^(scale-1);
    int const filterDimDivider2 = 2^scale;

    return;
}

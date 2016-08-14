template <class T>
void dctMatrix(T **DCT, int dim)
{
    double M = (double) dim;
    for (int p = 0; p < dim; ++p) {
	for (int q = 0; q < dim; ++q) {
	    if (p == 0) {
		DCT[p][q] = 1/sqrt(M);
	    } else {
		DCT[p][q] = sqrt(2/M)*std::cos(M_PI*(2*q+1)*p/(2*M));
	    }
	}
    }
    
    return;
}

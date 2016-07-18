#ifndef GUARD_LAPACK_FUNCTIONS
#define GUARD_LAPACK_FUNCTIONS

extern "C" {
    // LU decomposistion of a general matrix
    void dgetrf_(int* M, int* N, double* A, int* lda, int* IPIV, int* INFO);
    
    // generate inverse of a matrix given its LU decomposition
    void dgetri_(int* N, double* A, int* lda, int* IPIV, double* WORK, int* lwork, int* INFO);
}

void lapackInverse(double* A, int N, int& errorCode)
{
    int *IPIV = new int[N+1];
    int LWORK = N*N;
    double *WORK = new double[LWORK];
    int INFO;

    dgetrf_(&N,&N,A,&N,IPIV,&INFO);
    if(INFO != 0) {
	errorCode = INFO;
	delete[] IPIV;
	delete[] WORK;
	return;
    }

    dgetri_(&N,A,&N,IPIV,WORK,&LWORK,&INFO);
    if(INFO != 0) {
	errorCode = INFO;
	delete[] IPIV;
	delete[] WORK;
	return;
    }
    
    delete[] IPIV;
    delete[] WORK;
}

#endif

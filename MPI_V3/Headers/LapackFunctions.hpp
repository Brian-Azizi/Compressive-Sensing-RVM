#ifndef GUARD_LAPACK_FUNCTIONS
#define GUARD_LAPACK_FUNCTIONS

#include "Errors.hpp"

extern "C" {
    // LU decomposistion of a general matrix
    void dgetrf_(int* M, int* N, double* A, int* lda, int* IPIV, int* INFO);
    
    // generate inverse of a matrix given its LU decomposition
    void dgetri_(int* N, double* A, int* lda, int* IPIV, double* WORK, int* lwork, int* INFO);

    // Matrix - matrix multiplication (BLAS)
    void dgemm_(char* TRANSA, char* TRANSB, const int* M, const int* N, const int* K, double* alpha, double* A, const int* LDA,
		double* B, const int* LDB, double* beta, double *C, const int* LDC);
    
    // Dot product (BLAS)
    double ddot_(int* N, double* X, int* INCX, double* Y, int* INCY);
}

double blasDot(int NX, double* X, int NY, double* Y)
{
    if (NX < 1 || NY < 1) error("BLAS_DOT: input dimensions must be positive");
    if (NX != NY) error("BLAS_DOT: input vectors must have the same length");
    int inc = 1;
    return ddot_(&NX, X, &inc, Y, &inc);
}

void blasMatrixMultiplication(int rowsA, int colsA, double* A, int rowsB, int colsB, double* B, int rowsC, int colsC, double* C, double alpha = 1)
{
    // if (rowsA < 1 || colsA < 1 || rowsB < 1 || colsB < 1 || rowsC < 1 || colsC < 1) error("BLAS: non-positive dimensions!");
    // if (colsA != rowsB) error("BLAS: inner matrix dimensions don't agree");
    // if ((rowsA != rowsC) || (colsB != colsC)) error("BLAS: outer matrix dimensions are inconsistend");
    
    char transA = 'N', transB = 'N';
    //double alpha = 1;
    double beta = 0;
    // somewhat weird order of arguments is to deal with the fact that Blas expects column-major ordering while I use row-major ordering
    dgemm_(&transA, &transB, &colsB, &rowsA, &colsA, &alpha, B, &colsB, A, &colsA, &beta, C, &colsC);    
}

enum transEnum {N,T};

//void blasDGEMM(transEnum transA, transEnum transB

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

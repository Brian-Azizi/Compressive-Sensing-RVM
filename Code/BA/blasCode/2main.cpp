#include <iostream>
#include <cblas.h>

extern "C"{
    double ddot_(const int& N, double* a, int& inca, double* b, int& incb);
    void dgemv_(char& trans, int& M, int& N, double& alpha, double* A, int& lda, double* x, int& incx, double& beta, double* y, int& incy); 
    //    void cblas_dgemv(char& trans, int& M, int& N, double& alpha, double* A, int& lda, double* x, int& incx, double& beta, double* y, int& incy); 
}

int main()
{
    int M = 3;
    int N = 2;
    double* A = new double[M*N];
    
    double* x = new double[N];
    double* y = new double[M];
    
    for (int i = 0; i < M; ++i) {
	for (int j = 0; j < N; ++j) {
	    A[i+j*M] = i + j*M;
	}
	y[i] = 0;
    }
    for (int j = 0; j < N; ++j) {
	x[j] = 2*j+1;
    }

    char trans = 'N';
    double alpha = 1;
    int incx = 1;
    int incy = 1;
    double beta = 0;
    
    cblas_dgemv(CblasColMajor,CblasNoTrans,M,N,1,A,M,x,1,0,y,1);
    //dgemv_(trans,M,N,alpha,A,M,x,incx,beta,y,incy);
    
    std::cout << "A = " << std::endl;
    for (int i = 0; i < M; ++i) {
	for (int j = 0; j < N; ++j) {
	    std::cout << A[i+M*j] << " ";
	}
	std::cout << std::endl;
    }
    
    std::cout << "x = " << std::endl;
    for (int j = 0; j < N; ++j) {
	std::cout << x[j] << std::endl;
    }
    
    std::cout << "y = A*x = " << std::endl;
    for (int i = 0; i < M; ++i) {
	std::cout  << y[i] << std::endl;
    }
}

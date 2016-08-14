#include <iostream>
#include <mpi.h>
#include <cmath>

#include "Headers/Signal.hpp"

int main(int argc, char* argv[])
{
    int nproc, rank;
    MPI_Status st;
    
    Signal<double> full(4,4);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank < 4) {
	for (int i = 0; i < 4; ++i) {
	    full(rank,i) = 10*rank + i;
	}
    }
    
    // collect results on zero
    //double* buffer = new double[4];
    Signal<double> buff(4);
    if (rank == 0) {
	for (int i = 1; i < nproc; ++i) {
	    if ( i < 4) {
		MPI_Recv(buff.data(), 4, MPI_DOUBLE, i, i, MPI_COMM_WORLD, &st);
		for (int j = 0; j < 4; ++j)
		    full(i,j) = buff(j);
	    }
	}
    } else {     
	for (int j = 0; j < 4; ++j) buff(j) = full(rank,j);
	MPI_Send(buff.data(), 4, MPI_DOUBLE, 0, rank, MPI_COMM_WORLD);
    }

    if (rank == 0) std::cout << rank << ":\n" << full << std::endl;
    MPI_Finalize();
    

    
}

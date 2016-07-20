#ifndef GUARD_INIT_HPP
#define GUARD_INIT_HPP

#include "utilities.hpp"

void initialize(double **BASIS, double *targets, double *scaling, double &alpha, double &beta, double noiseStd, double *phi_column, int &in_model, int N, int M)
{
    //std::cout << "Initializing parameters..." << std::endl;
	
	double alpha_min = 1.0e-3;
	double alpha_max = 1.0e3;
	
	// normalize each basis function to unit length and get the scaling for each basis

	for (int basis=0;basis<M;basis++)
		{
			double *column = new double[N];
			for (int rows=0;rows<N;rows++)
			{
				column[rows] = BASIS[rows][basis];
				
			}
						
			scaling[basis] = norm1vector(column, N);
			
			if (scaling[basis]==0)
				scaling[basis] = 1;
	
			for (int rows=0;rows<N;rows++)
				BASIS[rows][basis]  = BASIS[rows][basis]/scaling[basis];	
			delete[] column;
		}
	
		
	//end of normalization
	
	beta = 1/(noiseStd*noiseStd);
	
	//initialize PHI and alpha
	
	//choose the basis function that has the larget projection with the targets
	
	double max_projection = 0;
	
	for (int basis=0;basis<M;basis++)
		{
			double *column = new double[N];
			for (int rows=0;rows<N;rows++)
				{
				column[rows] = BASIS[rows][basis];
				
				}
			double projection = multiply1D1Dnum(column,targets,N);

			if (abs(projection) > abs(max_projection))
				{
				max_projection = projection;
				in_model = basis;
				}
			delete[] column;
		}
	
	for (int rows=0;rows<N;rows++)
		phi_column[rows] = BASIS[rows][in_model];
		
	//calculate alpha
	
	
	double p = beta*multiply1D1Dnum(phi_column,phi_column,N);

	double q = beta*multiply1D1Dnum(phi_column,targets,N);

	alpha = p*p / (q*q - p);

	if (alpha<0)
		alpha = alpha_max;
	
	//std::cout << "	Chosen init basis: " << in_model << std::endl;
	//std::cout << "	Initial alpha: " << alpha << std::endl;
	
	//end of alpha calculation
	

	
	
	//end of initialization
	
}

#endif

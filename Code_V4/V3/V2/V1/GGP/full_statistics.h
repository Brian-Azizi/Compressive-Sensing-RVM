#include <iomanip>

void full_statistics(double **BASIS, double **PHI, int included_basis, double *targets, int *indeces_used, bool *ins_model, double *alphas, double beta, double *mean, double **covariance, double *S_in, double *Q_in, double *S_out, double *Q_out, double *relevance_factors, int N, int M, double **BASIS_PHI, double **BETA_BASIS_PHI, double *BASIS_Targets, double *gamma)
{

	
	double **temp_cova = new double*[included_basis];
		for (int i=0;i<included_basis;i++)
			temp_cova[i] = new double[included_basis];
			
	double **temp_temp_cova = new double*[included_basis];
		for (int i=0;i<included_basis;i++)
			temp_temp_cova[i] = new double[included_basis];		
			
	double **temp_PHI = new double*[N];
		for (int i=0;i<N;i++)
			temp_PHI[i] = new double[included_basis];
			
	double **temp_PHI_transpose = new double*[included_basis];
		for (int i=0;i<included_basis;i++)
			temp_PHI_transpose[i] = new double[N];		
			
	for (int rows=0;rows<N;rows++)
		for (int cols=0;cols<included_basis;cols++)
			temp_PHI[rows][cols] = PHI[rows][cols];
	
	
	transpose2D(temp_PHI, temp_PHI_transpose, included_basis, N);
	
	
	for (int rows=0;rows<included_basis;rows++)
		for (int cols=0;cols<included_basis;cols++)
			temp_temp_cova[rows][cols] = 0;
		
	multiply2D2D(temp_PHI_transpose, temp_PHI, temp_temp_cova, included_basis, included_basis, N);

	
	
	
	for (int rows=0;rows<included_basis;rows++)
		for (int cols=0;cols<included_basis;cols++)
			if (rows==cols)
				temp_temp_cova[rows][cols] = beta*temp_temp_cova[rows][cols] + alphas[indeces_used[rows]];
			else
				temp_temp_cova[rows][cols] = beta*temp_temp_cova[rows][cols];

	double **chol = new double*[included_basis];
		for (int i=0;i<included_basis;i++)
			chol[i] = new double[included_basis];
			
	double **inv_chol = new double*[included_basis];
		for (int i=0;i<included_basis;i++)
			inv_chol[i] = new double[included_basis];		
	
		

	cholesky(temp_temp_cova, chol, included_basis);

	
	
	inversed(chol, included_basis, inv_chol);
	
	
	
	
	double **tran_inv_chol = new double*[included_basis];
		for (int i=0;i<included_basis;i++)
			tran_inv_chol[i] = new double[included_basis];
	
	transpose2D(inv_chol, tran_inv_chol, included_basis, included_basis);
	
	multiply2D2D(inv_chol, tran_inv_chol, temp_cova, included_basis, included_basis, included_basis);
	
	double *temp_mean = new double[included_basis];
	
	double *temp_temp_mean = new double[included_basis];
	
	multiply2D1D(temp_PHI_transpose, targets, temp_temp_mean, included_basis, N);
	
	multiply2D1D(temp_cova, temp_temp_mean, temp_mean, included_basis, included_basis);
	
	for (int rows=0;rows<included_basis;rows++)	
		temp_mean[rows] = beta*temp_mean[rows];

	

	double *y = new double[N];
	
	multiply2D1D(temp_PHI, temp_mean, y, N, included_basis);
	
	double *residual = new double[N];
	
	for (int rows=0;rows<N;rows++)
		residual[rows] = targets[rows] - y[rows];
	
	// calculating sparsity and quality factors
	
	
	
	
	
	double *sum_inv_chol = new double[included_basis];
	
	for (int rows=0; rows<included_basis;rows++)
		sum_inv_chol[rows] = 0;
		
	for (int rows=0;rows<included_basis;rows++)
		for (int cols=0;cols<included_basis;cols++)
			sum_inv_chol[rows] = sum_inv_chol[rows] + inv_chol[rows][cols]*inv_chol[rows][cols];
	
	
	for (int rows=0;rows<included_basis;rows++)
		gamma[rows] = 1 - alphas[indeces_used[rows]]*sum_inv_chol[rows];
	
	
	double **BETA_chol = new double*[M];
		for (int i=0;i<M;i++)
			BETA_chol[i] = new double[included_basis];
		
	
	for (int rows=0;rows<M;rows++)
		for (int cols=0;cols<included_basis;cols++)
			BETA_BASIS_PHI[rows][cols] = beta*BASIS_PHI[rows][cols];		 
	
	multiply2D2D(BETA_BASIS_PHI, inv_chol, BETA_chol, M, included_basis, included_basis);

	for (int rows=0;rows<M;rows++)
		for (int cols=0;cols<included_basis;cols++)
			BETA_chol[rows][cols] = BETA_chol[rows][cols]*BETA_chol[rows][cols];
					
			
	double *sums = new double[M];
	for (int rows=0;rows<M;rows++)
		sums[rows] = 0;
		
	for (int rows=0;rows<M;rows++)
		for (int cols=0;cols<included_basis;cols++)
		sums[rows] = sums[rows] + BETA_chol[rows][cols];
		

	for (int basis=0;basis<M;basis++)
	{
		
		double *column_basis_phi = new double[included_basis];
		
		for (int rows=0;rows<included_basis;rows++)
			column_basis_phi[rows] = BASIS_PHI[basis][rows];

		S_in[basis] = beta - sums[basis];
		Q_in[basis] = beta*BASIS_Targets[basis] - beta*multiply1D1Dnum(column_basis_phi, temp_mean, included_basis);
		
		delete[] column_basis_phi;

		S_out[basis] = S_in[basis];
		Q_out[basis] = Q_in[basis];
		
		if (ins_model[basis])
			{
				S_out[basis] = alphas[basis] * S_in[basis] / (alphas[basis] - S_in[basis]);
				Q_out[basis] = alphas[basis] * Q_in[basis] / (alphas[basis] - S_in[basis]);
			
			}
		
		relevance_factors[basis] = Q_out[basis]*Q_out[basis] - S_out[basis];
		

	}
	
	
	for (int rows=0;rows<included_basis;rows++)
		mean[rows] = temp_mean[rows];
	
	for (int rows=0;rows<included_basis;rows++)
		for (int cols=0;cols<included_basis;cols++)
			covariance[rows][cols] = temp_cova[rows][cols];
	
	for (int rows=0;rows<N;rows++)
		for (int cols=0;cols<included_basis;cols++)
			PHI[rows][cols] = temp_PHI[rows][cols];
	
	
	
	
	delete[] residual;
	delete[] y;
	delete[] temp_mean;
	delete[] temp_temp_mean;
	delete[] sums;
	
	delete[] sum_inv_chol;
	
	for (int i=0;i<included_basis;i++)
		delete[] chol[i];
		delete[] chol; 
	
	for (int i=0;i<M;i++)
		delete[] BETA_chol[i];
		delete[] BETA_chol; 
	
	for (int i=0;i<included_basis;i++)
		delete[] inv_chol[i];
		delete[] inv_chol; 
		
	for (int i=0;i<included_basis;i++)
		delete[] tran_inv_chol[i];
		delete[] tran_inv_chol;	
	
	for (int i=0;i<included_basis;i++)
		delete[] temp_temp_cova[i];
		delete[] temp_temp_cova; 
	
	for (int i=0;i<included_basis;i++)
		delete[] temp_cova[i];
		delete[] temp_cova; 
		
	for (int i=0;i<N;i++)
		delete[] temp_PHI[i];
		delete[] temp_PHI;
		
	for (int i=0;i<included_basis;i++)
		delete[] temp_PHI_transpose[i];
		delete[] temp_PHI_transpose;
		
			
	

}

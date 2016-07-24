#ifndef GUARD_FAST_UPDATES_HPP
#define GUARD_FAST_UPDATES_HPP
// BA edits: argument 6: changed pass-by-reference to pass-by-value
// BA edtis (24/5/16): used templates

#include "full_statistics.hpp"
#include "init.hpp"
#include "utilities.hpp"

template<class T>
void fast_updates(T **BASIS, T *targets, T *coefficients, int N, int M, double noiseStd, T *errors, T **PSI, bool calc, double change_in_ML_threshold, bool printToCOut)
{
    //define constants to guide the execution of the RVM   
    double zeroFactor = 1.0e-12; 	//used to control the numerical error in Q^2 - S , less than this number we set it to zero
    
    /*variable declarations to book-keeping the statistics of the RVM.*/
    double alpha, beta = 0;
    double *gamma = new double[M];
    for (int rows=0;rows<M;rows++)
	gamma[rows] = 0;
    
    int in_model = 0;
    
    double *phi_column = new double[N];
    double *scaling = new double[M];
    
    initialize(BASIS, targets, scaling, alpha, beta, noiseStd, phi_column, in_model, N, M);
    
    
    /*declare variables to be used throughout*/
    
    double **PHI = new double*[N];
    for (int i=0;i<N;i++)
	PHI[i] = new double[M];
    
    
    double *alphas = new double[M];
    for (int rows=0;rows<M;rows++)
	alphas[rows] = 0;
    alphas[in_model] = alpha;
    
    int *indeces_used = new int[M];
    bool *ins_model = new bool[M];
    int *where_is = new int[M];
    
    for (int rows=0;rows<M;rows++)
	where_is[rows] = 0;
    
    for (int rows=0;rows<M;rows++)
	ins_model[rows] = false;
    indeces_used[0] = in_model;
    where_is[in_model] = 0;
    ins_model[in_model] = true;		 
    int included_basis = 1;
    
    for (int rows=0;rows<N;rows++)
	for (int cols=0;cols<included_basis;cols++)
	    PHI[rows][cols] = phi_column[rows];
    
    double *mean = new double[M];
    double **covariance = new double*[M];
    for (int i=0;i<M;i++)
	covariance[i] = new double[M];
    
    double *S_in = new double[M];
    double *Q_in = new double[M];
    double *S_out = new double[M];
    double *Q_out = new double[M];
    double *relevance_factors = new double[M];
    
    
    double **BASIS_transpose = new double*[M];
    for (int i=0;i<M;i++)
	BASIS_transpose[i] = new double[N];
    
    transpose2D(BASIS, BASIS_transpose, M, N);
    
    double **BASIS_PHI = new double*[M];
    for (int i=0;i<M;i++)
	BASIS_PHI[i] = new double[M];
    
    double **BETA_BASIS_PHI = new double*[M];
    for (int i=0;i<M;i++)
	BETA_BASIS_PHI[i] = new double[M];		
    
    multiply2D2D(BASIS_transpose, PHI, BASIS_PHI, M, included_basis, N);
    
    
    for (int rows=0;rows<M;rows++)
	for (int cols=0;cols<included_basis;cols++)
	    BETA_BASIS_PHI[rows][cols] = beta*BASIS_PHI[rows][cols];	
    
    double *BASIS_Targets = new double[M];
    
    multiply2D1D(BASIS_transpose, targets, BASIS_Targets, M, N);
    
    
    uint64 statsTimeS = GetTimeMs64();
    full_statistics(BASIS, PHI, included_basis, targets, indeces_used, ins_model, alphas, beta, mean, covariance, S_in, Q_in, S_out, Q_out, relevance_factors, N, M, BASIS_PHI, BETA_BASIS_PHI, BASIS_Targets, gamma);
    statsTime += GetTimeMs64() - statsTimeS;
    statsIdx++;
    
    int added_basis = 0;
    int count = 0;
    

    //START THE BIG LOOP
    
    int iterations = 0;
    
    bool last_iteration = false;

    uint64 whileTimeS = GetTimeMs64();
    while (!last_iteration)
	{
	    
	    iterations = iterations + 1;
	    
	    if (printToCOut && iterations % 5 == 0) {
		std::cout << "." << std::flush;
	    }
	    
	    bool UpdateIteration = true;
	    bool anyToAdd = false;
	    int add_basis = 1;
	    //Compute the change in likelihood for all possible updates and basis functions
	    
	    double *DeltaML = new double[M];
	    double *Action = new double[M];
	    
	    for (int rows=0;rows<M;rows++)
		Action[rows] =0;
	    
	    for (int rows=0;rows<M;rows++)
		DeltaML[rows] = 0;
	    
	    //DeltaML
	    
	    //DeltaML for adding
	    
	    for (int basis=0; basis<M; basis++)
		{
		    if ((relevance_factors[basis] > 1e-12) && !(ins_model[basis]))
			{
			    double quot = Q_in[basis]*Q_in[basis] / S_in[basis];
			    DeltaML[basis] = (quot - 1 - log(quot))/2;
			    Action[basis] = add_basis;
			    anyToAdd = true;
			    
			}
		    
		}	
	    
	    
	    double max_DeltaML = 0;
	    int chosen_basis = 0;
	    int selected_action = 0;
	    
	    for (int basis=0;basis < M; basis++)
		if (DeltaML[basis] > max_DeltaML)
		    {
			max_DeltaML = DeltaML[basis];
			chosen_basis = basis;
			selected_action = Action[basis];
		    }
	    
	    
	    
	    //get the new basis function and calculate the new alpha for that particular basis function
	    
	    double *current_basis_function = new double[N];
	    
	    for (int rows=0;rows<N;rows++)
		current_basis_function[rows] = BASIS[rows][chosen_basis];
	    
	    double new_alpha = S_out[chosen_basis]*S_out[chosen_basis] / relevance_factors[chosen_basis];
	    
	    if (selected_action==add_basis)
		{
		    //std::cout << "Iteration " << iterations <<  " Adding " << chosen_basis << std::endl;
		    
		    indeces_used[included_basis] = chosen_basis;
		    
		    where_is[chosen_basis] = included_basis;
		    
		    ins_model[chosen_basis] = true;
		    
		    double *BASIS_phi = new double[M];
		    
		    multiply2D1D(BASIS_transpose, current_basis_function, BASIS_phi, M, N);
		    
		    for (int rows=0;rows<M;rows++)
			BASIS_PHI[rows][included_basis] = BASIS_phi[rows];
		    
		    double *B_phi = new double[M];
		    for (int rows=0;rows<M;rows++)
			B_phi[rows] = beta*current_basis_function[rows];
		    
		    
		    double *BETA_BASIS_phi = new double[M];
		    
		    for (int rows=0;rows<M;rows++)
			BETA_BASIS_phi[rows] = beta*BASIS_phi[rows];
		    
		    
		    double *temp1 = new double[included_basis];
		    double *temp2 = new double[included_basis]; 
		    
		    multiply1D2D(B_phi, PHI, temp1, included_basis, N);
		    multiply1D2D(temp1, covariance, temp2, included_basis, included_basis);
		    
		    
		    alphas[chosen_basis] = new_alpha;
		    
		    for (int rows=0;rows<N;rows++)
			PHI[rows][included_basis] = current_basis_function[rows];
		    
		    
		    double s_ii = 1 / (new_alpha + S_in[chosen_basis]);
		    
		    double *s_i = new double[included_basis];
		    for (int rows=0;rows<included_basis;rows++)
			s_i[rows] = - s_ii*temp2[rows];
		    
		    
		    double **TAU = new double*[included_basis];
		    for (int i=0;i<included_basis;i++)
			TAU[i] = new double[included_basis];
		    
		    
		    multiply1D1Dmat(s_i, temp2, TAU, included_basis, included_basis);
		    
		    
		    for (int rows=0;rows<included_basis;rows++)
			for (int cols=0;cols<included_basis;cols++)
			    covariance[rows][cols] = covariance[rows][cols] - TAU[rows][cols];
		    
		    for (int cols=0;cols<included_basis;cols++)
			covariance[included_basis][cols] = s_i[cols]; // Beta factor is NOT missing -> There is an error in the paper
		    
		    for (int rows=0;rows<included_basis;rows++)
			covariance[rows][included_basis] = s_i[rows]; // Same again.
		    
		    covariance[included_basis][included_basis] = s_ii;
		    
		    double mu_i = s_ii*Q_in[chosen_basis];
		    
		    double *delta_Mu = new double[included_basis+1];
		    
		    for (int rows=0;rows<included_basis;rows++)
			delta_Mu[rows] = -mu_i*temp2[rows];
		    delta_Mu[included_basis] = mu_i;
		    
		    for (int rows=0;rows<included_basis;rows++)
			mean[rows] = mean[rows] + delta_Mu[rows];
		    mean[included_basis] = delta_Mu[included_basis];
		    
		    
		    double *temp3 = new double[M];
		    
		    multiply2D1D(BETA_BASIS_PHI, temp2, temp3, M, included_basis);
		    
		    double *mCI = new double[M];
		    
		    
		    for (int rows=0;rows<M;rows++)
			{
			    mCI[rows]  = BETA_BASIS_phi[rows] - temp3[rows];
			}
		    for (int basis=0; basis<M; basis++)
			{
			    S_in[basis] = S_in[basis] - s_ii*mCI[basis]*mCI[basis];
			    Q_in[basis] = Q_in[basis] - mu_i*mCI[basis];	
			}
		    
		    
		    
		    delete[] BETA_BASIS_phi;
		    delete[] mCI;
		    delete[] temp3;
		    delete[] delta_Mu;
		    for (int i=0;i<included_basis;i++)
			delete[] TAU[i];
		    delete[] TAU; 
		    delete[] s_i;
		    delete[] temp1;
		    delete[] temp2;
		    delete[] B_phi;
		    delete[] BASIS_phi;
		    
		    
		    included_basis = included_basis + 1;
		    added_basis = added_basis + 1;
		    
		}				
	    
	    for (int basis=0; basis<M; basis++)
		{
		    S_out[basis] = S_in[basis];
		    Q_out[basis] = Q_in[basis];
		    
		    if (ins_model[basis])
			{
			    S_out[basis] = alphas[basis]*S_in[basis] / (alphas[basis] - S_in[basis] );
			    Q_out[basis] = alphas[basis]*Q_in[basis] / ( alphas[basis] - S_in[basis] );
			}
		    
		    relevance_factors[basis] = Q_out[basis]*Q_out[basis] - S_out[basis];
		}
	    
	    for (int rows=0;rows<M;rows++)
		for (int cols=0;cols<included_basis;cols++)
		    BETA_BASIS_PHI[rows][cols] = beta*BASIS_PHI[rows][cols];
	    
	    for (int rows=0;rows<included_basis;rows++)
		gamma[rows] = 1 - alphas[indeces_used[rows]]*covariance[rows][rows]; 
	    
	    
	    
	    
	    
	    
	    //std::cout << "Change in likelihood: " << max_DeltaML << std::endl;
	    
	    if (max_DeltaML<change_in_ML_threshold)
		{
		    last_iteration = true;
		    //printf("max_DeltaML = %.12f\n", max_DeltaML);
		}
	    
	    
	    delete[] current_basis_function;
	    delete[] DeltaML;
	    
	    
	    
	}
    //std::cout << "Added " << added_basis << std::endl;

    for (int rows=0;rows<M;rows++)
	coefficients[rows] = 0;

    for (int rows=0;rows<M;rows++)
	if (ins_model[rows])
	    coefficients[rows] = mean[where_is[rows]]/scaling[rows];
    
    
    //		noiseStd = 1 /beta;     //Commented out by BA - believed to be a bug.

    if(printToCOut) std::cout << std::endl;
    whileTime += GetTimeMs64() - whileTimeS;
    whileIdx++;
    //perform this if we are interested in the error bars of the estimation for the cascade
    if (calc)
	{
	    double **rel_covariance = new double*[M]; //matrix containing the covariance of the model
	    for (int i=0;i<M;i++)
		rel_covariance[i] = new double[M];
	    
	    double **temp_covariance = new double*[M]; //matrix containing the covariance of the model
	    for (int i=0;i<M;i++)
		temp_covariance[i] = new double[M];
	    
	    for (int rows=0;rows<M;rows++)
		for (int cols=0;cols<M;cols++)
		    {
			rel_covariance[rows][cols]=0;
			temp_covariance[rows][cols]=0;
		    }
	    
	    for (int rows=0;rows<M;rows++)
		for (int cols=0;cols<M;cols++)
		    if (ins_model[rows])
			temp_covariance[rows][cols] = covariance[where_is[rows]][cols];
	    
	    for (int rows=0;rows<M;rows++)
		for (int cols=0;cols<M;cols++)
		    if (ins_model[cols])
			rel_covariance[rows][cols] = temp_covariance[rows][where_is[cols]];
	    
	    for (int point=0;point<M;point++)
		{
		    double *point_column = new double[M];
		    double *temp_column = new double[M];
		    for (int rows=0;rows<M;rows++)
			point_column[rows] = PSI[point][rows];
		    
		    multiply1D2D(point_column,rel_covariance,temp_column,M,M);
		    
		    errors[point] = multiply1D1Dnum(temp_column,point_column,M);
		    
		    delete[] point_column;
		    delete[] temp_column;
		    
		}
	    
	    
	    
	    for (int i=0;i<M;++i)
		delete[] temp_covariance[i];
	    delete[] temp_covariance;
	    for (int i=0;i<M;++i)
		delete[] rel_covariance[i];
	    delete[] rel_covariance;
	    
    	}
    

	delete[] gamma;
	delete[] ins_model;
	delete[] where_is;
	delete[] phi_column;
	delete[] scaling;
	delete[] indeces_used;
	delete[] mean;
	delete[] S_in;
	delete[] Q_in;
	delete[] S_out;
	delete[] Q_out;
	delete[] relevance_factors;
	
	delete[] BASIS_Targets;
	
	
	for (int i=0;i<M;i++)
		delete[] BASIS_transpose[i];
		delete[] BASIS_transpose; 
	
	for (int i=0;i<M;i++)
		delete[] BETA_BASIS_PHI[i];
		delete[] BETA_BASIS_PHI;
					
	for (int i=0;i<M;i++)
		delete[] BASIS_PHI[i];
		delete[] BASIS_PHI; 	
	
	for (int i=0;i<M;i++)
		delete[] covariance[i];
		delete[] covariance; 
	
	for (int i=0;i<N;i++)
		delete[] PHI[i];
		delete[] PHI;
		
}
#endif

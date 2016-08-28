#ifndef GUARD_RVM_HPP
#define GUARD_RVM_HPP

#include <cmath>

#include "Dim.hpp"
#include "Signal.hpp"
#include "Errors.hpp"

/* RVM class */

class RVM {
private:
    // RVM settings
    double m_stdDev;		// std deviation
    bool m_print;		// if true, prints progress to stdout
    double m_threshold;		// convergence criterion
    bool m_onlyAdd;		// if true, we only consider addition of basis functions, no deletions
  
    const static double zeroFactor; // = 1.0e-12. Doubles that differ by less than this will be considered equal


    // RVM internals
    bool m_trainingFinished;

    int m_M;		 // M = total number of functions in basis set
    int m_K;	 // number of basis functions currently in model

    Signal<double> m_mu;	// posterior mean  (length M vector). if inModel(i) == false, then m_mu(i) == 0
    Signal<double> m_Sigma;	// posterior covariance  (MxM psd matrix). if inModel(i) == false, then Sigma(i,:) == Sigma(:,i) == 0
    
    Signal<double> S_out;	// Sparsity and quality pre-values (eqn (22) in [tipping & faul 2001])
    Signal<double> Q_out;	// vectors of length M

    Signal<double> S_in;	// Sparsity and quality factors (eqn (23))
    Signal<double> Q_in;	// length M vectors

    static const int m_DUMMY = -999; // place holder variable to denote emptyness
    Signal<double> alphas;	      // length M vector holding value of alpha for each basis function

    Signal<double> thetas;	// length M vector holding relevance factors q^2-s
    Signal<double> deltaML;	// length M vector holding change in marginal likelihood for each basis function if it was updated

    Signal<bool> inModel;	// length M vector. ith entry is true if basis function is included in the model
    Signal<int> currentSet; // length M vector. holds identities of basis functions currently included. fill with placeholder
    Signal<int> whereIs;	// length M vector. ith entry gives contains index of ith basis function with currentSet

    /* reserves the memory to hold the member data once we know the number of basis functions  */
    void reserveMembers(int numBasisFunctions);
    
    /* Finds the initial basis function and adds it to the model */
    void getFirstBasisFunction(const Signal<double>& designMatrix, const Signal<double>& targets);
    /* Used by getFirstBasisFunction(...). */
    int idxMaxProjection(const Signal<double>& designMatrix, const Signal<double>& targets) const;

    /* Computes all the statistics for the current state of the model using their full formulae */
    void computeFullStatistics(const Signal<double>& designMatrix, const Signal<double>& targets);
    /* Same as computeFullStatistic(...) but assumes there is currently only a single basis function in the model.*/
    void firstFullStatistics(const Signal<double>& designMatrix, const Signal<double>& targets);

    /* Used in the loop of the SSBL algorithm to select the next basis function */
    int selectBasisFunction(double& max_deltaML);
    /* Adds basis function to the model. */
    void addNewBasisFunction(int index, const Signal<double>& designMatrix);

    void reestimateBasisFunction(int index, const Signal<double>& designMatrix); // NOT IMPLEMENTED
    void deleteBasisFunction(int index, const Signal<double>& designMatrix); // NOT IMPLEMENTED
public:
    RVM(double stdDev = 1, double threshold = 1, bool print = true, bool onlyAdd = true); // Constructer

    /* greedy train (no reestimation or deletion of basis functions) */
    void train(const Signal<double>& designMatrix, const Signal<double>& targets); 
    
    /* points is NxM matrix where M is #basis functions and N is #points for which to predict. output is Nx1 vector of model predictions for points */
    Signal<double> predict(const Signal<double>& points) const; 

    /* points is NxM matrix where M is #basis functions and N is #points for which to predict. output is Nx1 vector of errors in corresponding model predictions */
    Signal<double> predictionErrors(const Signal<double>& points) const;    

    /* return mean of posterior */
    Signal<double> mu() const;	  
    //Signal<double> fastMu() const;

    /* return covariance of posterior */
    Signal<double> Sigma() const; 
    
    void setPrint(bool p) { m_print = p;};
};

/* Include implementation file */
#include "RVM.tpp"

#endif // GUARD_RVM_HPP

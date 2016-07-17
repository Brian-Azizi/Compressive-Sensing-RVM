#ifndef GUARD_RVM_HPP
#define GUARD_RVM_HPP

#include <cmath>

#include "Dim.hpp"
#include "Signal.hpp"
#include "Errors.hpp"


class RVM {
private:
    // RVM settings
    double m_stdDev;		// std deviation
    bool m_print;
    double m_threshold;
    bool m_onlyAdd;		// if true, we only consider addition of basis functions, no deletions
    const static double zeroFactor = 1.0e-12;
    
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

    void reserveMembers(int numBasisFunctions);	// reserves the memory to hold the member data once we know the number of basis functions
    void getFirstBasisFunction(const Signal<double>& designMatrix, const Signal<double>& targets);
    int idxMaxProjection(const Signal<double>& designMatrix, const Signal<double>& targets) const;
    void computeFullStatistics(const Signal<double>& designMatrix, const Signal<double>& targets);
    int selectBasisFunction(double& max_deltaML);
    void addNewBasisFunction(int index, const Signal<double>& designMatrix);
    void reestimateBasisFunction(int index, const Signal<double>& designMatrix);
    void deleteBasisFunction(int index, const Signal<double>& designMatrix);    
public:
    RVM(double stdDev = 1, double threshold = 1, bool print = true, bool onlyAdd = true);

    // greedy train (no reestimation of deletion)
    void train(const Signal<double>& designMatrix, const Signal<double>& targets); 
    
    // points is NxM matrix where M is #basis functions and N is #points for which to predict. output is Nx1 vector of model predictions for points
    Signal<double> predict(const Signal<double>& points) const; 

    // points is NxM matrix where M is #basis functions and N is #points for which to predict. output is Nx1 vector of errors in corresponding model predictions
    Signal<double> predictionErrors(const Signal<double>& points) const;    

    // return mean of posterior
    Signal<double> mu() const;	  

    // return covariance of posterior    
    Signal<double> Sigma() const; 
};

RVM::RVM(double stdDev, double threshold, bool print, bool onlyAdd)
    : m_stdDev(stdDev), m_threshold(threshold), m_print(print), m_onlyAdd(onlyAdd),
	  m_trainingFinished(false)
    {
	if (stdDev <= zeroFactor) error("RVM: std deviation must be positive");
	if (threshold <= zeroFactor) error("RVM: threshold for change in marginal likelihood must be positive");
    }


Signal<double> RVM::mu() const {
    if (!m_trainingFinished) {
	if (m_print) std::cerr << "RVM: Cannot access mu before training the model" << std::endl;
	return Signal<double>();
    }

    Signal<double> ret = m_mu;

    // replace dummies by zeros    
    for (int j = 0; j < m_M; ++j)
	if (!inModel(j)) ret(j) = 0;

    return ret;
}

Signal<double> RVM::Sigma() const {
    if (!m_trainingFinished) {
	if (m_print) std::cerr << "RVM: Cannot access Sigma before training the model" << std::endl;
	return Signal<double>();
    }

    Signal<double> ret = m_Sigma;
    
    // replace dummies by zeros
    for (int i = 0; i < m_M; ++i)
	for (int j = 0; j < m_M; ++j)	   
	    if (!inModel(i) || !inModel(j))
		ret(i,j) = 0;

    return ret;
}

void RVM::reserveMembers(int M)
{
    m_M = M;
    m_K = 0;
    m_mu = Signal<double>(M);
    m_Sigma = Signal<double>(M,M);
    S_out = Signal<double>(M);
    Q_out = Signal<double>(M);
    S_in = Signal<double>(M);
    Q_in = Signal<double>(M);
    alphas = Signal<double>(M);
    thetas = Signal<double>(M);
    deltaML = Signal<double>(M);
    inModel = Signal<bool>(M);
    currentSet = Signal<int>(M);
    whereIs = Signal<int>(M);
	
    m_mu.fill(m_DUMMY);
    m_Sigma.fill(m_DUMMY);
    S_out.fill(m_DUMMY);
    Q_out.fill(m_DUMMY);
    S_in.fill(m_DUMMY);
    Q_in.fill(m_DUMMY);
    alphas.fill(m_DUMMY);
    thetas.fill(m_DUMMY);
    deltaML.fill(m_DUMMY);
    inModel.fill(false);
    currentSet.fill(m_DUMMY);
    whereIs.fill(m_DUMMY);
}

int RVM::idxMaxProjection(const Signal<double>& designMatrix, const Signal<double>& targets) const
{
    int N = designMatrix.height();
    int M = m_M;
    if(M != designMatrix.width()) error("RVM: dimension mismatch");

    Signal<double> phi(N);
    
    double maxProj = -1;
    int maxIdx = -1;

    for (int j = 0; j < M; ++j) {
	for (int i = 0; i < N; ++i) phi(i) = designMatrix(i,j); // jth basis function
	double num = dot(phi,targets);
	double denom = dot(phi,phi);
	double projection;

	if(denom < 0) error("RVM: there is an error in Signal::dot");
	if(denom < zeroFactor) {
	    if(m_print) std::cerr << "RVM: Basis function " << j << " has zero length\n";
	    projection = 1;
	    //	    projection = 0;
	} else
	    projection = (num*num)/denom;
		
	if (projection > maxProj) {
		maxProj = projection;
		maxIdx = j;		
	}
    }
    if (maxIdx < 0 || maxProj < 0) error("RVM: could not select initial basis function.");
    return maxIdx;
}

//selects the basis function with largest projection onto target vector. 
void RVM::getFirstBasisFunction(const Signal<double>& designMatrix, const Signal<double>& targets)
{
    if (m_K != 0) error("RVM: current basis set is non-empty");

    int N = designMatrix.height();
    int firstIdx = idxMaxProjection(designMatrix, targets);
    Signal<double> firstBasis = designMatrix.getSlice(0,N,firstIdx,firstIdx+1);

    // update intenals
    inModel(firstIdx) = true;
    whereIs(firstIdx) = m_K;
    currentSet(m_K) = firstIdx;
    ++m_K;

    // set alpha = (||phi||^2)/(dot(phi,t)^2/||phi||^2 - sig^2);
    double num = dot(firstBasis, firstBasis);
    double denom = dot(firstBasis,targets);
    denom = denom*denom/num - m_stdDev*m_stdDev;

    double alpha = num/denom;
    if (alpha < 0) {
	if(m_print) std::cerr << "RVM: alpha < 0! Setting alpha to 1e3";
	alpha = 1.0e3; // little hack for dealing with numerical issues
    }
    alphas(firstIdx) = alpha;
}

// computes internal statistics using full formulas. Assumes alpha is up-to-date
void RVM::computeFullStatistics(const Signal<double>& designMatrix, const Signal<double>& targets)
{
    const int K = m_K;		// current size of basis set
    const int N = designMatrix.height();
    const int M = designMatrix.width();
    if (K == 0) error("RVM: current basis set is empty");

    const double beta = 1.0/(m_stdDev*m_stdDev);   

    // order in these temporary variables is consistent with order in full variables.
    // This is NOT consistent with the order of 'currentSet'
    Signal<double> mu(K);	// part of mean corresponding to current basis set
    Signal<double> Sig(K,K);	// ditto for covariance
    Signal<double> Phi(N,K);	// current basis set
    int bidx = 0;		// index in current basis set, bidx = 0,1,...,K-1
    for (int j = 0; j < M; ++j)
	if (inModel(j)) {
	    for (int i = 0; i < N; ++i)
		Phi(i,bidx) = designMatrix(i,j); 
	    ++bidx;
	}
    if (bidx != K) error("RVM: internal error in computing full statistics. K!=size of current basis set");

    // get indeces of functions in current basis set in increasing order
    Signal<double> sortedIdx(K);    
    int aidx = 0;		// index in complete basis set, aidx = 0,1,...,M-1
    for (bidx = 0; bidx < K; ++bidx) {
	while(!inModel(aidx)) ++aidx;
	sortedIdx(bidx) = aidx;
	++aidx;
	if(aidx > M) error("RVM: index of basis set went out of range");
    }
    
    
    /*** Sigma and mu (eqn 6) ***/
    Signal<double> invSigma(K,K);
    invSigma = matMult(transpose(Phi),Phi) * beta;    
    for (bidx = 0; bidx < K; ++bidx) {
	aidx = sortedIdx(bidx);
	invSigma(bidx,bidx) += alphas(aidx);
    }
    Sig = inverse(invSigma);
    mu = matMult(Sig,matMult(transpose(Phi),targets)) * beta;

    /*** S_out, Q_out (eqn 24, 25) ***/
    Signal<double> innerFactor = matMult(Phi,matMult(Sig, transpose(Phi))) * beta * beta ; // NxN matrix: B.Phi.Sigma.Phi^T.B  from eqn 24
    Signal<double> phi(N);	// column of design matrix
    for (int j = 0; j < M; ++j) {
	for (int i = 0; i < N; ++i) phi(i) = designMatrix(i,j); // get jth column
	S_out(j) = beta*dot(phi,phi) - dot(phi, matMult(innerFactor, phi));
	Q_out(j) = beta*dot(phi,targets) - dot(phi, matMult(innerFactor, targets));
    }

    /*** S_in, Q_in (eqn 23) and thetas***/
    for (int j = 0; j < M; ++j) {
	if(!inModel(j)) {
	    S_in(j) = S_out(j);
	    Q_in(j) = Q_out(j);
	} else {
	    double denom = alphas(j) - S_out(j);
	    S_in(j) = alphas(j)*S_out(j) / denom;
	    Q_in(j) = alphas(j)*Q_out(j) / denom;
	}

	thetas(j) = Q_in(j)*Q_in(j) - S_in(j);
    }

    /*** save mu and Sig in internal state ***/    
    for (bidx = 0; bidx < K; ++bidx) {
	aidx = sortedIdx(bidx);
	m_mu(aidx) = mu(bidx);
	for(int bidx2 = 0; bidx2 < K; ++bidx2) {
	    int aidx2 = sortedIdx(bidx2);
	    m_Sigma(aidx,aidx2) = Sig(bidx,bidx2);
	}
    }
    
    
    return;
}

// assumes relevance factors, S, Q are up to date
int RVM::selectBasisFunction(double& maxDML)
{
    const int M = m_M;
    for (int j = 0; j < M; ++j) {
	if (!inModel(j) && thetas(j) > 0) { // potential addition
	    double term = Q_out(j) * Q_out(j) / S_out(j);		    
	    if (term < 0) deltaML(j) = 0; // Nasty: this shouldn't happen theoretically. :/
	    else deltaML(j) = 0.5 * (term - 1.0 - std::log(term));	
	} 
	else if(!m_onlyAdd) {
	    if (inModel(j) && thetas(j) > 0) { // potentially re-estimate
		double newAlpha = S_in(j)*S_in(j) / (Q_in(j)*Q_in(j) - S_in(j));
		if (abs(newAlpha - alphas(j)) < zeroFactor) { // no change in alpha
		    deltaML(j) = 0;
		    continue;
		}
		double denom = S_in(j) + 1.0/(1.0/newAlpha - 1.0/alphas(j));
		double logterm = S_in(j) * (1.0/newAlpha - 1.0/alphas(j));
		deltaML(j) = 0.5 *(Q_in(j)*Q_in(j) / denom - std::log(1 + logterm));
	    } 
	    else if (inModel(j) && thetas(j) < 0) { // potentially delete
		double term = Q_out(j)*Q_out(j) / (S_out(j) - alphas(j));
		deltaML(j) = 0.5 *(term - std::log(1.0 - S_out(j)/alphas(j)));
	    } 
	}
	else {		// do nothing
	    deltaML(j) = m_DUMMY;
	}
    }

    int idx = -1;
    double max = -1;
    for (int j = 0; j < M; ++j) {
	if (deltaML(j) > max) {
	    max = deltaML(j);
	    idx = j;
	}
    }

    maxDML = max;
    return idx;
}

void RVM::addNewBasisFunction(int currentIdx, const Signal<double>& designMatrix)
{
    int M = m_M;
    int N = designMatrix.height();
    int K = m_K;		// current number of basis functions (before addition)
    double beta = 1.0/(m_stdDev*m_stdDev);
    if(designMatrix.dim() != Dim(N,M,1)) error("RVM: argument 2 in addNewBasis has wrong dimensions");
    
    // get indeces of current basis set in increasing order
    Signal<double> sortedIdx(K);
    int aidx = 0;
    for(int bidx = 0; bidx < K; ++bidx) {
	while(!inModel(aidx)) ++aidx;
	sortedIdx(bidx) = aidx;
	if (aidx == currentIdx) error("RVM: internal logic error");
	++aidx;
    }
    
    // get temporaries
    Signal<double> phi = designMatrix.getSlice(0,N,currentIdx,currentIdx+1); 
    Signal<double> mu(K);
    Signal<double> Sig(K,K);
    Signal<double> Phi(N,K);

    for (int bidx = 0; bidx < K; ++bidx) {
	aidx = sortedIdx(bidx);
	mu(bidx) = m_mu(aidx);	// mu
	for(int bidx2 = 0; bidx2 < K; ++bidx2) { // Sig
	    int aidx2 = sortedIdx(bidx2);
	    Sig(bidx,bidx2) = m_Sigma(aidx,aidx2);
	}
	for(int i = 0; i < N; ++i) Phi(i,bidx) = designMatrix(i,aidx); // Phi
    }
    
    // get alpha
    double alphaNum = S_in(currentIdx)*S_in(currentIdx); // S_out = S_in at currentIdx since phi was is currently not in model
    double alphaDem = Q_in(currentIdx)*Q_in(currentIdx) - S_in(currentIdx); 
    double alpha = alphaNum / alphaDem;

    // get new statistics
    double S_ii = 1.0/(alpha + S_out(currentIdx));
    double m_i = S_ii*Q_out(currentIdx);
    Signal<double> e_i(N);
    Signal<double> SigPhiTphi = matMult(Sig,matMult(transpose(Phi),phi)); // should have length K
    Signal<double> tempE = matMult(Phi,SigPhiTphi) * beta; // length N
    for (int i = 0; i < N; ++i) e_i(i) = phi(i) - tempE(i);
    
    /* note: We are not extending the temporary variables mu and Sig. 
       Instead, we insert the new rows and columns directly into the internal variables.  */
    Signal<double> tempK = SigPhiTphi * (m_i*beta);
    for (int bidx = 0; bidx < K; ++bidx) mu(bidx) -= tempK(bidx);
    Signal<double> tempS = matMult(SigPhiTphi,transpose(SigPhiTphi)) * (beta*beta*S_ii);
    Sig = add(Sig, tempS);

    Signal<double> newSigCol = SigPhiTphi * (-beta*S_ii); // note the error in the paper! it should be beta, Not beta^2.

    // update internal statistics
    for (int bidx = 0; bidx < K; ++bidx) {
	aidx = sortedIdx(bidx);
	m_mu(aidx) = mu(bidx);

	for (int bidx2 = 0; bidx2 < K; ++bidx2) {
	    int aidx2 = sortedIdx(bidx2);
	    m_Sigma(aidx,aidx2) = Sig(bidx,bidx2);
	}
	m_Sigma(aidx,currentIdx) = newSigCol(bidx);
	m_Sigma(currentIdx, aidx) = newSigCol(bidx);	
    }
    m_mu(currentIdx) = m_i;
    m_Sigma(currentIdx,currentIdx) = S_ii;
    
    // update state
    alphas(currentIdx) = alpha;
    inModel(currentIdx) = true;
    whereIs(currentIdx) = K;
    currentSet(K) = currentIdx;
    ++K;
    ++m_K;

    // update S and Q and thetas
    Signal<double> phi_m(N);
    for (int j = 0; j < M; ++j) {
	for(int i = 0; i < N; ++i) phi_m(i) = designMatrix(i,j); // get jth basis function
	double term = beta * dot(phi_m, e_i);
	S_out(j) -= S_ii * term * term;
	Q_out(j) -= m_i * term;

	if (!inModel(j)) {
	    S_in(j) = S_out(j);
	    Q_in(j) = Q_out(j);
	} else {
	    S_in(j) = alphas(j)*S_out(j) / (alphas(j) - S_out(j));
	    Q_in(j) = alphas(j)*Q_out(j) / (alphas(j) - S_out(j));
	}

	thetas(j) = Q_in(j)*Q_in(j) - S_in(j);
    }
    
}

void RVM::train(const Signal<double>& designMatrix, const Signal<double>& targets)
{   
    /*** 0) Initialize member data ***/
    int N = designMatrix.height();
    int M = designMatrix.width();

    
    /*** some tests ***/
    if (designMatrix.frames() != 1) error("RVM: Design Matrix (argument 1) must have third dimension = 1");
    if (targets.height() != N) error("RVM: target vector (arg 2) must have same number of rows as the design matrix (arg 1)");
    if (targets.dim() != Dim(N,1,1)) error("RVM: targets (arg 2) must be a vector (i.e. dim 2 == dim 3 == 1)");
    
    reserveMembers(M);
    double var = m_stdDev*m_stdDev;
    double beta = 1.0/var;

    // QQ: Do I need to normalize the basis functions?

    /*** 1+2) Initialise with a single basis vector and set alpha: pick the one with the largest normalised projection onto the target vector ***/
    getFirstBasisFunction(designMatrix, targets);    

    /*** 3) Compute the intial full statistics (mu, Sigma, s, q, S, Q, thetas) ***/
    computeFullStatistics(designMatrix, targets);


    /**** Start the main loop ****/
    Signal<double> phi(N);	// holds basis function currently being considered 
    bool lastIteration = false;
    double maxDML = m_DUMMY;
    while(!lastIteration) {	
	/*** 4) Selection: Calculate relevance, alphas and delta ML for all bases and pick argmax delta ml ***/
	int currentIdx = selectBasisFunction(maxDML); // index of next basis function to update, get change in ML
	for (int i = 0; i < N; ++i) phi(i) = designMatrix(i, currentIdx); // get basis function;	


	/*** 9) Adjust basis set and Update Sigma, Mu, S and Q ***/	
	if (thetas(currentIdx) > 0 && !inModel(currentIdx)) // add
	    addNewBasisFunction(currentIdx, designMatrix);

	// after full training is implemented, don't forget to check sign at delta_ML for addition in some function above	   
	// else if (thetas(currentIdx) > 0 && inModel(currentIdx)) // reestimate alpha
	//     //	    reestimateBasisFunction(currentIdx, designMatrix);							
	// else if (thetas(currentIdx) <= 0 && inModel(currentIdx)) // delete
	//     //	    deleteBasisFunction(currentIdx, designMatrix);
	// else			// do nothing
	//     ;
	
	/** 10) If converged: stop. Else goto 4. ***/
	if (maxDML < m_threshold)
	    lastIteration = true;
    }
    m_trainingFinished = true;
}

Signal<double> RVM::predict(const Signal<double>& points) const
{
    if (!m_trainingFinished) {
	if (m_print) std::cerr << "RVM: Cannot use model for predicting before training the model" << std::endl;
	return Signal<double>();
    }
    
    if(points.frames() != 1) error("input must be no more than a 2D array (frames must = 1)");
    if(points.width() != m_M) error("width of input matrix must equal number of basis functions that were used for training");
    
    int N = points.height();	// number of samples for prediction
    Signal<double> predictions(N);

    for (int i = 0; i < N; ++i) {
	double tempSum = 0;
	for (int j = 0; j < m_M; ++j)
	    if (inModel(j)) tempSum += m_mu(j) * points(i,j);
	
	predictions(i) = tempSum;
    }
    return predictions;
}

Signal<double> RVM::predictionErrors(const Signal<double>& points) const
{
    if (!m_trainingFinished) {
	if (m_print) std::cerr << "RVM: Cannot use model for predicting before training the model" << std::endl;
	return Signal<double>();
    }
    
    if(points.frames() != 1) error("input must be no more than a 2D array (frames must = 1)");
    if(points.width() != m_M) error("width of input matrix must equal number of basis functions that were used for training");
    
    int N = points.height();	// number of samples for prediction
    Signal<double> errors(N);
    
    Signal<double> temp(m_K); 	// Sigma * phi, but only for current basis set
    for (int n = 0; n < N; ++n) {
	int idx = 0;
	for (int i = 0; i < m_M; ++i) {
	    if(inModel(i)) {
		double tempSum = 0;
		for (int j = 0; j < m_M; ++j)
		    if(inModel(j)) tempSum += m_Sigma(i,j)*points(n,j);
		temp(idx) = tempSum;
		++idx;
	    }
	}
	idx = 0;
	double tempSum = 0;
	for (int i = 0; i < m_M; ++i) {	
	    if (inModel(i)) {
		tempSum += points(n,i)*temp(idx);
		++idx;
	    }
	}
	errors(n) = tempSum;	
    }

    return errors;			    
}

#endif // GUARD_RVM_HPP

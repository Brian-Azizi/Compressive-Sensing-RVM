#ifndef GUARD_RVM_TPP
#define GUARD_RVM_TPP

const double RVM::zeroFactor = 1.0e-12; 

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

    //replace dummies by zeros    
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
	    //if(m_print) std::cerr << "RVM: Basis function " << j << " has zero length\n";
	    //projection = 1;
	    projection = 0;
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
	//if(m_print) std::cerr << "RVM: alpha < 0! Setting alpha to 1e3";
	alpha = 1.0e3; // little hack for dealing with numerical issues
    }
    alphas(firstIdx) = alpha;
}

/*** computes the internal statistics (mu, Sigma, S, Q, s, q, thetas) using full formulae.
Assumes (alphas, inModel, whereIs, currentSet) are up-to-date  ***/
void RVM::computeFullStatistics(const Signal<double>& designMatrix, const Signal<double>& targets)
{ 
    const int K = m_K;		// current size of basis set
    const int N = designMatrix.height();
    const int M = designMatrix.width();
    if (K == 0) error("RVM: current basis set is empty");

    const double beta = 1.0/(m_stdDev*m_stdDev);   

    // order in these temporary variables is consistent with order in full variables.
    // This is NOT consistent with the order of 'currentSet'
    //    Signal<double> mu(K);	// part of mean corresponding to current basis set
    //    Signal<double> Sig(K,K);	// ditto for covariance
    Signal<double> Phi(N,K,false);	// current basis set
    int bidx = 0;		// index in current basis set, bidx = 0,1,...,K-1
    for (int j = 0; j < M; ++j)
	if (inModel(j)) {
	    for (int i = 0; i < N; ++i)
		Phi(i,bidx) = designMatrix(i,j); 
	    ++bidx;
	}
    if (bidx != K) error("RVM: internal error in computing full statistics. K!=size of current basis set");

    // get indeces of functions in current basis set in increasing order
    Signal<double> sortedIdx(K,false);    
    int aidx = 0;		// index in complete basis set, aidx = 0,1,...,M-1
    for (bidx = 0; bidx < K; ++bidx) {
	while(!inModel(aidx)) ++aidx;
	sortedIdx(bidx) = aidx;
	++aidx;
	if(aidx > M) error("RVM: index of basis set went out of range");
    }
    
    /*** Sigma and mu (eqn 6) ***/
    //    Signal<double> invSigma(K,K);
    //    Signal<double> invSigma = matMult(transpose(Phi),Phi) * beta;    
    Signal<double> PhiT = transpose(Phi);
    Signal<double> invSigma = matMult(PhiT,Phi,beta);
    for (bidx = 0; bidx < K; ++bidx) {
	aidx = sortedIdx(bidx);
	invSigma(bidx,bidx) += alphas(aidx);
    }

    Signal<double> Sig = inverse(invSigma);
    //Signal<double> PhiT_targets = matMult(PhiT, targets);
    //Signal<double> mu = matMult(Sig,PhiT_targets,beta);
    Signal<double> mu = matMult(Sig,matMult(PhiT, targets),beta);
    //Signal<double> mu = matMult(Sig,matMult(transpose(Phi),targets)) * beta;
    
    /*** S_out, Q_out (eqn 24, 25) ***/
    //Signal<double> innerFactor = matMult(Phi,matMult(Sig, transpose(Phi))) * (beta * beta) ; // NxN matrix: B.Phi.Sigma.Phi^T.B  from eqn 24
    Signal<double> innerFactor = matMult(Phi,matMult(Sig, PhiT),beta*beta);
    Signal<double> phi(N,false);	// column of design matrix
    
    Signal<double> tempIfPhi = matMult(innerFactor,designMatrix);

    Signal<double> tempIfT = matMult(innerFactor, targets);
    Signal<double> tempCol(N,false);

    for (int j = 0; j < M; ++j) {
	for (int i = 0; i < N; ++i) phi(i) = designMatrix(i,j); // get jth column
	for (int i = 0; i < N; ++i) tempCol(i) = tempIfPhi(i,j); // get jth column
	S_out(j) = beta*dot(phi,phi) - dot(phi, tempCol);
	Q_out(j) = beta*dot(phi,targets) - dot(phi, tempIfT);	
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

/*** Same as computeFullStatistics, but assumes we have only a single basis function in the model
     (i.e. works only if m_K == 1). It uses scalars rather than compound classes. ***/
void RVM::firstFullStatistics(const Signal<double>& designMatrix, const Signal<double>& targets) 
{
    const int K = m_K;		// current size of basis set
    const int N = designMatrix.height();
    const int M = designMatrix.width();
    if (K != 1) error("RVM: firstFullStatistics can only be called if there is a single basis function in the model");

    const double beta = 1.0/(m_stdDev*m_stdDev);   
    
    Signal<double> Phi(N,false);	// current basis set
    int aidx = currentSet(0);		// index of current basis function

    for (int i = 0; i < N; ++i)
	Phi(i) = designMatrix(i,aidx); 

    double PhiTargets = dot(Phi,targets);
    double phiBphi = dot(Phi,Phi) * beta;
    double invSigma = phiBphi + alphas(aidx);

    double Sig = 1.0/invSigma;
    double mu = PhiTargets * (Sig * beta);

    /*** S_out, Q_out (eqn 24, 25) ***/
    Signal<double> temp = matMult(transpose(Phi),designMatrix,beta);
    Signal<double> phi(N,false);

    for (int j = 0; j < M; ++j) {
	for (int i = 0; i < N; ++i) phi(i) = designMatrix(i,j); // get jth column
	double num = temp(0,j)*Sig;
	S_out(j) = beta*dot(phi,phi) - num*temp(0,j);
	Q_out(j) = beta*dot(phi,targets) - num*PhiTargets;

	if (j == aidx) {
	    double denom = alphas(j) - S_out(j);
	    S_in(j) = alphas(j)*S_out(j) / denom;
	    Q_in(j) = alphas(j)*Q_out(j) / denom;
	} else {
	    S_in(j) = S_out(j);
	    Q_in(j) = Q_out(j);
	}

	thetas(j) = Q_in(j)*Q_in(j) - S_in(j);
    }

    /*** save mu and Sig in internal state ***/    
    m_mu(aidx) = mu;
    m_Sigma(aidx,aidx) = Sig;
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
		if (std::abs(newAlpha - alphas(j)) < zeroFactor) { // no change in alpha
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
  

/*** Trains the RVM model given a NxM designMatrix, and an Nx1 vector of targets using the fast update formulae
     by [Tipping & Faul (2003)]. Currently only considers addition of basis functions. We stop the algorithm
     if the increase in marginal likelihood is below m_threshold, or if the algorithm decides not to add a
     basis function for the next iteration (and e.g. chooses to re-estimate or delete instead). ***/     
void RVM::train(const Signal<double>& designMatrix, const Signal<double>& targets)
{   
    /*** 0) Initialize member data ***/
    int N = designMatrix.height();
    int M = designMatrix.width();
    
    /*** some tests ***/
    if (designMatrix.frames() != 1) error("RVM: Design Matrix (argument 1) must have third dimension = 1");
    if (targets.height() != N) error("RVM: target vector (arg 2) must have same number of rows as the design matrix (arg 1)");
    if(targets.dim() != Dim(N,1,1)) error("RVM: targets (arg 2) must be a vector (i.e. dim 2 == dim 3 == 1)");
    
    reserveMembers(M);
    double var = m_stdDev*m_stdDev;
    double beta = 1.0/var;

    // QQ: Do I need to normalize the basis functions?


    /*** 1+2) Initialise with a single basis vector and set alpha:
	 pick the one with the largest normalised projection onto the target vector ***/
    getFirstBasisFunction(designMatrix, targets);    

    /*** 3) Compute the intial full statistics (mu, Sigma, s, q, S, Q, thetas). 
	 Can call either computeFullStatistics or firstFullStatistics at this point since we currently only
	 have a single basis function in out model. The output and effect to the state of the RVM should be
	 identical, though there me be differences in speed. ***/
         //computeFullStatistics(designMatrix, targets);
    firstFullStatistics(designMatrix, targets); 

    /**** Start the main loop ****/
    //    Signal<double> phi(N);	// holds basis function currently being considered 
    bool lastIteration = false;
    double maxDML = m_DUMMY;
    int count = 0;
    while(!lastIteration) {	
	++ count;
	if(m_print && count % 10 == 0) std::cout << "." << std::flush;

	/*** 4) Selection: Calculate relevance, alphas and delta ML for all bases and pick argmax delta ml ***/
	int currentIdx = selectBasisFunction(maxDML); // index of next basis function to update, get change in ML
	if (currentIdx < 0)			      // no more basis functions to update
	    break;
	
	//	for (int i = 0; i < N; ++i) phi(i) = designMatrix(i, currentIdx); // get basis function;	

	/*** 9) Adjust basis set and Update Sigma, Mu, S and Q ***/	
	if (thetas(currentIdx) > 0 && !inModel(currentIdx)) // add
	    addNewBasisFunction(currentIdx, designMatrix);
	
	/* NOT IMPLEMENTED
	// after full training is implemented, don't forget to check sign at delta_ML for addition in some function above	   
	// else if (thetas(currentIdx) > 0 && inModel(currentIdx)) // reestimate alpha
	//     //	    reestimateBasisFunction(currentIdx, designMatrix);							
	// else if (thetas(currentIdx) <= 0 && inModel(currentIdx)) // delete
	//     //	    deleteBasisFunction(currentIdx, designMatrix);
	// else			// do nothing
	//     ;
	*/

	/** 10) If converged: stop. Else goto 4. ***/
	if (maxDML < m_threshold)
	    lastIteration = true;
    }

    if(m_print) std::cout << std::endl;
    m_trainingFinished = true;
}

/*** Returns predictions made by the RVM. Cannot be called before we have trained the model. 
     Input is a PxM matrix (using 2D signals), where P is the number of points and M is the number of basis functions
     in the dictionary. The output is a Px1 vector containing the predictions ***/
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

/*** Returns the estimated error in the predictions of the RVM. Cannot be called before we have trained the model.
     Does not actually compute the predictions, only the estimated error. Input is PxM matrix of points (each row is
     a point, i.e. a vector of coefficients of the basis functions. The basis dictionary is assumed to be the same as 
     the one used for training. Output is a Px1 vector of prediction errors. ***/
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

#endif

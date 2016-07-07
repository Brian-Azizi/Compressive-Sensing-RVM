template <typename T> class RVM 
{
private:
    double m_sigma;		// std deviation
    double m_deltaML;
    bool m_printProgress;
    const static double zeroFactor = 1.0e-12;
    Signal<T> m_coeff;
    Signal<T> m_errors;
public:
    RVM() 
	: m_sigma(1), m_deltaML(1),
	m_printProgress(true) { };
    double sigma() const { return m_sigma; }
    double deltaML() const { return m_deltaML; }
    bool doWePrint() const { return m_printProgress; }
    Signal<T> coefficients() const { return m_coeff; }
    Signal<T> errors() const { return m_errors; }
    void setSigma(double sigma);
    void setDeltaML(double deltaML);
    void setPrintProgress(bool print) {m_printProgress = print;}
    void train_fastUpdates(Signal<T> basis, const Signal<T>& targets);
};

template<class T>
void RVM<T>::setSigma(double sigma) 
{
    if (sigma < zeroFactor) error("sigma must be positive");
    m_sigma = sigma;
}
template<class T> 
void RVM<T>::setDeltaML(double deltaML)
{
    if (deltaML < zeroFactor) error("threshold for the change in ML must be positive");
    m_deltaML = deltaML;
}

template<class T>
void RVM<T>::train_fastUpdates(Signal<T> basis, const Signal<T>& targets)
{
    // note: we normalize basis during training, but we restore the original before returning
    // check inputs make sense;
    const int n_rows = basis.height();
    const int n_cols = basis.width();
    if (n_rows < 1 || n_cols < 1) error("empty basis");
    if (basis.frames() != 1) error("basis needs to be a matrix (i.e. depth must be 1)");
    if (targets.width() != 1 || targets.frames() != 1) error("targets must be a vector (i.e. width = depth = 1)");
    if (targets.height() != n_rows) error("basis and targets must have the same number of rows");
    
    // initialize coeffs
    m_coeff = Signal<T>(n_cols);
    const double beta = 1/(m_sigma*m_sigma);

    // normalize each basis function to unit length and get the scaling for each basis
    Signal<double> scaling(n_cols);
    for (int j = 0; j < n_cols; ++j) {
	Signal<double> column = basis.getPatch(0,j,n_rows,1);
	scaling(j) = norm(column);
	if (scaling(j) == 0) scaling(j) = 1;
	
	for (int i = 0; i < n_rows; ++i) basis(i,j) /= scaling(j);
    }

    // choose the basis function with the largest projection onto the targets    
    double maxProjection = 0;
    int inModel = 0;
    for (int j = 0; j < n_cols; ++j) {
	Signal<double> column = basis.getPatch(0,j,n_rows,1);
	double projection = dot(column, targets);
	if (std::abs(projection) > std::abs(maxProjection)){
	    maxProjection = projection;
	    inModel = j;
	}
    }
    Signal<double> phiColumn = basis.getPatch(0,inModel,n_rows,1);

    // calculate alpha
    double alpha_min = 1.0e-3;
    double alpha_max = 1.0e3;
    double p = beta * dot(phiColumn, phiColumn);
    double q = beta * dot(phiColumn, targets);
    double alpha = p*p / (q*q - p);
    if (alpha < 0) {//if (alpha < alpha_min) {
	//	std::cerr << "Warning: alpha < alpha_min. Setting alpha = alpha_max" << std::endl;
	std::cerr << "Warning: alpha < 0. Setting alpha = alpha_max" << std::endl;
	alpha = alpha_max;
    }

    // more declarations
    Signal<double> Phi(n_rows, n_cols);
    Signal<double> alphas(n_cols);
    Signal<int> indecesUsed(n_cols);
    Signal<bool> insModel(n_cols); // defaults to false
    Signal<int> whereIs(n_cols);
    int includedBasis = 1;

    alphas(inModel) = alpha;
    indecesUsed(0) = inModel;
    whereIs(inModel) = 0;
    insModel(inModel) = true;
    Phi.putPatch(phiColumn,0,0);
    
    Signal<double> mean(n_cols);
    Signal<double> covariance(n_cols, n_cols);
    Signal<double> S_in(n_cols), Q_in(n_cols), S_out(n_cols), Q_out(n_cols);
    Signal<double> relevanceFactors(n_cols);
    
    Signal<double> basisTranspose = transpose(basis);
    Signal<double> basisPhi = matMult(basisTranspose,Phi);
    Signal<double> betaBasisPhi = basisPhi * beta;
    Signal<double> basisTargets = matMult(basisTranspose,targets);
    
    // full statistics
    fullStatistics(basis, Phi, includedBasis, targets, indecesUsed, insModel, alphas, beta, mean, covariance, S_in, Q_in, S_out, Q_out, relevanceFactors, basisPhi, betaBasisPhi, basisTargets);
    
    int addedBasis = 0;
    int count = 0;
    int iterations = 0;
    bool lastIteration = false;
    // start the big loop
    while(!lastIteration) {
	iterations++;
	if (m_printProgress && iterations % 5 == 0) std::cout << "." << std::flush;

	bool updateIteration = true;
	bool anyToAdd = false;
	int addBasis = 1;
	// compute change in ML for all possible updates
	Signal<double> deltaML(n_cols);
	Signal<int> action(n_cols);
	for (int j = 0; j < n_cols; ++j) {
	    if (relevanceFactors(j) > zeroFactor && !insModel(j)) {
		double quot = Q_in(j)*Q_in(j) / S_in(j);
		deltaML(j) = (quot - 1 - log(quot))/2;
		action(j) = addBasis;
		anyToAdd = true;
	    }
	}
	double maxDeltaML = 0;
	int chosenBasis = 0;
	int selectedAction = 0;
	for (int j = 0; j < n_cols; ++j) {
	    if (deltaML(j) > maxDeltaML) {
		maxDeltaML = deltaML(j);
		chosenBasis = j;
		selectedAction = action(j);
	    }
	}

	Signal<double> currentBasisFunction = basis.getPatch(0,chosenBasis,n_rows,1);
	double newAlpha = S_out(chosenBasis)*S_out(chosenBasis) / relevanceFactors(chosenBasis);

	if (selectedAction == addBasis) {
	    indecesUsed(includedBasis) = chosenBasis;
	    whereIs(chosenBasis) = includedBasis;
	    insModel(chosenBasis) = true;
	    Signal<double> BASIS_phi = matMult(basisTranspose, currentBasisFunction);
	    basisPhi.putPatch(BASIS_phi,0,includedBasis);
	    Signal<double> B_phi = currentBasisFunction * beta;
	    Signal<double> betaBASIS_phi = BASIS_phi * beta;
	    Signal<double> temp1 = matMult(transpose(B_phi), Phi).getPatch(0,0,1,includedBasis);
	    Signal<double> temp2 = matMult(temp1, covariance.getPatch(0,0,includedBasis,includedBasis));
	    alphas(chosenBasis) = newAlpha;
	    Phi.putPatch(currentBasisFunction,0,includedBasis);
	    
	    double s_ii = 1/(newAlpha + S_in(chosenBasis));
	    Signal<double> s_i(includedBasis);
	    for (int i = 0; i < includedBasis; ++i) s_i(i) = -s_ii*temp2(1,i);
	    Signal<double> tau(includedBasis,includedBasis);
	    tau = matMult(s_i, temp2);
	    for (int i = 0; i < includedBasis; ++i) 
		for (int j = 0; j < includedBasis; ++j)
		    covariance(i,j) = covariance(i,j) - tau(i,j);
	    for (int j = 0; j < includedBasis; ++j)
		covariance(j,includedBasis) = covariance(includedBasis,j) = s_i(j);
	    covariance(includedBasis, includedBasis) = s_ii;
	    
	    double mu_i = s_ii*Q_in(chosenBasis);
	    Signal<double> deltaMu(includedBasis+1);
	    for (int i = 0; i < includedBasis; ++i) 
		deltaMu(i) = -mu_i*temp2(i);
	    deltaMu(includedBasis) = mu_i;

	    for (int i = 0; i < includedBasis; ++i)
		mean(i) += deltaMu(i);
	    mean(includedBasis) = deltaMu(includedBasis);
	    
	    Signal<double> temp3 = matMult(betaBasisPhi.getPatch(0,0,n_cols,includedBasis), temp2);
	    Signal<double> mCI(n_cols);
	    for (int i = 0; i < n_cols; ++i)
		mCI(i) = betaBASIS_phi(i) - temp3(i);
	    for (int j = 0; j < n_cols; ++j) {
		S_in(j) -= s_ii*mCI(j)*mCI(j);
		Q_in(j) -= mu_i*mCI(j);
	    }
	    includedBasis++;
	    addedBasis++;
	}	    		   	
	
	for (int j = 0; j < n_cols; ++j) {
	    S_out(j) = S_in(j);
	    Q_out(j) = Q_in(j);
	    if (insModel(j)) {
		S_out(j) = alphas(j)*S_in(j) / (alphas(j) - S_in(j));
		Q_out(j) = alphas(j)*Q_in(j) / (alphas(j) - S_in(j));
	    }
	    relevanceFactors(j) = Q_out(j)*Q_out(j) - S_out(j);
	}
	betaBasisPhi = basisPhi * beta;

	if (maxDeltaML < m_deltaML) lastIteration = true;
    }

    for (int i = 0; i < n_cols; ++i)
	if (insModel(i))
	    m_coeff(i) = mean(whereIs(i))/scaling(i);
       
}

void fullStatistics(const Signal<double>& basis, const Signal<double>& phi, int includedBasis, const Signal<double> targets, const Signal<int> indecesUsed, const Signal<bool> insModel, const Signal<double>& alphas, double beta, Signal<double>& mean, Signal<double>& covariance, Signal<double>& S_in, Signal<double>& Q_in, Signal<double>& S_out, Signal<double> Q_out, Signal<double>& relevanceFactors, const Signal<double>& basisPhi, Signal<double>& betaBasisPhi, Signal<double>& basisTargets)
{
    int n_rows = basis.height();
    int n_cols = basis.width();
    Signal<double> tempPhi = phi.getPatch(0,0,n_rows, includedBasis);
    Signal<double> tempPhiTranspose = transpose(tempPhi);
    Signal<double> tempTempCova = matMult(tempPhiTranspose, tempPhi);

    for (int i = 0; i < includedBasis; ++i)
	for (int j = 0; j < includedBasis; ++j)
	    if (i==j) tempTempCova(i,j) = beta*tempTempCova(i,j) + alphas(indecesUsed(i));
	    else tempTempCova(i,j) *= beta;
    
    Signal<double> chol = cholesky(tempTempCova);
    Signal<double> invChol = inversed(chol);
    Signal<double> invCholTranspose = transpose(invChol);
    Signal<double> tempCova = matMult(invChol, invCholTranspose);
    
    Signal<double> tempTempMean = matMult(tempPhiTranspose, targets);
    Signal<double> tempMean = beta * matMult(tempCova, tempTempMean);

    Signal<double> sumInvChol(includedBasis);
    for (int i = 0; i < includedBasis; ++i) 
	for (int j = 0; j < includedBasis; ++j)
	    sumInvChol(i) += invChol(i) + invChol(i,j)*invChol(i,j);

    Signal<double> betaBasisPhiNew = betaBasisPhi.getPatch(0,0,n_cols,includedBasis);
    Signal<double> betaChol = matMult(betaBasisPhiNew, invChol);
    for (int i = 0; i < n_cols; ++i)
	for (int j = 0; j < includedBasis; ++j)
	    betaChol(i,j) *= betaChol(i,j);

    Signal<double> sums(n_cols);
    for (int i = 0; i < n_cols; ++i)
	for (int j = 0; j < includedBasis; ++j)
	    sums(i) += betaChol(i,j);
    
    for (int j = 0; j < includedBasis; ++j) {
	Signal<double> columnBasisPhi = basisPhi.getPatch(j,0,includedBasis, 1);
	S_out(j) = S_in(j) = beta - sums(j);
	Q_out(j) = Q_in(j) = beta*basisTargets(j) - beta*dot(columnBasisPhi, tempMean);
	
	if(insModel(j)) {
	    S_out(j) = alphas(j)*S_in(j) / (alphas(j) - S_in(j));
	    Q_out(j) = alphas(j)*Q_in(j) / (alphas(j) - S_in(j));
	}
	relevanceFactors(j) = Q_out(j)*Q_out(j) - S_out(j);
    }
    for (int i = 0; i < includedBasis; ++i){
	mean(i) = tempMean(i);
	for (int j = 0; j < includedBasis; ++j) {
	    covariance(i,j) = tempCova(i,j);
	}
    }

}

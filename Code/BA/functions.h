#include <cmath>

template <typename T> 
Signal<T> corruptSignal(const Signal<T>& orig, Signal<bool>& sensed, const Corrupter& corr)
{
    if (orig.height() != sensed.height() || 
	orig.width() != sensed.width() ||
	orig.frames() != sensed.frames())
	error("Signal and Mask must have same dimensions");

    int h = orig.height();
    int w = orig.width();
    int f = orig.frames();
    double randNum;
    Signal<bool> filterBlock(h,w,f); // initialized to bool(0) == false
    Signal<T> corrSignal(h,w,f);     // initialized to 0

    for (int i = 0; i < h; ++i) {
	for (int j = 0; j < w; ++j) {
	    for (int k = 0; k < f; ++k) {
		randNum = ((double) rand()) / RAND_MAX; //between 0 and 1
		if (randNum < corr.perc/100) filterBlock(i,j,k) = false;
		else filterBlock(i,j,k) = true;
	    }
	}
    }

    for (int k = 0; k < f; ++k) {
	for (int i = 0; i < h; ++i) {
	    for (int j = 0; j < w; ++j) {
		switch (corr.setting) {
		case Corrupter::uniform:
		    sensed(i,j,k) = filterBlock(i,j,k);
		    break;
		case Corrupter::timeRays:
		    sensed(i,j,k) = filterBlock(i,j,0);
		    break;
		case Corrupter::verticalFlicker:
		    sensed(i,j,k) = filterBlock(0,j,k);
		    break;
		case Corrupter::horizontalFlicker:
		    sensed(i,j,k) = filterBlock(i,0,k);
		    break;
		case Corrupter::missingFrames:
		    sensed(i,j,k) = filterBlock(0,0,k);
		    break;
		case Corrupter::verticalLines:
		    sensed(i,j,k) = filterBlock(0,j,0);
		    break;
		case Corrupter::horizontalLines:
		    sensed(i,j,k) = filterBlock(i,0,0);
		    break;
		}

		if (sensed(i,j,k)) corrSignal(i,j,k) = orig(i,j,k);
		else corrSignal(i,j,k) = 0;
	    }
	}
    }

    return corrSignal;
}

Signal<double> haarPhiMatrixTranspose(int rows)
{
    if (rows < 1) error("need positive number of rows");
    if (rows%2 != 0) error("number of rows must be even");
    
    Signal<double> phiT(rows, rows/2); // default initialized to zero
    const double irt2 = 1/std::sqrt(2);

    for (int i = 0; i < rows; ++i)
	for (int j = 0; j < rows/2; ++j)
	    if (i == 2*j || i == 2*j + 1) phiT(i,j) = irt2;

    return phiT;
}

Signal<double> haarPsiMatrixTranspose(int rows)
{
    if (rows < 1) error("need positive number of rows");
    if (rows%2 != 0) error("number of rows must be even");
    
    Signal<double> psiT(rows, rows/2); // default initialized to zero
    const double irt2 = 1/std::sqrt(2);

    for (int i = 0; i < rows; ++i) {
	for (int j = 0; j < rows/2; ++j) {
	    if (i == 2*j) psiT(i,j) = irt2;
	    else if (i == 2*j + 1) psiT(i,j) = -irt2;
	}
    }	    

    return psiT;
}

template<typename T>
Signal<T> kronecker(const Signal<T>& A, const Signal<T>& B)
{
    if (A.frames() != 1) error("A needs to be at most 2D");
    if (B.frames() != 1) error("B needs to be at most 2D");

    Signal<T> C(A.height()*B.height(), A.width()*B.width());
    for (int i = 0; i < A.height(); ++i) 
	for (int j = 0; j < A.width(); ++j)
	    for (int p = 0; p < B.height(); ++p) 
		for (int q = 0; q < B.width(); ++q) 
		    C(i*B.height()+p,j*B.width()+q) = A(i,j) * B(p,q);
    return C;
}

template<typename T>
Signal<T> matMult(const Signal<T>& A, const Signal<T>& B)
{
    if (A.frames() != 1 || B.frames() != 1) 
	error("multiplication not defined for 3D signals");
    if (A.width() != B.height())
	error("inner matrix dimensions must agree");
    Signal<T> ret(A.height(), B.width());
    for (int i = 0; i < A.height(); ++i)
	for (int j = 0; j < B.width(); ++j)
	    for (int k = 0; k < A.width(); ++k)
		ret(i,j) += A(i,k) * B(k,j);

    return ret;
}

Signal<double> generateLL(int scale, int currentScale, int h, int w, 
			  Signal<double> rPreFactor, Signal<double> cPreFactor)
{
    int const dimDivider = pow(2, scale-1);
    int const currentDimDivider = pow(2, currentScale);
    int const smallH = h/currentDimDivider;
    int const smallW = w/currentDimDivider;
    int const smallestH = h/dimDivider;
    int const smallestW = w/dimDivider;
    
    Signal<double> LL(h*w, h*w/4);

    if (currentScale == scale) {
	LL = kronecker(cPreFactor, rPreFactor);
	return LL;
    }

    if (currentScale < scale) {
	Signal<double> new_rPhiT = haarPhiMatrixTranspose(smallH);
	Signal<double> new_rPsiT = haarPsiMatrixTranspose(smallH);
	Signal<double> new_cPhiT = haarPhiMatrixTranspose(smallW);
	Signal<double> new_cPsiT = haarPsiMatrixTranspose(smallW);
	
	Signal<double> rPre_Phi = matMult(rPreFactor,new_rPhiT);
	Signal<double> rPre_Psi = matMult(rPreFactor,new_rPsiT);
	Signal<double> cPre_Phi = matMult(cPreFactor,new_cPhiT);
	Signal<double> cPre_Psi = matMult(cPreFactor,new_cPsiT);
		
	int full_dim1 = h*w;
	int full_dim2 = smallH * smallW / 4;
	Signal<double> tempFull(full_dim1, full_dim2);
	Signal<double> new_LL(full_dim1, full_dim2);

	tempFull = kronecker(cPre_Phi, rPre_Psi); // new LH
	LL.putPatch(tempFull, 0, full_dim2);
	tempFull = kronecker(cPre_Psi, rPre_Phi); // new HL
	LL.putPatch(tempFull, 0, 2*full_dim2);
	tempFull = kronecker(cPre_Psi, rPre_Psi); // new HH
	LL.putPatch(tempFull, 0, 3*full_dim2);

	new_LL = generateLL(scale, currentScale+1, h, w, rPre_Phi, cPre_Phi);
	LL.putPatch(new_LL, 0, 0);
	return LL;
    }

    if (currentScale > scale) error("impossible");
}

Signal<double> haarBasis2D(int h, int w, int scale)
{
    Signal<double> rPhiT(h, h/2);	// filters
    Signal<double> rPsiT(h, h/2);
    Signal<double> cPhiT(w, w/2);
    Signal<double> cPsiT(w, w/2);
    
    int sizeFull = h*w;		
    Signal<double> tmp(sizeFull, sizeFull/4);    
    Signal<double> LL(sizeFull, sizeFull/4);

    rPhiT = haarPhiMatrixTranspose(h); // generate filters
    rPsiT = haarPsiMatrixTranspose(h);
    cPhiT = haarPhiMatrixTranspose(w);
    cPsiT = haarPsiMatrixTranspose(w);
    
    Signal<double> basis(sizeFull, sizeFull);
    tmp = kronecker(cPhiT, rPsiT); // LH
    basis.putPatch(tmp,0,sizeFull/4);
    tmp = kronecker(cPsiT, rPhiT); // HL
    basis.putPatch(tmp,0, 2*sizeFull/4);
    tmp = kronecker(cPsiT, rPsiT); // HH
    basis.putPatch(tmp,0, 3*sizeFull/4);

    int startScale = 1;
    tmp = generateLL(scale, startScale, h, w, rPhiT, cPhiT);
    basis.putPatch(tmp, 0, 0);	// LL

    return basis;
}	 

Signal<double> generateLLL(int scale, int currentScale, int h, int w, int f,
			   Signal<double> rPreFactor, Signal<double> cPreFactor,
			   Signal<double> sPreFactor)
{
    int const dimDivider = pow(2, scale-1);
    int const currentDimDivider = pow(2, currentScale);
    int const smallH = h/currentDimDivider;
    int const smallW = w/currentDimDivider;
    int const smallF = f/currentDimDivider;
    int const smallestH = h/dimDivider;
    int const smallestW = w/dimDivider;
    int const smallestF = f/dimDivider;
    
    Signal<double> LLL(h*w*f, h*w*f/8);

    if (currentScale == scale) {
	int CR_dim1 = w*h, CR_dim2 = smallestW*smallestH /4;
	int full_dim1 = f*w*h;
	int full_dim2 = smallestF*smallestW*smallestH / 8;
	Signal<double> tempCR(CR_dim1, CR_dim2);
	tempCR = kronecker(cPreFactor, rPreFactor);
	LLL = kronecker(sPreFactor, tempCR);
	return LLL;
    }

    if (currentScale < scale) {


	Signal<double> new_rPhiT = haarPhiMatrixTranspose(smallH);
	Signal<double> new_rPsiT = haarPsiMatrixTranspose(smallH);
	Signal<double> new_cPhiT = haarPhiMatrixTranspose(smallW);
	Signal<double> new_cPsiT = haarPsiMatrixTranspose(smallW);
	Signal<double> new_sPhiT = haarPhiMatrixTranspose(smallF);
	Signal<double> new_sPsiT = haarPsiMatrixTranspose(smallF);

	Signal<double> rPre_Phi = matMult(rPreFactor,new_rPhiT);
	Signal<double> rPre_Psi = matMult(rPreFactor,new_rPsiT);
	Signal<double> cPre_Phi = matMult(cPreFactor,new_cPhiT);
 	Signal<double> cPre_Psi = matMult(cPreFactor,new_cPsiT);
	Signal<double> sPre_Phi = matMult(sPreFactor,new_sPhiT);
	Signal<double> sPre_Psi = matMult(sPreFactor,new_sPsiT);

	int CR_dim1 = h*w;
	int CR_dim2 = smallH * smallW / 4;
	int full_dim1 = h*w*f;
	int full_dim2 = smallH * smallW * smallF / 8;
	Signal<double> tempCR(CR_dim1, CR_dim2);
	Signal<double> tempFull(full_dim1, full_dim2);
	Signal<double> new_LLL(full_dim1, full_dim2);

	tempCR = kronecker(cPre_Phi, rPre_Psi); // new LLH
	tempFull = kronecker(sPre_Phi, tempCR);
	LLL.putPatch(tempFull, 0, full_dim2);
	tempCR = kronecker(cPre_Psi, rPre_Phi); // new LHL
	tempFull = kronecker(sPre_Phi, tempCR);
	LLL.putPatch(tempFull, 0, 2*full_dim2);
	tempCR = kronecker(cPre_Psi, rPre_Psi); // new LHH
	tempFull = kronecker(sPre_Phi, tempCR);
	LLL.putPatch(tempFull, 0, 3*full_dim2);

	tempCR = kronecker(cPre_Phi, rPre_Phi); // new HLL
	tempFull = kronecker(sPre_Psi, tempCR);
	LLL.putPatch(tempFull, 0, 4*full_dim2);
	tempCR = kronecker(cPre_Phi, rPre_Psi); // new HLH
	tempFull = kronecker(sPre_Psi, tempCR);
	LLL.putPatch(tempFull, 0, 5*full_dim2);
	tempCR = kronecker(cPre_Psi, rPre_Phi); // new HHL
	tempFull = kronecker(sPre_Psi, tempCR);
	LLL.putPatch(tempFull, 0, 6*full_dim2);
	tempCR = kronecker(cPre_Psi, rPre_Psi); // new HHH
	tempFull = kronecker(sPre_Psi, tempCR);
	LLL.putPatch(tempFull, 0, 7*full_dim2);

	new_LLL = generateLLL(scale, currentScale+1, h, w, f, rPre_Phi, cPre_Phi, sPre_Phi);
	LLL.putPatch(new_LLL, 0, 0);
	return LLL;
    }

    if (currentScale > scale) error("impossible");
}


Signal<double> haarBasis(int h, int w, int f, int scale)
{
    Signal<double> rPhiT(h, h/2);	// filters
    Signal<double> rPsiT(h, h/2);
    Signal<double> cPhiT(w, w/2);
    Signal<double> cPsiT(w, w/2);
    Signal<double> sPhiT(f, f/2);
    Signal<double> sPsiT(f, f/2);
    
    int sizeFull = h*w*f, sizeCR = w*h;
    Signal<double> tmp(sizeFull, sizeFull/8);    
    Signal<double> LLL(sizeFull, sizeFull/8);
    Signal<double> tmpCR(sizeCR, sizeCR/4);

    rPhiT = haarPhiMatrixTranspose(h); // generate filters
    rPsiT = haarPsiMatrixTranspose(h);
    cPhiT = haarPhiMatrixTranspose(w);
    cPsiT = haarPsiMatrixTranspose(w);
    sPhiT = haarPhiMatrixTranspose(f);
    sPsiT = haarPsiMatrixTranspose(f);

    Signal<double> basis(sizeFull, sizeFull);
    tmpCR = kronecker(cPhiT, rPsiT); // LLH
    tmp = kronecker(sPhiT, tmpCR);
    basis.putPatch(tmp,0,sizeFull/8);
    tmpCR = kronecker(cPsiT, rPhiT); // LHL
    tmp = kronecker(sPhiT, tmpCR);
    basis.putPatch(tmp,0, 2*sizeFull/8);
    tmpCR = kronecker(cPsiT, rPsiT); // LHH
    tmp = kronecker(sPhiT, tmpCR);
    basis.putPatch(tmp,0, 3*sizeFull/8);

    
    tmpCR = kronecker(cPhiT, rPhiT); // HLL
    tmp = kronecker(sPsiT, tmpCR);   
    basis.putPatch(tmp, 0, 4*sizeFull/8);
    tmpCR = kronecker(cPhiT, rPsiT); // HLH
    tmp = kronecker(sPsiT, tmpCR);
    basis.putPatch(tmp, 0, 5*sizeFull/8);
    tmpCR = kronecker(cPsiT, rPhiT); // HHL
    tmp = kronecker(sPsiT, tmpCR);
    basis.putPatch(tmp, 0, 6*sizeFull/8);
    tmpCR = kronecker(cPsiT, rPsiT); // HHH
    tmp = kronecker(sPsiT, tmpCR);
    basis.putPatch(tmp, 0, 7*sizeFull/8);
		  
    int startScale = 1;		
    tmp = generateLLL(scale, startScale, h, w, f, rPhiT, cPhiT, sPhiT);
    basis.putPatch(tmp, 0, 0);	// LL

    return basis;
}	 
 
Signal<double> dctMatrix(int dim)
{
    double M = (double) dim;
    Signal<double> dct(dim, dim);
    for (int p = 0; p < dim; ++p) {
	for (int q = 0; q < dim; ++q) {
	    if (p == 0) dct(p,q) = 1/std::sqrt(M);
	    else dct(p,q) = std::sqrt(2/M)*std::cos(M_PI*(2*q+1)*p/(2*M));
	}
    }
    return dct;
}

Signal<double> dctBasis2D(int h, int w)
{
    Signal<double> basis(w*h, w*h);
    Signal<double> basisTran(w*h, w*h);
    Signal<double> rowDCT = dctMatrix(h);
    Signal<double> colDCT = dctMatrix(w);

    basisTran = kronecker(colDCT, rowDCT);
    basis = transpose(basisTran);
    return basis;
}

Signal<double> dctBasis(int h, int w, int f)
{
    if (f == 1) 
	return dctBasis2D(h, w);
    
    int dictionarySize = h*w*f;
    Signal<double> basis(dictionarySize, dictionarySize);

    Signal<double> rowDCT = dctMatrix(h);
    Signal<double> colDCT = dctMatrix(w);
    Signal<double> sliceDCT = dctMatrix(f);
    
    Signal<double> tmp(h*w, h*w);
    Signal<double> basisTran(dictionarySize, dictionarySize);    
    tmp = kronecker(colDCT, rowDCT);
    basisTran = kronecker(sliceDCT, tmp);
    
    basis = transpose(basisTran);
    return basis;
}


Signal<double> getBasis(int height, int width, int frames, basisFunctionMode basisMode, int scale = 1)
{
    Signal<double> ret(height*width*frames, height*width*frames);
    switch (basisMode) {
    case haar:
	ret = haarBasis(height, width, frames, scale);
	return ret;
    case dct:
	ret = dctBasis(height, width, frames);
	return ret;
    }
}

int countSensed(const Signal<bool>& sensed)
{
    int count = 0; 
    int sz = sensed.size();
    for (int i = 0; i < sz; ++i) 
	if (sensed.data()[i]) ++count;

    return count;	
}

template <typename T>
Signal<T> getTargets (const Signal<T>& corrSignal,
		      const Signal<bool>& sensed)
{
    Signal<T> target(countSensed(sensed));
    int currentIdx = 0;
    int sz = corrSignal.size();
    for (int i = 0; i < sz; ++i) {
	if (sensed(i)) {	// Assumes Signals are vectorized
	    target(currentIdx) = corrSignal(i);
	    ++currentIdx;
	}
    }
    return target;
}


template <typename T>
Signal<T> getDesignMatrix(const Signal<T>& orig,
			  const Signal<bool>& sensed)
{
    int currentIdx = 0;
    int h = orig.height();
    int w = orig.width();
    Signal<T> designMatrix(countSensed(sensed), w);
    
    for (int i = 0; i < h; ++i) {
	if (sensed(i)) {	// assumes vectorized
	    for (int j = 0; j < w; ++j) {		
		designMatrix(currentIdx, j) = orig(i,j); // assumes orig is 2D
	    }
	    ++currentIdx;
	}
    }

    return designMatrix;
}

template<typename T>
Signal<T> reshape(Signal<T> orig, int h, int w, int f = 1) 
{
    int sz = h*w*f;
    if (orig.size() != sz) error("number of elements must not change");

    Signal<T> ret(h, w, f);
    for (int i = 0; i < sz; ++i) ret.data()[i] = orig.data()[i];
    return ret;
}

template<typename T>
Signal<T> reshape(Signal<T> orig, SigDim dim) 
{
    return reshape(orig, dim.height(), dim.width(), dim.frames());
}

Signal<double> read(const std::string& inputFile, int numFrames = 1)
{
    if (numFrames < 1) error("numFrames must be positive");
    std::ifstream in(inputFile.c_str());
    if(!in) error("couldn't open file ", inputFile);
    
    std::string line;
    double entry;
    
    int rows = 0;
    int cols = 0;
    int prevCols = 0;		// for checking if all rows have same #cols
    bool firstLine = true;
    
    while(std::getline(in, line)) {
	++rows;

	std::istringstream os(line);
	cols = 0;
	while(os >> entry) {
	    ++cols;
	}
	if (firstLine) firstLine = false;
	else 
	    if (cols != prevCols) 
		error("rows must have same number of entries in file ", inputFile);
	
	prevCols = cols;
    }

    // initialize Signal and go back to beginning of the file
    if (rows % numFrames != 0) {
	std::cerr << "numFrames does not divide number of rows in file "
		  << inputFile << ": Setting numFrames == 1 (default)\n\n";
	numFrames = 1;
    }
    
    rows /= numFrames;

    Signal<double> ret(rows,cols, numFrames);
    in.clear();
    in.seekg(0, std::ios::beg);

    int idx = 0;
    while(in >> entry) {
	ret.data()[idx] = entry;
	++idx;
    }

    return ret;
}

double norm(const Signal<double> A)
{
    if (A.width() != 1 || A.frames() != 1) error("can only take norm of a vector");

    double p = 0;    
    int length = A.height();
    for (int i = 0; i < length; ++i) p += A(i)*A(i);
	
    return std::sqrt(p);
}

double dot(const Signal<double> a, const Signal<double> b)
{
    if (a.width() != 1 || a.frames() != 1 || b.width() != 1 || b.frames() != 1
	|| a.height() != b.height()) error("arguments must be vectors of the same length");
    double ret = 0;
    int length = a.height();
    for (int i = 0; i < length; ++i) ret += a(i)*b(i);

    return ret;
}


Signal<double> cholesky(const Signal<double>& A)
{
    if (A.frames() != 1 || A.height() != A.width()) error("input must be square matrix");
    int N = A.height();

    Signal<double> chol(N,N);
    for (int i = 0; i < N; ++i) {
	chol(i,i) = A(i,i);
	for (int k = 0; k < i; ++k) chol(i,i) -= chol(k,i)*chol(k,i);
	chol(i,i) = std::sqrt(chol(i,i));

	for (int j = i+1; j < N; ++j) {
	    chol(i,j) = A(i,j);
	    for (int k = 0; k < i; ++k) chol(i,j) -= chol(k,i)*chol(k,i);
	    chol(i,j) /= chol(i,i);
	}
    }
    
    return chol;
}

Signal<double> inversed(const Signal<double>& A)
{
   if (A.frames() != 1 || A.height() != A.width()) error("input must be square matrix");
   int N = A.height();

   Signal<double> inv(N,N);
   for (int j = 0; j < N; ++j) {
       inv(j,j) = 1.0/A(j,j);
       
       for (int i = 0; i < j; ++i)
	   for (int k = 0; k < j; ++k) 
	       inv(i,j) += inv(i,k)*A(k,j);
       for (int k = 0; k < j; ++k)
	   inv(k,j) /= -A(j,j);
   }

   return inv;
}

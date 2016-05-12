void testBasis(std::string inputFile, int h, int w, int f, int scale, double errorTolerance)
{    
    const int dictSize = h*w*f;
    basisType **truePSI = new basisType*[dictSize];
    basisType **testPSI = new basisType*[dictSize];
    for (int i = 0; i < dictSize; ++i) {
	truePSI[i] = new basisType[dictSize];
	testPSI[i] = new basisType[dictSize];
    }
    
    std::ifstream trueBasis(inputFile.c_str());
    basisType number;
    int row = 0;
    int col = 0;
    while(trueBasis >> number) {
	truePSI[row][col] = number;
	if (col ==  dictSize-1) {
	    col = 0;
	    ++row;
	} else {
	    ++col;
	}		 
    }
    trueBasis.close();
    
    haarBasis(testPSI,h,w,f,scale);

    double totalError = 0;
    int misMatches = 0;
    for (int i = 0; i < dictSize; ++i) {
	for (int j = 0; j < dictSize; ++j) {
	    double currentError = fabs(truePSI[i][j] - testPSI[i][j]);
	    if (currentError > errorTolerance) {
		++misMatches;
	    }
	    totalError += currentError;
	}
    }

    for (int i = 0; i < dictSize; ++i) {
	delete[] truePSI[i];
	delete[] testPSI[i];
    }
    delete[] truePSI;
    delete[] testPSI;
    
    if (misMatches != 0 or totalError > errorTolerance*dictSize) {
	std::stringstream errorMessage;
	errorMessage << "Mismatch in file " << inputFile
		     << "\nNumber of Mismatches = "
		     << misMatches << " / "
		     << dictSize*dictSize 
		     << "\nAverage Error = "
		     << totalError/dictSize << "\n\n"; 
	throw std::runtime_error(errorMessage.str());
    }
}

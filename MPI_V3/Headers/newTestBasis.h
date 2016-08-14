void newTestBasis(std::string inputFile, int h, int w, int f, int scale, double errorTolerance)
{
    const int dictSize = h*w*f;

    Signal<double> truePSI(dictSize, dictSize);
    Signal<double> testPSI(dictSize, dictSize);
    
    truePSI.read(inputFile);
    testPSI = haarBasis(h,w,f,scale);

    double totalError = 0;
    int misMatches = 0;
    for (int i = 0; i < dictSize; ++i) {
	for (int j = 0; j < dictSize; ++j) {
	    double currentError = fabs(truePSI(i,j) - testPSI(i,j));
	    if (currentError > errorTolerance) {
		++misMatches;
	    }
	    totalError += currentError;
	}
    }
    if (misMatches != 0 or totalError > errorTolerance*dictSize) {
	std::stringstream outName;
	outName << "/local/data/public/ba308/testFiles/FAIL_" << h << "_" << w << "_" << f << "_" << scale << ".txt";
	std::ofstream out(outName.str().c_str()); 
	if (!out) std::cout << "Error: could not open " << outName.str();
	out << testPSI;
	
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

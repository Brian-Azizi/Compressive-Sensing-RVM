#define _Use_MATH_DEFINES

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>		// Needed by full_statistics (I think)
#include <stdexcept>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

#include "classes.h"		// class definitions
#include "functions.h"		// function definitions
#include "rvm.h"

// GGP's files
// #include "Headers/utilities.h"	// linear algebra utilities
// #include "Headers/init.h"	// Needed for fast_updates
// #include "Headers/full_statistics.h" // Needed for fast_updates
// #include "Headers/fast_updates.h"    // RVM engine

// BA's files
#include "settingsFile.h"
#include "Headers/settingsTester.h" // test that settings file is okay
//#include "Headers/output3D.h"	// saves output into text files 

int main()
{
    /* Test if Settings make sense and define constants.
       Note: Does not test if input txt file is valid! */
    testSettings();
    
    /*** RNG Settings ***/
    std::srand(rngSeed);
    
    /*** Program constants ***/
    unsigned int const numBlocksHeight = signalHeight / blockHeight;
    unsigned int const numBlocksWidth = signalWidth / blockWidth;
    unsigned int const numBlocksFrames = signalFrames / blockFrames;
    
    unsigned int const signalSize = signalHeight*signalWidth*signalFrames;
    unsigned int const blockSize = blockHeight*blockWidth*blockFrames;
    unsigned int const dictionarySize = blockSize;

    unsigned int const cascadeSize = endScale - startScale + 1;
    
    /*** Memory allocation ***/
    Signal<signalType> signal(signalHeight, signalWidth, signalFrames);
    Signal<signalType> corruptedSignal(signalHeight, signalWidth, signalFrames);
    Signal<bool> sensedEntries(signalHeight, signalWidth, signalFrames);

    Signal<signalType> signalPatch(blockHeight, blockWidth, blockFrames);
    Signal<bool> sensedPatch(blockHeight, blockWidth, blockFrames);

    Signal<signalType> signalPatchVector(blockSize);
    Signal<bool> sensedPatchVector(blockSize);
    Signal<bool> initialSensedVector(blockSize);
    Signal<signalType> initialSignalVector(blockSize);
    Signal<basisType> recoveredVector(blockSize);
    Signal<basisType> recoveredPatch(blockHeight, blockWidth, blockFrames);
    
    std::vector<Signal<basisType> > cascadeRecoveredSignals
	(cascadeSize, Signal<basisType>(signalHeight, signalWidth, signalFrames));
    std::vector<Signal<basisType> > cascadeBasis
	(cascadeSize, Signal<basisType>(dictionarySize, dictionarySize));
    
    /*** start logic ***/
    signal.read(inputFile);
    corruptedSignal = corruptSignal(signal, sensedEntries, corr);
        
    // Get basis matrices for various scales
    for (int s = 0; s < cascadeSize; ++s) {
	cascadeBasis[s] = getBasis(blockHeight, blockWidth,  blockFrames, basisMode, startScale+s);
    }
    
    // Loop over blocks of original signal
    for (int blockIndexRows = 0; blockIndexRows < numBlocksHeight; ++blockIndexRows) {
	for (int blockIndexCols = 0; blockIndexCols < numBlocksWidth; ++blockIndexCols) {
	    for (int blockIndexFrames = 0; blockIndexFrames < numBlocksFrames; ++blockIndexFrames) {
		if (printToCOut) {
		    std::cout << "\tPatch (" << blockIndexRows+1 << "," 
			      << blockIndexCols+1 << "," << blockIndexFrames+1
			      << ")    of    (" << numBlocksHeight << "," 
			      << numBlocksWidth << "," << numBlocksFrames 
			      << ")" << std::endl;
		}
		signalPatch = corruptedSignal
		    .getPatch(blockIndexRows*blockHeight, blockIndexCols*blockWidth,
				blockIndexFrames*blockFrames, blockHeight, blockWidth, blockFrames);
		sensedPatch = sensedEntries
		    .getPatch(blockIndexRows*blockHeight, blockIndexCols*blockWidth,
				blockIndexFrames*blockFrames, blockHeight, blockWidth, blockFrames);
		
		signalPatchVector = vectorize(signalPatch);
		sensedPatchVector = vectorize(sensedPatch);
		initialSignalVector = vectorize(signalPatch);
		initialSensedVector = vectorize(sensedPatch);
		
		for (int s = 0; s < cascadeSize; ++s) {		    
		    int measurements = countSensed(sensedPatchVector);
		    
		    /*** Declare and define RVM variables ***/
		    Signal<basisType> targets(measurements);
		    Signal<basisType> designMatrix(measurements, dictionarySize);
		    Signal<basisType> estimatedCoeff(dictionarySize); // init to 0
		    Signal<basisType> errors(dictionarySize);
		    
		    targets = getTargets(signalPatchVector, sensedPatchVector);
		    designMatrix = getDesignMatrix(cascadeBasis[s], sensedPatchVector);

		    /*** Start the RVM ***/
		    bool useCascade;
		    if (s < cascadeSize - 1) useCascade = true;
		    else useCascade = false;
		    
		    RVM<double> rvm;
		    rvm.setSigma(noiseStD);
		    rvm.setDeltaML(deltaML_threshold);
		    rvm.train_fastUpdates(designMatrix, targets);
		    estimatedCoeff = rvm.coefficients();
		    // std::cout << estimatedCoeff;
		    // assert(false);
		    recoveredVector = matMult(cascadeBasis[s], estimatedCoeff);
		    recoveredVector.fill(initialSignalVector, initialSensedVector);
		    
		    /*** Save recovered patch ***/
		    recoveredPatch = reshape(recoveredVector, blockHeight, 
					     blockWidth, blockFrames);
		    
		    cascadeRecoveredSignals[s]
			.putPatch(recoveredPatch, blockIndexRows*blockHeight,
				  blockIndexCols*blockWidth, blockIndexFrames*blockFrames);
							
		    /*** Prepare for next part of cascade ***/
		    if (useCascade) {
			for (int i = 0; i < blockSize; ++i) { 
			    if (errors(i) != 0) sensedPatchVector(i) = true; // get new mask
			    else sensedPatchVector(i) = false;
			    
			    // Recovered becomes new signal Patch to get targets for next stage
			    signalPatchVector(i) = recoveredVector(i); 
			}

		    }
		}				    
	    }
	}
    }

    /*** Output ***/
    std::ofstream corrOut("corrupted.out");
    corrOut << corruptedSignal;
    
    // output3Dsignals(sensedEntries, "mask", actualSimulation);
    // output3Dsignals(corruptedSignal, "corrupted", actualSimulation);
    for (int s = 0; s < cascadeSize; ++s) {
    	std::stringstream label;
    	label << "recovered_" << s+1 << "_of_" << endScale << ".out";
	//    	output3Dsignals(cascadeRecoveredSignals[s], label.str(), \
    	//		actualSimulation);
	std::ofstream recOut(label.str().c_str());
	recOut << cascadeRecoveredSignals[s];
	recOut.close();
    }
    
    
    return 0;   
}

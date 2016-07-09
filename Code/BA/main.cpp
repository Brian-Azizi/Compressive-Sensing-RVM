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

#include "Settings.hpp"
#include "SignalBasis.hpp"
#include "Corrupter.hpp"
#include "Signal.hpp"
#include "RVM.hpp"
#include "settingsFile.hpp"

typedef unsigned int signalType; 
typedef double basisType;


//#include "Headers/settingsTester.h" // test that settings file is okay

int main(int argc, char* argv[])
{
    // if (argc < 2) error("No input file specified");
    // if (argc > 2) error("To many input arguments");
    
    std::srand(rngSeed);    

    /* Input Original Signal */
    Signal<signalType> signal = readSignal(inputFile);
    Signal<signalType> block(blockDim.height(), blockDim.width(), blockDim.frames());

    // /* Test if Settings make sense and define constants.
    //    Note: Does not test if input txt file is valid! */
    // testSettings();

    
    /*** Program constants ***/
    unsigned int const numBlocksHeight = signal.height() / blockDim.height();
    unsigned int const numBlocksWidth = signal.width() / blockDim.width();
    unsigned int const numBlocksFrames = signal.frames() / blockDim.frames();
    unsigned int const blockHeight = blockDim.height();
    unsigned int const blockWidth = blockDim.width();
    unsigned int const blockFrames = blockDim.frames();

    unsigned int const signalSize = signal.size();
    unsigned int const dictionarySize = blockDim.size(), blockSize = blockDim.size();

    unsigned int const cascadeSize = endScale - startScale + 1;
    
    /*** Memory allocation ***/
    Signal<signalType> corruptedSignal(signal.dim());
    Signal<bool> sensedEntries(signal.dim());
    Signal<signalType> signalPatch(blockDim);
    Signal<bool> sensedPatch(blockDim);

    Signal<signalType> signalPatchVector(blockSize);
    Signal<bool> sensedPatchVector(blockSize);
    Signal<bool> initialSensedVector(blockSize);
    Signal<signalType> initialSignalVector(blockSize);
    Signal<basisType> recoveredVector(blockSize);
    Signal<basisType> recoveredPatch(blockDim);
    
    std::vector<Signal<basisType> > cascadeRecoveredSignals
	(cascadeSize, Signal<basisType>(signal.dim()));
    std::vector<Signal<basisType> > cascadeBasis
	(cascadeSize, Signal<basisType>(dictionarySize, dictionarySize));
    
    /*** start logic ***/
    corruptedSignal = corruptSignal(signal, sensedEntries, corr);
        
    // Get basis matrices for various scales
    for (int s = 0; s < cascadeSize; ++s) {
	cascadeBasis[s] = getBasis(blockDim, basisMode, startScale+s);
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
		    .getPatch(blockIndexRows*blockDim.height(), blockIndexCols*blockDim.width(),
			      blockIndexFrames*blockDim.frames(), blockHeight, blockWidth, blockFrames);
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

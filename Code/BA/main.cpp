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


// GGP's files
#include "Headers/utilities.h"	// linear algebra utilities
//#include "Headers/kronecker.h"	     // To calculate kronecker products
#include "Headers/init.h"	// Needed for fast_updates
#include "Headers/full_statistics.h" // Needed for fast_updates
#include "Headers/fast_updates.h"    // RVM engine

// BA's files
#include "Headers/definitions.h" // Needed by settings file
#include "settingsFile.h"
#include "Headers/settingsTester.h" // test that settings file is okay
//#include "Headers/input3D.h" // gets input data from raw txt file and stores in rank3 array
//#include "Headers/print3D.h" // outputs rank3 arrays to an output stream
//#include "Headers/print2D.h" // outputs rank2 arrays to an output stream
//#include "Headers/print1D.h" // outputs rank1 arrays to an output stream
//#include "Headers/getPatch3D.h"	// copies a block out of the larger 3D signal
//#include "Headers/vectorize3D.h" // turns rank3 array (block) into a rank1 array
//#include "Headers/getBasis.h"	 // Interface for basis functions
//#include "Headers/_LL_generate2D.h" // Needed for haarBasis2D
//#include "Headers/haarBasis2D.h"    // Needed for haarBasis
//#include "Headers/_LLL_generate.h"  // Needed for haarBasis
//#include "Headers/haarBasis.h"	    // Needed for getBasis
//#include "Headers/corruptSignal.h" // applies corruption to signal block
//#include "Headers/countSensed.h" // counts uncorrupted pixels in signal block
//#include "Headers/getTargets.h"	// gets target vector from sensed signal pixels
//#include "Headers/getDesignMatrix.h" // gets design matrix from dictionary matrix
//#include "Headers/fillSensedInfo.h" // fills reconstructed signal with previously sensed pixels
//#include "Headers/deVectorize.h" // turns rank1 arrays back into rank3 arrays
//#include "Headers/putPatch3D.h"	// copies a signal block back into the larger 3D signal
#include "Headers/output3D.h"	// saves output into text files 
//#include "Headers/dctMatrix.h" // needed by dctBasis.h and dctBasis2D.h
//#include "Headers/dctBasis2D.h"	// needed by dctBasis.h
//#include "Headers/dctBasis.h"	// needed by getBasis

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
		    .getPatch3D(blockIndexRows*blockHeight, blockIndexCols*blockWidth,
				blockIndexFrames*blockFrames, blockHeight, blockWidth, blockFrames);
		sensedPatch = sensedEntries
		    .getPatch3D(blockIndexRows*blockHeight, blockIndexCols*blockWidth,
				blockIndexFrames*blockFrames, blockHeight, blockWidth, blockFrames);
		
		signalPatchVector = vectorize(signalPatch);
		sensedPatchVector = vectorize(sensedPatch);
		initialSignalVector = vectorize(signalPatch);
		initialSensedVector = vectorize(sensedPatch);
		
		for (int s = 0; s < cascadeSize; ++s) {		    
		    int measurements = countSensed(sensedPatchVector);
		    
		    /*** Declare and define RVM variables ***/
		    Signal<basisType> target(measurements);
		    Signal<basisType> designMatrix(measurements, dictionarySize);
		    Signal<basisType> estimatedCoeff(dictionarysize); // init to 0
		    signal<basisType> errors(dictionarySize);
		    
		    target = getTargets(signalPatchVector, sensedPatchVector);
		    designMatrix = getDesignMatrix(cascadeBasis[s], sensedPatchVector);
		    		    
		    /*** Start the RVM ***/
		    bool useCascade;
		    if (s < cascadeSize - 1) useCascade = true;
		    else useCascade = false;
		    
		    fast_updates(designMatrix, target, estimatedCoeff,\
				 measurements, dictionarySize, noiseStD,\
				 errors, cascadeBasis[s], useCascade,\
				 deltaML_threshold, printToCOut);
		    if (printToCOut)
			std::cout << "||" << std::endl;
		    
		    recoveredVector = matMult(cascadeBasis[s], estimatedCoeff);
		    recoveredVector.fill(initialSignalVector, initialSensedVector);
		    
		    /*** Save recovered patch ***/
		    recoveredPatch = reshape(recoveredVector, blockHeight, 
					     blockWidth, blockFrames);
		    
		    cascadeRecoveredSignals[s]
			.putPatch(recoveredPatch, blockIndexRows*blockHeight,
				  blockIndexCols*blockWidth, blockIndexFrame*blockFrames);
							
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
    output3Dsignals(sensedEntries, "mask", actualSimulation);
    output3Dsignals(corruptedSignal, "corrupted", actualSimulation);
    for (int s = 0; s < cascadeSize; ++s) {
	std::stringstream label;
	label << "recovered_" << s+1 << "_of_" << endScale;
	output3Dsignals(cascadeRecoveredSignals[s], label.str(),\
			actualSimulation);
    }

    return 0;   
}

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <vector>

// GGP's files
#include "Headers/utilities.h"
#include "Headers/kronecker.h"
#include "Headers/fast_updates.h"

// BA's files
#include "settingsFile.h"
#include "Headers/settingsTester.h"
#include "Headers/input3D.h"
#include "Headers/print3D.h"
#include "Headers/print2D.h"
#include "Headers/print1D.h"
#include "Headers/haarTransform3D.h"
#include "Headers/getPatch3D.h"
#include "Headers/vectorize3D.h"
#include "Headers/haarBasis.h"
#include "Headers/corruptSignal.h"
#include "Headers/countSensed.h"
#include "Headers/getTargets.h"
#include "Headers/getDesignMatrix.h"
#include "Headers/fillSensedInfo.h"
#include "Headers/deVectorize.h"
#include "Headers/putPatch3D.h"
#include "Headers/output3D.h"

int main()
{
    /* Test if Settings make sense and define constants.
       Note: No testing if input txt file is valid! */
    testSettings();
    
    /*** RNG Settings ***/
    std::srand(2); 
    
    /*** Program constants ***/
    unsigned int const numBlocksHeight = signalHeight / blockHeight;
    unsigned int const numBlocksWidth = signalWidth / blockWidth;
    unsigned int const numBlocksFrames = signalFrames / blockFrames;
    
    unsigned int const signalSize = signalHeight*signalWidth*signalFrames;
    unsigned int const blockSize = blockHeight*blockWidth*blockFrames;
    unsigned int const dictionarySize = blockSize;

    unsigned int const cascadeSize = endScale - startScale + 1;
    
    /*** Memory allocation ***/
    signalType ***signal = new signalType**[signalHeight]; // Original signal container 
    signalType ***corruptedSignal = new signalType**[signalHeight]; // Corrupted signal container
    bool ***sensedEntries = new bool**[signalHeight]; /* Container for which entries of signal were
							 sensed (=true) */
    for (int i = 0; i < signalHeight; ++i) {
    	signal[i] = new signalType*[signalWidth];
	corruptedSignal[i] = new signalType*[signalWidth];
	sensedEntries[i] = new bool*[signalWidth];
    	for (int j = 0; j < signalWidth; ++j) {
    	    signal[i][j] = new signalType[signalFrames];
	    corruptedSignal[i][j] = new signalType[signalFrames];
	    sensedEntries[i][j] = new bool[signalFrames];
    	}
    }    
    signalType ***signalPatch = new signalType** [blockHeight]; /* container for blocks of distorted
								   signal */
    bool ***sensedPatch = new bool**[blockHeight];		/* container for blocks of 
								   sensedEntries array */
    for (int i = 0; i < blockHeight; ++i) { 
	signalPatch[i] = new signalType* [blockWidth];
	sensedPatch[i] = new bool* [blockWidth];
	for (int j = 0; j < blockWidth; ++j) {
	    signalPatch[i][j] = new signalType [blockFrames];
	    sensedPatch[i][j] = new bool [blockFrames];
	}
    }
    signalType *signalPatchVector = new signalType [blockSize]; /*container for vectorized distorted 
								  signal block */
    bool *sensedPatchVector = new bool [blockSize];		/* container for vectorizes
								   sensedEntries block */
    bool *initialSensedVector = new bool [blockSize]; 
    signalType *initialSignalVector = new signalType [blockSize];      
    basisType *recoveredVector = new basisType[blockSize]; // container for RVM predictions vector
    basisType ***recoveredPatch = new basisType**[blockHeight]; /* container for RVM predicted 
								   signal patch */
    for (int i = 0; i < blockHeight; ++i) {
	recoveredPatch[i] = new basisType*[blockWidth];
	for (int j = 0; j < blockWidth; ++j) {
	    recoveredPatch[i][j] = new basisType[blockFrames];
	}
    }    
    std::vector<basisType***> cascadeRecoveredSignals(cascadeSize);
    for (int s = 0; s < cascadeSize; ++s) {
	cascadeRecoveredSignals[s] = new basisType**[signalHeight]; /* container for storing the
								       complete recovered signal */
	for (int i = 0; i < signalHeight; ++i) {
	    cascadeRecoveredSignals[s][i] = new basisType*[signalWidth];
	    for (int j = 0; j < signalWidth; ++j) {
		cascadeRecoveredSignals[s][i][j] = new basisType[signalFrames];
	    }
	}
    }
    std::vector<basisType**> cascadeBasis(cascadeSize);
    for (int s = 0; s < cascadeSize; ++s) {
	cascadeBasis[s] = new basisType*[dictionarySize];
	for (int j = 0; j < dictionarySize; ++j) {
	    cascadeBasis[s][j] = new basisType[dictionarySize];
	}
    }

    
    /*** start logic ***/
    
    // Input Original signal
    input3D(signal, inputFile, signalHeight, signalWidth, signalFrames); /* get original input signal
									    and store in a cube */
     // Corrupt original signal and get indeces of sensed entries
    corruptSignal(signal, corruptedSignal, sensedEntries, signalHeight, \
		  signalWidth, signalFrames, percentage, corrupterSetting);
    
    // Get basis matrices for various scales
    for (int s = 0; s < cascadeSize; ++s) {
	haarBasis(cascadeBasis[s], blockHeight, blockWidth, \
		  blockFrames, startScale+s);
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
		getPatch3D(corruptedSignal, signalPatch, blockHeight,\
			   blockWidth,blockFrames, blockIndexRows,\
			   blockIndexCols, blockIndexFrames);
		getPatch3D(sensedEntries, sensedPatch, blockHeight,\
			   blockWidth, blockFrames, blockIndexRows,\
			   blockIndexCols, blockIndexFrames);
		vectorize3D(signalPatch, signalPatchVector,\
			    blockHeight, blockWidth, blockFrames);
		vectorize3D(sensedPatch, sensedPatchVector,\
			    blockHeight, blockWidth, blockFrames);
		vectorize3D(signalPatch, initialSignalVector,\
			    blockHeight, blockWidth, blockFrames);
		vectorize3D(sensedPatch, initialSensedVector,\
			    blockHeight, blockWidth, blockFrames);
		
		for (int s = 0; s < cascadeSize; ++s) {		    
		    int measurements = countSensed(sensedPatchVector, blockSize);
		    
		    /*** Declare and define RVM variables ***/
		    basisType *target = new basisType[measurements];   // need basisType for RVM
		    basisType **designMatrix = new basisType*[measurements];
		    for (int i = 0; i < measurements; ++i) {
			designMatrix[i] = new basisType[dictionarySize];
		    }
		    getTargets(signalPatchVector, target,\
			       sensedPatchVector, blockSize, measurements);
		    getDesignMatrix(cascadeBasis[s], designMatrix,\
				    sensedPatchVector, dictionarySize, measurements);
		    
		    basisType *estimatedCoeff = new basisType[dictionarySize];
		    basisType *errors = new basisType[dictionarySize];
		    for (int i = 0; i < dictionarySize; ++i) {
			estimatedCoeff[i] = 0;
			errors[i] = 0;
		    }
		    
		    /*** Start the RVM ***/
		    bool useCascade;
		    if (s < cascadeSize - 1) {
			useCascade = true;
		    } else {
			useCascade = false;
		    }
		    fast_updates(designMatrix, target, estimatedCoeff,\
				 measurements, dictionarySize, noiseStD,\
				 errors, cascadeBasis[s], useCascade,\
				 deltaML_threshold, printToCOut);
		    if (printToCOut) {
			std::cout << std::endl;
		    }
		    multiply2D1D(cascadeBasis[s], estimatedCoeff,\
				 recoveredVector, blockSize, dictionarySize);
		    fillSensedInfo(initialSignalVector, recoveredVector,\
				   initialSensedVector, blockSize);
		    
		    /*** Save recovered patch ***/
		    deVectorize(recoveredVector, recoveredPatch,\
				blockHeight, blockWidth, blockFrames);
		    putPatch3D(recoveredPatch, cascadeRecoveredSignals[s],\
			       blockHeight, blockWidth, blockFrames,	\
			       blockIndexRows, blockIndexCols, blockIndexFrames);
		    
		    /*** Prepare for next part of cascade ***/
		    if (useCascade) {
			for (int i = 0; i < blockSize; ++i) { 
			    if (errors[i] != 0) { // Get new mask 
				sensedPatchVector[i] = true;
			    } else {
				sensedPatchVector[i] = false;
			    }
			    // Recovered becomes new signal Patch to get targets for next stage
			    signalPatchVector[i] = recoveredVector[i]; 
			}

		    }

		    /*** Clean-up loop ***/		    
		    for (int i = 0; i < measurements; ++i) {
			delete[] designMatrix[i];
		    }
		    delete[] designMatrix;
		    delete[] target;
		
		    delete[] errors;
		    delete[] estimatedCoeff;		
		}				    
	    }
	}
    }

    /*** Output ***/
    output3Dsignals(corruptedSignal, "corrupted", actualSimulation);
    for (int s = 0; s < cascadeSize; ++s) {
	std::stringstream ss;
	ss << s+1;
	output3Dsignals(cascadeRecoveredSignals[s], "recovered_" + ss.str(),\
			actualSimulation);
    }


	
    /*** Clean-up ***/
    for (int s = 0; s < cascadeSize; ++s) {
	for (int i = 0; i < signalHeight; ++i) {
	    for (int j = 0; j < signalWidth; ++j) {
		delete[] cascadeRecoveredSignals[s][i][j];
	    }
	    delete[] cascadeRecoveredSignals[s][i];
	}
	delete[] cascadeRecoveredSignals[s];
    }
    for (int i = 0; i < blockHeight; ++i) {
	for (int j = 0; j < blockWidth; ++j) {
	    delete[] recoveredPatch[i][j];
	}
	delete[] recoveredPatch[i];
    }
    delete[] recoveredPatch;
    delete[] recoveredVector;
    delete[] sensedPatchVector;
    delete[] signalPatchVector;
    for (int i = 0; i < blockHeight; ++i) {
	for (int j = 0; j < blockWidth; ++j) {
	    delete[] sensedPatch[i][j];
	    delete[] signalPatch[i][j];
	}
	delete[] sensedPatch[i];
	delete[] signalPatch[i];
    }
    delete[] sensedPatch;
    delete[] signalPatch;
    for (int s = 0; s < cascadeSize; ++s) {
	for (int i = 0; i < blockSize; ++i) {
	    delete[] cascadeBasis[s][i];
	}
	delete[] cascadeBasis[s];
    }
    for (int i = 0; i < signalHeight; ++i) {
	for (int j = 0; j < signalWidth; ++j) {
	    delete[] sensedEntries[i][j];
	    delete[] corruptedSignal[i][j];
	    delete[] signal[i][j];
	}
	delete[] sensedEntries[i];
	delete[] corruptedSignal[i];
	delete[] signal[i];
    }
    delete[] sensedEntries;
    delete[] corruptedSignal;
    delete[] signal;

    return 0;   
}

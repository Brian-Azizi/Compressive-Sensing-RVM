#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// GGP's files
#include "utilities.h"
#include "kronecker.h"
#include "fast_updates.h"

// BA's files
#include "settingsFile.h"
#include "settingsTester.h"
#include "input3D.h"
#include "print3D.h"
#include "print2D.h"
#include "print1D.h"
#include "haarTransform3D.h"
#include "getPatch3D.h"
#include "vectorize3D.h"
#include "haarBasis.h"
#include "corruptSignal.h"
#include "countSensed.h"
#include "getTargets.h"
#include "getDesignMatrix.h"
#include "fillSensedInfo.h"
#include "deVectorize.h"
#include "putPatch3D.h"

int main()
{
    /* Test if Settings make sense and define constants.
       Note: No testing if input txt file is valid! */
    testSettings();

    /*** Program constants ***/
    unsigned int const numBlocksHeight = signalHeight / blockHeight;
    unsigned int const numBlocksWidth = signalWidth / blockWidth;
    unsigned int const numBlocksFrames = signalFrames / blockFrames;

    unsigned int const signalSize = signalHeight*signalWidth*signalFrames;
    unsigned int const blockSize = blockHeight*blockWidth*blockFrames;
    unsigned int const dictionarySize = blockSize;
    
    /*** Memory allocation ***/
    signalType ***signal = new signalType**[signalHeight]; // Original signal container
    signalType ***corruptedSignal = new signalType**[signalHeight]; // Corrupted signal container
    bool ***sensedEntries = new bool**[signalHeight]; // Container for which entries of signal were sensed (=true)
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
    basisType **PSI = new basisType*[dictionarySize]; // Basis matrix container
    for (int i = 0; i < dictionarySize; ++i) {
	PSI[i] = new basisType[dictionarySize];
    }
    signalType ***signalPatch = new signalType** [blockHeight]; // container for blocks of distorted signal
    bool ***sensedPatch = new bool**[blockHeight];		// container for blocks of sensedEntries array
    for (int i = 0; i < blockHeight; ++i) { 
	signalPatch[i] = new signalType* [blockWidth];
	sensedPatch[i] = new bool* [blockWidth];
	for (int j = 0; j < blockWidth; ++j) {
	    signalPatch[i][j] = new signalType [blockFrames];
	    sensedPatch[i][j] = new bool [blockFrames];
	}
    }
    signalType *signalPatchVector = new signalType [blockSize]; // container for vectorized distorted signal block
    bool *sensedPatchVector = new bool [blockSize];		// container for vectorizes sensedEntries block
    
    
    basisType *recoveredVector = new basisType[blockSize]; // container for RVM predictions vector
    basisType ***recoveredPatch = new basisType**[blockHeight]; // container for RVM predicted signal patch
    for (int i = 0; i < blockHeight; ++i) {
	recoveredPatch[i] = new basisType*[blockWidth];
	for (int j = 0; j < blockWidth; ++j) {
	    recoveredPatch[i][j] = new basisType[blockFrames];
	}
    }

    basisType ***recoveredSignal = new basisType**[signalHeight]; // container for storing the complete recovered signal
    for (int i = 0; i < signalHeight; ++i) {
	recoveredSignal[i] = new basisType*[signalWidth];
	for (int j = 0; j < signalWidth; ++j) {
	    recoveredSignal[i][j] = new basisType[signalFrames];
	}
    }

    /*** start logic ***/

    input3D(signal, inputFile, signalHeight, signalWidth, signalFrames); // get original input signal and store in a cube
    corruptSignal(signal, corruptedSignal, sensedEntries, signalHeight, signalWidth, signalFrames, percentage); // simulate a corrupted signal and get indeces of sensed entries
    haarBasis(PSI, blockHeight, blockWidth, blockFrames); // Get Basis matrix
    

    // Loop over blocks of original signal
    for (int blockIndexRows = 0; blockIndexRows < numBlocksHeight; ++blockIndexRows) {
	for (int blockIndexCols = 0; blockIndexCols < numBlocksWidth; ++blockIndexCols) {
	    for (int blockIndexFrames = 0; blockIndexFrames < numBlocksFrames; ++blockIndexFrames) {
		std::cout << "Patch (" << blockIndexRows+1 << "," << blockIndexCols+1 << "," << blockIndexFrames+1
			  << ")    of    (" << numBlocksHeight << "," << numBlocksWidth << "," << numBlocksFrames << ")" << std::endl;
		
		getPatch3D(corruptedSignal, signalPatch, blockHeight, blockWidth, blockFrames, blockIndexRows, blockIndexCols, blockIndexFrames);
		getPatch3D(sensedEntries, sensedPatch, blockHeight, blockWidth, blockFrames, blockIndexRows, blockIndexCols, blockIndexFrames);
		vectorize3D(signalPatch, signalPatchVector, blockHeight, blockWidth, blockFrames);
		vectorize3D(sensedPatch, sensedPatchVector, blockHeight, blockWidth, blockFrames);
		
		int measurements = countSensed(sensedPatchVector, blockSize);
		
		/*** Declare and define RVM variables ***/
		basisType *target = new basisType[measurements];   // need basisType for RVM
		basisType **designMatrix = new basisType*[measurements];
		for (int i = 0; i < measurements; ++i) {
		    designMatrix[i] = new basisType[dictionarySize];
		}
		getTargets(signalPatchVector, target, sensedPatchVector, blockSize, measurements);
		getDesignMatrix(PSI, designMatrix, sensedPatchVector, dictionarySize, measurements);
		
		basisType *estimatedCoeff = new basisType[dictionarySize];
		basisType *errors = new basisType[dictionarySize];
		for (int i = 0; i < dictionarySize; ++i) {
		    estimatedCoeff[i] = 0;
		    errors[i] = 0;
		}
		

		
		/*** Start the RVM ***/
		fast_updates(designMatrix, target, estimatedCoeff, measurements, dictionarySize, noiseStD, errors, PSI, use_cascade, deltaML_threshold);
		std::cout << std::endl;
		multiply2D1D(PSI, estimatedCoeff, recoveredVector, blockSize, dictionarySize);
		fillSensedInfo(signalPatchVector, recoveredVector, sensedPatchVector, blockSize);

		
		/*** Save recovered patch ***/
		deVectorize(recoveredVector, recoveredPatch, blockHeight, blockWidth, blockFrames);
		putPatch3D(recoveredPatch, recoveredSignal, blockHeight, blockWidth, blockFrames, blockIndexRows, blockIndexCols, blockIndexFrames);
		
		/*** Clean-up loop ***/
		delete[] errors;
		delete[] estimatedCoeff;

		for (int i = 0; i < measurements; ++i) {
		    delete[] designMatrix[i];
		}
		delete[] designMatrix;
		delete[] target;
	    }
	}
    }

    /*** Output ***/
    std::ofstream corrOut(corruptSigFile.c_str());
    std::ofstream recOut(recovSigFile.c_str());
    print3D(corrOut, corruptedSignal, signalHeight, signalWidth, signalFrames);
    print3D(recOut, recoveredSignal, signalHeight, signalWidth, signalFrames);
    corrOut.close();
    recOut.close();

			
    /*** Clean-up ***/
    for (int i = 0; i < signalHeight; ++i) {
	for (int j = 0; j < signalWidth; ++j) {
	    delete[] recoveredSignal[i][j];
	}
	delete[] recoveredSignal[i];
    }
    delete[] recoveredSignal;

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

    for (int i = 0; i < blockSize; ++i) {
	delete[] PSI[i];
    }
    delete[] PSI;

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

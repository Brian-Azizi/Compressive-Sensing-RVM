#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>


#include "settingsFile.h"
#include "settingsTester.h"
#include "input3D.h"
#include "print3D.h"
#include "print2D.h"
#include "haarTransform3D.h"
#include "getPatch3D.h"
#include "vectorize3D.h"
#include "utilities.h"
#include "kronecker.h"
#include "haarBasis.h"

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

    
    /*** Memory allocation ***/

    signalType ***signal = new signalType**[signalHeight]; // Original signal container
    for (int i = 0; i < signalHeight; ++i) {
    	signal[i] = new signalType*[signalWidth];
    	for (int j = 0; j < signalWidth; ++j) {
    	    signal[i][j] = new signalType[signalFrames];
    	}
    }

    basisType **basis = new basisType*[blockSize]; // Basis matrix container
    for (int i = 0; i < blockSize; ++i) {
	basis[i] = new basisType[blockSize];
    }

    signalType ***signalPatchTruth = new signalType** [blockHeight]; // container for blocks of original signal
    for (int i = 0; i < blockHeight; ++i) { 
	signalPatchTruth[i] = new signalType* [blockWidth];
	for (int j = 0; j < blockWidth; ++j) {
	    signalPatchTruth[i][j] = new signalType [blockFrames];
	}
    }
    signalType *patchVectorTruth = new signalType [blockSize]; // container for vectorized signal block
    

    /*** Allocation Done. Start logic ***/

    input3D(signal, inputFile, signalHeight, signalWidth, signalFrames); // get original input signal and store in a cube
    haarBasis(basis, blockHeight, blockWidth, blockFrames); // Get Basis matrix
   
    // Loop over blocks of original signal
    for (int blockIndexRows = 0; blockIndexRows < numBlocksHeight; ++blockIndexRows) {
	for (int blockIndexCols = 0; blockIndexCols < numBlocksWidth; ++blockIndexCols) {
	    for (int blockIndexFrames = 0; blockIndexFrames < numBlocksFrames; ++blockIndexFrames) {
		getPatch3D(signal, signalPatchTruth, blockHeight, blockWidth, blockFrames, blockIndexRows, blockIndexCols, blockIndexFrames);
		vectorize3D(signalPatchTruth, patchVectorTruth, blockHeight, blockWidth, blockFrames);
	    }
	}
    }


    // Clean-up
    delete[] patchVectorTruth;

    for (int i = 0; i < blockHeight; ++i) {
	for (int j = 0; j < blockWidth; ++j) {
	    delete[] signalPatchTruth[i][j];
	}
	delete[] signalPatchTruth[i];
    }
    delete[] signalPatchTruth;

    for (int i = 0; i < blockSize; ++i) {
	delete[] basis[i];
    }
    delete[] basis;

    for (int i = 0; i < signalHeight; ++i) {
	for (int j = 0; j < signalWidth; ++j) {
	    delete[] signal[i][j];
	}
	delete[] signal[i];
    }
    delete[] signal;

    return 0;   
}

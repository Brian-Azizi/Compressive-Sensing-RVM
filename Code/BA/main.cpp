#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>


#include "settingsFile.h"
#include "settingsTester.h"
#include "input3D.h"
#include "print3D.h"
#include "haarTransform3D.h"
#include "getPatch3D.h"
#include "vectorize3D.h"

int main()
{
    /* Test if Settings make sense and define constants.
       Note: No testing if input txt file is valid! */
    testSettings();
    unsigned int const numBlocksHeight = signalHeight / blockHeight;
    unsigned int const numBlocksWidth = signalWidth / blockWidth;
    unsigned int const numBlocksFrames = signalFrames / blockFrames;

    unsigned int const signalSize = signalHeight*signalWidth*signalFrames;
    unsigned int const blockSize = blockHeight*blockWidth*blockFrames;
    std::cout << "Settings done." << std::endl;

    
    // Get original input signal and store in a cube
    signalType ***signal = new signalType**[signalHeight];
    for (int i = 0; i < signalHeight; ++i) {
    	signal[i] = new signalType*[signalWidth];
    	for (int j = 0; j < signalWidth; ++j) {
    	    signal[i][j] = new signalType[signalFrames];
    	}
    }
    input3D(signal, inputFile, signalHeight, signalWidth, signalFrames);
    std::cout << "Input done." << std::endl;
    
    
    
    // 3D Haar transform;
    double ***transform = new double**[signalHeight];
    for (int i = 0; i < signalHeight; ++i) {
	transform[i] = new double*[signalWidth];
	for (int j = 0; j < signalWidth; ++j) {
	    transform[i][j] = new double[signalFrames];
	}
    }
    haarTransform3D(signal, transform, signalHeight, signalWidth, signalFrames);
    std::cout << "Haar transform done." << std::endl;
   
    
    // Declare signal patch
    signalType ***signalPatchTruth = new signalType** [blockHeight];
    double ***signalPatchTransform = new double** [blockHeight];
    for (int i = 0; i < blockHeight; ++i) {
	signalPatchTruth[i] = new signalType* [blockWidth];
	signalPatchTransform[i] = new double* [blockWidth];
	for (int j = 0; j < blockWidth; ++j) {
	    signalPatchTruth[i][j] = new signalType [blockFrames];
	    signalPatchTransform[i][j] = new double [blockFrames];
	}
    }
    signalType *patchVectorTruth = new signalType [blockSize];
    double *patchVectorTransform = new double [blockSize];
    
    for (int blockIndexRows = 0; blockIndexRows < numBlocksHeight; ++blockIndexRows) {
	for (int blockIndexCols = 0; blockIndexCols < numBlocksWidth; ++blockIndexCols) {
	    for (int blockIndexFrames = 0; blockIndexFrames < numBlocksFrames; ++blockIndexFrames) {
		// Get patch 
		getPatch3D(signal, signalPatchTruth, blockHeight, blockWidth, blockFrames, blockIndexRows, blockIndexCols, blockIndexFrames);
		// haarTransform3D(signalPatchTruth, signalPatchTransform, blockHeight, blockWidth, blockFrames);
		std::cout << "Patch (" << blockIndexRows << "," << blockIndexCols << "," << blockIndexFrames << ") :" << std::endl;
		print3D(std::cout, signalPatchTruth, blockHeight, blockWidth, blockFrames);
		// std::cout << "Haar transform:" << std::endl;
		// print3D(std::cout, signalPatchTransform, blockHeight, blockWidth, blockFrames);
		
		std::cout << "Vectorized: " << std::endl;
		vectorize3D(signalPatchTruth, patchVectorTruth, blockHeight, blockWidth, blockFrames);
		for (int i = 0; i < blockSize; ++i) {
		    std::cout << patchVectorTruth[i] << std::endl;
		}
		
	    }
	}
    }


    // Clean-up
    for (int i = 0; i < blockHeight; ++i) {
	for (int j = 0; j < blockWidth; ++j) {
	    delete[] signalPatchTruth[i][j];
	    delete[] signalPatchTransform[i][j];
	}
	delete[] signalPatchTruth[i];
	delete[] signalPatchTransform[i];
    }
    delete[] signalPatchTruth;
    delete[] signalPatchTransform;
    
    delete[] patchVectorTruth;
    delete[] patchVectorTransform;

    for (int i = 0; i < signalHeight; ++i) {
	for (int j = 0; j < signalWidth; ++j) {
	    delete[] signal[i][j];
	    delete[] transform[i][j];
	}
	delete[] signal[i];
	delete[] transform[i];
    }
    delete[] signal;
    delete[] transform;

    return 0;   
}

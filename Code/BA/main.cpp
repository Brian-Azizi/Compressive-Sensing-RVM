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

int main()
{
    /* Test if Settings make sense and define constants.
       Note: No testing if input txt file is valid! */
    testSettings();
    unsigned int const numBlocksWidth = signalWidth / blockWidth;
    unsigned int const numBlocksHeight = signalHeight / blockHeight;
    unsigned int const numBlocksFrames = signalFrames / blockFrames;
    std::cout << "Settings done." << std::endl;

    
    // Get original input signal and store in a cube
    signalType ***signal = new signalType**[signalHeight];
    for (int i = 0; i < signalHeight; ++i) {
    	signal[i] = new signalType*[signalWidth];
    	for (int j = 0; j < signalWidth; ++j) {
    	    signal[i][j] = new signalType[signalFrames];
    	}
    }
    input3D(signal, inputFile, signalWidth, signalHeight, signalFrames);
    std::cout << "Input done." << std::endl;
    
    
    
    // 3D Haar transform;
    double ***transform = new double**[signalHeight];
    for (int i = 0; i < signalHeight; ++i) {
	transform[i] = new double*[signalWidth];
	for (int j = 0; j < signalWidth; ++j) {
	    transform[i][j] = new double[signalFrames];
	}
    }
    haarTransform3D(signal, transform, signalWidth, signalHeight, signalFrames);
    std::cout << "Haar transform done." << std::endl;
   
    
    // Declare signal patch
    signalType ***signalPatchTruth = new signalType** [blockHeight];
    for (int i = 0; i < blockHeight; ++i) {
	signalPatchTruth[i] = new signalType* [blockWidth];
	for (int j = 0; j < blockWidth; ++j) {
	    signalPatchTruth[i][j] = new signalType [blockFrames];
	}
    }
    for (int blockIndexRows = 0; blockIndexRows < numBlocksHeight; ++blockIndexRows) {
	for (int blockIndexCols = 0; blockIndexCols < numBlocksWidth; ++blockIndexCols) {
	    for (int blockIndexFrames = 0; blockIndexFrames < numBlocksFrames; ++blockIndexFrames) {
		// Get patch 
		getPatch3D(signal, signalPatchTruth, blockWidth, blockHeight, blockFrames, blockIndexRows, blockIndexCols, blockIndexFrames);
		std::cout << "Patch (" << blockIndexCols << "," << blockIndexRows << "," << blockIndexFrames << ") :" << std::endl;
		print3D(std::cout, signalPatchTruth, blockWidth, blockHeight, blockFrames);
		std::cout << std::endl;
		
	    }
	}
    }


    // Clean-up
    for (int i = 0; i < blockHeight; ++i) {
	for (int j = 0; j < blockWidth; ++j) {
	    delete[] signalPatchTruth[i][j];
	}
	delete[] signalPatchTruth[i];
    }
    delete[] signalPatchTruth;

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

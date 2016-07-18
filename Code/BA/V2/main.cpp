#define _Use_MATH_DEFINES

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "SignalSettings.hpp"
#include "SignalBasis.hpp"
#include "Corrupter.hpp"
#include "Signal.hpp"
//#include "oldRVM.hpp"
#include "RVM.hpp"

typedef unsigned int signalType; // Specify C++ type of input signal

int main(int argc, char* argv[])
{
    if (argc < 2) error("No input file specified");
    if (argc > 2) error("To many input arguments");
    SignalSettings cfg(argv[1]);
    
    if(cfg.printProgress) std::cout << cfg << std::endl;
    std::srand(cfg.rngSeed);    

    /*** Memory allocation ***/
    Signal<signalType> signal(cfg.signalDim);
    Signal<signalType> block(cfg.blockDim);        
    
    Signal<signalType> corruptedSignal(signal.dim());
    Signal<bool> sensedEntries(signal.dim());
    Signal<signalType> signalPatch(block.dim());
    Signal<bool> sensedPatch(block.dim());

    Signal<signalType> signalPatchVector(block.size());
    Signal<bool> sensedPatchVector(block.size());
    Signal<bool> initialSensedVector(block.size());
    Signal<signalType> initialSignalVector(block.size());
    Signal<double> recoveredVector(block.size());
    Signal<double> recoveredPatch(block.dim());

    std::vector<Signal<double> > cascadeRecoveredSignals // initialized to zero
    	(cfg.endScale, Signal<double>(signal.dim()));
    std::vector<Signal<double> > cascadeBasis
    	(cfg.endScale, Signal<double>(block.size(), block.size()));

    /*** Program constants ***/
        
    /*** start logic ***/
    signal.read(cfg.inputFile);
    
    if (cfg.simulateCorruption)
	corruptedSignal = corruptSignal(signal, sensedEntries, cfg.corr);
    else {
	sensedEntries.read(cfg.maskFile);
	corruptedSignal = corruptSignal(signal, sensedEntries);
    }
       
    // Get basis matrices for various scales
    for (int scale = 0; scale < cfg.endScale; ++scale)
    	cascadeBasis[scale] = getBasis(block.dim(), cfg.basisMode, scale+1);


    // Loop over blocks of original signal
    unsigned int const numBlocksHeight = signal.height() / block.height();
    unsigned int const numBlocksWidth = signal.width() / block.width();
    unsigned int const numBlocksFrames = signal.frames() / block.frames();

    for (int blockIndexRows = 0; blockIndexRows < numBlocksHeight; ++blockIndexRows) {
    	for (int blockIndexCols = 0; blockIndexCols < numBlocksWidth; ++blockIndexCols) {
    	    for (int blockIndexFrames = 0; blockIndexFrames < numBlocksFrames; ++blockIndexFrames) {
    		if (cfg.printProgress) {
    		    std::cout << "\tPatch (" << blockIndexRows+1 << "," 
    			      << blockIndexCols+1 << "," << blockIndexFrames+1
    			      << ")    of    (" << numBlocksHeight << "," 
    			      << numBlocksWidth << "," << numBlocksFrames 
    			      << ")" << std::endl;
    		}

    		signalPatch = corruptedSignal
    		    .getPatch(blockIndexRows*block.height(), blockIndexCols*block.width(),
    			      blockIndexFrames*block.frames(), block.height(), block.width(), block.frames());
    		sensedPatch = sensedEntries
    		    .getPatch(blockIndexRows*block.height(), blockIndexCols*block.width(),
			      blockIndexFrames*block.frames(), block.height(), block.width(), block.frames());

    		signalPatchVector = vectorize(signalPatch);
    		sensedPatchVector = vectorize(sensedPatch);
    		initialSignalVector = vectorize(signalPatch);
    		initialSensedVector = vectorize(sensedPatch);

    		for (int scale = 0; scale < cfg.endScale; ++scale) {		    
    		    int measurements = countSensed(sensedPatchVector);
		    if (measurements == 0) continue;

    		    /*** Declare and define RVM variables ***/
    		    Signal<double> targets(measurements);
    		    Signal<double> designMatrix(measurements, block.size());
    		    Signal<double> estimatedCoeff(block.size()); // init to 0
		    Signal<double> errors(block.size());

    		    targets = getTargets(signalPatchVector, sensedPatchVector);
    		    designMatrix = getDesignMatrix(cascadeBasis[scale], sensedPatchVector);

    		    /*** Start the RVM ***/
    		    bool useCascade;
    		    if (scale+1 < cfg.endScale) useCascade = true;
    		    else useCascade = false;
		    
    		    RVM rvm(cfg.stdDev, cfg.deltaML_threshold, cfg.printProgress);
		    rvm.train(designMatrix, targets);		    
		    estimatedCoeff = rvm.mu();

		    recoveredVector = rvm.predict(cascadeBasis[scale]);		    
		    //recoveredVector.fill(signalPatchVector, sensedPatchVector);
    		    recoveredVector.fill(initialSignalVector, initialSensedVector);

    		    /*** Save recovered patch ***/
    		    recoveredPatch = reshape(recoveredVector, block.height(), block.width(), block.frames());
		    
    		    cascadeRecoveredSignals[scale]
    			.putPatch(recoveredPatch, blockIndexRows*block.height(),
				  blockIndexCols*block.width(), blockIndexFrames*block.frames());
		    
    		    /*** Prepare for next part of cascade ***/
    		    if (true||useCascade) {			
			errors = rvm.predictionErrors(cascadeBasis[scale]);
    			for (int i = 0; i < block.size(); ++i) { 
    			    if (errors(i) != 0) sensedPatchVector(i) = true; // get new mask
    			    else sensedPatchVector(i) = false;
			    
    			    // Recovered becomes new signal Patch to get targets for next stage
    			    signalPatchVector(i) = recoveredVector(i); 
    			}
    		    }
		    std::stringstream ll;
		    ll << scale;
		    outputSignal(errors,"errors" + ll.str(), cfg);
		    std::stringstream cc;
		    cc << scale;
		    outputSignal(estimatedCoeff,"coeff" + cc.str(), cfg);

    		}				    
    	    }
    	}
    }

    /*** Output ***/
    outputSignal(corruptedSignal, "corrupted", cfg);
    outputSignal(sensedEntries, "mask", cfg);
    for (int scale = 0; scale < cfg.endScale; ++scale) {
    	std::stringstream label;
    	label << "recovered_" << scale+1 << "_of_" << cfg.endScale;		
	outputSignal(cascadeRecoveredSignals[scale], label.str(), cfg); 
    }
    
    return 0;   
}

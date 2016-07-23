#ifndef GUARD_INTERPOLATOR_HPP
#define GUARD_INTERPOLATOR_HPP

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "Timer.hpp"

uint64 statsTime=0, whileTime=0, trainTime=0;
uint64 focusTime1=0, focusTime2=0, focusTime3=0, focusTime4=0;
int statsIdx=0, whileIdx=0, trainIdx=0;
int focusIdx1=0, focusIdx2=0, focusIdx3=0, focusIdx4=0;

#include "SignalSettings.hpp"
#include "SignalBasis.hpp"
#include "Corrupter.hpp"
#include "Signal.hpp"
#include "RVM.hpp"


class Interpolator {
private:
    typedef unsigned int signalType; // Specify C++ type of input
    Signal<signalType> signal;
    Signal<signalType> block;

    Signal<signalType> corruptedSignal;
    Signal<bool> sensedEntries;
    Signal<signalType> signalPatch;
    Signal<bool> sensedPatch;
    
    Signal<signalType> signalPatchVector;
    Signal<bool> sensedPatchVector;
    Signal<bool> initialSensedVector;
    Signal<signalType> initialSignalVector;
    Signal<double> recoveredVector;
    Signal<double> recoveredPatch;

    std::vector<Signal<double> > cascadeRecoveredSignals;
    std::vector<Signal<double> > cascadeBasis;

    SignalSettings cfg;

    bool m_printProgress;

    void reconstruct();
    uint64 bigLoopTime;
    uint64 loopSetupTime;
    int itt;
    
public:
    Interpolator(std::string settingsFile); // construct from settings file name
    bool printProgress() const { return m_printProgress; }
    void run();				    // run interpolator
};

Interpolator::Interpolator(std::string fileName)
    : cfg(fileName)
{
    signal = Signal<signalType>(cfg.signalDim, false); // 'false' here means 'do not initialize'
    block = Signal<signalType>(cfg.blockDim, false);        
    sensedEntries = Signal<bool>(signal.dim()); // initialized to false
    cascadeRecoveredSignals = 
	std::vector<Signal<double> >(cfg.endScale, Signal<double>(signal.dim(), false));
    m_printProgress = cfg.printProgress;
}

void Interpolator::run() 
{
    // set rng seed, print settings
    std::srand(cfg.rngSeed);
    if (cfg.printProgress) std::cout << cfg << std::endl;

    // input original signal
    signal.read(cfg.inputFile);

    // apply corruption
    if (cfg.simulateCorruption) corruptedSignal = corruptSignal(signal, sensedEntries, cfg.corr);
    else {
	sensedEntries.read(cfg.maskFile);
	corruptedSignal = corruptSignal(signal, sensedEntries);
    }
    
    // Get basis matrices for various scales
    for (int scale = 0; scale < cfg.endScale; ++scale)
	cascadeBasis.push_back(getBasis(block.dim(), cfg.basisMode, scale+1));

    // Reconstruct the signal
    bigLoopTime = 0;
    loopSetupTime = 0;
    itt = 0;
    if (cfg.printProgress) std::cout << "\t*** Start Reconstruction ***\n";
    reconstruct();
    
    // Print settings again
    if (cfg.printProgress) std::cout << cfg;

    // Write the output to disk and store the output file names in '.rvmOutputFilenames.txt'
    if (cfg.printProgress) std::cout<< "\n\t *** Output Files ***";        
    std::string outNames = "./rvmOutputFilenames.txt";
    std::ofstream outF(outNames.c_str());
    if (!outF) { 
	if (cfg.printProgress) std::cerr << "\nCould not open save names of output files in " << outNames << std::endl;;
    } else if(cfg.printProgress) std::cout << "\nRelevant file names for interfacing with Matlab have been saved in:\t" << outNames << std::endl;
    std::string name;
    
    // original
    name = cfg.inputFile;
    if (outF) outF << "original\t\t" << name << std::endl;
    if (cfg.printProgress) std::cout << "\nOriginal signal file:\t\t\t" << name;

    // corrupted
    name = outputSignal(corruptedSignal, "_CORRUPTED", cfg); // writes to disk and returns filename
    if (outF) outF << "corrupted\t\t" << name << std::endl;
    if (cfg.printProgress) std::cout << "\nCorrupted signal saved at:\t\t" << name;
    
    // mask
    name = outputSignal(sensedEntries, "_MASK", cfg);
    if (outF) outF << "mask\t\t\t" << name << std::endl;
    if (cfg.printProgress) std::cout << "\nSignal mask saved at:\t\t\t" << name;
    
    // recovered
    for (int scale = 0; scale < cfg.endScale; ++scale) {
	std::stringstream label;
	label << "_RECOVERED_" << scale+1 << "_OF_" << cfg.endScale;
	name = outputSignal(cascadeRecoveredSignals[scale], label.str(), cfg);
	if (outF) outF << "recovered_" << scale+1 << "\t\t" << name << std::endl;
	if(cfg.printProgress) std::cout << "\nRecovered Signal (scale " << scale+1 << ") saved at:\t" << name;
    }

    // Output times    
    if (cfg.printProgress) std::cout << "\n\n\t *** Measured Time: ***\n";
    if (cfg.printProgress) std::cout << "Average loop time: " << bigLoopTime/(double)itt << " ms." << std::endl;
    if (cfg.printProgress) std::cout << "Average  RVM train time: " << loopSetupTime/(double)itt << " ms." << std::endl;
    if (cfg.printProgress) std::cout << "Average train time: " << trainTime/(double)trainIdx << " ms." << std::endl;
    if (cfg.printProgress) std::cout << "Average stats time: " << statsTime/(double)statsIdx << " ms." << std::endl;
    if (cfg.printProgress) std::cout << "Average while loop time: " << whileTime/(double)whileIdx << " ms." << std::endl;
    if (cfg.printProgress) std::cout << "Average focus time 1: " << focusTime1/(double)focusIdx1 << " ms." << std::endl;
    if (cfg.printProgress) std::cout << "Average focus time 2: " << focusTime2/(double)focusIdx2 << " ms." << std::endl;
    if (cfg.printProgress) std::cout << "Average focus time 3: " << focusTime3/(double)focusIdx3 << " ms." << std::endl;
    if (cfg.printProgress) std::cout << "Average focus time 4: " << focusTime4/(double)focusIdx4 << " ms." << std::endl;
}

void Interpolator::reconstruct()
{
    const int numBlocksHeight = signal.height() / block.height();
    const int numBlocksWidth = signal.width() / block.width();
    const int numBlocksFrames = signal.frames() / block.frames();

    for (int rowIdx = 0; rowIdx < numBlocksHeight; ++rowIdx)
	for (int colIdx = 0; colIdx < numBlocksWidth; ++colIdx)
	    for (int frameIdx = 0; frameIdx < numBlocksFrames; ++frameIdx) {
		if (cfg.printProgress)
		    std::cout << "Block (" << rowIdx+1 << "," << colIdx+1
			      << "," << frameIdx+1 << ")   \tof\t("
			      << numBlocksHeight << "," << numBlocksWidth
			      << "," << numBlocksFrames << ")\t";
		uint64 startLoopTime = GetTimeMs64();
		
		signalPatch = corruptedSignal
		    .getPatch(rowIdx*block.height(), colIdx*block.width(), 
			      frameIdx*block.frames(), block.height(), block.width(),
			      block.frames());
		sensedPatch = sensedEntries
		    .getPatch(rowIdx*block.height(), colIdx*block.width(), 
			      frameIdx*block.frames(), block.height(), block.width(),
			      block.frames());
		signalPatchVector = vectorize(signalPatch);
		sensedPatchVector = vectorize(sensedPatch);
		initialSignalVector = vectorize(signalPatch);
		initialSensedVector = vectorize(sensedPatch);

		for (int scale = 0; scale < cfg.endScale; ++scale) {
		    int measurements = countSensed(sensedPatchVector);
		    if (measurements == 0) {
			if (cfg.printProgress) {
			    if (scale == 0) std::cout << "(empty)" << std::endl;
			    else std::cout << "\t\t\t\t\t\t(empty)" << std::endl;
			}
			continue;
		    }
		    
		    /*** Declare and define RVM variables ***/
    		    Signal<double> targets = getTargets(signalPatchVector, sensedPatchVector);
    		    Signal<double> designMatrix = getDesignMatrix(cascadeBasis[scale], sensedPatchVector);
		    Signal<double> errors(block.size()); // for fastUpdates

    		    /*** Start the RVM ***/
    		    bool useCascade;
    		    if (scale+1 < cfg.endScale) useCascade = true;
    		    else useCascade = false;
	    
		    uint64 startSetupTime = GetTimeMs64();    
    		    RVM rvm(cfg.stdDev, cfg.deltaML_threshold, cfg.printProgress);
		    //rvm.train(designMatrix, targets);		    
		    rvm.fastUpdates(designMatrix, targets, useCascade, cascadeBasis[scale], errors); // for fastUpdates
		    loopSetupTime += (GetTimeMs64() - startSetupTime);
		    //recoveredVector = rvm.predict(cascadeBasis[scale]);		    		    
		    recoveredVector = matMult(cascadeBasis[scale], rvm.fastMu()); // for fastUpdates
		    recoveredVector.fill(initialSignalVector, initialSensedVector);

    		    /*** Save recovered patch ***/
    		    recoveredPatch = reshape(recoveredVector, block.height(), block.width(), block.frames());
    		    cascadeRecoveredSignals[scale]
    			.putPatch(recoveredPatch, rowIdx*block.height(),
				  colIdx*block.width(), frameIdx*block.frames());

    		    /*** Prepare for next part of cascade ***/
    		    if (useCascade) {			
			//Signal<double> errors = rvm.predictionErrors(cascadeBasis[scale]); // remove for fastupdates
    			for (int i = 0; i < block.size(); ++i) { 
    			    if (errors(i) != 0) sensedPatchVector(i) = true; // get new mask
    			    else sensedPatchVector(i) = false;
			    
    			    // Recovered becomes new signal Patch to get targets for next stage
    			    signalPatchVector(i) = recoveredVector(i); 
    			}
			if (cfg.printProgress) std::cout << "\t\t\t\t\t\t";
    		    }
		    bigLoopTime += (GetTimeMs64() - startLoopTime);
		    ++itt;		    
		}
	    }
}


#endif

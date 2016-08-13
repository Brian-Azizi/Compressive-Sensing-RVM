#ifndef GUARD_INTERPOLATOR_HPP
#define GUARD_INTERPOLATOR_HPP

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "Timer.hpp"

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
public:
    Interpolator(const SignalSettings& settingsFile);//std::string settingsFile); // construct from settings file name
    bool printProgress() const { return m_printProgress; }
    void run();				    // run interpolator

    int getIntRange(double input, int min = 0, int max = 255);
    double computeMSE(const Signal<double>& original, const Signal<double>& reconstructed);
    double MSEtoPSNR(double mse);
};

Interpolator::Interpolator(const SignalSettings& settingsFile)//std::string fileName)
    : cfg(settingsFile) //: cfg(fileName)
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
    /*** set rng seed, print settings ***/
    std::srand(cfg.rngSeed);
    if (cfg.printProgress) std::cout << cfg << std::endl;

    /*** input original signal ***/
    signal.read(cfg.inputFile);

    /*** apply corruption ***/

    if (cfg.simulateCorruption) corruptedSignal = corruptSignal(signal, sensedEntries, cfg.corr);
    else {
	sensedEntries.read(cfg.maskFile);
	corruptedSignal = corruptSignal(signal, sensedEntries);
    }
    
    /*** Get basis matrices for various scales ***/
    for (int scale = 0; scale < cfg.endScale; ++scale)
	cascadeBasis.push_back(getBasis(block.dim(), cfg.basisMode, scale+1));


    /*** Reconstruct the signal ***/
    if (cfg.printProgress) std::cout << "\t*** Start Reconstruction ***\n";
    reconstruct();
    
    /*** Print settings again ***/
    if (cfg.printProgress) std::cout << cfg;

    /*** Write the output to disk and store the output file names in the fls file ***/
    if (cfg.printProgress) std::cout<< "\n\t *** Output Files ***";        
    //std::string outNames = "./rvmOutputFilenames.txt";
    std::ofstream flsFile(cfg.flsFileName.c_str());
    if (!flsFile) { 
	if (cfg.printProgress) std::cerr << "\nCould not save names of output files in " << cfg.flsFileName << std::endl;;
    } else if(cfg.printProgress) std::cout << "\nRelevant file names for interfacing with Matlab have been saved in:\t" << cfg.flsFileName << std::endl;
    std::string name;
    
    // original
    name = cfg.inputFile;
    if (flsFile) flsFile << "original\t\t" << name << std::endl;
    if (cfg.printProgress) std::cout << "\nOriginal signal file:\t\t\t" << name;

    // corrupted
    name = outputSignal(corruptedSignal, "_CORRUPTED", cfg); // writes to disk and returns filename
    if (flsFile) flsFile << "corrupted\t\t" << name << std::endl;
    if (cfg.printProgress) std::cout << "\nCorrupted signal saved at:\t\t" << name;
    
    // mask
    name = outputSignal(sensedEntries, "_MASK", cfg);
    if (flsFile) flsFile << "mask\t\t\t" << name << std::endl;
    if (cfg.printProgress) std::cout << "\nSignal mask saved at:\t\t\t" << name;
    
    // recovered
    for (int scale = 0; scale < cfg.endScale; ++scale) {
	std::stringstream label;
	label << "_RECOVERED_" << scale+1 << "_OF_" << cfg.endScale;
	name = outputSignal(cascadeRecoveredSignals[scale], label.str(), cfg);
	if (flsFile) flsFile << "recovered_" << scale+1 << "\t\t" << name << std::endl;
	if(cfg.printProgress) std::cout << "\nRecovered Signal (scale " << scale+1 << ") saved at:\t" << name;
    }
    
    /*** Output MSE and PSNR for each stage ***/
    if (cfg.computePSNR) {
	if (cfg.printProgress) std::cout << "\n\n\t*** Accuracy of Reconstruction ***\n";
	for (int scale = 0; scale < cfg.endScale; ++scale) {
	    double mse = computeMSE(signal, cascadeRecoveredSignals[scale]);
	    double psnr = MSEtoPSNR(mse);
	    std::stringstream record;
	    record << "Scale " << scale+1 << ":  MSE  = " << mse << "\n";
	    record << "\t  PSNR = " << psnr << "\n";
	    std::cout << record.str();
	}
    }    
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

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // NEW FEATURE: RANDOM SENSING MATRIX 
		// create sensing matrix Theta (Gaussian or Bernoulli) with dim dictSize x dictSize
		// -> Theta; dim = blockSize x blockSize
		// get Theta * origSignalPatchVector
		// -> origSignalPatch = signal.getPatch(...); dim = blockDim		     
		// -> origSignalPatchVector = vectorize(origSignalPatch); dim = blockSize
		// -> mangledSignalVector = Theta * origSignalPatchVector; dim = blockSize		
		// -> mangledSignal = devectorize(mangledSignalVector); dim = blockDim		
		// apply mask		
		// -> corruptedSignalPatch = corrupt(mangledSignal, sensedPatch); dim = blockDim
		// -> corruptedSignalPatchVector = vectorize(/\); dim = blockSize
		// targets and basis
		// -> targets = getTargets(corruptedSignalPatchVector, sensedPatchVector); dim = measurements
		// -> mangledBasis = Theta * cascadeBasis[scale]; dim = blockSize x blockSize
		// -> designMatrix = getDesignMatrix(mangledBasis, sensedPatchVector); dim = measurements x blockSize
		// std::cout << "\nblockDim = " << block.dim() << "\nblockSize = " << block.size() 
		// 	  << "\norigSigPatch = \n"<< origSignalPatch << "\ntheta =\n " << theta
		// 	  << "\norig vector =\n " << origSignalPatchVector
		// 	  << "\nmangledSignalVecto =\n " << mangledSignalVector
		// 	  << "\nmangledSignal = \n" << mangledSignal
		// 	  << "\nsensedEntries = \n" << sensedPatch
		// 	  << "\nsensedPatchVector = \n" << sensedPatchVector
		// 	  << "\ncorruptedSignalPatch =\n " << corruptedSignalPatch
		// 	  << "\ncorruptedSignalPatchVector =\n " << corruptedSignalPatchVector << "\n"
		// 	  << "\ntargets = \n " << targets
		// 	  << "\nbasis = \n " << cascadeBasis[0]
		// 	  << "\nmangledBasis = \n" << mangledBasis
		// 	  << "\ndesignMatrix = \n" << designMatrix
		//     ;
		// assert(false);

		Signal<double> theta = bernoulliSamples(Dim(block.size(),block.size()), 0.5, std::sqrt(block.size()), 1/std::sqrt(block.size()));	 
		//Signal<double> theta = gaussianSamples(Dim(block.size(),block.size()), 0, 1/std::sqrt(block.size()));
		//Signal<double> theta = eye(block.size());

		Signal<double> origSignalPatch = signal.getPatch(rowIdx*block.height(), colIdx*block.width(),
								 frameIdx*block.frames(), block.height(),
								 block.width(), block.frames());		
		Signal<double> origSignalPatchVector = vectorize(origSignalPatch);		
		Signal<double> mangledSignalVector = matMult(theta,origSignalPatchVector);		
		Signal<double> mangledSignal = reshape(mangledSignalVector, block.dim());
		Signal<double> corruptedSignalPatch = corruptSignal(mangledSignal, sensedPatch);
		Signal<double> corruptedSignalPatchVector = vectorize(corruptedSignalPatch);
		Signal<double> targetPatch(block.dim());
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		
		

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
    		    // Signal<double> targets = getTargets(signalPatchVector, sensedPatchVector);
    		    // Signal<double> designMatrix = getDesignMatrix(cascadeBasis[scale], sensedPatchVector);	

		    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		    Signal<double> targets = getTargets(corruptedSignalPatchVector, sensedPatchVector);
		    Signal<double> mangledBasis = matMult(theta,cascadeBasis[scale]);
		    Signal<double> designMatrix = getDesignMatrix(mangledBasis, sensedPatchVector);
		    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    		    /*** Start the RVM ***/
    		    bool useCascade;
    		    if (scale+1 < cfg.endScale) useCascade = true;
    		    else useCascade = false;
		    
		    RVM rvm(cfg.stdDev, cfg.deltaML_threshold, cfg.printProgress);

		    uint64 trainTimeStart = GetTimeMs64();
		    rvm.train(designMatrix, targets);	
		    trainTime += (GetTimeMs64() - trainTimeStart); trainTimeCount++;

		    uint64 predictTimeStart = GetTimeMs64();
		    recoveredVector = rvm.predict(cascadeBasis[scale]);		    		    		    
		    predictTime += (GetTimeMs64() - predictTimeStart); predictTimeCount++;

		    //recoveredvector.fill(initialSignalVector, initialSensedVector); // DEL %%%%%
		    
    		    /*** Save recovered patch ***/
    		    recoveredPatch = reshape(recoveredVector, block.height(), block.width(), block.frames());
    		    cascadeRecoveredSignals[scale]
    			.putPatch(recoveredPatch, rowIdx*block.height(),
				  colIdx*block.width(), frameIdx*block.frames());
		    
		    //%%%%%%%%%%%%%%%% NEEDS TO BE MODIFIED %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    		    /*** Prepare for next part of cascade ***/
    		    if (useCascade) {			
			uint64 errorTimeStart = GetTimeMs64();
			Signal<double> errors = rvm.predictionErrors(cascadeBasis[scale]);
    			errorTime += (GetTimeMs64() - errorTimeStart); ++errorTimeCount;
			for (int i = 0; i < block.size(); ++i) { 
    			    if (errors(i) != 0) sensedPatchVector(i) = true; // get new mask
    			    else sensedPatchVector(i) = false;
			    
    			    // Recovered becomes new signal Patch to get targets for next stage
    			    signalPatchVector(i) = recoveredVector(i); 
    			}
			if (cfg.printProgress) std::cout << "\t\t\t\t\t\t";
    		    }
		    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
		}
	    }
}

double Interpolator::computeMSE(const Signal<double>& orig, const Signal<double>& rec)
{
    // check input
    if (orig.dim() != rec.dim()) error("MSE: inputs must have the same dimensions");
    
    int maxEntry = 255;
    int minEntry = 0;

    double error = 0;
    int xo, xr;			// note, this will convert to integers

    for (int i = 0; i < orig.size(); ++i) {
	xo = getIntRange(orig.data()[i], minEntry, maxEntry);
	xr = getIntRange(rec.data()[i], minEntry, maxEntry);
	error += ((xo-xr) * (xo-xr));
    }
    double ret = (double) error / orig.size();

    return ret;
}

int Interpolator::getIntRange(double input, int min, int max)
{
    int ret;
    if (input <= min) ret = min;
    else if (input >= max) ret = max;
    else ret = (int) (input + 0.5); // rounds to the nearest integer;    

    return ret;
}

double Interpolator::MSEtoPSNR(double mse)
{
    double maxValue = 255;
    if (mse < 0) error("MSE must be non-negative");
    double psnr = -10 * std::log10 ( mse / (maxValue*maxValue));
    
    return psnr;
}

#endif

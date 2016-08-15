#ifndef GUARD_INTERPOLATOR_HPP
#define GUARD_INTERPOLATOR_HPP

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#ifdef USE_MPI
#include <mpi.h>
#endif

#include "Timer.hpp"

#include "SignalSettings.hpp"
#include "SignalBasis.hpp"
#include "Mask.hpp"
#include "Signal.hpp"
#include "RVM.hpp"


class Interpolator {
private:
    typedef double signalType; // Specify C++ type of input
    Signal<signalType> signal;
    Signal<signalType> block;

    //Signal<signalType> sensingMatrices;
    Signal<signalType> maskedSignal;
    Signal<signalType> allTargets; // save the measurements
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

    //    bool m_printProgress;

    int rank, nproc;
    Signal<int> grid;
    std::map<int, std::vector<Signal<int> > > gridMap;
    int m_DUMMY;   
    void reconstruct();
    int getIntRange(double input, int min = 0, int max = 255);
    double computeMSE(const Signal<double>& original, const Signal<double>& reconstructed);
    double MSEtoPSNR(double mse);


public:
    Interpolator(const SignalSettings& settingsFile);
    //bool printProgress() const { return m_printProgress; }
    void run();				    // run interpolator
    };

Interpolator::Interpolator(const SignalSettings& settingsFile)
    : cfg(settingsFile)
{
    signal = Signal<signalType>(cfg.signalDim, false); // 'false' here means 'do not initialize'
    block = Signal<signalType>(cfg.blockDim, false);        
    sensedEntries = Signal<bool>(signal.dim()); // initialized to false
    cascadeRecoveredSignals = 
	std::vector<Signal<double> >(cfg.endScale, Signal<double>(signal.dim(), false));
    //m_printProgress = cfg.printProgress;
    m_DUMMY = 9999;
}

void Interpolator::run() 
{
    /*** set rng seed, print settings ***/
    std::srand(cfg.rngSeed);
    if (cfg.rank==0) std::cout << cfg << std::endl;

    /*** input original signal ***/
    signal.read(cfg.inputFile);

    /*** apply corruption ***/
    if (cfg.simulateCorruption) maskedSignal = applyMask(signal, sensedEntries, cfg.mask);
    else {
	sensedEntries.read(cfg.maskFile);
	maskedSignal = applyMask(signal, sensedEntries);
    }
    
    /*** Init allTargets ***/
    allTargets = Signal<double>(signal.dim(),false);    

    /*** Get basis matrices for various scales ***/
    for (int scale = 0; scale < cfg.endScale; ++scale)
	cascadeBasis.push_back(getBasis(block.dim(), cfg.basisMode, scale+1));

    /*** Reconstruct the signal ***/
    if (cfg.printProgress && cfg.rank==0) std::cout << "\t*** Start Reconstruction ***\n";
    
    // Define grid and gird map
    rank = cfg.rank;
    nproc = cfg.nproc;
    grid = Signal<int> ( signal.height()/block.height(), signal.width()/block.width(), signal.frames()/block.frames());
    for (int i = 0; i < grid.size(); ++i) grid.data()[i] = i % nproc;
    for (int i = 0; i < grid.size(); ++i) gridMap[grid.data()[i]].push_back(grid.index(i));
    int rootCalc = 0;
    for (int i = 0; i < grid.size(); ++i) if (grid.data()[i] == 0) ++rootCalc;
    
    // run the big loop
    reconstruct();

    // MPI: Collect the results on rank 0
#ifdef USE_MPI
    MPI_Status st;
    Signal<double> buffer(block.dim(),false);
    
    // allTargets
    if (rank == 0) {
	for (int i = 0; i < grid.size() - rootCalc; ++i) {	    
	    MPI_Recv(buffer.data(), buffer.size(), MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
	    int tag = st.MPI_TAG;
	    int source = st.MPI_SOURCE;
	    Signal<double> grIdx = grid.index(tag);
	    allTargets.putPatch(buffer, grIdx(0)*block.height(), grIdx(1)*block.width(), grIdx(2)*block.frames());	    
	    std::cout << "Gathering Measurements: " << tag << "\tof " << grid.size() << "\t...\n";
	}	    
    } 	
    else {	    
	for (int i = 0; i < grid.size(); ++i) {
	    Signal<double> grIdx = grid.index(i);	    
	    if (grid.data()[i] == rank) {
		buffer = allTargets.getPatch(grIdx(0)*block.height(), grIdx(1)*block.width(), grIdx(2)*block.frames(),
					     block.height(), block.width(), block.frames());
		MPI_Send(buffer.data(), buffer.size(), MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
	    }
	}
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // cascadeRecoveredSignals
    for (int scale = 0; scale < cfg.endScale; ++scale) {
	for (int i = 0; i < grid.size(); ++i) {
	    Signal<double> grIdx = grid.index(i);
	    if (rank == 0) {
		std::cout << "Gathering Result_" << scale+1 <<": " << i << "\tof " << grid.size() << "\t...\n";
		if (grid.data()[i] != 0) {
		    MPI_Recv(buffer.data(), buffer.size(), MPI_DOUBLE, grid.data()[i], i, MPI_COMM_WORLD, &st);
		    cascadeRecoveredSignals[scale]
			.putPatch(buffer, grIdx(0)*block.height(), grIdx(1)*block.width(), grIdx(2)*block.frames());
		}				   	    
	    } 	
	    else {	    
		if (grid.data()[i] == rank) {		   
		    buffer = cascadeRecoveredSignals[scale]
			.getPatch(grIdx(0)*block.height(), grIdx(1)*block.width(), grIdx(2)*block.frames(),
				  block.height(), block.width(), block.frames());
		    MPI_Send(buffer.data(), buffer.size(), MPI_DOUBLE, 0, i, MPI_COMM_WORLD);
		}
	    }	
	}
	MPI_Barrier(MPI_COMM_WORLD);    	
    }
#endif    
    
    /*** Post-proccessing ***/
    if (rank == 0) {
	/*** Arrange allTargets Matrix for measurements to be consecutive ***/
	Signal<double> allT2(allTargets.dim()); // init to zero
	int iidx = 0;
	for (int i = 0; i < allTargets.size(); ++i) {
	    if (allTargets.data()[i] != m_DUMMY) {
		allT2.data()[iidx] = allTargets.data()[i];
		++iidx;
	    }
	}

	/*** Print settings again ***/
	std::cout << cfg;
	
	/*** Write the output to disk and store the output file names in the fls file ***/
	std::cout << "\n\t *** Output Files ***";        
	std::ofstream flsFile(cfg.flsFileName.c_str());
	if (!flsFile) { 
	    if (cfg.printProgress) std::cerr << "\nCould not save names of output files in " << cfg.flsFileName << std::endl;;
	} else
	    std::cout << "\nRelevant file names for interfacing with Matlab have been saved in:\t" << cfg.flsFileName << std::endl;

	
	std::string name;	

	// original
	name = cfg.inputFile;
	if (flsFile) flsFile << "original\t\t" << name << std::endl;
	std::cout << "\nOriginal signal file:\t\t\t" << name;
	
	// masked signal
	if (cfg.sensor.setting() == Sensor::mask) {
	    name = outputSignal(maskedSignal, "_MASKED", cfg); // writes to disk and returns filename
	    if (flsFile) flsFile << "masked\t\t" << name << std::endl;
	    std::cout << "\nMasked signal saved at:\t\t\t" << name;
	}
	
	// targets	
	name = outputSignal(allT2, "_MEASUREMENTS", cfg);
	if (flsFile) flsFile << "measurements\t\t\t" << name << std::endl;
	std::cout << "\nMeasurements saved at:\t\t\t" << name;
	
	// recovered
	for (int scale = 0; scale < cfg.endScale; ++scale) {
	    std::stringstream label;
	    label << "_RECOVERED_" << scale+1 << "_OF_" << cfg.endScale;
	    name = outputSignal(cascadeRecoveredSignals[scale], label.str(), cfg);
	    if (flsFile) flsFile << "recovered_" << scale+1 << "\t\t" << name << std::endl;
	    std::cout << "\nRecovered Signal (scale " << scale+1 << ") saved at:\t" << name;
	}
	
	/*** Output MSE and PSNR for each stage ***/
	if (cfg.computePSNR) {
	    std::cout << "\n\n\n\n\n";
	    std::cout << "\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
		      << "%%%%%%%%%%%%%%% Accuracy of Reconstruction %%%%%%%%%%%%%%\n\n";
	    
	    for (int scale = 0; scale < cfg.endScale; ++scale) {
		double mse = computeMSE(signal, cascadeRecoveredSignals[scale]);
		double psnr = MSEtoPSNR(mse);
		std::stringstream record;
		record << "Scale " << scale+1 << ":  MSE  = " << mse << "\n";
		record << "\t  PSNR = " << psnr << "\n";
		std::cout << record.str();
	    }
	    std::cout << "\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%";
	    std::cout << "\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n\n\n\n\n";
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
		
		if (rank != grid(rowIdx, colIdx, frameIdx)) continue;
		
		if (rank == 0)
		    std::cout << "Block (" << rowIdx+1 << "," << colIdx+1
			      << "," << frameIdx+1 << ")   \tof\t("
			      << numBlocksHeight << "," << numBlocksWidth
			      << "," << numBlocksFrames << ")\t";

		sensedPatch = sensedEntries
		    .getPatch(rowIdx*block.height(), colIdx*block.width(), 
			      frameIdx*block.frames(), block.height(), block.width(),
			      block.frames());
		sensedPatchVector = vectorize(sensedPatch);
		initialSensedVector = vectorize(sensedPatch);


		Signal<double> theta = getSensingMatrix(block.size(), cfg.sensor.setting());
		Signal<double> origSignalPatch = signal.getPatch(rowIdx*block.height(), colIdx*block.width(),
								 frameIdx*block.frames(), block.height(),
								 block.width(), block.frames());		
		Signal<double> origSignalPatchVector = vectorize(origSignalPatch);		
		Signal<double> mangledSignalVector;
		if (cfg.sensor.setting() == Sensor::mask) mangledSignalVector = origSignalPatchVector;
		else mangledSignalVector = matMult(theta,origSignalPatchVector);				
		
		Signal<double> initialMangledVector = mangledSignalVector;
		Signal<double> mangledSignal = reshape(mangledSignalVector, block.dim());
		Signal<double> targetPatch(block.dim()); // set to zero
		targetPatch.fill(m_DUMMY);

			       
		for (int scale = 0; scale < cfg.endScale; ++scale) {
		    int measurements = countSensed(sensedPatchVector);
		    if (measurements == 0) {
			if (cfg.rank==0) {
			    if (scale == 0) std::cout << "(empty)" << std::endl;
			    else std::cout << "\t\t\t\t\t\t(empty)" << std::endl;
			}
			continue;
		    }
		    
		    Signal<double> measuredSignalPatchVector = applyMask(mangledSignalVector, sensedPatchVector);		    

		    /*** Declare and define RVM variables ***/
		    Signal<double> targets = getTargets(measuredSignalPatchVector, sensedPatchVector);
		    Signal<double> mangledBasis;
		    if (cfg.sensor.setting() == Sensor::mask) mangledBasis = cascadeBasis[scale];
		    else mangledBasis = matMult(theta,cascadeBasis[scale]);
		    Signal<double> designMatrix = getDesignMatrix(mangledBasis, sensedPatchVector);

    		    /*** Start the RVM ***/
    		    bool useCascade;
    		    if (scale+1 < cfg.endScale) useCascade = true;
    		    else useCascade = false;
		    
		    
		    RVM rvm(cfg.stdDev, cfg.deltaML_threshold, rank==0); // Third arg is "RVM prints progress?"

		    uint64 trainTimeStart = GetTimeMs64();
		    rvm.train(designMatrix, targets);	
		    trainTime += (GetTimeMs64() - trainTimeStart); trainTimeCount++;

		    uint64 predictTimeStart = GetTimeMs64();
		    recoveredVector = rvm.predict(cascadeBasis[scale]);		    		    		    
		    predictTime += (GetTimeMs64() - predictTimeStart); predictTimeCount++;

		    if (cfg.maskFill) recoveredVector.fill(origSignalPatchVector, initialSensedVector);
		    
    		    /*** Save recovered patch ***/
    		    recoveredPatch = reshape(recoveredVector, block.height(), block.width(), block.frames());
    		    cascadeRecoveredSignals[scale]
    			.putPatch(recoveredPatch, rowIdx*block.height(),
				  colIdx*block.width(), frameIdx*block.frames());
		    
		    
		    /*** Save original target patch ***/
		    if (scale == 0) {
			for (int i = 0; i < measurements; ++i) 
			    targetPatch.data()[i] = targets(i);
			allTargets.putPatch(targetPatch, rowIdx*block.height(),
					    colIdx*block.width(), frameIdx*block.frames());
		    }

    		    /*** Prepare for next part of cascade ***/
    		    if (useCascade) {			
			uint64 errorTimeStart = GetTimeMs64();			
			Signal<double> errors = rvm.predictionErrors(mangledBasis);
    			errorTime += (GetTimeMs64() - errorTimeStart); ++errorTimeCount;
			
			for (int i = 0; i < block.size(); ++i) { 
    			    if (cfg.sensor.setting() == Sensor::mask) { 
				if (errors(i) != 0) sensedPatchVector(i) = true; // get new mask
				else sensedPatchVector(i) = false;
			    } else {	 // %%%%%%% Temporary Hack. Other sensors don't work with Cascade yet. %%%%%%%%%%%
				if (initialSensedVector(i) != 0) sensedPatchVector(i) = true;
				else sensedPatchVector(i) = false;
			    }
    			    // Recovered becomes new signal Patch to get targets for next stage
			    mangledSignalVector = rvm.predict(mangledBasis);
			    mangledSignalVector.fill(initialMangledVector,initialSensedVector);
    			}
			if (cfg.printProgress && cfg.rank==0) std::cout << "\t\t\t\t\t\t";
    		    }
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

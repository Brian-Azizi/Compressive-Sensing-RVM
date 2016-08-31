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
    typedef double signalType; // Specify C++ type of input. NOTE: code was only tested with signalType = double
    Signal<signalType> signal; // holds original signal
    Signal<signalType> block; // holds the current signal block

    //Signal<signalType> sensingMatrices;
    Signal<signalType> maskedSignal;
    Signal<signalType> allTargets; // save the measurements in concatenated form
    Signal<bool> sensedEntries; // true at positions in which we maskedSignal has data 
    Signal<signalType> signalPatch; // subarray of maskedSignal
    Signal<bool> sensedPatch; // subarray of sensedEntries
    
    // Vectorized versions
    Signal<signalType> signalPatchVector; 
    Signal<bool> sensedPatchVector;
    Signal<bool> initialSensedVector;
    Signal<signalType> initialSignalVector;
    Signal<double> recoveredVector;
    Signal<double> recoveredPatch; // reshaped to have block.dim() dimensions
    
    std::vector<Signal<double> > cascadeRecoveredSignals; // holds output of each stage of cascade
    std::vector<Signal<double> > cascadeBasis; // holds basis matrices for each stage of cascade

    SignalSettings cfg;

    // MPI variables
    int rank, nproc; 
    /* Used to decide how to split the work load between the processes. (i,j,k)th entry contains index of rank that will reconstruct block (i,j,k) */
    Signal<int> grid;
    std::map<int, std::vector<Signal<int> > > gridMap; // maps rank number to list containing blocks it is responsible for.

    int m_DUMMY;   // a dummy integer. Usually used to denote empty data

    void reconstruct(); // helper function that contains the main loop of the algorithm

    /* Rounds input to the nearest integer and clamps to the range [min, max] */
    int getIntRange(double input, int min = 0, int max = 255);

    double computeMSE(const Signal<double>& original, const Signal<double>& reconstructed);
    double MSEtoPSNR(double mse);
    double computeRRE(const Signal<double>& original, const Signal<double>& reconstructed);

public:
    Interpolator(const SignalSettings& settingsFile); // Construct using settings file
    void run();				    // run interpolator
    };

Interpolator::Interpolator(const SignalSettings& settingsFile)
    : cfg(settingsFile)
{
    signal = Signal<signalType>(cfg.signalDim, false); // 'false' here means 'do not initialize'
    block = Signal<signalType>(cfg.blockDim, false);        
    sensedEntries = Signal<bool>(signal.dim()); // initialized to false
    cascadeRecoveredSignals = 
	std::vector<Signal<double> >(cfg.numCascades, Signal<double>(signal.dim(), false));
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
    for (int scale = 0; scale < cfg.numCascades; ++scale)
	cascadeBasis.push_back(getBasis(block.dim(), cfg.basisMode, cfg.startScale+scale));

    /*** Reconstruct the signal ***/
    if (cfg.printProgress && cfg.rank==0) std::cout << "\t*** Start Reconstruction ***\n";
    
    // Define grid and grid map
    rank = cfg.rank;
    nproc = cfg.nproc;
    grid = Signal<int> ( signal.height()/block.height(), signal.width()/block.width(), signal.frames()/block.frames());
    for (int i = 0; i < grid.size(); ++i) grid.data()[i] = i % nproc;
    for (int i = 0; i < grid.size(); ++i) gridMap[grid.data()[i]].push_back(grid.index(i));

    // Calculate number of blocks that root process is responsible for.
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
	    //std::cout << "Gathering Measurements: " << tag << "\tof " << grid.size() << "\t...\n";
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
    for (int scale = 0; scale < cfg.numCascades; ++scale) {
	for (int i = 0; i < grid.size(); ++i) {
	    Signal<double> grIdx = grid.index(i);
	    if (rank == 0) {
		//std::cout << "Gathering Result_" << scale+1 <<": " << i << "\tof " << grid.size() << "\t...\n";
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
	//std::cout << cfg;
	
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

	// signal mask
	if (cfg.sensor.setting() == Sensor::mask) {
	    name = outputSignal(sensedEntries, "_MASK", cfg); // writes to disk and returns filename
	    //if (flsFile) flsFile << "masked\t\t" << name << std::endl;
	    std::cout << "\nMask saved at:\t\t\t" << name;
	}
	
	// targets	
	name = outputSignal(allT2, "_MEASUREMENTS", cfg);
	if (flsFile) flsFile << "measurements\t\t\t" << name << std::endl;
	std::cout << "\nMeasurements saved at:\t\t\t" << name;
	
	// recovered
	for (int scale = 0; scale < cfg.numCascades; ++scale) {
	    std::stringstream label;
	    label << "_RECOVERED_" << scale+cfg.startScale << "_OF_" << cfg.numCascades;
	    name = outputSignal(cascadeRecoveredSignals[scale], label.str(), cfg);
	    if (flsFile) flsFile << "recovered_" << scale+1 << "\t\t" << name << std::endl;
	    std::cout << "\nRecovered Signal (scale " << scale+1 << ") saved at:\t" << name;
	}
	
	/*** Output MSE and PSNR for each stage ***/
	if (cfg.computePSNR) {
	    std::cout << "\n\n\n\n\n";
	    std::cout << "\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
		      << "%%%%%%%%%%%%%%% Accuracy of Reconstruction %%%%%%%%%%%%%%\n\n";
	    
	    for (int scale = 0; scale < cfg.numCascades; ++scale) {
		double mse = computeMSE(signal, cascadeRecoveredSignals[scale]);
		double psnr = MSEtoPSNR(mse);
		double rre = computeRRE(signal, cascadeRecoveredSignals[scale]);

		std::stringstream record;
		record << "Scale " << scale+cfg.startScale << ":  MSE  = " << mse << "\n";
		record << "\t  PSNR = " << psnr << "\n";
		record << "\t  RRE  = " << rre << "\n";
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
		
		if (rank != grid(rowIdx, colIdx, frameIdx)) continue; // only process the block if its assigned to your rank
		
		if (rank == 0) 	// we print progress on the root process
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

		/* Note, we compute a full-sized sensing matrix here. We use sensedEntries later on to delete rows */
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

		// Start MSCE Algorithm 
		for (int scale = 0; scale < cfg.numCascades; ++scale) { // loop over cascades
		    int measurements = countSensed(sensedPatchVector); 
		    if (measurements == 0) { // skip empty blocks
			if (cfg.rank==0) {
			    if (scale == 0) std::cout << "(empty)" << std::endl;
			    else std::cout << "\t\t\t\t\t\t(empty)" << std::endl;
			}
			continue;
		    }
		    
		    // Set the entries that are not measured to zero
		    Signal<double> measuredSignalPatchVector = applyMask(mangledSignalVector, sensedPatchVector);     

		    /*** Declare and define RVM variables ***/
		    Signal<double> targets = getTargets(measuredSignalPatchVector, sensedPatchVector); // delete rows
		    Signal<double> mangledBasis;
		    if (cfg.sensor.setting() == Sensor::mask) mangledBasis = cascadeBasis[scale];
		    else mangledBasis = matMult(theta,cascadeBasis[scale]);
		    Signal<double> designMatrix = getDesignMatrix(mangledBasis, sensedPatchVector); // delete rows

    		    /*** Start the RVM ***/
    		    bool useCascade; // true if we have not reached the final cascade yet
    		    if (scale+1 < cfg.numCascades) useCascade = true;
    		    else useCascade = false;
		    
		    /* Instantiate the RVM */
		    RVM rvm(cfg.stdDev, cfg.deltaML_threshold, rank==0); // Third arg is "RVM prints progress?"

		    uint64 trainTimeStart = GetTimeMs64();
		    rvm.train(designMatrix, targets);	// train using SSBL Algorithm
		    trainTime += (GetTimeMs64() - trainTimeStart); trainTimeCount++;

		    uint64 predictTimeStart = GetTimeMs64();
		    recoveredVector = rvm.predict(cascadeBasis[scale]);	// sparse multiplication of basis matrix and weights
		    predictTime += (GetTimeMs64() - predictTimeStart); predictTimeCount++;

		    if (cfg.maskFill && cfg.sensor.setting() == Sensor::mask) recoveredVector.fill(origSignalPatchVector, initialSensedVector); // fill recovered vector with original data, if desired.
		    
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
				if (errors(i) != 0) sensedPatchVector(i) = true; // get updated mask
				else sensedPatchVector(i) = false;
			    } else {	 // %%%%%%% Temporary Hack. Other sensors don't work with Cascade yet. %%%%%%%%%%%
				//if (errors(i) != 0 && std::abs(errors(i)) < 1.5) sensedPatchVector(i) = true; // get new mask
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
	/* So far, we have not restricted the range of the predicted pixel values. 
	   However, when viewing the data as a video file, all pixel values will be clamped to a certain range ([0,255] usually).
	   Thus, we need to convert the predicted pixels to integers in that range in order to compute the accurate PSNR*/
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

double Interpolator::computeRRE(const Signal<double>& orig, const Signal<double>& rec)
{
   // check input
    if (orig.dim() != rec.dim()) error("MSE: inputs must have the same dimensions");
    
    int maxEntry = 255;
    int minEntry = 0;

    double top = 0;
    double bot = 0;
    int xo, xr;			// note, this will convert to integers

    for (int i = 0; i < orig.size(); ++i) {
	/* So far, we have not restricted the range of the predicted pixel values. 
	   However, when viewing the data as a video file, all pixel values will be clamped to a certain range ([0,255] usually).
	   Thus, we need to convert the predicted pixels to integers in that range in order to compute the accurate PSNR*/
	xo = getIntRange(orig.data()[i], minEntry, maxEntry);
	xr = getIntRange(rec.data()[i], minEntry, maxEntry);
	top += ((xo-xr) * (xo-xr));
	bot += xo*xo;
    }
    double ret = (double) std::sqrt(top/bot);

    return ret;
}


#endif

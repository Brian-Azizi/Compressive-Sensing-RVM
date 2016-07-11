#ifndef GUARD_SIGNALSETTING_TPP
#define GUARD_SIGNALSETTING_TPP

#include <cmath>
#include <map>
#include <ostream>
#include <string>

#include "ConfigFile.hpp"
#include "Corrupter.hpp"
#include "Dim.hpp"
#include "Errors.hpp"
#include "SignalBasis.hpp"

SignalSettings::SignalSettings(const std::string& settingsFile)
    : cfg(settingsFile)
{
    initParameters();
    check();
}

void SignalSettings::initParameters() {
    printProgress = cfg.getValueOfKey<bool>("printProgress", true, false);

    inputFile = cfg.getValueOfKey<std::string>("inputFile","", printProgress);
    signalDim = dim_from_Signal_file(inputFile);

    int blockHeight = cfg.getValueOfKey<int>("blockHeight", 8, printProgress);
    int blockWidth = cfg.getValueOfKey<int>("blockWidth", 8, printProgress);
    int blockFrames = cfg.getValueOfKey<int>("blockFrames", 1, printProgress);
    blockDim = Dim(blockHeight, blockWidth, blockFrames);

    rngSeed = cfg.getValueOfKey<int>("rngSeed", 1, printProgress); // seed = 1 means time(NULL)
    if (rngSeed == 1) rngSeed = time(NULL);
    
    std::string corrupterMode = cfg.getValueOfKey<std::string>("corrupterMode", "uniform", printProgress);
    double corrupterPercentage = cfg.getValueOfKey<double>("corrupterPercentage", 0, printProgress);
    corr = Corrupter(corrupterMode, corrupterPercentage);

    std::string basisFunctionMode = cfg.getValueOfKey<std::string>("basisFunctionMode", "dct", printProgress);
    int basisEndScale = cfg.getValueOfKey<int>("basisEndScale", 1, printProgress);
    sigBasis = SignalBasis(basisFunctionMode, basisEndScale);


    
    StdDev = cfg.getValueOfKey<double>("StdDev", 1, printProgress);
    deltaML_threshold = cfg.getValueOfKey<double>("deltaML_threshold", 1, printProgress);
    if (StdDev <= 0 || deltaML_threshold <= 0) error("RVM parameters (stdDev and deltaML_threshold threshold) need to be positive)");
}

void SignalSettings::check()
{
    // Test if any block dimension is zero;
    if (blockDim.height() == 0 || blockDim.width() == 0 || blockDim.frames() == 0)
	error("height of blocks is zero");

    // Test if blocks are smaller or equal to input signal
    if (blockDim.height() > signalDim.height()) error("height of blocks exceeds height of signal");
    if (blockDim.width() > signalDim.width()) error("width of blocks exceeds width of signal");
    if (blockDim.frames() > signalDim.frames()) error("depth of blocks exceeds depth of signal");
    
    // Test if blocks divide signal
    if (signalDim.height() % blockDim.height() != 0) error("height of block does not divide height of signal");
    if (signalDim.width() % blockDim.width() != 0) error("width of block does not divide width of signal");
    if (signalDim.frames() % blockDim.frames() != 0) error("depth of block does not divide depth of signal");
    
    // Test that scale is greater than zero
    if (sigBasis.scale() < 1) error("End scale of basis is less than 1");

    
    // Test if block dimensions are divisible by 2^scale
    int const scaleDim = std::pow(2,sigBasis.scale());
    if (blockDim.height() % scaleDim != 0) error("block height does not support scale of wavelet");
    if (blockDim.width() % scaleDim != 0) error("block width does not support scale of wavelet");
    if (blockDim.frames() % scaleDim != 0) {
	if (blockDim.frames() != 1) error("block depth does not support scale of wavelet");
	else 
	    if (printProgress) std::cout << "block depth = 1 --> Using 2D wavelets" << std::endl;	
    }
            
    /* If all tests pass, then the signal is correctly divided up into block patches.*/
    if(printProgress) std::cout << "Settings test successful!" << std::endl;
    return;

}

std::ostream& operator<<(std::ostream& os, const SignalSettings& cfg)
{
    os << "Input File:\t\t" << cfg.inputFile
       << "\nSignal Dimensions:\t" << cfg.signalDim
       << "\nBlock Dimensions:\t" << cfg.blockDim
       << "\nRNG Seed:\t\t" << cfg.rngSeed
       << "\nCorruption Mode:\t" << cfg.corr.settingString()
       << "\nCorruption Percentage:\t" << cfg.corr.percentage()
       << "\nSignal Basis Mode:\t" << cfg.sigBasis.setting()
       << "\nSignal Basis End Scale:\t" << cfg.sigBasis.scale()
       << "\nPrint Progress:\t\t" << cfg.printProgress
       << "\nRVM std deviation:\t" << cfg.StdDev
       << "\nRVM delta ML threshold:\t" << cfg.deltaML_threshold << "\n";
}

#endif

#ifndef GUARD_SIGNALSETTING_TPP
#define GUARD_SIGNALSETTING_TPP

#include <cmath>
#include <map>
#include <ostream>
#include <string>
#include <sys/stat.h>

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
    signalDim = dim_from_Signal_file(inputFile); // basic checks for validity

    maskFile = cfg.getValueOfKey<std::string>("maskFile", "", printProgress);
    simulateCorruption = cfg.getValueOfKey<bool>("simulateCorruption", false, printProgress);

    outputDirectory = cfg.getValueOfKey<std::string>("outputDirectory","./");
    if (outputDirectory.at(outputDirectory.size()-1) != '/')
	outputDirectory.push_back('/');

    struct stat sb;
    if (stat(outputDirectory.c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode))
	error("Output directory '" + outputDirectory + "' not found");

    int blockHeight = cfg.getValueOfKey<int>("blockHeight", 8, printProgress);
    int blockWidth = cfg.getValueOfKey<int>("blockWidth", 8, printProgress);
    int blockFrames = cfg.getValueOfKey<int>("blockFrames", 1, printProgress);
    blockDim = Dim(blockHeight, blockWidth, blockFrames);

    rngSeed = cfg.getValueOfKey<int>("rngSeed", 1, printProgress); // seed = 1 means time(NULL)
    if (rngSeed == 1) rngSeed = time(NULL);
    
    std::string corrupterMode = cfg.getValueOfKey<std::string>("corrupterMode", "uniform", printProgress);
    double corrupterPercentage = cfg.getValueOfKey<double>("corrupterPercentage", 0, printProgress);
    corr = Corrupter(corrupterMode, corrupterPercentage);

    std::string basisFunctionMode = cfg.getValueOfKey<std::string>("basisMode", "dct", printProgress);
    basisMode = strToBasisMode(basisFunctionMode);
    endScale = cfg.getValueOfKey<int>("basisEndScale", 1, printProgress);
    
    SignalBasis(basisMode, endScale); // construct to see if valid

    stdDev = cfg.getValueOfKey<double>("stdDev", 1, printProgress);
    deltaML_threshold = cfg.getValueOfKey<double>("deltaML_threshold", 1, printProgress);    
}

void SignalSettings::check()
{
    if (simulateCorruption) {	// test of corruption settings were specified
	if(!cfg.keyExists("corrupterMode")) error("No corruption mode specified for simulation");
	if(!cfg.keyExists("corrupterPercentage")) error("No corrupter percentage specified");
	if(printProgress) std::cout << "Simulating corruption: maskFile will be ignored\n";
    } else {			// test if maskFile is valid
	Dim maskDim = dim_from_Signal_file(maskFile); // basic validity checks
	if(maskDim != signalDim) error("maskFile and inputFile must have the same dimensions");
	if(printProgress) std::cout << "Not simulating corruption: corrupterMode and corrupterPercentage will be ignored\n";
    }

    // test if any block dimension is zero;
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
    if (endScale < 1) error("End scale of basis is less than 1");

    // Test if block dimensions are divisible by 2^scale
    int const scaleDim = std::pow(2,endScale);
    if (blockDim.height() % scaleDim != 0) error("block height does not support scale of wavelet");
    if (blockDim.width() % scaleDim != 0) error("block width does not support scale of wavelet");
    if (blockDim.frames() % scaleDim != 0) {
	if (blockDim.frames() != 1) error("block depth does not support scale of wavelet");
	else 
	    if (printProgress) std::cout << "block depth = 1 --> Using 2D wavelets" << std::endl;	
    }
    
    if (stdDev <= 0 || deltaML_threshold <= 0) error("RVM parameters (stdDev and deltaML_threshold threshold) need to be positive)");

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
       << "\nMask File:\t\t" << cfg.maskFile
       << "\nSimulating Corruption:\t" << (cfg.simulateCorruption ? "yes" : "no")
       << "\nCorruption Mode:\t" << cfg.corr.settingString()
       << "\nCorruption Percentage:\t" << cfg.corr.percentage()
       << "\nSignal Basis Mode:\t" << modeToString(cfg.basisMode)
       << "\nSignal Basis End Scale:\t" << cfg.endScale
       << "\nPrinting Progress:\t" << (cfg.printProgress ? "yes" : "no")
       << "\nRVM std deviation:\t" << cfg.stdDev
       << "\nRVM delta ML threshold:\t" << cfg.deltaML_threshold 
       << "\nOutput Directory:\t" << cfg.outputDirectory << "\n";

    return os;
}

#endif

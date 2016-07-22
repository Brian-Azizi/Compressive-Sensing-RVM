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
    /*** First: check if we print ***/
    printProgress = cfg.getValueOfKey<bool>("printProgress", true); // default is 'true'

    /*** Get input file and check it ***/
    if (!cfg.keyExists("inputFile")) error("CFG: No input file specified.\n");
    inputFile = cfg.getValueOfKey<std::string>("inputFile");
    try {
	signalDim = dim_from_Signal_file(inputFile); // tries to open the file and checks for basic checks for validity 
    } catch(...) {
	std::cerr << "CFG: Error in opening input file '" << inputFile << "'." << std::endl;
	throw;
    }	
    
    /*** If we don't simulate, we require a mask file. Else, we require options for the corrupter.
	 Default is to simulate 30% uniform mask ***/
    simulateCorruption = cfg.getValueOfKey<bool>("simulateCorruption", true); // default is 'true'
    if (!simulateCorruption) {
	if(!cfg.keyExists("maskFile")) error("CFG: No mask file specified for corruption");
	maskFile = cfg.getValueOfKey<std::string>("maskFile", "");
	try {					      // try to open the specified file and check basic validity
	    Dim maskDim = dim_from_Signal_file(maskFile);
	    if(maskDim != signalDim) error("maskFile and inputFile must have the same dimensions");
	    if(printProgress) std::cout << "CFG: Using a mask file - corrupterMode and corrupterPercentage will be ignored\n";
	} catch(...) {
	    std::cerr << "CFG: Error in opening mask file '" << maskFile << "'." << std::endl;
	    throw;
	}
    } 
    if(simulateCorruption) {			// Read corrupter settings and instantiate corrupter
	std::string corrupterMode = cfg.getValueOfKey<std::string>("corrupterMode", "uniform");
	double corrupterPercentage = cfg.getValueOfKey<double>("corrupterPercentage", 30);
	try {
	    corr = Corrupter(corrupterMode, corrupterPercentage);	
	    if(printProgress) std::cout << "CFG: Simulating corruption - maskFile will be ignored\n";
	} catch(...) {
	    std::cerr << "CFG: Error in instantiating corrupter.\n";
	    throw;
	}
    }
    
    /*** Get output directory and check if it exists ***/  // Does not check whether we can write to it, nor whether we have enough space!
    outputDirectory = cfg.getValueOfKey<std::string>("outputDirectory","./"); // Default is current directory
    if (outputDirectory.at(outputDirectory.size()-1) != '/')
	outputDirectory.push_back('/');

    struct stat sb;
    if (stat(outputDirectory.c_str(), &sb) != 0 || !S_ISDIR(sb.st_mode))
	error("Output directory '" + outputDirectory + "' not found");

    /*** Get block dimensions ***/
    int blockHeight = cfg.getValueOfKey<int>("blockHeight", 2);
    int blockWidth = cfg.getValueOfKey<int>("blockWidth", 2);
    int blockFrames = cfg.getValueOfKey<int>("blockFrames", 1);
    try {
	blockDim = Dim(blockHeight, blockWidth, blockFrames); // default is (2,2,1);
    } catch(...) {
	std::cerr << "CFG: Error with specified block dimensions!\n";
	throw;
    }
      
    /*** Get Basis Function options ***/
    std::string basisFunctionMode = cfg.getValueOfKey<std::string>("basisMode", "dct");
    endScale = cfg.getValueOfKey<int>("basisEndScale", 1);
    if (basisFunctionMode == "dct") {
	if (printProgress) std::cout << "CFG: Using DCT basis - ignoring 'basisEndScale'\n";
	endScale = 1;
    }
    try {
	basisMode = strToBasisMode(basisFunctionMode);
	SignalBasis(basisMode, endScale); // construct to see if valid
    } catch(...) {
	std::cerr << "Error with specified basis function options.\n";
	throw;
    }

    stdDev = cfg.getValueOfKey<double>("stdDev", 1); // default is 1
    deltaML_threshold = cfg.getValueOfKey<double>("deltaML_threshold", 1); // default is 1
    
    /*** Get RNG seed ***/
    if (!cfg.keyExists("rngSeed")) { 	   // default is current time.
	if (printProgress) std::cerr << "CFG: RNG Seed not specified. Using current time as seed\n";
	rngSeed = time(NULL);
    } else {
	rngSeed = cfg.getValueOfKey<int>("rngSeed", 1); 
    }
}

void SignalSettings::check()
{
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
    if (basisMode != SignalBasis::dct) {
	if (blockDim.height() % scaleDim != 0) error("block height does not support scale of wavelet");
	if (blockDim.width() % scaleDim != 0) error("block width does not support scale of wavelet");
	if (blockDim.frames() % scaleDim != 0) {
	    if (blockDim.frames() != 1) error("block depth does not support scale of wavelet");
	    else 
		if (printProgress) std::cout << "CFG: block depth == 1 --> Using 2D wavelets" << std::endl;	
	}
    }
    if (stdDev <= 0 || deltaML_threshold <= 0) error("RVM parameters (stdDev and deltaML_threshold threshold) need to be positive)");

    /* If all tests pass, then the signal is correctly divided up into block patches.*/
    if(printProgress) std::cout << "CFG: Settings test successful!" << std::endl;
    return;

}

std::ostream& operator<<(std::ostream& os, const SignalSettings& setting)
{
    os << "\n\t*** Settings: ***\n";
    os << "Input File:\t\t" << setting.inputFile;
    os << "\nSignal Dimensions:\t" << setting.signalDim;

    os << "\nOutput Directory:\t" << setting.outputDirectory;
    if (!setting.cfg.keyExists("outputDirectory")) os << "\t\t(default)";
    
    if (!setting.cfg.keyExists("blockHeight") || !setting.cfg.keyExists("blockWidth") || !setting.cfg.keyExists("blockFrames")) {
	os << "\nBlock Height:\t\t" << setting.blockDim.height();
	if (!setting.cfg.keyExists("blockHeight")) os << "\t\t(default)";
	os << "\nBlock Width:\t\t" << setting.blockDim.width();
	if (!setting.cfg.keyExists("blockWidth")) os << "\t\t(default)";
	os << "\nBlock Frames:\t\t" << setting.blockDim.frames();
	if (!setting.cfg.keyExists("blockFrames")) os << "\t\t(default)";	
    } else os << "\nBlock Dimensions:\t" << setting.blockDim;
    
    os << "\nSimulating Corruption:\t" << (setting.simulateCorruption ? "yes" : "no");    
    if (!setting.cfg.keyExists("simulateCorruption")) os << "\t\t(default)";
    if (!setting.simulateCorruption)
	os << "\nMask File:\t\t" << setting.maskFile;
    else {
	os << "\nCorruption Mode:\t" << setting.corr.settingString();
	if (!setting.cfg.keyExists("corrupterMode")) os << "\t\t(default)";
	os << "\nCorruption Percentage:\t" << setting.corr.percentage();
	if (!setting.cfg.keyExists("corrupterPercentage")) os << "\t\t(default)";
    }
        
    os << "\nSignal Basis Mode:\t" << modeToString(setting.basisMode);
    if (!setting.cfg.keyExists("basisMode")) os << "\t\t(default)";
    
    if (setting.basisMode != SignalBasis::dct) {
	os << "\nSignal Basis End Scale:\t" << setting.endScale;
	if (!setting.cfg.keyExists("basisEndScale")) os << "\t\t(default)";
    }
    
    os << "\nRVM std deviation:\t" << setting.stdDev;
    if (!setting.cfg.keyExists("stdDev")) os << "\t\t(default)";
    os << "\nRVM delta ML threshold:\t" << setting.deltaML_threshold ;
    if (!setting.cfg.keyExists("deltaML_threshold")) os << "\t\t(default)";

    os << "\nPrinting Progress:\t" << (setting.printProgress ? "yes" : "no");
    if (!setting.cfg.keyExists("printProgress")) os << "\t\t(default)";
    
    os << "\nRNG Seed:\t\t" << setting.rngSeed;
    if (!setting.cfg.keyExists("rngSeed")) os << "\t(default: current time)";
    os << "\n";
    return os;
}

#endif

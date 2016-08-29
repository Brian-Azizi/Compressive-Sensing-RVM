#ifndef GUARD_SIGNALSETTING_TPP
#define GUARD_SIGNALSETTING_TPP

#include <cmath>
#include <cstdio>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

#include "ConfigFile.hpp"
#include "Mask.hpp"
#include "Dim.hpp"
#include "Sensor.hpp"
#include "Errors.hpp"
#include "Signal.hpp"
#include "SignalBasis.hpp"

SignalSettings::SignalSettings(const std::string& settingsFile)
    : cfg(settingsFile), cfgFile(settingsFile), rank(0), nproc(1)
{
    initParameters();
    check();
}

SignalSettings::SignalSettings(const std::string& settingsFile, int r, int n)
    : cfg(settingsFile), cfgFile(settingsFile), rank(r), nproc(n)
{
    if (nproc < 1) error("CFG: invalid nproc");
    if (r < 0 || r > nproc) error("CFG: invalid rank");
    initParameters();
    check();
}


void SignalSettings::initParameters() {
    /*** First: check if we print ***/
    printProgress = cfg.getValueOfKey<bool>("printToLogFile", true); // default is 'true'    

    if(printProgress) {
	/*** Open the log file ***/    
	if (!cfg.keyExists("logFile")) { // default log file
	    logFileName = cfgFile;
	    logFileName.erase(logFileName.find_last_of('.'));
	    logFileName += ".log";
	} else {
	    logFileName = cfg.getValueOfKey<std::string>("logFile");
	}	    
	if (!std::ofstream(logFileName.c_str())) error("Log file could not be created");
	
	FILE* cout_ff = std::freopen(logFileName.c_str(), "w", stdout);
	FILE* cerr_ff = std::freopen(logFileName.c_str(), "w", stderr);
    }
       
    /*** Set filename for fls file and check if we can open it ***/
    flsFileName = cfgFile;
    flsFileName.erase(flsFileName.find_last_of('.'));
    flsFileName += ".fls";
    if (!std::ofstream(flsFileName.c_str())) error("fls file could not be created");

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
	    sensor.setMode(Sensor::mask);
	    maskFill = cfg.getValueOfKey<bool>("maskFill", true);
	    if(rank == 0) std::cout << "CFG: Using a mask file - maskMode, sensorMode and percentage will be ignored\n";
	} catch(...) {
	    std::cerr << "CFG: Error in opening mask file '" << maskFile << "'." << std::endl;
	    throw;
	}
    } 
    if(simulateCorruption) {			// Read corrupter settings and instantiate corrupter	
	std::string sensorMode = cfg.getValueOfKey<std::string>("sensorMode", "mask");
	percentage = cfg.getValueOfKey<double>("percentage", 50);
	sensor = Sensor(sensorMode, percentage);
	
	std::string maskMode;
	if (sensor.setting() == Sensor::mask) {
	    maskMode = cfg.getValueOfKey<std::string>("maskMode", "uniform");	    	
	    maskFill = cfg.getValueOfKey<bool>("maskFill", true);
	}
	else {
	    maskMode = "uniform";
	    maskFill = false;
	    if(rank == 0) std::cout << "CFG: sensor mode != mask => maskMode and maskFill will be ignored" << std::endl;
	}

	try {
	    mask = Mask(maskMode, percentage);	
	    if(rank == 0) std::cout << "CFG: Simulating corruption - maskFile will be ignored\n";
	} catch(...) {
	    std::cerr << "CFG: Error in instantiating Mask.\n";
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

    /*** Get output name ***/
    outputName = cfg.getValueOfKey<std::string>("outputName",""); // If no name is given, default will be generated
    
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
    startScale = cfg.getValueOfKey<int>("basisStartScale", 1);
    if (basisFunctionMode == "dct") {
	if (rank == 0) std::cout << "CFG: Using DCT basis - ignoring 'basisEndScale' and 'basisStartScale'.\n";
	startScale = 1;
	endScale = 1;
    }
    try {
	basisMode = strToBasisMode(basisFunctionMode);
	SignalBasis(basisMode, endScale); // construct to see if valid
    } catch(...) {
	std::cerr << "Error with specified basis function options.\n";
	throw;
    }

    numCascades = endScale - startScale + 1;

    stdDev = cfg.getValueOfKey<double>("stdDev", 1); // default is 1
    deltaML_threshold = cfg.getValueOfKey<double>("deltaML_threshold", 1); // default is 1
    
    /*** Get RNG seed ***/
    if (!cfg.keyExists("rngSeed")) { 	   // default is current time.
	if (rank == 0) std::cout << "CFG: RNG Seed not specified. Using current time as seed\n";
	rngSeed = time(NULL);
    } else {
	rngSeed = cfg.getValueOfKey<int>("rngSeed", 1); 
    }

#ifdef USE_MPI
    // Make sure all processes use the same RNG seed
    MPI_Bcast(&rngSeed, 1, MPI_INT, 0, MPI_COMM_WORLD);
#endif

    /*** Check if we convert output to media file ***/
    convertToMedia = cfg.getValueOfKey<bool>("convertToMedia", true); // default is true
    frameRate = cfg.getValueOfKey<double>("frameRate", 25); // default is 25 fps

    /*** Check if we should compute the PSNR ***/
    computePSNR = cfg.getValueOfKey<bool>("computePSNR", true); // default is yes
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
    if (startScale < 1) error("Start scale of basis is less than 1");
    if (endScale < startScale) error("End scale of basis is less than startScale");

    // Test if block dimensions are divisible by 2^scale
    int const scaleDim = std::pow(2,endScale);
    if (basisMode != SignalBasis::dct) {
	if (blockDim.height() % scaleDim != 0) error("block height does not support scale of wavelet");
	if (blockDim.width() % scaleDim != 0) error("block width does not support scale of wavelet");
	if (blockDim.frames() % scaleDim != 0) {
	    if (blockDim.frames() != 1) error("block depth does not support scale of wavelet");
	    else 
		if (rank == 0) std::cout << "CFG: block depth == 1 --> Using 2D wavelets" << std::endl;	
	}
    }
    if (stdDev <= 0 || deltaML_threshold <= 0) error("RVM parameters (stdDev and deltaML_threshold threshold) need to be positive");

    // check if fps is positive
    if (signalDim.frames() != 1 && frameRate <= 0) error("Frame Rate needs to be positive");

    /* If all tests pass, then the signal is correctly divided up into block patches.*/
    if(rank == 0) std::cout << "CFG: Settings test successful!" << std::endl;
    return;

}

std::ostream& operator<<(std::ostream& os, const SignalSettings& setting)
{
    os << "\n\t*** Settings: ***\n";
    os << "Settings File:\t\t\t" << setting.cfgFile;
    os << "\nInput File:\t\t\t" << setting.inputFile;
    os << "\nSignal Dimensions:\t\t" << setting.signalDim;

    os << "\nOutput Directory:\t\t" << setting.outputDirectory;
    if (!setting.cfg.keyExists("outputDirectory")) os << "\t\t(default)";

    if (setting.cfg.keyExists("outputName")) os << "\nOutput Name:\t\t\t" << setting.outputName;
    
    if (!setting.cfg.keyExists("blockHeight") || !setting.cfg.keyExists("blockWidth") || !setting.cfg.keyExists("blockFrames")) {
	os << "\nBlock Height:\t\t\t" << setting.blockDim.height();
	if (!setting.cfg.keyExists("blockHeight")) os << "\t\t(default)";
	os << "\nBlock Width:\t\t\t" << setting.blockDim.width();
	if (!setting.cfg.keyExists("blockWidth")) os << "\t\t(default)";
	os << "\nBlock Frames:\t\t\t" << setting.blockDim.frames();
	if (!setting.cfg.keyExists("blockFrames")) os << "\t\t(default)";	
    } else os << "\nBlock Dimensions:\t\t" << setting.blockDim;
    
    os << "\nSimulating Corruption:\t\t" << (setting.simulateCorruption ? "yes" : "no");    
    if (!setting.cfg.keyExists("simulateCorruption")) os << "\t\t(default)";
    if (!setting.simulateCorruption) {
	os << "\nMask File:\t\t\t" << setting.maskFile;
	os << "\nMask Fill:\t\t\t" << (setting.maskFill ? "yes" : "no");
	if (!setting.cfg.keyExists("maskFill")) os << "\t\t(default)";
    }
    else {
	os << "\nSensor Mode:\t\t\t" << setting.sensor.settingString();
	if (!setting.cfg.keyExists("sensorMode")) os << "\t\t(default)";
	os << "\nPercentage:\t\t\t" << setting.percentage;
	if (!setting.cfg.keyExists("percentage")) os << "\t\t(default)";
	if (setting.sensor.setting() == Sensor::mask) {
	    os << "\nMask Mode:\t\t\t" << setting.mask.settingString();
	    if (!setting.cfg.keyExists("maskMode")) os << "\t\t(default)";
	    os << "\nMask Fill:\t\t\t" << (setting.maskFill ? "yes" : "no");
	    if (!setting.cfg.keyExists("maskFill")) os << "\t\t(default)";
	}
    }
    
    os << "\nSignal Basis Mode:\t\t" << modeToString(setting.basisMode);
    if (!setting.cfg.keyExists("basisMode")) os << "\t\t(default)";
    
    if (setting.basisMode != SignalBasis::dct) {
	os << "\nSignal Basis Start Scale:\t" << setting.startScale;
	if (!setting.cfg.keyExists("basisStartScale")) os << "\t\t(default)";
	os << "\nSignal Basis End Scale:\t\t" << setting.endScale;
	if (!setting.cfg.keyExists("basisEndScale")) os << "\t\t(default)";
    }
    
    os << "\nRVM std deviation:\t\t" << setting.stdDev;
    if (!setting.cfg.keyExists("stdDev")) os << "\t\t(default)";
    os << "\nRVM delta ML threshold:\t\t" << setting.deltaML_threshold ;
    if (!setting.cfg.keyExists("deltaML_threshold")) os << "\t\t(default)";

    os << "\nCompute PSNR:\t\t\t" << (setting.computePSNR ? "yes" : "no");
    if (!setting.cfg.keyExists("computePSNR")) os << "\t\t(default)";
    
    os << "\nRNG Seed:\t\t\t" << setting.rngSeed;
    if (!setting.cfg.keyExists("rngSeed")) os << "\t(default: current time)";
    os << "\nConverting to media:\t\t" << (setting.convertToMedia ? "yes" : "no");
    if (!setting.cfg.keyExists("convertToMedia")) os << "\t\t(default)";
    if (setting.signalDim.frames() != 1 && setting.convertToMedia) {
	os << "\nFrame Rate:\t\t\t" << setting.frameRate;
	if (!setting.cfg.keyExists("frameRate")) os << "\t\t(default)";
    }
    os << "\n";
    os << "\nPrint to log file:\t\t" << (setting.printProgress ? "yes" : "no");
    if (!setting.cfg.keyExists("printProgress")) os << "\t\t(default)";    
    if (setting.printProgress) 
	os << "\nLog file:\t\t\t" << setting.logFileName;

    os << "\nFLS file:\t\t\t" << setting.flsFileName;
    os << "\nNumber of Process:\t\t" << setting.nproc;
    os << "\n";
    return os;
}

std::string helpMessage(const std::string& argv_0)
{
    std::stringstream ss;
    ss << "Usage : " << argv_0 << " [settingsFile]\n"
       << "Default settings file is '.rvmsettings.cfg'\n"
       << "The format is \t\tsetting = val\n"
       << "Possible settings:\n\n"
       << "  inputFile\t\tName of input file name\n"
       << "  outputDirectory\tName of output directory (default: ./)\n"
       << "  outputName\t\tA label for the names of the output files (optional) \n"
       << "  blockHeight\t\tHeight of signal blocks (default: 2)\n"
       << "  blockWidth\t\tWidth signal blocks (default: 2)\n"
       << "  blockFrames\t\tDepth of signal blocks (default: 1)\n"
       << "  simulateCorruption\tIf 1, simulate corrupted signal via a sensor, else mask file is used (default: 1)\n"
       << "  maskFile\t\tFile name of signal mask. (Required if simulateCorruption=0, else ignored)\n"
       << "  percentage\t\tPercentage measurements vs full signal length (default: 50)\n"
       << "  sensorMode\t\tType of sensing matrix (mask, gaussian, bernoulli) (default: mask; ignored if simulateCorruption=0)\n"
       << "  maskMode\t\tDecimation pattern of simulated mask. (Default: uniform. Ignored if sensorMode is not mask)\n"       
       << "  maskFill\t\tIf 1, fill up the recovered signals with original measurements for masked signals (default: 1)\n"
       << "  basisMode\t\tType of basis transform. Possible values: haar, dct (default: dct)\n"
       << "  basisStartScale\t\tMinimum scale of basis functions (default: 1; ignored if using dct)\n"
       << "  basisEndScale\t\tMaximum scale of basis functions (default: 1; ignored if using dct)\n"
       << "  stdDev\t\tStandard deviation of the noise in the RVM (default: 1.0)\n"
       << "  deltaML_threshold\tThreshold for change in marginal likelihood in the RVM (default: 1.0)\n"
       << "  rngSeed\t\tSeed for random number generator for reproducability (optional)\n"
       << "  computePSNR\t\tIf 1, output reconstruction performance metrics (mse and psnr).\n"
       << "  printToLogFile\tIf 1, direct stdout and stderr to a log file (default: 0)\n"
       << "  logFile\t\tFile name for log file (oprional) (ignored if printToLogFile=0)\n"
       << "  convertToMedia\tIf 1, call Matlab to convert output to media format (default: 1)\n"
       << "  frameRate\t\tFrame rate for output videos (default: 25)\n"
       << std::endl;
    
    return ss.str();
}

std::string longHelpMessage(const std::string& argv_0)
{
    std::stringstream ss;
    ss << "Usage : " << argv_0 << " [settingsFile]\n"
       << "Default settings file is '.rvmsettings.cfg'\n"
       << "The format is \t\tsetting = val\n"
       << "The names of the output files will be saved in the a .fls file where the stem of the file is the one of the settingsfile (i.e. default is .rvmsettings.fls). This is to allow interfacing with other programs for later analysis\n"
       << "\nPossible settings:\n"
       << "  inputFile\t\tInput file name. Txt file containing input signal pixel values. Frames are seperated by empty lines. Each frame must have a consistent number of rows and columns, respectively. (No default; must be specified)\n"
       << "  outputDirectory\tName of output directory (default: ./)\n"
       << "  outputName\t\tA label for the names of the output files. Everything up to final '/' character will be ignored. The default name is a long string containing information on all the settings\n"
       << "  blockHeight\t\tHeight of signal blocks (default: 2)\n"
       << "  blockWidth\t\tWidth signal blocks (default: 2)\n"
       << "  blockFrames\t\tDepth of signal blocks (default: 1)\n"
       << "  simulateCorruption\tIf 1, simulate corrupted signal via a sensor, else mask file is used (default: 1)\n"
       << "  maskFile\t\tFile name of signal mask. (No default; must be specified if we don't simulate corruption)\n"
       << "  percentage\tPercentage of measurements to full signal length (default: 50; ignored if we are not simulating corruption)\n"
       << "  sensorMode\t\tType of sensing matrix. Possible values: mask, gaussian, bernoulli (default: mask; ignored if we are not simulating corruption)"
       << "  maskMode\t\tDecimation pattern of simulated mask. Possible values: uniform, timeRays, verticalFlicker, horizontalFlicker, missingFrames, verticalLines, horizontalLines (default: uniform; ignored if we are not simulating corruption or not using 'mask' as sensor mode)\n"       
       << "  maskFill\t\tIf 1 and simulating a sensormode = mask, we fill up the recovered signals with original measurements where available\n"
       << "  basisMode\t\tForm of basis functions used to represent the signal. Possible values: haar, dct (default: dct)\n"
       << "  basisStartScale\t\tMinimum scale of basis functions (default: 1; ignored if using dct)\n"
       << "  basisEndScale\t\tMaximum scale of basis functions (default: 1; ignored if using dct)\n"
       << "  stdDev\t\tStandard deviation of the noise in the RVM (default: 1.0)\n"
       << "  deltaML_threshold\tThreshold for change in marginal likelihood in the RVM (default: 1.0)\n"
       << "  rngSeed\t\tSeed for random number generator for reproducability (default: set to current time)\n"
       << "  computePSNR\t\tIf 1, the Mean Square Error and Peak Signal-to-Noise Ratio for each reconstruction stage will be computed and displayed on standard out. Note that this computation will cap Signal entries in the range [0,255]. Actual saved output is not affected by this. (default: 1)\n"
       << "  printToLogFile\t\tIf 1, direct stdout and stderr to a log file (default: 0)\n"
       << "  logFile\t\t\tFile name for a log file (ignored it 'printToLogFile=1') (default is name of settingsfile but with .log extension)\n"
       << "  convertToMedia\tIf 1, output .txt files will also be converted to media files (.png for images and .avi for videos). Requires MATLAB. (default: 1)\n"
       << "  frameRate\t\tFrame rate for output videos (default: 25; ignored if input is an image or if we are not converting to media).\n"
       << std::endl;
    
    return ss.str();
}

template<class T>		
std::string outputSignal(const Signal<T>& S, const std::string& label, const SignalSettings& cfg)
{
    std::stringstream ss;
    std::stringstream outputName;

    std::string inputName = cfg.inputFile;
    inputName.erase(0, inputName.find_last_of('/')+1);
    
    if (cfg.cfg.keyExists("outputName")) {
	std::string name = cfg.outputName;
	name.erase(0, name.find_last_of('/')+1);
	outputName << name << label << ".txt";
    } else {
	outputName << modeToString(cfg.basisMode);
	outputName << "_blockDim-" << cfg.blockDim.height() << "-" 
		   << cfg.blockDim.width() << "-" << cfg.blockDim.frames() << "_";
	
	if (cfg.simulateCorruption) outputName << cfg.mask.settingString() << "-" << cfg.mask.percentage() << "%";
	else {
	    std::string maskName = cfg.maskFile;
	    maskName.erase(0, maskName.find_last_of('/')+1);
	    outputName << maskName;
	}
	outputName << "_stdDev-" << cfg.stdDev << "_threshold-" << cfg.deltaML_threshold;
	outputName << label << "_" << inputName;
    }


    ss << cfg.outputDirectory << outputName.str();	
    std::string ret = ss.str();

    std::ofstream out;
    out.open(ss.str().c_str());
    if(!out) {
	if (cfg.printProgress)
	    std::cerr << "Warning: could not open output file '" + ss.str() + "'!"
		      << "\nAttempting output in current directory...";
	ret = outputName.str();
	out.open(outputName.str().c_str());
	if (!out) error("could not open output file '" + outputName.str());
	else 
	    if (cfg.printProgress) std::cerr << "OK!\n\n";
    }
    out << S;
    out.close();

    return ret;		// return name of output file
}

#endif

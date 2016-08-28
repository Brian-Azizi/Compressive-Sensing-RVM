#ifndef GUARD_SIGNALSETTINGS_HPP
#define GUARD_SIGNALSETTINGS_HPP

/* Class for storing the settings of the program */

#include <cmath>
#include <fstream>
#include <ostream>
#include <string>

#include "Signal.hpp"
#include "ConfigFile.hpp"
#include "Mask.hpp"
#include "Sensor.hpp"
#include "Dim.hpp"
#include "SignalBasis.hpp"

struct SignalSettings {
    std::string cfgFile;	// name of settings file
    std::string inputFile;    	// name of file containing input signal (required)
    std::string maskFile;	// name of file containing signal mask
    std::string outputName;	// label for the output files
    bool simulateCorruption;	// if yes, we simulate a mask file
    int rngSeed;		// if undefined, we use the current time as seed

    Sensor sensor;
    double percentage;

    Mask mask;			// contains information on mask
    bool maskFill;		// if true and using mask, we fill recovered signal with original sensed data

    SignalBasis::mode basisMode; 
    int startScale;		// initial scale of basis functions (ignored if basisMode == dct)
    int endScale;		// final scale of basis functions (ignored if basisMode == dct)
    Dim blockDim;		// hold dimensions of the signal blocks
    Dim signalDim;		// holds dimensions of input signal
    bool printProgress;		// if yes, we print direct output to a log file instead of stdout
    double stdDev;		// standard deviation of noise variable in the RVM
    double deltaML_threshold;	// convergence criterion for the RVM
    std::string outputDirectory; // directory in which to save the output files

    bool computePSNR;		// if yes, we compute the MSE and PSNR of the reconstructed signals

    std::string logFileName;	// name of log file. We redirect ouput to std::cout or std::cerr to the log file.
    std::string flsFileName;	// name of .fls file containing filenames of output (and input) files. Used for interfacing with Matlab
    
    bool convertToMedia;	// if yes, we call Matlab function 'convertSignal' to convert the output (and input) txt files to image or video files
    double frameRate;		// frame rate for output videos

    //%%%% MPI %%%%
    int rank, nproc;
    void setRank(int r) {rank = r;}
    void setNproc(int n) {nproc = n;}
    //%%%%%%%%%%%%%
    
    // Constructers
    SignalSettings(const std::string& settingsFile); // construct settings class from filename of settings file
    SignalSettings(const std::string& settingsFile, int rank, int nproc); // construct settings class from filename of settings file, MPI version
    
    ConfigFile configFile() const { return cfg; } // Return the underlying config file.
    void check();				  // Check that specified settings are internally consistent

    ConfigFile cfg;		 // Underlying ConfigFile that handles the reading of the actual settings file.
    void initParameters();	// initialize members of SignalSettings class
};

// Print settings to stream
std::ostream& operator<<(std::ostream& os, const SignalSettings& cfg);

std::string helpMessage(const std::string& argv_0); // print help message containing info on the format of .cfg files
std::string longHelpMessage(const std::string& argv_0); // more verbose help message

// Used for saving signal S to file
template<class T> std::string outputSignal(const Signal<T>& S, const std::string& label, const SignalSettings& cfg);

#include "SignalSettings.tpp"

#endif

#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include "Timer.hpp"

#include "Interpolator.hpp"


void errorMessage(const std::string& argv_0)
{
    std::cerr << "Usage : " << argv_0 << " [settingsFile]\n"
	      << "Default settings file is '.rvmsettings.cfg'\n"
	      << "The format is \t\tsetting = val\n"
	      << "The names of the output files will be saved in the file './rvmOutputFilenames.txt'. This is to allow interfacing with other programs for later analysis\n"
	      << "\nPossible settings:\n"
	      << "  inputFile\t\tInput file name. Txt file containing input signal pixel values. Frames are seperated by empty lines. Each frame must have a consistent number of rows and columns, respectively. (No default; must be specified)\n"
	      << "  outputDirectory\tName of output directory (default: ./)\n"
	      << "  outputName\t\tA label for the names of the output files. Everything up to final '/' character will be ignored. The default name is a long string containing information on all the settings\n"
	      << "  blockHeight\t\tHeight of signal blocks (default: 2)\n"
	      << "  blockWidth\t\tWidth signal blocks (default: 2)\n"
	      << "  blockFrames\t\tDepth of signal blocks (default: 1)\n"
	      << "  simulateCorruption\tIf 1, simulate corrupted signal, else mask file is used (default: 1)\n"
	      << "  maskFile\t\tFile name of signal mask. (No default; must be specified if we don't simulate corruption)\n"
	      << "  corrupterMode\t\tDecimation pattern of simulated mask. Possible values: uniform, timeRays, verticalFlicker, horizontalFlicker, missingFrames, verticalLines, horizontalLines (default: uniform; ignored if we are not simulating corruption)\n"
	      << "  corrupterPercentage\tProportion of masked measurements (default: 30; ignored if we are not simulating corruption)\n"
	      << "  basisMode\t\tForm of basis functions used to represent the signal. Possible values: haar, dct (default: dct)\n"
	      << "  basisEndScale\t\tMaximum scale of basis functions (default: 1; ignored if using dct)\n"
	      << "  stdDev\t\tStandard deviation of the noise in the RVM (default: 1.0)\n"
	      << "  deltaML_threshold\tThreshold for change in marginal likelihood in the RVM (default: 1.0)\n"
	      << "  rngSeed\t\tSeed for random number generator for reproducability (default: set to current time)\n"
	      << "  printProgress\t\tIf 1, print progress to standard out (default: 1)\n"
	      << std::endl;
	      
    return;
}


int main(int argc, char* argv[]) 
{
    uint64 startTime = GetTimeMs64(); // start timer
    
    std::string settingsFile;
    if (argc < 2) {
	settingsFile = ".rvmsettings.cfg";
	if (!std::ifstream(settingsFile.c_str())) {
	    std::cerr << "Could not open default settings file '" << settingsFile << "'. Abort.\n";
	    errorMessage(argv[0]);
	    return 1;
	}
    } else settingsFile = argv[1];

    if (settingsFile == "-h" || settingsFile == "--help") {
	errorMessage(argv[0]);
	return 0;
    }

    bool successfulRead = false;
    try {
	Interpolator interpol(settingsFile);   
	successfulRead = true;
	interpol.run();	
	if (interpol.printProgress()) std::cout << "Total execution time: " << GetTimeMs64() - startTime << " ms." << std::endl << std::endl;
    }
    catch (const std::exception& e) {
	if (!successfulRead) {
	    std::cerr << "Error with settings file '" << settingsFile << "': " << e.what() << std::endl;
	    errorMessage(argv[0]);
	}
	std::cerr << "An unexpected error occured! Error message: " << e.what() << "\n";
    } 

    // run matlab script
    int status = system("matlab -nodisplay -nosplash -nodesktop -r \"interpolatorOutput; exit;\" ");
}

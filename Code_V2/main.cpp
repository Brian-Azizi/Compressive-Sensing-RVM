#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include "Timer.hpp"

#include "Interpolator.hpp"


int main(int argc, char* argv[]) 
{
    uint64 startTime = GetTimeMs64(); // start timer
      
    /*** Handle settings file ***/
    std::string fileName;
    if (argc < 2) fileName = ".rvmsettings.cfg";
    else fileName = argv[1];

    // Check whether we should display help message
    if (fileName == "-h" || fileName == "--help") {
	helpMessage(argv[0]);
	return 0;
    }

    // Attempt to open settings file
    if (!std::ifstream(fileName.c_str())) {
	std::cerr << "Could not open default settings file '" << fileName << "'. Abort.\n";
	helpMessage(argv[0]);
	return 1;
    }
    
    /*** Start Interpolator ***/
    bool successfulRead = false;
    bool successfulRun = false;
    try {
	/*** Parse the Settings File ***/
	SignalSettings cfg = SignalSettings(fileName);
	Interpolator interpol(cfg);   
	successfulRead = true;

	/*** Run the Interpolator ***/
	interpol.run();	
	successfulRun = true;
	
	/*** Run Matlab Script ***/
	if (cfg.convertToMedia) {
	    std::stringstream fps;
	    fps << cfg.frameRate;
	    if (cfg.printProgress) std::cout << "\n\t***Converting output to media format using Matlab ***" << std::endl;
	    const std::string matlabScript = "convertSignal";
	    const std::string openMatlab = "matlab -nodisplay -nosplash -nodesktop -r ";
	    const std::string matlabCommand = "\"try frameRate = " + fps.str() + "; "
		+ matlabScript + "; catch disp('Matlab Error: Unable to run ''" 
		+ matlabScript + "''.'); end; exit;\" ";
	    
	    int status = system((openMatlab + matlabCommand + " | tail -n +14 ").c_str()); // call matlab
	}

	/*** Display Time ***/
	if (cfg.printProgress) 
	    std::cout << "\nTotal execution time: " << GetTimeMs64() - startTime << " ms.\n\n";	
    } 
    catch (const std::exception& e) {
	if (!successfulRead) {
	    std::cerr << "Error with settings file '" << fileName << "': " << e.what() << "\n\n";
	    helpMessage(argv[0]);
	    return 1;
	} else if (!successfulRun) {
	    std::cerr << "An error occured while running the interpolator: " << e.what() << "\n\n";
	    return 2;
	} else {
	    std::cerr << "An unexpected error occured! Error message: " << e.what() << "\n\n";
	    return 3;
	}     
    }   
}

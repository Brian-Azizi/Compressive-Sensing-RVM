#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <climits>
#include "Headers/Timer.hpp"


uint64 startTime = 0;
uint64 trainTime = 0; int trainTimeCount = 0;
uint64 predictTime = 0; int predictTimeCount = 0;
uint64 errorTime = 0; int errorTimeCount = 0;

#include "Headers/Interpolator.hpp" // needs to be after declaration of global timing variables

int main(int argc, char* argv[]) 
{
    uint64 startTime = GetTimeMs64(); // start timer
      
    /*** Handle settings file ***/
    std::string fileName;
    if (argc < 2) fileName = ".rvmsettings.cfg";
    else fileName = argv[1];

    // Check whether we should display help message
    if (fileName == "-h" || fileName == "--help") {
	std::cout << helpMessage(argv[0]);
	return 0;
    }

    // Attempt to open settings file
    if (!std::ifstream(fileName.c_str())) {
	std::cerr << "Could not open default settings file '" << fileName << "'. Abort.\n";
	std::cerr << helpMessage(argv[0]);
	return 1;
    }
    
    /*** Start Interpolator ***/
    bool successfulRead = false;
    bool successfulRun = false;
    try {
	/*** Parse the Settings File and instantiate the interpolator ***/
	SignalSettings cfg = SignalSettings(fileName);
	Interpolator interpol(cfg);   
	successfulRead = true;

	/*** Redirect std::cout and std::cerr to the log file ***/
	if (cfg.printProgress) std::cout << "Redirecting output messages to '" << cfg.logFileName << "' from here on." << std::endl;
	std::freopen(cfg.logFileName.c_str(), "w", stdout);
	std::freopen(cfg.logFileName.c_str(), "w", stderr);
	
	/*** Run the Interpolator ***/
	interpol.run();	
	successfulRun = true;
	
	/*** Display Time ***/
	if (cfg.printProgress) {
	    std::cout << "\n\t*** Measured Times ***\n"
		      << "Total execution time: " << GetTimeMs64() - startTime << " ms\n" // does not include time for converting to media
		      << "Average time for training: " << trainTime / (double) trainTimeCount << " ms\n"
		      << "Average time for prediction: " << predictTime / (double) predictTimeCount << " ms\n"
		      << "Average time for errors: " << errorTime / (double) errorTimeCount << " ms\n\n";
	    

	    /*** Display host and user name ***/
	    char hostname[HOST_NAME_MAX];
	    gethostname(hostname,HOST_NAME_MAX);
	    std::cout << "Program was called from host: " << hostname << std::endl;
	}

	/*** Run Matlab Script ***/
	if (cfg.convertToMedia) {
	    if (cfg.printProgress) std::cout << "\n\t*** Open Matlab for converting output to media format ***" << std::endl;

	    /*** Construct Matlab command ***/
	    std::stringstream fps;
	    fps << cfg.frameRate;
	    const std::string matlabFunction = "convertSignal('" + cfg.flsFileName + "', " + fps.str() + ")";
	    std::string command = "try; " + matlabFunction + "; catch me; disp(['MATLAB ERROR: ', me.message, ]); disp('Unable to convert output to media'); end; exit;";

	    /*** Create a child process to deal with Matlab. I believe this does not work on Windows. Matlab needs to be installed and in the PATH ***/
	    pid_t pid = fork();
	    if (pid == -1)
		std::cerr << "Error: call to 'fork()' failed. Unable to convert output to media\n";
	    else if (pid == 0) { // child
		execlp("matlab","MATLAB", "-nodisplay", "-nosplash", "-nodesktop", "-r", command.c_str(), (char *)0);
		std::cerr << "Error: call to 'execlp' failed. Unabled to convert output to media\n";
		return 1;
	    }
	    
	    // only parent should get here

	    return 0;
	}
    } 
    catch (const std::exception& e) {
	if (!successfulRead) {	    
	    std::cerr << "Error with settings file '" << fileName << "': " << e.what() << "\n\n";
	    std::cerr << helpMessage(argv[0]);
	    return 1;
	} else if (!successfulRun) {
	    std::cerr << "An error occured while running the interpolator: " << e.what() << "\n\n";
	    return 2;
	} else {
	    std::cerr << "An unexpected error occured! Error message: " << e.what() << "\n\n";
	    return 3;
	}     
    }   
    catch (...) {
	std::cerr << "An unexpected exception was thrown\n";
	throw;
    }
}

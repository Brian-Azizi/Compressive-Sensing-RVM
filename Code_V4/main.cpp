//#define USE_MPI			// Comment out if MPI is not used

#include <climits>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <sys/types.h>
#include <unistd.h>

#ifdef USE_MPI
 #include <mpi.h>
#endif

// global variables used for measuring execution time of program
#include "Headers/Timer.hpp"
uint64 startTime = 0;
uint64 trainTime = 0; int trainTimeCount = 0;
uint64 predictTime = 0; int predictTimeCount = 0;
uint64 errorTime = 0; int errorTimeCount = 0;

#include "Headers/Signal.hpp"
#include "Headers/Interpolator.hpp" // needs to be after declaration of global timing variables


int main(int argc, char* argv[]) 
{
    int rank = 0, nproc = 1; 
    
#ifdef USE_MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    
#endif

    uint64 startTime = GetTimeMs64(); // start timer
      
    /******************** Handle settings file *********************************/
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
    
    /********************* Start Interpolator **********************************/
    bool successfulRead = false;
    bool successfulRun = false;
    try {
	// Parse the Settings File and instantiate the interpolator
	SignalSettings cfg = SignalSettings(fileName,rank,nproc);
	Interpolator interpol(cfg);   
	successfulRead = true;
	
	// Run the Interpolator
	interpol.run();	
	successfulRun = true;
	
#ifdef USE_MPI
	MPI_Barrier(MPI_COMM_WORLD);
#endif

	/**********  Display Time and Host for each process **************************************/
	/*(Total time does not include the call to Matlab*/
		
#ifdef HOST_NAME_MAX		// seems to not be available on osx
	long int host_name_max = HOST_NAME_MAX;
#else 
	int host_name_max = 64;
#endif
	char hostname[host_name_max];
	gethostname(hostname,host_name_max);
	
	std::stringstream sco;
	sco << "\n\t*** Measured Times for rank " << rank << " on Host " << hostname << " ***\n"
	    << "Total execution time: " << GetTimeMs64() - startTime << " ms\n"
	    << "Average time for training: " << trainTime / (double) trainTimeCount << " ms\n"
	    << "Average time for prediction: " << predictTime / (double) predictTimeCount << " ms\n"
	    << "Average time for errors: " << errorTime / (double) errorTimeCount << " ms\n\n";	    
	std::cout << sco.str();	
	
#ifdef USE_MPI
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
#endif	

	/********************* Run Matlab Script ***************************************/
	if (rank == 0) {	    
	    if (cfg.convertToMedia) {
		std::cout << "\n\t*** Open Matlab for converting output to media format ***" << std::endl;
		
		// Construct Matlab command
		std::stringstream fps;
		fps << cfg.frameRate;
		const std::string matlabFunction = "convertSignal('" + cfg.flsFileName + "', " + fps.str() + ")";
		std::string command = "try; " + matlabFunction + "; catch me; disp(['MATLAB ERROR: ', me.message, ]); disp('Unable to convert output to media'); end; exit;";
		
		/* Create a child process to deal with Matlab. 
		   Matlab needs to be installed and in the PATH.
		   I believe this does not work on Windows.*/
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
    } 
    
    /************************ Error Handling **************************************/
    catch (const std::exception& e) {
	if (!successfulRead) {	    
	    std::cerr << "Error with settings file '" << fileName << "': " << e.what() << "\n\n";
	    std::cerr << helpMessage(argv[0]);
	    return 1;
	} else if (!successfulRun) {
	    std::cerr << "An error occured while running the interpolator: " << e.what() << "\n\n";
	    return 1;
	} else {
	    std::cerr << "An unexpected error occured! Error message: " << e.what() << "\n\n";
	    return 1;
	}     
    }   
    catch (...) {
	std::cerr << "An unexpected exception was thrown\n";
	throw;
    }
}


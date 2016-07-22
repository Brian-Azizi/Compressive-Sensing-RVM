#include "Timer.hpp"

#include "Interpolator.hpp"

int main(int argc, char* argv[]) 
{
    uint64 startTime = GetTimeMs64(); // start timer
    
    if (argc < 2) error("No input file specified");
    if (argc > 2) error("To many input arguments");
    
    Interpolator interpol(argv[1]);   

    interpol.run();

    std::cout << "\nTotal execution time: " << GetTimeMs64() - startTime << " ms." << std::endl;
}

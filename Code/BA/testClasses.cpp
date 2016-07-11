#include <iostream>

#include "SignalSettings.hpp"
#include "Signal.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2) error("No input file specified");
    if (argc > 2) error("To many input arguments");
 
    const std::string cfgFile = argv[1];

    SignalSettings cfg("setting1.cfg");
    Signal<double> sig = readSignal(cfg.inputFile);
    
    std::cout << cfg;
}

#include <iostream>

#include "SignalSettings.hpp"


int main(int argc, char* argv[])
{
    if (argc < 2) error("No input file specified");
    if (argc > 2) error("To many input arguments");
 
    const std::string cfgFile = argv[1];

    SignalSettings cfg("setting1.cfg");

    std::srand(cfg.rngSeed);    
    std::cout << cfg;
}

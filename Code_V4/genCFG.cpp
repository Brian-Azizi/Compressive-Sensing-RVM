// Can be usedthis to generate settings files
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>

int main()
{
    // Specify constant settings
    //std::string inputFile = "inputFile = ../Data/InputFiles/lenna_512-512-1.txt\n";
    std::string inputFile = "inputFile = ../Data/InputFiles/foreman_128-128-64.txt\n";
    std::string outputDir = "outputDirectory = ../Data/ResultsDump/\n";
   
    std::string h = "blockHeight = 8\n";
    std::string w = "blockWidth = 8\n";
    std::string f = "blockFrames = 8\n";
    
    std::string mode = "basisMode = haar\n sensorMode = mask\n";
    std::string scale = "basisStartScale = 3\n basisEndScale = 3\n";
    
    std::string dml = "deltaML_threshold = 1\n";
    std::string conv = "convertToMedia = 0\n";
    std::string sigma = "stdDev = 1\n";
    std::string mm = "maskMode = uniform\n";
    std::string rng = "rngSeed = 20081993\n";
    // Desired params
    std::vector<double> perc;
    for (double i = 10; i <= 100; i += 10) perc.push_back(i);

    int N = perc.size();

    std::string fileStem = "foreman";

    std::string shellScript = "simulate.sh";
    std::ofstream script(shellScript.c_str());

    if (!script) {std::cout << "ERROR!" << std::endl; return 1;}

    for (int i = 0; i < N; ++i) {
	double p = perc[i];
	std::stringstream percentage;
	percentage << "percentage = " << p << "\n";

	std::stringstream outName;
	outName << "outputName = " << fileStem << "_" << i << "\n";

	std::stringstream logFile;
	logFile << "logFile = ../Data/ResultsDump/" << fileStem << "_" << i << ".log\n";

	std::stringstream cfgName;
	cfgName << fileStem << "_" << i << ".cfg";
	
	std::ofstream of(cfgName.str().c_str());
	if (!of) {std::cout << "ERROR!" << std::endl; return 1;}
	of << inputFile << outputDir << h << w << f << mode << scale << dml 
	   << conv << sigma << outName.str() << logFile.str() << percentage.str() << mm;
	of.close();

	script << "./largeRun.sh " << cfgName.str() << "\n";
    }

    script.close();

}

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
    std::string outputDir = "outputDirectory = ../Data/Simulations/noise/\n";
   
    std::string h = "blockHeight = 4\n";
    std::string w = "blockWidth = 4\n";
    std::string f = "blockFrames = 4\n";
    
    std::string mode = "basisMode = haar\n";
    std::string scale = "basisEndScale = 2\n";

    std::string dml = "deltaML_threshold = 1\n";
    std::string conv = "convertToMedia = 0\n";

    // Desired params
    //double sigs[] = {0.001, 0.003, 0.01, 0.03, 0.1, 0.3, 1, 3, 10, 30, 100, 300, 1000, 3000};
    std::vector<double> sigs;
    for (double i = 0.1; i <= 2; i += 0.2) sigs.push_back(i);
    //double sigs[] = {-3,-2.5,-2,-1.5,-1,-0.5,0,0.5,1,1.5,2,2.5,3,3.5,4};
    int N = sigs.size();

    std::string fileStem = "foreman";

    std::string shellScript = "simulate.sh";
    std::ofstream script(shellScript.c_str());

    if (!script) {std::cout << "ERROR!" << std::endl; return 1;}

    for (int i = 0; i < N; ++i) {
	//double s = std::pow(10,sigs[i]);
	double s = sigs[i];
	std::stringstream noise;
	noise << "stdDev = " << s << "\n";

	std::stringstream outName;
	outName << "outputName = " << fileStem << "_" << i << "\n";

	std::stringstream logFile;
	logFile << "logFile = ../Data/Simulations/noise/" << fileStem << "_" << i << ".log\n";

	std::stringstream cfgName;
	cfgName << fileStem << "_" << i << ".cfg";
	
	std::ofstream of(cfgName.str().c_str());
	if (!of) {std::cout << "ERROR!" << std::endl; return 1;}
	of << inputFile << outputDir << h << w << f << mode << scale << dml << conv << noise.str() << outName.str() << logFile.str();
	of.close();

	script << "./smallRun.sh " << cfgName.str() << "\n";
    }

    script.close();

}

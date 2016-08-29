#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

int main()
{
    std::vector<double> sig;
    std::vector<double> psnr;

    int N = 11;

    for (int i = 0; i < N; ++i) {
	std::stringstream ss;
	ss << "../Data/Simulations/noise/foreman_" << i << ".log";
	std::ifstream inp(ss.str().c_str());

	std::istringstream noiseLine;
	std::istringstream psnrLine;
	bool nextLine = false;
	
	std::string line;
	while(std::getline(inp,line)) {
	    if (line.substr(0,7) == "RVM std") {
		noiseLine.str(line.substr(20));
		double q;
		noiseLine >> q;
		sig.push_back(q);
	    }

	    if (nextLine) {
		psnrLine.str(line.substr(10));
		double p;
		psnrLine >> p;
		psnr.push_back(p);
	
		nextLine = false;
	    }


	    if (line.substr(0,7) == "Scale 2") {
		nextLine = true;
	    }

	}		
	inp.close();
    }

    std::ofstream of("noise4.txt");
    if (!of) {std::cout << "ERROR" << std::endl; return 1;}

    for (int i = 0; i < N; ++i)  {
	of << sig[i] << " " << psnr[i] << std::endl;
    }

}

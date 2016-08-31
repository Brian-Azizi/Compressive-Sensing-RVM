#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

int main()
{
    std::vector<double> perc;
    std::vector<double> psnr;
    std::vector<double> rre;

    std::string logDirectory = "../Data/ResultsDump/MASK_1/";
    std::string stem = "foreman";
	
    int N = 10;

    for (int i = 0; i < N; ++i) {
	std::stringstream ss;
	ss << logDirectory << stem << "_" << i << ".log";
	std::ifstream inp(ss.str().c_str());

	std::istringstream percLine;
	std::istringstream psnrLine;
	std::istringstream rreLine;
	bool nextLine = false;
	
	std::string line;
	while(std::getline(inp,line)) {
	    if (line.substr(0,10) == "Percentage") {
		percLine.str(line.substr(12));
		std::string dum;
		double q;
		percLine >> q;
		perc.push_back(q);
	    }

	    if (nextLine) {
		psnrLine.str(line.substr(10));
		//std::cout << psnrLine.str() << std::endl;
		double p;
		psnrLine >> p;
		psnr.push_back(p);
		
		std::getline(inp,line);
		rreLine.str(line.substr(10));		
		//		std::cout << psnrLine.str() << std::endl;
		double r;
		rreLine >> r;
		rre.push_back(r);
		
		nextLine = false;
	    }


	    if (line.substr(0,7) == "Scale 1") 
		nextLine = true;
	    }
	
	inp.close();
    }

    std::ofstream of("MASK_1.txt");
    if (!of) {std::cout << "ERROR" << std::endl; return 1;}
    
    of << "pc psnr rre\n";
    for (int i = 0; i < N; ++i)  {	
	of << perc[i] << " " << psnr[i] << " " << rre[i] <<  std::endl;
    }
    
}

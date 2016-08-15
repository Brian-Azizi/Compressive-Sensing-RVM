
int idx;

#include "Signal.hpp"
#include "Headers/basisTestParams.h"
#include "newTestBasis.h"

int main()
{

    // Dummy comment

    // test Basis function generation

    double errorTolerance = 1e-8;

    int fails = 0;
    std::cout << "Testing haarBasis:" << std::endl;
    
    for (idx = 0; idx < numTests; ++idx) {
	try {
	    int h = basisTestParams[idx][0];
	    int w = basisTestParams[idx][1];
	    int f = basisTestParams[idx][2];
	    int scale = basisTestParams[idx][3];
	    
	    std::stringstream inputName;
	    inputName << "/local/data/public/ba308/testFiles/basis_" << scale << "_" << h << "-" << w
		      << "-" << f << ".txt";
	    newTestBasis(inputName.str(),h,w,f,scale,errorTolerance);
	} catch(std::runtime_error e) {
	    ++fails;
	    std::cerr << e.what();
	} catch(...) {
	    std::cerr << "Some error with the test script occured!\n";
	}
    }
    if (fails == 0) {
	std::cout << "All " << numTests << " tests passed!\n";
    } else {
	std::cout << fails << " / " << numTests
		  << " tests failed!\n";
    }   
}

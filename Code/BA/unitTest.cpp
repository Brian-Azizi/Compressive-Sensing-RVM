#include <cassert>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <string>
#include <sys/stat.h>

// GGP's files
#include "Headers/utilities.h"
#include "Headers/kronecker.h"
#include "Headers/fast_updates.h"

// BA's files
#include "settingsFile.h"
#include "Headers/settingsTester.h"
#include "Headers/input3D.h"
#include "Headers/print3D.h"
#include "Headers/print2D.h"
#include "Headers/print1D.h"
#include "Headers/haarTransform3D.h"
#include "Headers/getPatch3D.h"
#include "Headers/vectorize3D.h"
#include "Headers/haarBasis.h"
#include "Headers/corruptSignal.h"
#include "Headers/countSensed.h"
#include "Headers/getTargets.h"
#include "Headers/getDesignMatrix.h"
#include "Headers/fillSensedInfo.h"
#include "Headers/deVectorize.h"
#include "Headers/putPatch3D.h"
#include "Headers/output3D.h"

#include "Headers/testBasis.h"
#include "Headers/basisTestParams.h"

int main()
{

    // test Basis function generation

    double errorTolerance = 1e-8;

    int fails = 0;
    std::cout << "Testing haarBasis:" << std::endl;

    for (int i = 0; i < numTests; ++i) {
	try {
	    int h = basisTestParams[i][0];
	    int w = basisTestParams[i][1];
	    int f = basisTestParams[i][2];
	    int scale = basisTestParams[i][3];
	    
	    std::stringstream inputName;
	    inputName << "testFiles/basis_" << scale << "_" << h << "-" << w
		      << "-" << f << ".txt";
	    testBasis(inputName.str(),h,w,f,scale,errorTolerance);
	} catch(std::runtime_error e) {
	    ++fails;
	    std::cerr << e.what();
	}	
    }
    if (fails == 0) {
	std::cout << "All " << numTests << " tests passed!\n";
    } else {
	std::cout << fails << " / " << numTests
		  << " tests failed!\n";
    }   
}

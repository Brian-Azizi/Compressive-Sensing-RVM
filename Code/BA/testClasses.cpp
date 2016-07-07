#include <stdexcept>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include "classes.h"
#include "functions.h"
#include "rvm.h"
#include <iomanip>

int main() {
    RVM<double> rvm;
    std::cout << "Sigma = " << rvm.sigma() << std::endl
    	      << "deltaML = " << rvm.deltaML() << std::endl
    	      << "printing progress?: " << ( rvm.doWePrint() ? "yes" : "no") << std::endl;
    
    Signal<int> B = read("test_4-8-2.txt");
    Signal<int> t = B.getPatch(0,2,8,1);
    std::cout << "B = \n" << B << std::endl
    	      << "t = \n" << t << std::endl;
    rvm.train_fastUpdates(B,t);
    std::cout << "coeff = \n" << rvm.coefficients();
    
}

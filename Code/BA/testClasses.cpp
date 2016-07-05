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
    Signal<int> A = read("test_4-8-2.txt", 2);

    std::cout << A << A.dim() << std::endl;
    RVM<double> rvm;
    rvm.setSigma(2);

}

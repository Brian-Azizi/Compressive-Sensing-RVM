#include <iostream>

#include "Signal.hpp"

//#include "rvm.h"

int main() {
    Corrupter corr(23,Corrupter::uniform);
    std::cout << corr.percentage() << "\n" << corr.setting() << "\n" << corr.settingString() << "\n";
}

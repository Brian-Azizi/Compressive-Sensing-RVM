#ifndef GUARD_ERRORS_TPP
#define GUARD_ERRORS_TPP

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

/*** Error helper functions ***/
void error() {
    throw std::runtime_error("");
}
void error(const std::string& s) {
    throw std::runtime_error(s);
}
void error(const std::string& s1, const std::string& s2) {
    error(s1 + s2);
}
void error(const std::string& s1, int i) {
    std::ostringstream os;
    os << s1 << ": " << i;
    error(os.str());
}
void exitWithError(const std::string& error)
{
    std::cout << error;
    //std::cin.ignore();
    std::cin.get();

    exit(EXIT_FAILURE);
}


#endif

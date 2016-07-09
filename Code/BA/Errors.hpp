#ifndef GUARD_ERRORS_HPP
#define GUARD_ERRORS_HPP

#include <string>

/*** Error helper functions ***/
void error(const std::string&);
void error(const std::string&, const std::string&);
void error(const std::string&, int);


// Implementation File
#include "Errors.tpp"

#endif

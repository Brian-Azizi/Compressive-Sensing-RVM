#ifndef GUARD_ERRORS_HPP
#define GUARD_ERRORS_HPP

#include <string>

/*** Error helper functions ***/
/* Throw a std:runtime_exception. If arguments are passed, we use them to form an error message*/
void error(); 
void error(const std::string&);
void error(const std::string&, const std::string&);
void error(const std::string&, int);

/* Exits the program with a FAILURE statue */
void exitWithError(const std::string& error);

// Implementation File
#include "Errors.tpp"

#endif

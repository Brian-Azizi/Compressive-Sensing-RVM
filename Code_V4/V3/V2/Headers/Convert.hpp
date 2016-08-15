#ifndef GUARD_CONVERT_HPP
#define GUARD_CONVERT_HPP

#include <sstream>
#include <typeinfo>

#include "Errors.hpp"

class Convert {
public:
    // Convert T which should be a primitive, to a std::string
    template <typename T>
    static std::string T_to_string(const T& val)
    {
	std::ostringstream ostr;
	ostr << val;

	return ostr.str();
    }

    // Convert a std::string to T
    template <typename T>
    static T string_to_T(std::string const &val)
    {
	std::istringstream istr(val);
	T returnVal;
	if (!(istr >> returnVal))
	    error("CFG: Not a valid " + (std::string)typeid(T).name() + " received!");
		
	return returnVal;
    }
};


// specialize for strings
template<>
std::string Convert::string_to_T(std::string const &val)
{
    return val;
}

#endif

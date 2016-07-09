#ifndef GUARD_SIGNALBASIS_TPP
#define GUARD_SIGNALBASIS_TPP

#include <string>

#include "Errors.hpp"


// static data
const std::string SignalBasis::modeString[] = {"dct", "haar"};

// constructers
SignalBasis::SignalBasis(const std::string& m, int s)
    : m_scale(s), m_setting(strToBasisMode(m))
{
    check();
}

// print member data
std::string SignalBasis::settingString() const {
    return modeToString(m_setting); 
}

// set member data
void SignalBasis::setScale(int scale) 
{
    m_scale = scale;
    check();
}

void SignalBasis::setMode(mode m)
{
    m_setting = m;
    check();
}

void SignalBasis::setMode(const std::string& m)
{
    m_setting = strToBasisMode(m);
    check();
}

// check validity
void SignalBasis::check() const
{
    if (m_scale < 1) error("Scale needs to be a positive integer");
    if (m_setting == dct && m_scale != 1) error("Scale must equal 1 for DCT mode!");
}


// helper functions
std::string modeToString(const SignalBasis::mode& setting) 
{
    return SignalBasis::modeString[setting];
}

SignalBasis::mode strToBasisMode(const std::string& ss) 
{
    int idx;
    for (idx = 0; idx < SignalBasis::numModes; ++idx)
	if (ss == SignalBasis::modeString[idx] || ss == ("SignalBasis::" + SignalBasis::modeString[idx])) 
	    return SignalBasis::mode(idx);

    error("invalid argument ", ss);
}

std::ostream& operator<<(std::ostream& os, const SignalBasis& bas)
{
    os << "Signal Basis Mode:\t " << bas.settingString() << std::endl;
    
    if (bas.setting() != SignalBasis::dct)
	os << "Scale:\t\t\t " << bas.scale() << "\n" << std::flush;

    return os;
}


#endif

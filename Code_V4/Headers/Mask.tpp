#ifndef GUARD_MASK_TPP
#define GUARD_MASK_TPP

#include <ostream>
#include <string>

#include "Errors.hpp"

// static data
const std::string Mask::modeString[] = {"uniform", "timeRays", "verticalFlicker", "horizontalFlicker", "missingFrames", "verticalLines", "horizontalLines"};

// Constructers
Mask::Mask(std::string s, double p) 
    : m_perc(p), m_setting(strToMaskMode(s))
{
    check();
}

Mask::Mask(double p, std::string s)    
    : m_perc(p), m_setting(strToMaskMode(s))
{
    check();
}

// print member data
std::string Mask::settingString() const 
{ 
    return modeToString(m_setting);
}

// set member data
void Mask::setPercentage(double perc) 
{
    if (perc < 0 || perc > 100) error("percentage must be in range [0,100]");
    m_perc = perc;
}

void Mask::setMode(const std::string& ss) 
{
    Mask::mode m = strToMaskMode(ss);
    m_setting = m;
}

// check validity of member data
void Mask::check() const
{
    if (m_perc < 0 || m_perc > 100) error("Percentage must be in the range [0, 100]");
}


// Helper functions

std::string modeToString(const Mask::mode& setting) 
{
    return Mask::modeString[setting];
}

Mask::mode strToMaskMode(const std::string& ss) 
{
    int idx;
    for (idx = 0; idx < Mask::numModes; ++idx)
	if (ss == Mask::modeString[idx] || ss == ("Mask::" + Mask::modeString[idx])) 
	    return Mask::mode(idx);
    
    error("invalid argument ", ss);
}

std::ostream& operator<<(std::ostream& os, const Mask& mask)
{
    os << "Mask Mode:\t" << mask.settingString()
       << "\nMask Cover Percentage:\t" << mask.percentage() << "%\n" << std::flush;
    return os;
}


#endif

#ifndef GUARD_CORRUPTER_TPP
#define GUARD_CORRUPTER_TPP

#include <ostream>
#include <string>

#include "Errors.hpp"

// static data
const std::string Corrupter::modeString[] = {"uniform", "timeRays", "verticalFlicker", "horizontalFlicker", "missingFrames", "verticalLines", "horizontalLines"};

// Constructers
Corrupter::Corrupter(std::string s, double p) 
    : m_perc(p), m_setting(strToCorrMode(s))
{
    check();
}

Corrupter::Corrupter(double p, std::string s)    
    : m_perc(p), m_setting(strToCorrMode(s))
{
    check();
}

// print member data
std::string Corrupter::settingString() const 
{ 
    return modeToString(m_setting);
}

// set member data
void Corrupter::setPercentage(double perc) 
{
    if (perc < 0 || perc > 100) error("percentage must be in range [0,100]");
    m_perc = perc;
}

void Corrupter::setMode(const std::string& ss) 
{
    Corrupter::mode m = strToCorrMode(ss);
    m_setting = m;
}

// check validity of member data
void Corrupter::check() const
{
    if (m_perc < 0 || m_perc > 100) error("Percentage must be in the range [0, 100]");
}


// Helper functions

std::string modeToString(const Corrupter::mode& setting) 
{
    return Corrupter::modeString[setting];
}

Corrupter::mode strToCorrMode(const std::string& ss) 
{
    int idx;
    for (idx = 0; idx < Corrupter::numModes; ++idx)
	if (ss == Corrupter::modeString[idx] || ss == ("Corrupter::" + Corrupter::modeString[idx])) 
	    return Corrupter::mode(idx);
    
    error("invalid argument ", ss);
}

std::ostream& operator<<(std::ostream& os, const Corrupter& corr)
{
    os << "Corruption Mode:\t" << corr.settingString()
       << "\nCorruption Percentage:\t" << corr.percentage() << "%\n" << std::flush;
    return os;
}

#endif

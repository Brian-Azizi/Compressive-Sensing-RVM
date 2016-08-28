#ifndef GUARD_SENSOR_TPP
#define GUARD_SENSOR_TPP

#include <ostream>
#include <string>

#include "Errors.hpp"

// static data
const std::string Sensor::modeString[] = {"mask", "gaussian", "bernoulli"};


// Constructers
Sensor::Sensor(std::string s, double p)
    : m_perc(p), m_setting(strToSensorMode(s)), m_mask(Mask(p,Mask::uniform))
{
    check();
}

Sensor::Sensor(double p, std::string s)
    : m_perc(p), m_setting(strToSensorMode(s)),  m_mask(Mask(p,Mask::uniform))
{
    check();
}

// print member data
std::string Sensor::settingString() const
{
    return modeToString(m_setting);
}


// set member data
void Sensor::setPercentage(double perc)
{
    if (perc < 0 || perc > 100) error("SENSOR: Percentage must be in range [0,100]");
    m_perc = perc;
    m_mask.setPercentage(perc);
}

void Sensor::setMode(const std::string& ss)
{
    Sensor::mode m = strToSensorMode(ss);
    m_setting = m;
}


// check validity
void Sensor::check() const
{
    if (m_perc < 0 || m_perc > 100) error("SENSOR: Percentage must be in the range [0, 100]");
}


// Helper functions

std::string modeToString(const Sensor::mode& setting) 
{
    return Sensor::modeString[setting];
}

Sensor::mode strToSensorMode(const std::string& ss) 
{
    int idx;
    for (idx = 0; idx < Sensor::numModes; ++idx)
	if (ss == Sensor::modeString[idx] || ss == ("Sensor::" + Sensor::modeString[idx])) 
	    return Sensor::mode(idx);
    
    error("SENSOR: invalid argument ", ss);
}

std::ostream& operator<<(std::ostream& os, const Sensor& sensor)
{
    os << "Sensor Mode:\t\t" << sensor.settingString()
       << "\nSensor Percentage:\t" << sensor.percentage() << "%\n" << std::flush;
    return os;
}

#endif

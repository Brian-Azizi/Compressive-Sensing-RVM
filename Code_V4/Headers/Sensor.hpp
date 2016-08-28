#ifndef GUARD_SENSOR_HPP
#define GUARD_SENSOR_HPP

#include <ostream>
#include <string>

#include "Mask.hpp"
#include "Signal.hpp"

/* A helper class for storing information about the sensing matrix.*/
class Sensor{
public:
    enum mode {mask, gaussian, bernoulli};

    // Constructers
    Sensor() : m_setting(mask), m_perc(0), m_mask(Mask()) { check(); }
    Sensor(double p, mode m) : m_perc(p), m_setting(m), m_mask(Mask(p,Mask::uniform)) { check(); }
    Sensor(mode m, double p) : m_perc(p), m_setting(m), m_mask(Mask(p,Mask::uniform)) { check(); }
    Sensor(std::string s, double p);
    Sensor(double p, std::string s);
    
    // print member data
    double percentage() const { return m_perc; }
    mode setting() const { return m_setting; }
    std::string settingString() const;

   // set Member data
    void setPercentage(double perc);
    void setMode(mode setting) { m_setting = setting; }
    void setMode(const std::string& settingString);

    // static data
    static const std::string modeString[];
    static const int numModes = 3;

    // check if member data is valid
    void check() const;

private:
    mode m_setting;
    double m_perc;
    Mask m_mask;
};

// helper functions
std::string modeToString(const Sensor::mode& setting);
Sensor::mode strToSensorMode(const std::string& ss);
std::ostream& operator<<(std::ostream&, const Sensor&);

// implementation file
#include "Sensor.tpp"

#endif

#ifndef GUARD_CORRUPTER_HPP
#define GUARD_CORRUPTER_HPP

#include <ostream>
#include <string>

// simple class for holding settings for Signal masks
class Corrupter{
public:
    enum mode {uniform, timeRays, verticalFlicker, horizontalFlicker, missingFrames, verticalLines, horizontalLines};

    // Constructers
    Corrupter() : m_perc(0), m_setting(uniform) { check(); }
    Corrupter(double p, mode m) : m_perc(p), m_setting(m) { check(); }
    Corrupter(mode m, double p) : m_perc(p), m_setting(m) { check(); }
    Corrupter(std::string s, double p);
    Corrupter(double p, std::string s);

    // print member data
    double percentage() const { return m_perc; }
    mode setting() const { return m_setting; }
    std::string settingString() const;

    // set member data
    void setPercentage(double perc);
    void setMode(mode setting) { m_setting = setting; }
    void setMode(const std::string& settingString);

    // static data
    static const std::string modeString[]; // defined in implementation file
    static const int numModes = 7;

    // check if member data is valid
    void check() const;

private:
    mode m_setting;
    double m_perc;
};

// helper functions
std::string modeToString(const Corrupter::mode& setting);
Corrupter::mode strToCorrMode(const std::string& ss);
std::ostream& operator<<(std::ostream&, const Corrupter&);


// implementation file
#include "Corrupter.tpp"

#endif

#ifndef GUARD_MASK_HPP
#define GUARD_MASK_HPP

#include <ostream>
#include <string>

// simple class for holding settings for Signal masks
class Mask{
public:
    enum mode {uniform, timeRays, verticalFlicker, horizontalFlicker, missingFrames, verticalLines, horizontalLines};

    // Constructers
    Mask() : m_perc(0), m_setting(uniform) { check(); }
    Mask(double p, mode m) : m_perc(p), m_setting(m) { check(); }
    Mask(mode m, double p) : m_perc(p), m_setting(m) { check(); }
    Mask(std::string s, double p);
    Mask(double p, std::string s);

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
std::string modeToString(const Mask::mode& setting);
Mask::mode strToMaskMode(const std::string& ss);
std::ostream& operator<<(std::ostream&, const Mask&);


// implementation file
#include "Mask.tpp"

#endif

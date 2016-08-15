#ifndef GUARD_SIGNALBASIS_HPP
#define GUARD_SIGNALBASIS_HPP

#include <string>
//#include "Signal.hpp"

/*** simple class for basis function Mode ***/
class SignalBasis{
public:
    enum mode {dct, haar};

    // Constructers    
    SignalBasis() : m_scale(1), m_setting(dct) { check(); }
    SignalBasis(mode m, int scale = 1) : m_scale(scale), m_setting(m) { check(); }
    SignalBasis(const std::string& m, int s = 1);

    // print member data
    mode setting() const { return m_setting; }
    int scale() const { return m_scale; }
    std::string settingString() const;
    
    // set member data
    void setScale(int scale);
    void setMode(mode setting);
    void setMode(const std::string& settingString);
    
    // static data
    static const std::string modeString[];
    static const int numModes = 2;
    
    // check validity of data
    void check() const;

private:
    mode m_setting;		
    int m_scale;		// scale must equal 1 for dct
};


// helper functions
std::string modeToString(const SignalBasis::mode& setting);
SignalBasis::mode strToBasisMode(const std::string& ss);
std::ostream& operator<<(std::ostream&, const SignalBasis&);


// implementation file
#include "SignalBasis.tpp"

#endif

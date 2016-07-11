#ifndef GUARD_SIGNALSETTINGS_HPP
#define GUARD_SIGNALSETTINGS_HPP

#include <cmath>
#include <ostream>
#include <string>

#include "ConfigFile.hpp"
#include "Corrupter.hpp"
#include "Dim.hpp"
#include "SignalBasis.hpp"

struct SignalSettings {
    std::string inputFile;    
    int rngSeed;

    Corrupter corr;
    SignalBasis sigBasis;
    Dim blockDim;
    Dim signalDim;
    bool printProgress;
    double StdDev;
    double deltaML_threshold;

    SignalSettings(const std::string& settingsFile);
    ConfigFile configFile() const { return cfg; }
    void check();
private:
    ConfigFile cfg;
    void initParameters();
};

std::ostream& operator<<(std::ostream& os, const SignalSettings& cfg);


#include "SignalSettings.tpp"

#endif

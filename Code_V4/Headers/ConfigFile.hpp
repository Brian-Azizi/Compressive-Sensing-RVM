#ifndef GUARD_CONFIGFILE_HPP
#define GUARD_CONFIGFILE_HPP

/* Class for handling configuration files. Based on "http://www.dreamincode.net/forums/topic/183191-create-a-simple-configuration-file-parser/" */

#include <string>
#include <map>
#include <ostream>

class ConfigFile
{
private:
    std::map<std::string, std::string> m_contents;
    std::string fName;

    void removeComments(std::string& line) const;
    bool onlyWhitespace(const std::string& line) const;
    bool validLine(const std::string& line) const;
    std::string extractKey(const std::string& line, size_t const& sepPos) const;
    std::string extractValue(const std::string& line, size_t const& sepPos) const;
    void extractContents(const std::string& line);
    void parseLine(const std::string& line, const size_t lineNo);
    void ExtractKeys();
public:
    ConfigFile(const std::string& fName);
    ConfigFile(const ConfigFile& cfg);
    bool keyExists(const std::string& key) const;
    template<typename T> T getValueOfKey(const std::string& key, const T& defaultValue = T(), bool printWarning = false);

    std::string file() const { return fName;}
    std::map<std::string, std::string> contents() const { return m_contents; }
};

std::ostream& operator<<(std::ostream& os, const ConfigFile& cfg);

// implementation file
#include "ConfigFile.tpp"


#endif

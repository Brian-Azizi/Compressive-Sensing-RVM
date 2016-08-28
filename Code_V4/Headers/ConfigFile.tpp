#ifndef GUARD_CONFIGFILE_TPP
#define GUARD_CONFIGFILE_TPP

/* Class for handling configuration files. Based on "http://www.dreamincode.net/forums/topic/183191-create-a-simple-configuration-file-parser/" */

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <fstream>

#include "Convert.hpp"

void ConfigFile::removeComments(std::string& line) const
{
    if(line.find('#') != line.npos)
	line.erase(line.find('#'));
}
bool ConfigFile::onlyWhitespace(const std::string& line) const
{
    return (line.find_first_not_of(' ')) == line.npos;
}
bool ConfigFile::validLine(const std::string& line) const
{
    std::string temp = line;
    temp.erase(0, temp.find_first_not_of("\t "));
    if (temp[0] == '=')
	return false;

    for (size_t i = temp.find('=') + 1; i < temp.length(); i++)
	if (temp[i] != ' ')
	    return true;

    return false;
}

// assumes leading whitespace before line has already been removed
std::string ConfigFile::extractKey(const std::string& line, const size_t& sepPos) const
{
    std::string key = line.substr(0, sepPos);
    if (key.find('\t') != line.npos || key.find(' ') != line.npos)
	key.erase(key.find_first_of("\t "));
    
    return key;
}
	
// sepPos = position of '=' 
std::string ConfigFile::extractValue(const std::string& line, const size_t& sepPos) const
{
    std::string value = line.substr(sepPos+1);
    value.erase(0, value.find_first_not_of("\t ")); // erase leading whitespace
    value.erase(value.find_last_not_of("\t ") + 1); // erase trailing whitespace
    
    return value;
}
void ConfigFile::extractContents(const std::string& line)
{
    std::string temp = line;
    temp.erase(0, temp.find_first_not_of("\t ")); // erase leading whitespace
    size_t sepPos = temp.find('=');

    std::string key = extractKey(temp, sepPos);
    std::string value = extractValue(temp, sepPos);

    if (!keyExists(key))
	m_contents.insert(std::pair<std::string, std::string>(key, value));
    else
	error("CONFIG: Can only have unique key names: ", key);
}
void ConfigFile::parseLine(const std::string& line, const size_t lineNo)
{
    if (line.find('=') == line.npos)
	error("CONFIG: couldn't find separator on line", lineNo);
    
    if (!validLine(line))
	error("CONFIG: Bad format for line", lineNo);

    extractContents(line);
}

void ConfigFile::ExtractKeys()
{
    std::ifstream file;
    file.open(fName.c_str());
    if (!file)
	error("CONFIG: File " + fName + " couldn't be found");

    std::string line; 
    size_t lineNo = 0;
    while(std::getline(file, line)) {
	lineNo++;
	std::string temp = line;
	
	if (temp.empty())
	    continue;

	removeComments(temp);
	if (onlyWhitespace(temp))
	    continue;

	parseLine(temp, lineNo);
    }    
    file.close();
}

ConfigFile::ConfigFile(const std::string& fileName)
    : fName(fileName)
{
    ExtractKeys();
}
ConfigFile::ConfigFile(const ConfigFile& cfg)
    : m_contents(cfg.contents()), fName(cfg.file())
{
}

bool ConfigFile::keyExists(const std::string& key) const
{
    return m_contents.find(key) != m_contents.end();
}

template<typename T> T ConfigFile::getValueOfKey(const std::string& key, const T& defaultValue, bool printWarning)
{
    if (!keyExists(key)) {
	std::string val = Convert::T_to_string(defaultValue);
	if (printWarning)
	    std::cerr << "CFG Warning: key '" + key + "' not specified in " + fName + ". Using default: '" + val + "'\n";
	return defaultValue;
    }

    return Convert::string_to_T<T>(m_contents.find(key)->second);
}

std::ostream& operator<<(std::ostream& os, const ConfigFile& cfg)
{
    typedef std::map<std::string, std::string>::iterator map_iter;
    std::map<std::string, std::string> contents = cfg.contents();

    for(map_iter it = contents.begin(); it != contents.end(); ++it)
	os << it->first << ":\t\t" << it->second << "\n";

    return os;
}

#endif

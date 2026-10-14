#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

class ArgumentParser {
public:
    struct Argument {
        std::string name;
        char shortOpt;
        std::string longOpt;
        std::string description;
        std::string defaultValue;
        bool required;
        bool hasValue;
        std::string value;
        
        Argument() : shortOpt(0), required(false), hasValue(false) {}
        Argument(const std::string& n, char shortO, const std::string& longO, 
                 const std::string& desc, const std::string& def = "", bool req = false)
            : name(n), shortOpt(shortO), longOpt(longO), description(desc), 
              defaultValue(def), required(req), hasValue(false) {}
    };
    
    ArgumentParser(const std::string& programName, const std::string& description)
        : programName_(programName), description_(description) {}
    
    // Add argument
    void addArgument(const std::string& name, char shortOpt, const std::string& longOpt,
                     const std::string& description, const std::string& defaultValue = "",
                     bool required = false, bool hasValue = true);
    
    // Parse arguments
    bool parse(int argc, char** argv, std::vector<std::string>& positionalArgs);
    
    // Get argument value
    std::string getValue(const std::string& name) const;
    bool hasValue(const std::string& name) const;
    int getValueInt(const std::string& name, int defaultValue = 0) const;
    double getValueDouble(const std::string& name, double defaultValue = 0.0) const;
    bool getValueBool(const std::string& name, bool defaultValue = false) const;
    
    // Check if argument was provided
    bool wasProvided(const std::string& name) const;
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const { return positionalArgs_; }
    
    // Help and version
    void printHelp() const;
    void printVersion(const std::string& version) const;
    
    // Validation
    bool validateRequired() const;
    
private:
    std::string programName_;
    std::string description_;
    std::vector<Argument> arguments_;
    std::vector<std::string> positionalArgs_;
    
    // Internal parsing helpers
    bool parseLongOption(const std::string& arg, size_t& i, int argc, char** argv);
    bool parseShortOption(const std::string& arg, size_t& i, int argc, char** argv);
    bool parseValue(const std::string& arg, size_t& i, int argc, char** argv);
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    char findShortOpt(const std::string& longOpt) const;
};

} // namespace nlm
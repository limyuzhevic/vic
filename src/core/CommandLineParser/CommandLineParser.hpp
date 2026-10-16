#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

class CommandLineParser {
public:
    struct Option {
        std::string name;
        std::string shortName;
        std::string description;
        bool required;
        bool hasValue;
        std::string defaultValue;
        std::string type;  
        std::string example;
        
        Option(const std::string& n, const std::string& desc, bool req = false,
               bool val = false, const std::string& def = "", 
               const std::string& t = "", const std::string& ex = "")
            : name(n), shortName(""), description(desc), required(req),
              hasValue(val), defaultValue(def), type(t), example(ex) {}
    
        Option(const std::string& n, const std::string& s, const std::string& desc,
               bool req = false, bool val = false, const std::string& def = "",
               const std::string& t = "", const std::string& ex = "")
            : name(n), shortName(s), description(desc), required(req),
              hasValue(val), defaultValue(def), type(t), example(ex) {}
    };

    CommandLineParser(const std::string& programName);
    
    // Add options
    void addOption(const std::string& name, const std::string& description, bool required = false, bool hasValue = false, const std::string& defaultValue = "", const std::string& type = "", const std::string& example = "");
    void addOption(const std::string& name, const std::string& shortName, const std::string& description, bool required = false, bool hasValue = false, const std::string& defaultValue = "", const std::string& type = "", const std::string& example = "");
    
    // Parse arguments
    bool parse(int argc, char** argv);
    
    // Get option values
    bool hasOption(const std::string& name) const;
    
    template<typename T>
    T getOption(const std::string& name, const T& defaultValue) const;
    
    std::string getOptionString(const std::string& name, const std::string& defaultValue = "") const;
    int getOptionInt(const std::string& name, int defaultValue = 0) const;
    double getOptionDouble(const std::string& name, double defaultValue = 0.0) const;
    bool getOptionBool(const std::string& name, bool defaultValue = false) const;
    
    // Get all unrecognized arguments
    std::vector<std::string> getRemainingArgs() const { return remainingArgs; }
    
    // Help/usage
    void printHelp() const;
    
    // Get program name
    const std::string& getProgramName() const { return programName; }
    
private:
    std::string programName;
    std::vector<Option> options;
    std::unordered_map<std::string, std::string> stringOptions;
    std::unordered_map<std::string, bool> boolOptions;
    std::vector<std::string> remainingArgs;
    
    bool parseValue(const std::string& valueStr, const Option& option);
    bool strToBool(const std::string& value) const;
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    
    template<typename T>
    T parseValueToType(const std::string& value) const;
};

} // namespace nlm

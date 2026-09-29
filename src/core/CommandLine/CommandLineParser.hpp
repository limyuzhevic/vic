#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Command-line argument parser for NLM applications
class CommandLineParser {
public:
    // Command help information
    struct CommandInfo {
        std::string name;
        std::string description;
        std::string usage;
        std::string defaultValue;
        bool hasValue;
        bool isHidden;
        
        CommandInfo() : name(), description(), usage(), defaultValue(), hasValue(false), isHidden(false) {}
        
        CommandInfo(const std::string& n, const std::string& desc, 
                    const std::string& usage = "",
                    const std::string& def = "", 
                    bool hasVal = false, bool hidden = false)
            : name(n), description(desc), usage(usage), defaultValue(def), 
              hasValue(hasVal), isHidden(hidden) {}
    };

    // Argument value (supports multiple types)
    struct ArgumentValue {
        std::string asString;
        bool isSet;
        
        ArgumentValue() : asString(), isSet(false) {}
    };

    CommandLineParser();
    ~CommandLineParser() = default;

    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Add a command-line option
    void addOption(const std::string& name, const std::string& description,
                   const std::string& usage = "",
                   const std::string& defaultValue = "", 
                   bool hasValue = false, bool isHidden = false);
    
    // Set option value (from command line)
    void set(const std::string& name, const std::string& value);
    
    // Get string value of an option
    std::string getString(const std::string& name, const std::string& defaultValue = "") const;
    
    // Get integer value of an option (with conversion)
    int getInt(const std::string& name, int defaultValue = 0) const;
    
    // Get boolean value of an option (with conversion)
    bool getBool(const std::string& name, bool defaultValue = false) const;
    
    // Check if option is set
    bool has(const std::string& name) const;
    
    // Get all option names
    std::vector<std::string> getOptionNames() const;
    
    // Get help text for all options
    std::string getHelp() const;
    
    // Get positional arguments
    std::vector<std::string> getPositionalArgs() const;
    
    // Add a positional argument
    void addPositionalArg(const std::string& name, const std::string& description);
    
    // Print usage information
    void printUsage(const std::string& programName) const;
    
    // Print version information
    static void printVersion(const std::string& version = "NLM Phase 6");

private:
    std::unordered_map<std::string, CommandInfo> options_;
    std::unordered_map<std::string, ArgumentValue> values_;
    std::vector<std::string> positionalArgs_;
    std::vector<std::string> positionalArgNames_;
    std::string usageLine_;
    
    // Parse a single argument
    bool parseArg(const std::string& arg);
    
    // Parse value string to appropriate type
    static bool parseValue(const std::string& value, ArgumentValue& target);
    
    // Get option name with dashes converted to underscores
    static std::string normalizeName(const std::string& name);
};

} // namespace nlm
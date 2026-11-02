// NLM (熙然) - Neural Learning Machine
// CLI Argument Parser
//
// This class provides comprehensive command line argument parsing
// for the NLM application, allowing users to specify experiments,
// configurations, and runtime parameters.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

namespace nlm {

// Command line argument types
enum class ArgType {
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    CHOICE
};

// Command line argument definition
struct ArgumentDef {
    std::string name;
    std::string shortName;
    std::string description;
    ArgType type;
    std::string defaultValue;
    std::vector<std::string> choices;
    bool required;
    std::function<void(const std::string&)> callback;
};

// CLI argument parser class
class CLIParser {
public:
    CLIParser(const std::string& programName);
    ~CLIParser() = default;
    
    // Add an argument
    void addArgument(const std::string& name, const std::string& shortName = "",
                     const std::string& description = "", const std::string& defaultValue = "",
                     ArgType type = ArgType::STRING, bool required = false,
                     const std::vector<std::string>& choices = {});
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get argument value
    template<typename T>
    T getValue(const std::string& name, const T& defaultValue = T()) const;
    
    // Get argument value as string
    std::string getStringValue(const std::string& name, const std::string& defaultValue = "") const;
    
    // Check if argument was specified
    bool hasArgument(const std::string& name) const;
    
    // Get all argument names
    std::vector<std::string> getArgumentNames() const;
    
    // Print help
    void printHelp() const;
    
    // Print version
    void printVersion() const;
    
    // Get program name
    const std::string& getProgramName() const { return programName; }
    
    // Set verbose mode
    void setVerbose(bool verbose) { this->verbose = verbose; }
    bool isVerbose() const { return verbose; }
    
private:
    std::string programName;
    std::vector<ArgumentDef> arguments;
    std::map<std::string, std::string> parsedValues;
    bool verbose;
    
    // Parse arguments
    bool parseArguments(int argc, char** argv);
    
    // Handle help and version
    bool handleHelpAndVersion(int argc, char** argv);
    
    // Validate arguments
    bool validateArguments() const;
    
    // Convert string to target type
    template<typename T>
    T stringToValue(const std::string& str) const;
};

} // namespace nlm
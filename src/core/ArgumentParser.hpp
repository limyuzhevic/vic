// Enhanced Argument Parser for NLM Phase 2
// Provides comprehensive command-line argument handling with validation and configuration support

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <cctype>

// Forward declarations
namespace nlm {
    class Config;
}

// Argument parsing formats
enum class ArgumentFormat {
    Flag,           // --flag
    Equals,         // --flag=value
    Space,          // --flag value
    ShortFlag,      // -f
    ShortEquals,    // -f=value
    ShortSpace      // -f value
};

// Test execution modes
enum class TestMode {
    Fast,       // Quick execution with minimal logging
    Normal,     // Standard execution with balanced logging
    Detailed,   // Detailed execution with extensive logging
    Debug       // Debug execution with maximum logging
};

// Argument configuration
struct ArgumentConfig {
    std::string name;              // Argument name (e.g., "test", "timestep")
    char shortName;                // Short name (e.g., 't', 'v')
    std::string description;       // Human-readable description
    std::string type;              // Argument type (string, int, double, bool, list)
    std::string defaultValue;      // Default value
    bool required;                 // Whether argument is required
    bool validated;                // Whether argument has been validated
    std::vector<std::string> validValues;  // Valid values for enum-like arguments
    std::string validationError;   // Validation error message
    
    ArgumentConfig(const std::string& n, char s, const std::string& desc, 
                   const std::string& t, const std::string& def = "", bool req = false)
        : name(n), shortName(s), description(desc), type(t), defaultValue(def),
          required(req), validated(false), validationError("") {}
};

// Parsed command line arguments
struct ParsedArgs {
    std::map<std::string, std::string> stringArgs;
    std::map<std::string, int> intArgs;
    std::map<std::string, double> doubleArgs;
    std::map<std::string, bool> boolArgs;
    std::map<std::string, std::vector<std::string>> listArgs;
    std::vector<std::string> positionalArgs;
    std::map<std::string, std::string> originalFormats;
    
    void clear() {
        stringArgs.clear();
        intArgs.clear();
        doubleArgs.clear();
        boolArgs.clear();
        listArgs.clear();
        positionalArgs.clear();
        originalFormats.clear();
    }
};

class ArgumentParser {
public:
    ArgumentParser();
    ~ArgumentParser() = default;
    
    // Configure argument
    void addArgument(const std::string& name, char shortName, 
                    const std::string& description, const std::string& type,
                    const std::string& defaultValue = "", bool required = false,
                    const std::vector<std::string>& validValues = {});
    
    // Parse command line arguments
    bool parse(int argc, char** argv, ParsedArgs& parsedArgs, 
               const std::string& configFile = "");
    
    // Get help text
    std::string getHelpText() const;
    
    // Get version information
    std::string getVersion() const { return "NLM Phase 2 Enhanced v2.0.0"; }
    
    // Validate parsed arguments
    bool validate(const ParsedArgs& parsedArgs, std::string& errorMessage) const;
    
    // Get test mode
    TestMode getTestMode() const { return testMode_; }
    
    // Get logging level
    std::string getLogLevel() const { return logLevel_; }
    
    // Check if verbose mode
    bool isVerbose() const { return verbose_; }
    
    // Get configuration file path
    std::string getConfigFile() const { return configFile_; }
    
    // Get test list
    std::vector<std::string> getTests() const { return testNames_; }
    
    // Get timestep
    double getTimestep() const { return timestep_; }
    
    // Get random seed
    uint64_t getSeed() const { return seed_; }
    
    // Check if reset between tests
    bool resetBetweenTests() const { return resetBetweenTests_; }
    
    // Check if persist state
    bool persistState() const { return persistState_; }
    
    // Check if save config
    bool saveConfig() const { return saveConfig_; }
    
    // Check if load state
    bool loadState() const { return loadState_; }
    
    // Check if logging to file
    bool logToFile() const { return logToFile_; }
    
    // Get log file name
    std::string getLogFileName() const { return logFileName_; }
    
    // Get simulation steps
    int getSimulationSteps() const { return simulationSteps_; }
    
    // Get energy decay rate
    float getEnergyDecayRate() const { return energyDecayRate_; }
    
    // Get max energy
    float getMaxEnergy() const { return maxEnergy_; }
    
    // Get environment dimensions
    std::pair<int, int> getEnvironmentDimensions() const { return environmentDimensions_; }
    
    // Check if visualization enabled
    bool visualizationEnabled() const { return visualizationEnabled_; }
    
    // Get visualization update rate
    float getVisualizationUpdateRate() const { return visualizationUpdateRate_; }
    
    // Get random seed for world
    uint64_t getWorldRandomSeed() const { return worldRandomSeed_; }
    
private:
    std::vector<ArgumentConfig> argumentConfigs_;
    std::map<std::string, int> shortNameToArgIndex_;
    
    // Parsed values
    TestMode testMode_;
    std::string logLevel_;
    bool verbose_;
    std::string configFile_;
    std::vector<std::string> testNames_;
    double timestep_;
    uint64_t seed_;
    bool resetBetweenTests_;
    bool persistState_;
    bool saveConfig_;
    bool loadState_;
    bool logToFile_;
    std::string logFileName_;
    int simulationSteps_;
    float energyDecayRate_;
    float maxEnergy_;
    std::pair<int, int> environmentDimensions_;
    bool visualizationEnabled_;
    float visualizationUpdateRate_;
    uint64_t worldRandomSeed_;
    
    // Internal parsing methods
    ArgumentFormat detectArgumentFormat(const std::string& arg) const;
    void parseEqualsFormat(const std::string& arg, ParsedArgs& parsedArgs);
    void parseSpaceFormat(const std::string& arg, const std::string& nextArg, 
                         ParsedArgs& parsedArgs, int& i, int argc, char** argv);
    void parseShortFormat(const std::string& arg, ParsedArgs& parsedArgs);
    std::string trim(const std::string& str) const;
    bool toLower(const std::string& str) const;
    std::string intToString(int value) const;
    std::string doubleToString(double value) const;
    bool stringToBool(const std::string& str) const;
    std::vector<std::string> stringToList(const std::string& str) const;
    std::string getValueFromArg(const std::string& arg, const std::string& expectedKey) const;
    bool isShortOption(const std::string& arg) const;
    bool isLongOption(const std::string& arg) const;
    bool isPositionalArg(const std::string& arg) const;
    std::string getCanonicalKey(const std::string& arg) const;
    std::string formatHelpKey(const ArgumentConfig& config) const;
};

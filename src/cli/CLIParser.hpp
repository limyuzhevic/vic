#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <optional>
#include <chrono>
#include <thread>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <set>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Command line arguments structure
struct CommandLineArgs {
    // Test selection
    std::set<std::string> testTypes;
    bool runAllTests;
    
    // Verbosity
    int verbosityLevel;  // -2 quiet, -1 verbose, 0 normal, 1 detailed, 2 debug
    
    // Output control
    bool saveResults;
    std::string resultsFile;
    bool enablePlotting;
    std::string plotFile;
    
    // Timing control
    std::chrono::seconds maxExecutionTime;
    double stepDelay;
    
    // Test-specific parameters
    std::map<std::string, std::map<std::string, std::string>> testParameters;
    
    // Configuration
    std::string configFile;
    
    // General options
    bool helpRequested;
    bool versionRequested;
    bool quietMode;
    bool progressRequested;
    bool noProgress;
    
    // File operations
    std::string inputFile;
    std::string outputFile;
    
    // Configuration preset
    std::string presetName;
    
    CommandLineArgs() 
        : runAllTests(false), verbosityLevel(0), saveResults(false), 
          enablePlotting(false), maxExecutionTime(std::chrono::seconds(3600)),
          stepDelay(0.001), helpRequested(false), versionRequested(false),
          quietMode(false), progressRequested(false), noProgress(false) {}
};

// CLI option definition
struct CLIOption {
    std::string longName;
    char shortName;
    std::string description;
    std::string valueType;
    std::string defaultValue;
    std::function<void(const std::string&, CommandLineArgs&)> handler;
    std::function<bool(const CommandLineArgs&)> validator;
};

// Help information
struct HelpInfo {
    std::string programName;
    std::string version;
    std::string description;
    std::vector<std::string> sections;
    std::map<std::string, std::vector<CLIOption>> optionsBySection;
};

// Main CLI parser class
class CLIParser {
public:
    CLIParser(std::shared_ptr<Config> config, std::shared_ptr<Logger> logger);
    ~CLIParser();
    
    // Parse command line arguments
    bool parse(int argc, char** argv, CommandLineArgs& args);
    
    // Get help information
    HelpInfo generateHelp() const;
    void displayHelp() const;
    
    // Configuration integration
    void setConfig(std::shared_ptr<Config> config);
    std::shared_ptr<Config> getConfig() const;
    
    // Error handling
    std::string getLastError() const;
    bool hasError() const;
    
    // Validation
    bool validateArgs(const CommandLineArgs& args) const;
    
    // Statistics
    size_t getTotalArgumentsProcessed() const;
    size_t getSuccessfulParses() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal parsing methods
    bool parseArguments(int argc, char** argv, CommandLineArgs& args);
    bool parseOption(const std::string& arg, CommandLineArgs& args);
    bool parseTestSelection(const std::string& arg, CommandLineArgs& args);
    bool parseTestParameter(const std::string& arg, CommandLineArgs& args);
    bool parseVerbosityOption(const std::string& arg, CommandLineArgs& args);
    bool parseOutputOption(const std::string& arg, CommandLineArgs& args);
    bool parseTimingOption(const std::string& arg, CommandLineArgs& args);
    bool parseConfigOption(const std::string& arg, CommandLineArgs& args);
    bool parsePositionalArgument(const std::string& arg, CommandLineArgs& args);
    
    // Option registration
    void registerAllOptions();
    void registerBasicOptions();
    void registerTestOptions();
    void registerOutputOptions();
    void registerTimingOptions();
    void registerVerbosityOptions();
    
    // Option lookup
    const CLIOption* findOption(const std::string& name) const;
    
    // Help generation
    void generateBasicHelp(HelpInfo& help) const;
    void generateTestHelp(HelpInfo& help) const;
    void generateOutputHelp(HelpInfo& help) const;
    void generateTimingHelp(HelpInfo& help) const;
    void generateVerbosityHelp(HelpInfo& help) const;
    
    // Error handling
    void setError(const std::string& error);
    
    // Statistics
    void incrementSuccessfulParse();
    void incrementArgumentProcessed();
};

// Utility functions
std::string trim(const std::string& str);
std::string toLower(const std::string& str);
std::string toUpper(const std::string& str);
bool stringToBool(const std::string& str);
double stringToDouble(const std::string& str);
int stringToInt(const std::string& str);

} // namespace nlm

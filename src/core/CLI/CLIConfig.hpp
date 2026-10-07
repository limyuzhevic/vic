// NLM (熙然) - Neural Learning Machine
// CLI Parser Implementation for Phase 2

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Test modes for CLI
enum class TestMode {
    Connectivity,
    Plasticity,
    STDP,
    All,
    Quick,
    None
};

// CLI configuration structure
struct CLIConfig {
    std::string configFile;
    TestMode testMode;
    std::string testModeString;
    bool verbose;
    bool quiet;
    std::string logLevel;
    std::string outputFormat;
    std::vector<std::string> runTests;
    std::vector<std::string> skipTests;
    int maxSteps;
    double timestep;
    bool showHelp;
    bool showUsage;
    bool showExamples;
    
    // Validation and parsing methods
    bool validate() const;
    LogLevel getLogLevel() const;
    std::string toString() const;
};

// CLI parser class
class CLIParser {
public:
    CLIParser(int argc, char** argv);
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed configuration
    const CLIConfig& getConfig() const { return config; }
    
    // Display help information
    static void showHelp();
    static void showUsage();
    static void showExamples();
    
    // Convert string to TestMode
    static TestMode stringToTestMode(const std::string& mode);
    
    // Convert TestMode to string
    static std::string testModeToString(TestMode mode);
    
    // Convert string to log level
    static LogLevel stringToLogLevel(const std::string& level);
    
    // Convert log level to string
    static std::string logLevelToString(LogLevel level);
    
    // Convert string to output format
    static std::string stringToOutputFormat(const std::string& format);
    
    // Check if a test should be run
    bool shouldRunTest(const std::string& testName) const;
    
    // Check if a test should be skipped
    bool shouldSkipTest(const std::string& testName) const;

private:
    int argc;
    char** argv;
    CLIConfig config;
    
    // Parse helper functions
    bool parseConfigFile(const std::string& arg);
    bool parseTestMode(const std::string& arg);
    bool parseVerbose(const std::string& arg);
    bool parseQuiet(const std::string& arg);
    bool parseLogLevel(const std::string& arg);
    bool parseOutputFormat(const std::string& arg);
    bool parseRunTests(const std::string& arg);
    bool parseSkipTests(const std::string& arg);
    bool parseSteps(const std::string& arg);
    bool parseTimestep(const std::string& arg);
    bool parseHelp(const std::string& arg);
    
    // Helper functions
    std::vector<std::string> splitString(const std::string& str, char delimiter) const;
    bool isFlag(const std::string& arg) const;
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
};

// Global function to parse CLI and setup logging
bool parseCLIAndSetupLogging(int argc, char** argv);

// Global function to display results in specified format
void displayResults(const std::string& outputFormat, const std::string& data);

} // namespace nlm

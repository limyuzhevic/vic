// NLM Main Application Interface
// Enhanced Command-Line Interface for Advanced Users
// 
// This module provides the interface for the enhanced command-line interface
// with advanced configuration options, testing capabilities, and state management.

#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations
class Brain;
class Config;
class Logger;

// Log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4
};

// Test mode enumeration
enum class TestMode {
    BasicConnectivity,
    PlasticityLearning,
    STDPVerification,
    Fast,
    Comprehensive,
    Custom
};

// Structure for CLI options
struct CLIOptions {
    std::string configFile;
    std::string outputFile;
    std::string logFile;
    LogLevel logLevel;
    bool enableColors;
    bool verbose;
    bool quiet;
    bool help;
    bool version;
    bool runAll;
    bool saveState;
    bool loadState;
    std::string testMode;
    double duration;
    size_t stepLimit;
    bool exportConfig;
    bool importConfig;
    std::string importFile;
    bool validateOnly;
    bool systemInfo;
    bool performanceStats;
    std::map<std::string, std::string> customArgs;
    
    CLIOptions();
};

// Function declarations for enhanced CLI
void printEnhancedBanner(const std::string& mode = "Phase 2");
void printHelp(const std::string& programName);
void printVersion();
void printConfigurationSummary(const Config& config);
void exportConfiguration(const Config& config, const std::string& filepath);
bool importConfiguration(Config& config, const std::string& filepath);
void setLogLevel(LogLevel level);
void enableColorOutput(bool enable);
void enableFileLogging(const std::string& filepath);
void printSystemInfo();
void runSingleTest(const std::shared_ptr<Brain>& brain, const std::string& testName);
void runAllTests(const std::shared_ptr<Brain>& brain);
void runCustomExperiment(const std::shared_ptr<Brain>& brain, const Config& config);
void saveBrainState(const std::shared_ptr<Brain>& brain, const std::string& filepath);
bool loadBrainState(std::shared_ptr<Brain>& brain, const std::string& filepath);
void printPerformanceStats(const std::shared_ptr<Brain>& brain);
void validateConfiguration(const Config& config);

// CLI argument parsing
CLIOptions parseCommandLineArgs(int argc, char** argv, const std::string& programName);

// Test helper functions
void runBasicConnectivityTest(const std::shared_ptr<Brain>& brain);
void runPlasticityExperiment(const std::shared_ptr<Brain>& brain);
void runStdpVerification(const std::shared_ptr<Brain>& brain);

} // namespace nlm

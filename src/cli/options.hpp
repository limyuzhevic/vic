// Command-line option definitions for NLM
// This file defines all command-line options and their handling

#ifndef NLM_CLI_OPTIONS_HPP
#define NLM_CLI_OPTIONS_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

namespace nlm {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

inline LogLevel stringToLogLevel(const std::string& level) {
    std::string upper = level;
    for (char& c : upper) c = std::toupper(c);
    
    if (upper == "DEBUG") return LogLevel::DEBUG;
    if (upper == "WARNING") return LogLevel::WARNING;
    if (upper == "ERROR") return LogLevel::ERROR;
    if (upper == "CRITICAL") return LogLevel::CRITICAL;
    return LogLevel::INFO;
}

inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::CRITICAL: return "CRITICAL";
        default: return "INFO";
    }
}

struct BenchmarkResult {
    std::string mode;
    std::chrono::milliseconds duration;
    bool success;
    std::unordered_map<std::string, double> metrics;
    std::string errorMessage;
};

struct TestSelection {
    bool basic = false;
    bool plasticity = false;
    bool stdp = false;
    bool all = false;
    
    TestSelection() {
        all = true;
        basic = true;
        plasticity = true;
        stdp = true;
    }
};

struct CommandLineOptions {
    // Mode selection
    std::string mode = "basic";
    
    // Configuration
    std::string configFile;
    std::unordered_map<std::string, std::string> settings;
    int neuronCount = -1;
    int episodeCount = -1;
    int trialCount = -1;
    double timestep = -1.0;
    
    // Output and reporting
    bool verbose = false;
    bool quiet = false;
    bool jsonOutput = false;
    bool showHelp = false;
    bool showVersion = false;
    bool benchmark = false;
    bool validateConfig = false;
    bool saveState = false;
    bool loadState = false;
    
    // Test selection
    bool fastMode = false;
    bool thoroughMode = false;
    std::string testList;
    TestSelection testSelection;
    
    // State management
    std::string saveStateFile;
    std::string loadStateFile;
    
    // Reporting
    std::string reportFile;
    std::string metricsFile;
    
    // Plugin and extension support
    std::string pluginDir;
    std::string experimentName;
    
    // Logging
    std::string logLevel = "INFO";
    
    // Internal
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
    CommandLineOptions() {
        startTime = std::chrono::high_resolution_clock::now();
    }
};

} // namespace nlm

#endif // NLM_CLI_OPTIONS_HPP
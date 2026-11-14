// NLM (熙然) - Unified Demo Framework
// Enhanced CLI and demo management system for NLM
// This framework provides a unified interface to all Phase demonstrations
// with advanced configuration, logging, and batch processing capabilities.

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <functional>

using namespace std;

// Demo phase enumeration
enum class DemoPhase {
    PHASE2 = 2,
    PHASE3 = 3,
    PHASE4 = 4,
    PHASE5 = 5,
    PHASE6 = 6,
    PHASE6_TEXT = 7,
    ALL = 0
};

// Command line options structure
struct CLIConfig {
    DemoPhase phase = DemoPhase::ALL;
    string configFile = "configs/default.cfg";
    string outputFile;
    bool verbose = false;
    bool quiet = false;
    bool benchmark = false;
    bool help = false;
    bool version = false;
    bool batchMode = false;
    string batchFile;
    int maxSteps = 0;
    double timestep = 0.0;
    map<string, string> extraArgs;
    map<string, string> paramOverrides;
};

// Command registration system
struct CommandInfo {
    string name;
    string description;
    function<bool(string)> validator;
    function<void(string)> executor;
};

// Performance metrics structure
struct PerformanceMetrics {
    chrono::time_point<chrono::system_clock> startTime;
    chrono::time_point<chrono::system_clock> endTime;
    double totalTime = 0.0;
    uint64_t stepsCompleted = 0;
    uint64_t totalSpikes = 0;
    double avgFiringRate = 0.0;
    double totalReward = 0.0;
    map<string, double> customMetrics;
};

class NLMCommander {
public:
    NLMCommander();
    virtual ~NLMCommander() = default;
    
    // Main entry point
    int run(int argc, char* argv[]);
    
    // CLI parsing
    bool parseArguments(int argc, char* argv[]);
    bool parseArguments(const vector<string>& args);
    
    // Configuration management
    bool loadConfig(const string& configFile);
    bool validateConfig() const;
    bool applyConfigOverrides() const;
    
    // Demo execution
    int executeDemo() const;
    int executePhase(DemoPhase phase) const;
    
    // Utility functions
    static string trim(const string& str);
    static vector<string> split(const string& str, char delimiter);
    static bool startsWith(const string& str, const string& prefix);
    static string toLower(const string& str);
    static string toUpper(const string& str);
    static string formatTime(double seconds);
    static string formatNumber(uint64_t num);
    
    // Output formatting
    void printHelp() const;
    void printVersion() const;
    void printBanner() const;
    void printProgress(const string& message, bool force = false) const;
    void printError(const string& error) const;
    void printSuccess(const string& success) const;
    void printMetrics(const PerformanceMetrics& metrics) const;
    
    // Plugin system
    void registerCommand(const CommandInfo& cmd);
    bool executeCommand(const string& cmd) const;
    
    // Batch processing
    bool loadBatchFile(const string& batchFile);
    bool executeBatchMode() const;
    
    // Performance monitoring
    void startPerformanceTimer() const;
    void stopPerformanceTimer() const;
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Configuration helpers
    string getEnvironmentVariable(const string& name, const string& defaultValue = "") const;
    bool getBooleanEnv(const string& name, bool defaultValue = false) const;
    int getIntEnv(const string& name, int defaultValue = 0) const;
    double getDoubleEnv(const string& name, double defaultValue = 0.0) const;
    
protected:
    CLIConfig config_;
    vector<CommandInfo> commands_;
    PerformanceMetrics performanceMetrics_;
    map<string, string> environmentOverrides_;
    bool performanceTimerActive_ = false;
    chrono::time_point<chrono::system_clock> timerStart_;
    
    // Internal helper functions
    bool setupLogging() const;
    void cleanup() const;
    bool isValidPhase(const string& phaseStr) const;
    DemoPhase parsePhase(const string& phaseStr) const;
    string phaseToString(DemoPhase phase) const;
    static bool fileExists(const string& path);
    static string getCurrentTimestamp();
};

// Command macros for easy registration
#define REGISTER_COMMAND(name, desc, validator, executor) \
    do { \
        CommandInfo cmd; \
        cmd.name = name; \
        cmd.description = desc; \
        cmd.validator = validator; \
        cmd.executor = executor; \
        commander.registerCommand(cmd); \
    } while(0)

// Performance measurement macro
#define NLM_PERF_MEASURE(name) \
    auto name##_start = chrono::high_resolution_clock::now(); \
    auto name##_end = chrono::high_resolution_clock::now(); \
    double name##_duration = chrono::duration<double>(name##_end - name##_start).count()

// Timing macro
#define NLM_TIME_BLOCK(name) \
    auto name##_start = chrono::high_resolution_clock::now(); \
    auto name##_end = chrono::high_resolution_clock::now(); \
    double name##_duration = chrono::duration<double>(name##_end - name##_start).count()

// Logging macros
#define NLM_LOG_COMMAND(name) \
    do { \
        if (commander.getVerbose()) { \
            commander.printProgress("Executing command: " + string(name), true); \
        } \
    } while(0)

// Configuration override macro
#define NLM_OVERRIDE_CONFIG(key, value) \
    config_.paramOverrides[key] = value

// Batch processing macro
#define NLM_BATCH_RUN(commands) \
    for (const auto& cmd : commands) { \
        commander.executeCommand(cmd); \
    }

// Example usage:
/*
int main(int argc, char* argv[]) {
    NLMCommander commander;
    return commander.run(argc, argv);
}

// Register commands in main or initialization
void registerDefaultCommands() {
    REGISTER_COMMAND("help", "Show help information", 
                     [](const string&) { return true; },
                     [](const string&) { commander.printHelp(); });
    
    REGISTER_COMMAND("version", "Show version information",
                     [](const string&) { return true; },
                     [](const string&) { commander.printVersion(); });
    
    REGISTER_COMMAND("run", "Run a specific demo",
                     [](const string& phase) { return isValidPhase(phase); },
                     [](const string& phase) { 
                         DemoPhase demoPhase = parsePhase(phase);
                         return commander.executePhase(demoPhase);
                     });
}
*/

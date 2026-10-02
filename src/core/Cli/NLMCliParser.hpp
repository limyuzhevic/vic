// Enhanced CLI Parser for NLM
// Uses CLI11 for robust command-line parsing with modern API

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <set>
#include <cstdint>

// CLI11 include - modern, header-only CLI library
#include "CLI/CLI.hpp"

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Constants.hpp"

namespace nlm {

// Forward declarations
class NLMCliParser;

// Enhanced CLI configuration options structure
struct EnhancedCliOptions {
    // Basic options
    bool help = false;
    bool version = false;
    bool verbose = false;
    std::string configFile = ConfigConstants::DEFAULT_CONFIG_FILE;
    std::map<std::string, std::string> customConfig;
    
    // Logging configuration
    std::string logLevel = "info";
    std::string logFile = "";
    bool enableColors = true;
    bool enableTimestamps = true;
    bool logToFile = false;
    bool logToConsole = true;
    
    // Simulation modes
    std::string simulationMode = "demo";  // demo, benchmark, experiment
    bool enableSimulation = true;
    uint64_t simulationSteps = 1000;
    double simulationTimeStep = ConfigConstants::DEFAULT_SIMULATION_TIMESTEP;
    
    // Output configuration
    std::string outputFormat = "text";  // text, json, csv
    std::string outputFile = "";
    std::string outputDir = ".";
    bool quiet = false;
    
    // Checkpoint/save configuration
    bool enableCheckpoint = false;
    uint64_t checkpointInterval = 100;
    std::string checkpointFile = "checkpoint.nlm";
    std::string checkpointDir = ".";
    bool saveOnExit = false;
    bool loadCheckpoint = false;
    std::string checkpointPath = "";
    
    // Performance optimization flags
    bool enableProfiling = false;
    bool enableProfilingDetailed = false;
    bool enableMemoryProfiling = false;
    bool enableTiming = false;
    uint64_t performanceSamplingRate = 1000;  // microseconds
    
    // Monitoring and statistics
    bool enableMonitoring = false;
    bool enableStatistics = false;
    bool enableResourceMonitoring = false;
    std::vector<std::string> enabledMetrics = {"neurons", "spikes", "synaptic_changes", "performance"};
    std::string metricsOutputFormat = "json";
    std::string metricsOutputFile = "metrics.json";
    
    // Result export functionality
    bool exportResults = false;
    std::string resultsFormat = "json";  // json, csv, binary
    std::string resultsFile = "results.json";
    bool exportBrainState = true;
    bool exportMetrics = true;
    bool exportLogs = false;
    
    // Advanced features
    bool enableDevelopmentalStages = false;
    bool enableNeuromodulation = true;
    bool enablePrediction = true;
    bool enableMemory = true;
    bool enableCognition = true;
    
    // Environment variable support
    bool useEnvironmentVariables = true;
    std::string environmentPrefix = "NLM_";
    bool strictEnvironmentValidation = false;
    
    // Validation and error handling
    bool enableValidation = true;
    bool validateAll = true;
    bool validateTypes = true;
    bool validateRanges = true;
    bool validateCrossParameters = true;
    bool failOnValidationError = true;
    
    // Configuration inheritance
    bool enableInheritance = true;
    std::string inheritancePrefix = "";
    bool inheritFromParent = false;
    bool inheritFromEnv = true;
    bool inheritFromFile = true;
    
    // Tab completion and CLI improvements
    bool enableTabCompletion = true;
    std::string completionShell = "bash";
    std::vector<std::string> customCompletions;
    
    // Performance and monitoring options
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    double totalExecutionTime = 0.0;
    uint64_t memoryUsage = 0;
    size_t peakMemoryUsage = 0;
    size_t currentMemoryUsage = 0;
    int cpuUsage = 0;
    int threadCount = 0;
    
    // Statistics collection
    size_t totalOperations = 0;
    double averageOperationTime = 0.0;
    std::map<std::string, double> customMetrics;
    
    // Configuration analysis
    std::string configAnalysis = "";
    std::vector<std::string> configWarnings;
    std::vector<std::string> configErrors;
};

// CLI Parser class with modern API
class NLMCliParser {
public:
    NLMCliParser();
    ~NLMCliParser();
    
    // Parse command line arguments
    bool parse(int argc, char** argv, EnhancedCliOptions& options);
    
    // Process configuration options
    bool processOptions(const EnhancedCliOptions& options, std::shared_ptr<Config>& config);
    
    // Generate help documentation
    void generateHelp(const EnhancedCliOptions& options) const;
    void generateUsageExamples() const;
    void generateDetailedHelp(const EnhancedCliOptions& options) const;
    
    // Configuration file operations
    bool loadConfigurationFile(const std::string& filepath, EnhancedCliOptions& options);
    bool saveConfigurationFile(const std::string& filepath, const EnhancedCliOptions& options) const;
    bool exportConfiguration(const std::string& filepath, const EnhancedCliOptions& options) const;
    
    // Validation and error handling
    bool validateOptions(const EnhancedCliOptions& options) const;
    std::vector<std::string> validateConfiguration(const EnhancedCliOptions& options, const Config& config) const;
    void displayValidationErrors(const std::vector<std::string>& errors) const;
    
    // Environment variable support
    bool loadEnvironmentVariables(EnhancedCliOptions& options) const;
    std::string getEnvironmentVariable(const std::string& key, const std::string& defaultValue = "") const;
    bool setEnvironmentVariable(const std::string& key, const std::string& value);
    
    // Configuration inheritance
    bool inheritConfiguration(const EnhancedCliOptions& baseOptions, EnhancedCliOptions& targetOptions) const;
    bool mergeConfiguration(const EnhancedCliOptions& baseOptions, EnhancedCliOptions& targetOptions) const;
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    double getElapsedTime() const;
    void collectPerformanceMetrics(EnhancedCliOptions& options) const;
    
    // Statistics collection
    void recordOperation(const std::string& operationName, double timeTaken);
    void collectStatistics(EnhancedCliOptions& options) const;
    void exportStatistics(const std::string& filepath, const EnhancedCliOptions& options) const;
    
    // Configuration analysis
    std::string analyzeConfiguration(const EnhancedCliOptions& options) const;
    void identifyConfigurationIssues(const EnhancedCliOptions& options, std::vector<std::string>& warnings, std::vector<std::string>& errors) const;
    
    // Tab completion
    bool generateTabCompletion(const std::string& shell) const;
    bool isTabCompletionSupported() const;
    
    // CLI11 app access
    CLI::App& getApp() { return app_; }
    
private:
    // Internal helper methods
    void setupOptions(CLI::App& app);
    void setupGroups(CLI::App& app);
    void setupValidation(CLI::App& app);
    void setupLogging(CLI::App& app);
    void setupSimulation(CLI::App& app);
    void setupOutput(CLI::App& app);
    void setupCheckpoint(CLI::App& app);
    void setupPerformance(CLI::App& app);
    void setupMonitoring(CLI::App& app);
    void setupExport(CLI::App& app);
    void setupAdvanced(CLI::App& app);
    void setupEnvironment(CLI::App& app);
    
    // Configuration file helpers
    bool parseConfigurationFile(const std::string& filepath, std::map<std::string, std::string>& config) const;
    bool writeConfigurationFile(const std::string& filepath, const std::map<std::string, std::string>& config) const;
    std::string formatConfigurationValue(const std::string& key, const std::string& value) const;
    
    // Environment variable helpers
    std::string sanitizeEnvironmentKey(const std::string& key) const;
    bool isEnvironmentVariableEnabled(const std::string& key) const;
    
    // Validation helpers
    bool validateLogLevel(const std::string& level) const;
    bool validateSimulationMode(const std::string& mode) const;
    bool validateOutputFormat(const std::string& format) const;
    bool validateResultsFormat(const std::string& format) const;
    bool validateRange(const double value, const double min, const double max, const std::string& name) const;
    
    // Performance monitoring helpers
    void updateResourceUsage() const;
    uint64_t getMemoryUsage() const;
    int getCpuUsage() const;
    
    // Statistics helpers
    void updateAverageOperationTime() const;
    
    // Configuration analysis helpers
    void analyzeConfigurationIssues(const EnhancedCliOptions& options, std::vector<std::string>& warnings, std::vector<std::string>& errors) const;
    void checkRequiredOptions(const EnhancedCliOptions& options, std::vector<std::string>& errors) const;
    void checkConfigurationConsistency(const EnhancedCliOptions& options, std::vector<std::string>& errors) const;
    
    // CLI11 app instance
    CLI::App app_;
    
    // Internal state
    bool isInitialized_ = false;
    std::chrono::steady_clock::time_point startTime_;
    
    // Configuration schemas
    std::map<std::string, std::function<bool(const std::string&)>> optionValidators_;
    std::map<std::string, std::string> optionDescriptions_;
    std::map<std::string, std::string> optionExamples_;
    
    // Statistics
    mutable size_t operationCount_ = 0;
    mutable double totalOperationTime_ = 0.0;
    mutable std::map<std::string, double> operationTimes_;
};

// Global CLI parser instance
std::unique_ptr<NLMCliParser> createCliParser();

// Convenience functions
bool parseCommandLineArgs(int argc, char** argv, EnhancedCliOptions& options);
bool initializeConfigurationFromOptions(const EnhancedCliOptions& options, std::shared_ptr<Config>& config);
void displayHelpAndExit(const EnhancedCliOptions& options);
void displayVersionAndExit();
void displayConfigurationSummary(const Config& config);

} // namespace nlm

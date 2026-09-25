#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <chrono>
#include <variant>
#include <optional>
#include <functional>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sstream>
#include "core/Config/ConfigManager.hpp"

namespace nlm {

// Forward declarations
class CLIParser;

// CLI option types
struct CLIOption {
    std::string name;
    std::string shortName;
    std::string description;
    bool hasValue;
    bool required;
    std::string defaultValue;
    std::string category;
    std::vector<std::string> allowedValues;
};

// CLI result structure
struct CLIResult {
    bool success;
    std::string message;
    std::string configFile;
    std::string outputFile;
    std::string logFile;
    std::string outputFormat;
    int verbosityLevel;
    bool showProgress;
    bool showStatistics;
    bool debugMode;
    int debugLevel;
    std::string checkpointFile;
    std::string checkpointDir;
    int checkpointInterval;
    bool validateCheckpoint;
    std::string environment;
    std::string simulationMode;
    std::string visualizationMode;
    std::string exportFormat;
    std::string importFormat;
    std::string customParamsFile;
    std::string pluginDir;
    std::string pluginName;
    bool disablePlugins;
    std::string batchFile;
    std::string batchOutputFile;
    std::string batchResultsFile;
    bool validateBatch;
    bool parallelBatch;
    std::string testMode;
    std::vector<std::string> runTests;
    std::vector<std::string> skipTests;
    int repeatCount;
    bool batchMode;
    bool parallelMode;
    bool continuousMode;
    bool validateConfig;
    std::string generateConfigFile;
    std::string mergeConfigFiles;
    std::string backupConfigFile;
    std::string restoreConfigFile;
    std::string exportConfigFile;
    std::string importConfigFile;
    bool benchmarkMode;
    bool profileMode;
    bool memoryTrace;
    bool performanceTrace;
    bool threadTrace;
    bool networkTrace;
    std::string traceFile;
    std::string analysisMode;
    std::string analysisConfigFile;
    std::string analysisReportFile;
    bool trendAnalysis;
    bool historicalAnalysis;
    bool performanceTest;
    bool memoryTest;
    bool stressTest;
    bool accuracyTest;
    bool timingTest;
    bool concurrencyTest;
    std::string customCommand;
    std::string customScript;
    std::string customConfigFile;
    std::string customParamsFile;
    std::string helpSection;
    bool showVersion;
    bool showHelp;
    
    CLIResult() : success(true), verbosityLevel(1), debugLevel(0), checkpointInterval(1),
                  validateCheckpoint(false), disablePlugins(false), validateBatch(false),
                  parallelBatch(false), repeatCount(1), batchMode(false), parallelMode(false),
                  continuousMode(false), validateConfig(false), benchmarkMode(false),
                  profileMode(false), memoryTrace(false), performanceTrace(false),
                  threadTrace(false), networkTrace(false), trendAnalysis(false),
                  historicalAnalysis(false), performanceTest(false), memoryTest(false),
                  stressTest(false), accuracyTest(false), timingTest(false), concurrencyTest(false),
                  showVersion(false), showHelp(false) {}
};

// CLI argument parser
class CLIParser {
public:
    CLIParser();
    ~CLIParser() = default;
    
    bool parseArguments(int argc, char** argv, CLIResult& result);
    bool validateArguments(const CLIResult& result) const;
    void printHelp(const std::string& section = "") const;
    void printVersion() const;
    bool generateConfigTemplate(const std::string& filepath) const;
    bool mergeConfigs(const std::string& file1, const std::string& file2) const;
    bool validateConfig(const std::string& configFile) const;
    bool backupConfig(const std::string& configFile, const std::string& backupFile) const;
    bool restoreConfig(const std::string& backupFile, const std::string& configFile) const;
    bool exportConfig(const std::string& configFile, const std::string& outputFile, const std::string& format) const;
    bool importConfig(const std::string& inputFile, const std::string& configFile, const std::string& format) const;
    bool createCheckpoint(const std::string& checkpointFile) const;
    bool restoreCheckpoint(const std::string& checkpointFile) const;
    bool validateCheckpoint(const std::string& checkpointFile) const;
    
    // Progress and statistics tracking
    ProgressTracker& getProgressTracker() { return progressTracker_; }
    StatisticsCollector& getStatisticsCollector() { return statisticsCollector_; }
    OutputFormatter& getOutputFormatter() { return outputFormatter_; }
    DebugTracer& getDebugTracer() { return debugTracer_; }
    EnvironmentManager& getEnvironmentManager() { return environmentManager_; }
    BatchProcessor& getBatchProcessor() { return batchProcessor_; }
    
    ProgressTracker& getProgressTracker() const { return progressTracker_; }
    StatisticsCollector& getStatisticsCollector() const { return statisticsCollector_; }
    OutputFormatter& getOutputFormatter() const { return outputFormatter_; }
    DebugTracer& getDebugTracer() const { return debugTracer_; }
    EnvironmentManager& getEnvironmentManager() const { return environmentManager_; }
    BatchProcessor& getBatchProcessor() const { return batchProcessor_; }
    
private:
    ProgressTracker progressTracker_;
    StatisticsCollector statisticsCollector_;
    OutputFormatter outputFormatter_;
    DebugTracer debugTracer_;
    EnvironmentManager environmentManager_;
    BatchProcessor batchProcessor_;
    
    void initializeOptions();
    bool parseOption(const std::string& arg, CLIResult& result) const;
    std::string valueToString(const std::variant<int, double, std::string, bool>& value) const;
    bool optionExists(const std::string& name) const;
    CLIOption* getOption(const std::string& name);
    void setupProgressDisplay() const;
    void setupStatisticsDisplay() const;
    void setupOutputFormatting() const;
    void setupDebugTracing() const;
    void setupEnvironmentManagement() const;
    void setupBatchProcessing() const;
};

} // namespace nlm

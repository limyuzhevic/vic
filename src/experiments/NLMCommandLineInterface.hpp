#pragma once

#include "core/Config/Config.hpp"
#include "core/Constants.hpp"
#include "experiments/Phase2UnifiedFramework.hpp"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <fstream>

namespace nlm {
namespace Phase2 {

// Enhanced command line interface for NLM experiments
class NLMCommandLineInterface {
public:
    struct CommandLineArgs {
        // Core arguments
        std::string experimentName;
        std::string outputFile;
        std::string configFile;
        std::string resultsFile;
        
        // Experiment selection
        std::vector<ExperimentMode> experiments;
        ExperimentMode primaryExperiment;
        bool runAllExperiments;
        bool benchmarkMode;
        
        // Configuration parameters
        std::map<std::string, std::string> configOverrides;
        std::map<std::string, std::string> experimentConfigs;
        
        // Execution control
        bool help;
        bool version;
        bool verbose;
        bool quiet;
        int maxSteps;
        int numEpisodes;
        double timestep;
        int64_t neuronCount;
        int regionCount;
        float connectionProbability;
        
        // Performance and monitoring
        bool enableMonitoring;
        bool enableProfiling;
        double monitoringInterval;
        std::string monitoringFile;
        
        // Output and formatting
        bool saveConfig;
        bool saveResults;
        std::string resultFormat;
        bool showProgress;
        bool showDetailedOutput;
        
        // Advanced options
        bool validateOnly;
        bool dryRun;
        bool continueOnError;
        int retryCount;
        double timeout;
        
        // Development and debugging
        bool debugMode;
        bool traceMode;
        std::string logLevel;
        std::string logFile;
        bool captureLogs;
        
        // Plugin and extension
        std::vector<std::string> plugins;
        std::vector<std::string> pluginArgs;
        bool enablePlugins;
        
        // Environment and system
        int cpuCores;
        size_t memoryLimitMB;
        std::string tempDirectory;
        
        CommandLineArgs() : 
            runAllExperiments(false),
            benchmarkMode(false),
            help(false),
            version(false),
            verbose(false),
            quiet(false),
            maxSteps(-1),
            numEpisodes(-1),
            timestep(-1.0),
            neuronCount(-1),
            regionCount(-1),
            connectionProbability(-1.0),
            enableMonitoring(false),
            enableProfiling(false),
            monitoringInterval(1.0),
            saveConfig(false),
            saveResults(true),
            resultFormat("text"),
            showProgress(true),
            showDetailedOutput(false),
            validateOnly(false),
            dryRun(false),
            continueOnError(false),
            retryCount(1),
            timeout(300.0),
            debugMode(false),
            traceMode(false),
            enablePlugins(false),
            cpuCores(-1),
            memoryLimitMB(0),
            primaryExperiment(ExperimentMode::ALL_TESTS) {}
    };

    struct UsageInfo {
        std::string programName;
        std::string version;
        std::string description;
        std::vector<std::string> sections;
        std::map<std::string, std::string> options;
    };

    struct ExperimentInfo {
        ExperimentMode mode;
        std::string name;
        std::string description;
        std::vector<std::string> requiredConfigKeys;
        std::vector<std::string> optionalConfigKeys;
        bool enabledByDefault;
        std::function<bool(const Config&)> validator;
    };

    // Main interface methods
    bool parseCommandLine(int argc, char** argv, CommandLineArgs& args);
    bool parseCommandLine(const std::string& commandLine, CommandLineArgs& args);
    bool validateArgs(const CommandLineArgs& args) const;
    bool execute(const CommandLineArgs& args);
    
    // Configuration methods
    bool loadConfigFile(Config& config, const std::string& configFile) const;
    bool applyConfigOverrides(Config& config, const std::map<std::string, std::string>& overrides) const;
    bool applyExperimentConfig(Config& config, const std::string& experimentName) const;
    
    // Experiment methods
    std::vector<ExperimentMode> parseExperimentList(const std::string& experimentList) const;
    ExperimentMode getExperimentMode(const std::string& experimentName) const;
    std::string getExperimentName(ExperimentMode mode) const;
    bool runExperiments(const CommandLineArgs& args);
    
    // Output methods
    void printHelp(const UsageInfo& usage) const;
    void printVersion(const UsageInfo& usage) const;
    void printExperimentList() const;
    void printConfiguration(const Config& config) const;
    void printResults(const std::vector<ExperimentResult>& results) const;
    void printPerformanceReport(const std::vector<ExperimentResult>& results) const;
    
    // Configuration methods
    UsageInfo getUsageInfo() const;
    std::vector<ExperimentInfo> getAvailableExperiments() const;
    std::map<std::string, std::string> getCommonOptions() const;
    std::map<std::string, std::string> getExperimentOptions() const;
    std::map<std::string, std::string> getOutputOptions() const;
    std::map<std::string, std::string> getPerformanceOptions() const;
    std::map<std::string, std::string> getDebugOptions() const;
    
    // Helper methods
    bool fileExists(const std::string& path) const;
    bool directoryExists(const std::string& path) const;
    std::string getAbsolutePath(const std::string& path) const;
    std::string getConfigFilePath(const std::string& experimentName) const;
    std::string getResultsFilePath(const std::string& baseName) const;
    std::string getLogFilePath(const std::string& logName) const;
    
private:
    // Internal parsing methods
    bool parsePositionalArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs);
    bool parseOptionArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs);
    bool parseExperimentArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs);
    bool parseConfigArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs);
    bool parseOutputArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs);
    bool parsePerformanceArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs);
    bool parseDebugArgs(const std::vector<std::string>& args, CommandLineArgs& parsedArgs);
    
    // Argument validation methods
    bool validateExperimentArgs(const CommandLineArgs& args) const;
    bool validateConfigArgs(const CommandLineArgs& args) const;
    bool validateOutputArgs(const CommandLineArgs& args) const;
    bool validatePerformanceArgs(const CommandLineArgs& args) const;
    bool validateDebugArgs(const CommandLineArgs& args) const;
    
    // Error handling methods
    void printError(const std::string& message) const;
    void printWarning(const std::string& message) const;
    void printInfo(const std::string& message) const;
    
    // Experiment validation methods
    bool validateExperimentPrerequisites(ExperimentMode mode, const Config& config) const;
    bool checkSystemResources(const CommandLineArgs& args) const;
    bool checkExperimentCompatibility(ExperimentMode mode, const Config& config) const;
    
    // Configuration loading methods
    bool loadDefaultConfig(Config& config) const;
    bool loadExperimentPreset(Config& config, const std::string& experimentName) const;
    bool mergeConfigs(Config& baseConfig, const Config& overrideConfig) const;
    
    // Output formatting methods
    std::string formatArgsHelp(const CommandLineArgs& args) const;
    std::string formatExperimentHelp() const;
    std::string formatConfigHelp() const;
    std::string formatOutputHelp() const;
    std::string formatPerformanceHelp() const;
    std::string formatDebugHelp() const;
    
    // Statistics and monitoring methods
    void updateStatistics(const CommandLineArgs& args, const std::vector<ExperimentResult>& results);
    void generatePerformanceReport(const CommandLineArgs& args, const std::vector<ExperimentResult>& results) const;
    void saveStatistics(const CommandLineArgs& args, const std::vector<ExperimentResult>& results) const;
    
    // Logging methods
    void setupLogging(const CommandLineArgs& args) const;
    void logCommandLineArgs(const CommandLineArgs& args) const;
    void logConfiguration(const Config& config) const;
    void logExperimentResults(const std::vector<ExperimentResult>& results) const;
    
    // Experiment execution methods
    bool executeExperiment(ExperimentMode mode, const Config& config, const CommandLineArgs& args);
    bool executeBenchmark(const Config& config, const CommandLineArgs& args);
    bool executeAllExperiments(const Config& config, const CommandLineArgs& args);
    
    // Error recovery methods
    bool handleExecutionError(ExperimentMode mode, const std::exception& e, const CommandLineArgs& args);
    bool retryExperiment(ExperimentMode mode, const Config& config, const CommandLineArgs& args, int attempt);
    
    // Configuration schema methods
    bool validateConfigSchema(const Config& config, ExperimentMode mode) const;
    bool migrateConfigSchema(Config& config, ExperimentMode mode) const;
    
    // Performance optimization methods
    bool optimizePerformance(Config& config, const CommandLineArgs& args) const;
    bool enableParallelProcessing(Config& config, const CommandLineArgs& args) const;
    bool configureMemoryUsage(Config& config, const CommandLineArgs& args) const;
    
    // Plugin methods
    bool loadPlugins(const std::vector<std::string>& plugins, const std::vector<std::string>& args);
    bool executePlugins(const std::vector<std::string>& plugins, const std::vector<std::string>& args);
    bool unloadPlugins();
    
    // Cleanup methods
    void cleanup() const;
    
    // Static data
    static const std::map<std::string, ExperimentMode> EXPERIMENT_MAP;
    static const std::map<ExperimentMode, std::string> EXPERIMENT_NAMES;
    static const std::map<ExperimentMode, ExperimentInfo> EXPERIMENT_INFO;
    static const std::vector<std::string> AVAILABLE_EXPERIMENTS;
    static const std::vector<std::string> COMMON_CONFIG_FILES;
    static const std::string DEFAULT_RESULTS_FILE;
    static const std::string DEFAULT_LOG_FILE;
    static const std::string DEFAULT_TEMP_DIR;
};

// Convenience functions
bool runNLMExperiment(int argc, char** argv);
bool runNLMExperiment(const std::string& commandLine);
std::vector<ExperimentResult> runNLMExperiments(const std::string& experimentList, const std::string& configFile);
std::vector<ExperimentResult> runNLMBenchmark(const std::string& configFile);

} // namespace Phase2
} // namespace nlm
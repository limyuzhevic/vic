#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <cmath>
#include <random>
#include <chrono>
#include <iomanip>
#include <set>
#include <regex>
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "nlm/Constants.hpp"
#include "nlm/CLI.hpp"
#include "nlm/Exception.hpp"
#include "nlm/Config.hpp"
#include "nlm/AdvancedConfig.hpp"

namespace nlm {

/**
 * Advanced experiment runner with support for multiple experiments, parallel execution,
 * result aggregation, and comprehensive reporting
 */
class ExperimentRunner {
public:
    // Experiment execution result
    struct RunResult {
        std::string experimentName;
        std::string timestamp;
        bool success;
        std::string errorMessage;
        
        // Performance metrics
        float executionTime;
        size_t memoryUsage;
        
        // Experiment metrics
        float averageFiringRate;
        size_t totalSpikes;
        float totalWeightChange;
        bool learningDetected;
        
        // Additional metrics
        std::map<std::string, float> customMetrics;
        
        RunResult() : success(false), executionTime(0.0f), memoryUsage(0),
                     averageFiringRate(0.0f), totalSpikes(0), totalWeightChange(0.0f),
                     learningDetected(false) {}
    };

    // Experiment execution configuration
    struct ExecutionConfig {
        std::vector<std::string> experimentNames;
        std::string experimentType; // "all", "connectivity", "plasticity", "stdp", "development", "memory", "neuromodulation", "advanced"
        bool runAll = false;
        size_t maxParallelExperiments = 1; // 0 for unlimited
        std::chrono::milliseconds timeoutPerExperiment{60000}; // 60 seconds
        std::string outputDirectory;
        bool saveResults = true;
        std::string resultFormat; // "json", "csv", "text"
        bool generateReport = true;
        
        // Configuration overrides
        std::map<std::string, std::string> configOverrides;
        
        // Experiment-specific parameters
        std::map<std::string, std::map<std::string, std::string>> experimentParams;
    };

    // Global experiment configuration
    struct GlobalConfig {
        std::string programName = "nlm";
        std::string version = "0.1.0";
        std::string description = "Neural Learning Machine";
        std::string author = "NLM Authors";
        std::string license = "MIT License";
        
        // Logging configuration
        std::string logLevel = "info";
        std::string logFile = "";
        std::string logFormat = "%timestamp %level %source: %message";
        
        // Performance configuration
        size_t maxMemoryUsage = 512; // MB
        bool enableCaching = true;
        bool enableProfiling = false;
        
        // Output configuration
        std::string outputFormat = "json";
        bool overwriteOutput = false;
        
        // Experiment configuration
        std::string defaultExperiment = "advanced";
        bool enableDevelopmentMode = false;
        bool enableVerboseOutput = false;
    };

    ExperimentRunner(const GlobalConfig& globalConfig = GlobalConfig());
    ~ExperimentRunner();

    // Configure global settings
    void setGlobalConfig(const GlobalConfig& config) { this->globalConfig = config; }
    const GlobalConfig& getGlobalConfig() const { return globalConfig; }
    
    // Configure experiment execution
    void setExecutionConfig(const ExecutionConfig& config) { this->executionConfig = config; }
    const ExecutionConfig& getExecutionConfig() const { return executionConfig; }
    
    // Run experiments
    std::vector<RunResult> runExperiments();
    
    // Run a specific experiment
    RunResult runExperiment(const std::string& experimentName);
    
    // Get experiment result
    const RunResult* getExperimentResult(const std::string& experimentName) const;
    
    // Get all experiment results
    std::vector<RunResult> getAllResults() const;
    
    // Clear results
    void clearResults();
    
    // Generate comprehensive report
    std::string generateReport() const;
    
    // Save results to file
    bool saveResults(const std::string& filename) const;
    
    // Load results from file
    bool loadResults(const std::string& filename);
    
    // Get experiment factory
    ExperimentFactory getExperimentFactory(const std::string& experimentName);
    
    // Register experiment factory
    void registerExperimentFactory(const std::string& name, ExperimentFactory factory);
    
    // Get list of available experiments
    std::vector<std::string> getAvailableExperiments() const;
    
    // Check if experiment is available
    bool hasExperiment(const std::string& name) const;
    
    // Validate experiment configuration
    bool validateExperimentConfig(const std::string& experimentName) const;
    
    // Get experiment configuration
    std::map<std::string, std::string> getExperimentConfig(const std::string& experimentName) const;
    
    // Set experiment configuration
    void setExperimentConfig(const std::string& experimentName,
                            const std::map<std::string, std::string>& config);
    
    // Get experiment status
    std::string getExperimentStatus(const std::string& experimentName) const;
    
    // Check if experiment is running
    bool isExperimentRunning(const std::string& experimentName) const;
    
    // Stop experiment
    void stopExperiment(const std::string& experimentName);
    
    // Get overall status
    std::string getOverallStatus() const;
    
    // Get total execution statistics
    struct Statistics {
        uint64_t totalExperiments = 0;
        uint64_t successfulExperiments = 0;
        uint64_t failedExperiments = 0;
        float totalExecutionTime = 0.0f;
        uint64_t totalMemoryUsage = 0;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
    };
    
    Statistics getStatistics() const { return stats; }

private:
    GlobalConfig globalConfig;
    ExecutionConfig executionConfig;
    
    // Experiment management
    std::map<std::string, std::unique_ptr<ExperimentBase>> experiments;
    std::map<std::string, std::unique_ptr<ExperimentFactory>> experimentFactories;
    
    // Execution management
    std::map<std::string, std::thread> experimentThreads;
    std::map<std::string, std::atomic<bool>> experimentRunning;
    std::mutex experimentsMutex;
    
    // Results management
    std::map<std::string, RunResult> results;
    std::mutex resultsMutex;
    
    // Statistics
    Statistics stats;
    mutable std::mutex statsMutex;
    
    // Configuration
    std::map<std::string, std::map<std::string, std::string>> experimentConfigs;
    
    // Helper methods
    void initializeExperiments();
    void cleanupExperiments();
    RunResult executeSingleExperiment(const std::string& experimentName);
    bool validateGlobalConfig() const;
    bool validateExecutionConfig() const;
    void updateStatistics(const RunResult& result);
    std::string generateExperimentName(const std::string& baseName) const;
    bool isExperimentNameValid(const std::string& name) const;
    
    // Configuration loading
    void loadExperimentConfigurations();
    void applyConfigOverrides();
    
    // Result processing
    void processExperimentResult(const std::string& experimentName, RunResult& result);
    void saveResultToFile(const RunResult& result) const;
    
    // Report generation
    std::string generateTextReport() const;
    std::string generateJSONReport() const;
    std::string generateCSVReport() const;
};

// Convenience functions for common experiment configurations
class ExperimentPresets {
public:
    // Get preset configuration for different experiment types
    static std::map<std::string, std::string> getConnectivityPreset();
    static std::map<std::string, std::string> getPlasticityPreset();
    static std::map<std::string, std::string> getStdpPreset();
    static std::map<std::string, std::string> getDevelopmentPreset();
    static std::map<std::string, std::string> getMemoryPreset();
    static std::map<std::string, std::string> getNeuromodulationPreset();
    static std::map<std::string, std::string> getAdvancedPreset();
    static std::map<std::string, std::string> getQuickTestPreset();
    static std::map<std::string, std::string> getComprehensivePreset();
    
    // Get all available presets
    static std::vector<std::string> getPresetNames();
    
    // Get preset description
    static std::string getPresetDescription(const std::string& presetName);
    
    // Apply preset to execution configuration
    static void applyPreset(ExperimentRunner::ExecutionConfig& config, const std::string& presetName);
    
    // Create execution configuration from preset
    static ExperimentRunner::ExecutionConfig createExecutionConfig(const std::string& presetName);
};

// Experiment command line interface
class ExperimentCLI {
public:
    ExperimentCLI(ExperimentRunner& runner);
    
    // Parse command line arguments for experiment execution
    bool parseArguments(int argc, char** argv);
    
    // Get experiment execution configuration
    const ExperimentRunner::ExecutionConfig& getExecutionConfig() const { return config; }
    
    // Print help information
    void printHelp() const;
    
    // Print available experiments
    void printAvailableExperiments() const;
    
    // Print preset information
    void printPresetInformation() const;
    
    // Validate command line arguments
    bool validateArguments() const;
    
    // Get command line option
    std::string getOption(const std::string& option) const;
    
    // Check if option is present
    bool hasOption(const std::string& option) const;
    
    // Get experiment from option
    std::string getExperimentFromOption(const std::string& option) const;

private:
    ExperimentRunner& runner;
    ExperimentRunner::ExecutionConfig config;
    std::map<std::string, std::string> options;
    
    // Parse argument
    void parseArgument(const std::string& arg);
    
    // Parse experiment list
    void parseExperimentList(const std::string& value);
    
    // Parse experiment parameters
    void parseExperimentParameters(const std::string& value);
    
    // Parse config overrides
    void parseConfigOverrides(const std::string& value);
    
    // Print help
    void printBasicHelp() const;
    void printExperimentHelp() const;
    void printPresetHelp() const;
}; 

} // namespace nlm

#pragma once

/**
 * Improved NLM Command Line Interface
 * 
 * This module provides a modern, extensible CLI interface for NLM (Neural Learning Machine)
 * with support for multiple experiment types, advanced configuration options,
 * and comprehensive user experience features.
 */

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "core/Config/Config.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

/**
 * Command line argument parsing and help system
 */
class CLIArgs {
public:
    struct Command {
        std::string name;
        std::string description;
        std::string usage;
        std::vector<std::string> examples;
        std::function<bool(const CLIArgs&)> executor;
        std::vector<std::string> options;
    };
    
    CLIArgs(int argc, char** argv);
    ~CLIArgs() = default;
    
    // Command management
    void registerCommand(const std::string& name, const std::string& description, 
                        const std::string& usage, const std::function<bool(const CLIArgs&)>& executor,
                        const std::vector<std::string>& options = {});
    
    // Option management
    void addOption(const std::string& name, const std::string& value = "");
    bool hasOption(const std::string& name) const;
    std::string getOption(const std::string& name) const;
    std::string getOptionOrDefault(const std::string& name, const std::string& defaultValue) const;
    int getOptionAsInt(const std::string& name, int defaultValue = 0) const;
    double getOptionAsDouble(const std::string& name, double defaultValue = 0.0) const;
    bool getOptionAsBool(const std::string& name, bool defaultValue = false) const;
    
    // Command execution
    bool executeCommand() const;
    bool showHelp() const;
    
    // Validation
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Access
    const std::vector<std::string>& getArgs() const { return args_; }
    const std::string& getCommand() const { return command_; }
    bool isHelpRequested() const { return helpRequested_; }
    
    // Statistics and profiling
    void startTimer(const std::string& operation);
    void stopTimer(const std::string& operation);
    double getElapsedTime(const std::string& operation) const;
    std::string getFormattedDuration(double seconds) const;
    
private:
    std::vector<std::string> args_;
    std::string command_;
    bool helpRequested_;
    std::map<std::string, std::string> options_;
    std::vector<Command> commands_;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> timers_;
    mutable std::string validationErrors_;
    
    void parseArgs();
    void parseOptions();
    bool validateOptions() const;
    std::string formatOptionValue(const std::string& value) const;
};

/**
 * Experiment registry for extensible experiment system
 */
class ExperimentRegistry {
public:
    struct ExperimentInfo {
        std::string name;
        std::string description;
        std::function<std::shared_ptr<Experiment>(const CLIArgs&)> factory;
        std::string version;
        std::vector<std::string> tags;
    };
    
    ExperimentRegistry();
    ~ExperimentRegistry() = default;
    
    // Register experiment types
    void registerExperiment(const std::string& name, const std::string& description,
                           const std::function<std::shared_ptr<Experiment>(const CLIArgs&)>& factory,
                           const std::string& version = "1.0",
                           const std::vector<std::string>& tags = {});
    
    // Get experiment info
    bool hasExperiment(const std::string& name) const;
    ExperimentInfo getExperimentInfo(const std::string& name) const;
    std::vector<ExperimentInfo> getAllExperiments() const;
    std::vector<ExperimentInfo> getExperimentsByTag(const std::string& tag) const;
    
    // Create experiments
    std::shared_ptr<Experiment> createExperiment(const std::string& name, const CLIArgs& args);
    
    // Built-in experiment factories
    static std::shared_ptr<Experiment> createPhase6Experiment(const CLIArgs& args);
    static std::shared_ptr<Experiment> createBasicConnectivityExperiment(const CLIArgs& args);
    static std::shared_ptr<Experiment> createPlasticityExperiment(const CLIArgs& args);
    static std::shared_ptr<Experiment> createStdpExperiment(const CLIArgs& args);
    
private:
    std::map<std::string, ExperimentInfo> experiments_;
};

/**
 * Advanced configuration manager for experiments
 */
class ExperimentConfigManager {
public:
    struct ExperimentPreset {
        std::string name;
        std::string description;
        std::map<std::string, std::string> configValues;
    };
    
    ExperimentConfigManager();
    ~ExperimentConfigManager() = default;
    
    // Configuration presets
    void registerPreset(const std::string& name, const std::string& description,
                       const std::map<std::string, std::string>& configValues);
    
    // Get configuration
    Phase6Config getPhase6Config(const CLIArgs& args) const;
    Config getBasicConfig(const CLIArgs& args) const;
    
    // Preset management
    std::vector<ExperimentPreset> getAllPresets() const;
    bool hasPreset(const std::string& name) const;
    
    // Configuration validation
    bool validateConfig(const std::string& experimentName, const Config& config) const;
    std::string getValidationMessages(const std::string& experimentName, const Config& config) const;
    
    // Preset application
    void applyPresetToConfig(const std::string& presetName, Config& config) const;
    
private:
    std::map<std::string, ExperimentPreset> presets_;
    std::map<std::string, std::vector<std::string>> validationRules_;
};

/**
 * Comprehensive statistics and profiling system
 */
class ProfilingSystem {
public:
    struct PerformanceMetrics {
        double wallClockTime;
        double cpuTime;
        size_t memoryUsage;
        size_t peakMemoryUsage;
        uint64_t totalSpikes;
        uint64_t totalNeurons;
        uint64_t totalSynapses;
        uint64_t firingNeuronCount;
        double averageFiringRate;
        double excitationInhibitionRatio;
    };
    
    ProfilingSystem();
    ~ProfilingSystem() = default;
    
    // Performance tracking
    void startMeasurement(const std::string& operation);
    void stopMeasurement(const std::string& operation);
    void recordMetrics(const PerformanceMetrics& metrics);
    
    // Metrics access
    PerformanceMetrics getLatestMetrics() const;
    std::vector<PerformanceMetrics> getAllMetrics() const;
    
    // Statistics
    double getAverageWallClockTime() const;
    double getAverageCpuTime() const;
    double getAverageMemoryUsage() const;
    
    // Report generation
    std::string generatePerformanceReport() const;
    std::string generateDetailedReport(const CLIArgs& args) const;
    
    // Comparison
    std::string compareWithBaseline(const ProfilingSystem& baseline) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Enhanced experiment runner with comprehensive features
 */
class EnhancedExperimentRunner {
public:
    EnhancedExperimentRunner();
    ~EnhancedExperimentRunner() = default;
    
    // Core execution
    bool runExperiment(const std::string& experimentName, const CLIArgs& args);
    bool runAllExperiments(const CLIArgs& args);
    
    // Configuration and setup
    void setLogger(std::shared_ptr<Logger> logger);
    void setConfigManager(std::shared_ptr<ExperimentConfigManager> configManager);
    
    // Statistics and reporting
    ProfilingSystem& getProfilingSystem();
    
    // Export and import
    bool exportResults(const std::string& filepath) const;
    bool importResults(const std::string& filepath);
    
    // Batch operations
    bool runExperimentBatch(const std::vector<std::string>& experimentNames, const CLIArgs& args);
    bool runExperimentSweep(const std::string& experimentName, const std::map<std::string, std::vector<std::string>>& paramVariations, const CLIArgs& args);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

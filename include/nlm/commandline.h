#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <functional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class ExperimentRunner;

// Enhanced CLI command structure
struct CLICommand {
    std::string name;
    std::string description;
    std::vector<std::string> flags;
    std::function<bool(const Config&, const std::vector<std::string>&)> execute;
};

// Enhanced CLI parser and execution
class CommandLineInterface {
public:
    CommandLineInterface();
    ~CommandLineInterface();
    
    // Register a command
    void registerCommand(const CLICommand& command);
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Execute the parsed command
    bool execute(const std::string& commandName);
    
    // Print help for a specific command
    void printCommandHelp(const std::string& commandName) const;
    
    // Print all commands help
    void printAllHelp() const;
    
    // Print version and build information
    void printVersion() const;
    
    // Print configuration summary
    void printConfigSummary(const Config& config) const;
    
    // Initialize and cleanup
    bool initialize(const std::string& configFile = "configs/default.cfg");
    void cleanup();
    
    // Get the brain instance for advanced operations
    std::shared_ptr<Brain> getBrain() const;
    
    // Get the experiment runner
    std::shared_ptr<ExperimentRunner> getExperimentRunner() const;
    
    // Get the current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get the current command name
    std::string getCurrentCommand() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced configuration management commands
namespace ConfigCommands {
    bool configList(const Config& config, const std::vector<std::string>& args);
    bool configExport(const Config& config, const std::vector<std::string>& args);
    bool configImport(const Config& config, const std::vector<std::string>& args);
    bool configValidate(const Config& config, const std::vector<std::string>& args);
    bool configCompare(const Config& config, const std::vector<std::string>& args);
    bool configInfo(const Config& config, const std::vector<std::string>& args);
}

// Performance profiling and benchmarking
namespace BenchmarkCommands {
    bool benchmarkRun(const Config& config, const std::vector<std::string>& args);
    bool benchmarkCompare(const Config& config, const std::vector<std::string>& args);
    bool benchmarkProfile(const Config& config, const std::vector<std::string>& args);
    bool benchmarkStats(const Config& config, const std::vector<std::string>& args);
}

// Advanced debugging and monitoring
namespace DebugCommands {
    bool debugMonitor(const Config& config, const std::vector<std::string>& args);
    bool debugTrace(const Config& config, const std::vector<std::string>& args);
    bool debugProfile(const Config& config, const std::vector<std::string>& args);
    bool debugAnalyze(const Config& config, const std::vector<std::string>& args);
    bool debugInspect(const Config& config, const std::vector<std::string>& args);
}

// Memory system inspection
namespace MemoryCommands {
    bool memoryInspect(const Config& config, const std::vector<std::string>& args);
    bool memoryStats(const Config& config, const std::vector<std::string>& args);
    bool memoryCompare(const Config& config, const std::vector<std::string>& args);
    bool memoryBenchmark(const Config& config, const std::vector<std::string>& args);
}

// Neuromodulation analysis
namespace NeuromodulationCommands {
    bool neuromodulationAnalyze(const Config& config, const std::vector<std::string>& args);
    bool neuromodulationProfile(const Config& config, const std::vector<std::string>& args);
    bool neuromodulationStats(const Config& config, const std::vector<std::string>& args);
    bool neuromodulationTrace(const Config& config, const std::vector<std::string>& args);
}

// Development stage management
namespace DevelopmentCommands {
    bool developmentStatus(const Config& config, const std::vector<std::string>& args);
    bool developmentStage(const Config& config, const std::vector<std::string>& args);
    bool developmentRun(const Config& config, const std::vector<std::string>& args);
    bool developmentProgress(const Config& config, const std::vector<std::string>& args);
}

// Experiment management
namespace ExperimentCommands {
    bool experimentList(const Config& config, const std::vector<std::string>& args);
    bool experimentRun(const Config& config, const std::vector<std::string>& args);
    bool experimentRunAll(const Config& config, const std::vector<std::string>& args);
    bool experimentStatus(const Config& config, const std::vector<std::string>& args);
    bool experimentSave(const Config& config, const std::vector<std::string>& args);
    bool experimentCompare(const Config& config, const std::vector<std::string>& args);
    bool experimentAnalyze(const Config& config, const std::vector<std::string>& args);
}

// Visualization and output generation
namespace VisualizationCommands {
    bool visualizationGenerate(const Config& config, const std::vector<std::string>& args);
    bool visualizationExport(const Config& config, const std::vector<std::string>& args);
    bool visualizationStats(const Config& config, const std::vector<std::string>& args);
    bool visualizationAnimate(const Config& config, const std::vector<std::string>& args);
}

// System initialization and cleanup
bool initializeSystem(const std::string& configFile, std::shared_ptr<Brain>& brain, std::shared_ptr<ExperimentRunner>& experimentRunner);
void cleanupSystem(std::shared_ptr<Brain>& brain, std::shared_ptr<ExperimentRunner>& experimentRunner);

} // namespace nlm
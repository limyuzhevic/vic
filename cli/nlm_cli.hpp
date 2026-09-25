// NLM (熙然) - Neural Learning Machine CLI
// Comprehensive command-line interface for brain simulation

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/NeuralRegion.hpp"
#include "environment/Environment.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "benchmark.cpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <set>

namespace nlm {

// Forward declarations for brain state
class Brain;
class Neuron;
class Synapse;
class NeuralRegion;

class NLMCLI {
public:
    NLMCLI(int argc, char** argv);
    ~NLMCLI() = default;
    
    bool execute();
    
    // Command execution methods
    bool executeRunCommand(const std::map<std::string, std::string>& args);
    bool executeInspectCommand(const std::map<std::string, std::string>& args);
    bool executeConfigCommand(const std::map<std::string, std::string>& args);
    bool executeBenchmarkCommand(const std::map<std::string, std::string>& args);
    bool executeReplayCommand(const std::map<std::string, std::string>& args);
    bool executeCheckpointCommand(const std::map<std::string, std::string>& args);
    bool executeDebugCommand(const std::map<std::string, std::string>& args);
    bool executeSimulateCommand(const std::map<std::string, std::string>& args);
    bool executeExploreCommand(const std::map<std::string, std::string>& args);
    
    // Helper methods
    std::shared_ptr<Config> loadConfiguration(const std::string& configFile);
    void printBanner() const;
    void printUsage(const std::string& programName) const;
    void printSubcommandHelp(const std::string& subcommand) const;
    void printVersion() const;
    
private:
    int argc;
    char** argv;
    std::string programName;
    std::string subcommand;
    std::map<std::string, std::string> args;
    bool showHelp;
    bool showVersion;
    bool quietMode;
    std::string verboseLevel;
    std::string outputFile;
    
    // Configuration management
    std::shared_ptr<Config> config;
    std::string configFile;
    
    // Execution state
    bool validationPassed;
    std::chrono::high_resolution_clock::time_point startTime;
    
    // Internal methods
    void parseArgs();
    bool parseSubcommand();
    void parseArgsInternal();
    void parseShortOptions();
    void parseLongOptions();
    void parseKeyValueArgs();
    bool validateArgs() const;
    std::string trim(const std::string& str) const;
    bool startsWith(const std::string& str, const std::string& prefix) const;
    
    // File operations
    bool saveBrainState(const std::string& filename, std::shared_ptr<Brain> brain);
    bool loadBrainState(const std::string& filename, std::shared_ptr<Brain>& brain);
    bool exportBrainState(const std::string& format, const std::string& filename, std::shared_ptr<Brain> brain);
    bool exportStatistics(const std::string& format, const std::string& filename, const std::map<std::string, double>& stats);
    
    // Subcommand implementations
    bool handleRunSubcommand();
    bool handleInspectSubcommand();
    bool handleConfigSubcommand();
    bool handleBenchmarkSubcommand();
    bool handleReplaySubcommand();
    bool handleCheckpointSubcommand();
    bool handleDebugSubcommand();
    bool handleSimulateSubcommand();
    bool handleExploreSubcommand();
    
    // State management
    std::map<std::string, std::string> loadConfigFromFile(const std::string& filename);
    bool saveConfigToFile(const std::string& filename);
    bool validateConfig(const std::map<std::string, std::string>& config) const;
    
    // Benchmark support
    struct BenchmarkResult {
        size_t neurons;
        size_t synapses;
        size_t steps;
        double totalTime;
        double stepsPerSecond;
        double spikesPerStep;
        double synapsesPerNeuron;
    };
    
    BenchmarkResult runBenchmark(size_t neuronCount, size_t steps);
    void printBenchmarkResults(const std::vector<BenchmarkResult>& results);
};

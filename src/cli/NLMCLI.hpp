#pragma once

#include "../core/Config/Config.hpp"
#include "../agent/AgentBrain.hpp"
#include "../memory/IMemorySystem.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <thread>
#include <iomanip>
#include <fstream>
#include <sstream>

namespace nlm {

// Command line argument parser
class CommandLineParser {
public:
    struct CommandArgs {
        std::string configFile;
        std::string outputFile;
        std::string experimentFile;
        std::string mode; // "demo", "run", "batch", "sweep", "analyze"
        SimulationStep maxSteps;
        double timestep;
        uint64_t seed;
        bool verbose;
        bool quiet;
        bool enableLogging;
        bool enableCheckpointing;
        bool enableDevelopment;
        bool enableNeuromodulation;
        bool enableMemorySystems;
        bool enablePrediction;
        bool enableCognition;
        std::map<std::string, std::string> configOverrides;
        std::vector<std::string> experimentNames;
    };
    
    static CommandArgs parse(int argc, char** argv);
    static void printUsage(const char* programName);
    static void printVersion();
};

// Advanced CLI interface for NLM
class NLMCLI {
public:
    NLMCLI();
    ~NLMCLI();
    
    // Main CLI entry point
    int run(int argc, char** argv);
    
    // Run demo mode (quick integration test)
    bool runDemoMode(const CommandLineParser::CommandArgs& args);
    
    // Run single experiment
    bool runExperiment(const CommandLineParser::CommandArgs& args);
    
    // Run batch of experiments
    bool runBatchMode(const CommandLineParser::CommandArgs& args);
    
    // Run parameter sweep
    bool runParameterSweep(const CommandLineParser::CommandArgs& args);
    
    // Analyze experiment results
    bool analyzeResults(const CommandLineParser::CommandArgs& args);
    
    // Export results in various formats
    bool exportResults(const CommandLineParser::CommandArgs& args);
    
    // Configuration management
    bool loadConfig(const std::string& filepath);
    bool saveConfig(const std::string& filepath);
    bool printConfig() const;
    bool setConfigValue(const std::string& key, const std::string& value);
    bool deleteConfigValue(const std::string& key);
    bool listConfigKeys() const;
    
    // Memory system management
    bool printMemoryStatus() const;
    bool enableMemorySystem(const std::string& name, bool enable);
    bool configureMemorySystem(const std::string& name, const std::map<std::string, std::string>& config);
    
    // Experiment management
    bool listExperiments() const;
    bool createExperiment(const std::string& name, const std::map<std::string, std::string>& config);
    bool deleteExperiment(const std::string& name);
    
    // System control
    bool printSystemInfo() const;
    bool printPerformanceStats() const;
    bool validateConfiguration() const;
    bool resetSystem() const;
    
    // Interactive mode
    bool runInteractiveMode();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string formatTime(std::chrono::duration<double> duration) const;
    std::string formatMemorySize(size_t bytes) const;
    std::string getSystemStatusString() const;
    bool initializeFromConfig(const std::string& configFile);
    bool createMemoryManager(std::shared_ptr<AgentBrain> agent);
};

// Helper functions for CLI
void printBanner();
void printError(const std::string& message);
void printWarning(const std::string& message);
void printInfo(const std::string& message);
void printSuccess(const std::string& message);
void printSeparator();

} // namespace nlm
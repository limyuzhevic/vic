// Phase 2 Command Line Interface
#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <iostream>

namespace nlm {
namespace phase2 {

/**
 * Command line argument parsing and help system
 */
class Phase2CLI {
public:
    struct CommandLineOptions {
        // General options
        bool help;
        bool version;
        std::string configFile;
        std::string outputFile;
        std::string checkpointFile;
        std::string logLevel;
        std::string logFile;
        bool enableColors;
        
        // Test selection
        std::vector<std::string> runTests;
        std::vector<std::string> skipTests;
        bool listTests;
        
        // Performance options
        bool enableBenchmark;
        bool enableProfiling;
        bool enableVerbose;
        uint32_t benchmarkIterations;
        
        // Simulation options
        uint64_t randomSeed;
        double timestep;
        uint64_t neuronCount;
        uint64_t regionCount;
        double connectionProbability;
        
        // Plasticity options
        float stdpLtpWeight;
        float stdpLtdWeight;
        float stdpTau;
        float synaptogenesisRate;
        float pruningRate;
        
        // Test-specific options
        uint32_t plasticitySteps;
        uint32_t connectivityTestSteps;
        uint32_t stdpTestTrials;
        double learningThreshold;
        
        // File I/O options
        bool saveCheckpoint;
        bool loadCheckpoint;
        bool saveResults;
        bool loadResults;
        
        CommandLineOptions() : 
            help(false),
            version(false),
            configFile(""),
            outputFile(""),
            checkpointFile(""),
            logLevel("INFO"),
            logFile(""),
            enableColors(true),
            listTests(false),
            enableBenchmark(false),
            enableProfiling(false),
            enableVerbose(false),
            benchmarkIterations(1),
            randomSeed(42),
            timestep(0.001),
            neuronCount(500),
            regionCount(1),
            connectionProbability(0.15),
            stdpLtpWeight(0.02f),
            stdpLtdWeight(0.015f),
            stdpTau(20.0f),
            synaptogenesisRate(0.0001f),
            pruningRate(0.00001f),
            plasticitySteps(1000),
            connectivityTestSteps(50),
            stdpTestTrials(50),
            learningThreshold(0.01f),
            saveCheckpoint(false),
            loadCheckpoint(false),
            saveResults(false),
            loadResults(false) {}
    };
    
    Phase2CLI();
    ~Phase2CLI();
    
    // Disable copying
    Phase2CLI(const Phase2CLI&) = delete;
    Phase2CLI& operator=(const Phase2CLI&) = delete;
    
    // Parse command line arguments
    bool parseArgs(int argc, char** argv);
    
    // Get parsed options
    const CommandLineOptions& getOptions() const { return options_; }
    
    // Print help information
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
    // Print available tests
    void printAvailableTests() const;
    
    // Validate command line options
    bool validateOptions() const;
    
    // Get error message for last validation failure
    std::string getErrorMessage() const { return errorMessage_; }
    
    // Register custom command line options
    void registerOption(const std::string& name, const std::string& description,
                       std::function<void(const std::string&)> callback);
    
    // Register test-specific command line options
    void registerTestOptions();
    
    // Convert options to configuration
    Phase2TestConfig toTestConfig() const;
    
    // Get the global CLI instance
    static Phase2CLI& getInstance();
    
    // Set global CLI instance
    static void setInstance(std::shared_ptr<Phase2CLI> cli);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    CommandLineOptions options_;
    std::string errorMessage_;
    
    // Internal helper functions
    void initializeDefaultOptions();
    void parseGeneralOptions(int argc, char** argv);
    void parseTestOptions(int argc, char** argv);
    void parsePerformanceOptions(int argc, char** argv);
    void parseSimulationOptions(int argc, char** argv);
    void parsePlasticityOptions(int argc, char** argv);
    void parseFileOptions(int argc, char** argv);
    bool parseOption(const std::string& option, const std::string& value);
    void printOptionHelp() const;
    void printTestHelp() const;
    void printPerformanceHelp() const;
    void printSimulationHelp() const;
    void printPlasticityHelp() const;
    void printFileHelp() const;
};

} // namespace phase2
} // namespace nlm
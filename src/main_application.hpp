// NLM Main Application
// Phase 2: Real Neural Computation
//
// Main application class for NLM Phase 2 with enhanced modularity,
// command-line parsing, and experiment management.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <map>

namespace nlm {

// Forward declarations
class Brain;
class Config;

/**
 * Application configuration manager
 */
struct AppConfig {
    // Core settings
    std::string configFile;
    std::string outputFile;
    std::string logLevel;
    bool verbose;
    bool debug;
    
    // Experiment settings
    std::string experimentType;
    bool runAllTests;
    std::vector<std::string> testNames;
    
    // Performance settings
    bool enableBenchmark;
    size_t benchmarkNeuronCount;
    size_t benchmarkSteps;
    
    // Output settings
    bool quiet;
    bool jsonOutput;
    std::string outputFormat;
    
    AppConfig()
        : configFile("configs/default.cfg"), outputFile(""), logLevel("Info"),
          verbose(false), debug(false), experimentType("default"),
          runAllTests(false), enableBenchmark(false),
          benchmarkNeuronCount(500), benchmarkSteps(1000),
          quiet(false), jsonOutput(false), outputFormat("text") {}
};

/**
 * Main NLM application class
 * Provides the core application structure for NLM Phase 2
 */
class NLMApplication {
public:
    NLMApplication(int argc, char** argv);
    ~NLMApplication();
    
    // Disable copying, enable moving
    NLMApplication(const NLMApplication&) = delete;
    NLMApplication& operator=(const NLMApplication&) = delete;
    NLMApplication(NLMApplication&&) noexcept = default;
    NLMApplication& operator=(NLMApplication&&) noexcept = default;
    
    // Application lifecycle
    void run();
    
    // Configuration access
    const AppConfig& getConfig() const { return config; }
    
    // Static help function
    static void printHelp();
    
private:
    int argc;
    char** argv;
    AppConfig config;
    
    // Helper methods
    void parseArguments();
    void loadConfigFile();
    void applyOverrides();
    void setOption(const std::string& key, const std::string& value);
    
    // Experiment execution
    void runExperiments(std::shared_ptr<Brain> brain);
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runLearningExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
    
    // Benchmark execution
    void runBenchmarks();
    void runBenchmark(size_t neuronCount, size_t steps);
    
    // Output and utilities
    void printBanner() const;
    void logConfiguration(std::shared_ptr<Config> nlmConfig) const;
};

} // namespace nlm



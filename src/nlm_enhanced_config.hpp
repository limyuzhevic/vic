// NLM - Enhanced Configuration Command Line Parser
// Advanced argument parsing for the NLM brain simulation
// Supports complex configuration scenarios and better error handling

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlm {

/**
 * Configuration Argument Parser
 * 
 * Advanced command-line argument parsing for NLM configuration
 * Features:
 * - Support for long-form arguments (--option=value, --option value)
 * - Support for short-form arguments (-o value)
 * - Configuration file inclusion
 * - Parameter validation
 * - Error reporting with helpful messages
 */
class ConfigArgumentParser {
public:
    struct ParsedArgs {
        std::unordered_map<std::string, std::string> args;
        std::vector<std::string> unknownArgs;
        std::vector<std::string> configFiles;
        bool verbose = false;
        bool quiet = false;
        bool help = false;
        bool version = false;
        std::string outputFile;
        std::string logLevel;
        size_t maxSteps = 0;
        double timestep = 0.0;
        size_t neuronCount = 0;
        size_t regionCount = 0;
        float connectionProb = 0.0f;
        std::string experimentType;
        bool saveConfig = false;
        bool benchmark = false;
        double benchmarkDuration = 0.0;
    };
    
    static ParsedArgs parse(int argc, char** argv);
private:
    static void handleShortArg(char shortArg, char** argv, int& i, ParsedArgs& result);
};

/**
 * Enhanced Configuration Manager
 * 
 * Improved configuration system with validation, save/load capabilities,
 * and better default value management.
 */
class EnhancedConfigManager {
public:
    struct ValidationResult {
        bool valid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    EnhancedConfigManager();
    
    void setVerbose(bool verbose) { verbose_ = verbose; }
    void setQuiet(bool quiet) { quiet_ = quiet; }
    
    // Apply parsed command line arguments
    bool applyParsedArgs(const ConfigArgumentParser::ParsedArgs& args);
    
    // Validate configuration
    ValidationResult validateConfiguration() const;
    
    // Get configuration summary for logging
    std::string getConfigurationSummary() const;
    
    // Save configuration to file
    bool saveConfiguration(const std::string& configFile) const;
    
private:
    bool verbose_ = false;
    bool quiet_ = false;
    // Additional configuration storage would go here
};

/**
 * Progress Reporter
 * 
 * Enhanced progress reporting with multiple output formats,
 * timing information, and progress bars.
 */
class ProgressReporter {
public:
    ProgressReporter(bool verbose = false, bool quiet = false, 
                     bool showProgress = true, bool showTiming = true);
    
    void start(const std::string& taskName);
    
    void update(size_t current, size_t total, const std::string& message = "");
    
    void complete(const std::string& result = "Completed");
    
    void log(const std::string& message, bool forceVerbose = false);
    
    void error(const std::string& message);
    
    void warning(const std::string& message);
    
    std::string getFormattedTime(double seconds) const;
    
private:
    bool verbose_ = false;
    bool quiet_ = false;
    bool showProgress_ = true;
    bool showTiming_ = true;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::string currentTask_;
};

/**
 * Benchmark Manager
 * 
 * Performance benchmarking capabilities for NLM experiments
 * Supports multiple benchmarks, timing, and performance metrics.
 */
class BenchmarkManager {
public:
    struct BenchmarkResult {
        std::string name;
        double duration;
        double cpuTime;
        double wallClockTime;
        size_t stepsCompleted;
        size_t neuronsUsed;
        float avgFiringRate;
        std::string details;
    };
    
    BenchmarkManager();
    
    void configure(bool enabled, double duration = 0.0);
    
    void startBenchmark(const std::string& name);
    
    void incrementSteps(size_t count = 1);
    
    void completeBenchmark(const std::string& details = "");
    
    void setMetrics(size_t neurons, float avgFiringRate);
    
    const std::vector<BenchmarkResult>& getResults() const { return benchmarkResults_; }
    
    void printAllResults() const;
    
private:
    void printResult(const BenchmarkResult& result) const;
    std::string getFormattedTime(double seconds) const;
    
    bool enabled_ = false;
    double duration_ = 0.0;
    std::string currentBenchmark_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::system_clock::time_point cpuStartTime_;
    size_t stepsCompleted_ = 0;
    size_t neuronsUsed_ = 0;
    float avgFiringRate_ = 0.0f;
    bool verbose_ = false;
    std::vector<BenchmarkResult> benchmarkResults_;
};

/**
 * Experiment Manager
 * 
 * Enhanced experiment management with support for different experiment types,
 * parameter validation, and result management.
 */
class ExperimentManager {
public:
    enum class ExperimentType {
        BASIC_CONNECTIVITY,
        PLASTICITY_LEARNING,
        STDP_VERIFICATION,
        PHASE6_INTEGRATION,
        PHASE4_COGNITION,
        CUSTOM
    };
    
    struct ExperimentConfig {
        ExperimentType type;
        size_t steps = 1000;
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float connectionProbability = 0.15f;
        uint64_t randomSeed = 42;
        double timestep = 0.001;
        bool enablePlasticity = true;
        bool enableSTDP = true;
        bool enableDevelopment = true;
        bool enableNeuromodulation = true;
        bool enableMemory = true;
        bool enablePrediction = true;
        bool enableCognition = true;
        std::map<std::string, std::string> customParameters;
    };
    
    ExperimentManager(ProgressReporter& progress, BenchmarkManager& benchmark);
    
    // Run different experiment types
    bool runExperiment(const ExperimentConfig& config);
    
    // Parse experiment type from string
    ExperimentType parseExperimentType(const std::string& typeStr);
    
    std::string getExperimentName(ExperimentType type);
    
private:
    bool runBasicConnectivityExperiment(const ExperimentConfig& config);
    bool runPlasticityExperiment(const ExperimentConfig& config);
    bool runStdpExperiment(const ExperimentConfig& config);
    bool runPhase6IntegrationExperiment(const ExperimentConfig& config);
    bool runPhase4CognitionExperiment(const ExperimentConfig& config);
    bool runCustomExperiment(const ExperimentConfig& config);
    
    ProgressReporter& progress_;
    BenchmarkManager& benchmark_;
};

// Function declarations
void printHelp();
void printVersion();

} // namespace nlm
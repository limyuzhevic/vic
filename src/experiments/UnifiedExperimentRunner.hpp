// Unified Experiment Runner for NLM
// Combines multiple experiment modes into a single, flexible framework

#ifndef NLM_UNIFIED_EXPERIMENT_RUNNER_HPP
#define NLM_UNIFIED_EXPERIMENT_RUNNER_HPP

#include "ExperimentRunner.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace nlm {

// Experiment configuration
struct UnifiedExperimentConfig {
    std::string name;
    std::string mode;
    uint64_t seed;
    int64_t neuronCount;
    int64_t regionCount;
    double timestep;
    float connectionProbability;
    float stdpLtpWeight;
    float stdpLtdWeight;
    float synaptogenesisRate;
    float pruningRate;
    size_t maxSteps;
    std::string outputFile;
    bool verbose;
    bool saveResults;
    std::vector<std::string> metricsToTrack;
    
    UnifiedExperimentConfig() : seed(42), neuronCount(500), regionCount(1), 
                               timestep(0.001), connectionProbability(0.15f),
                               stdpLtpWeight(0.02f), stdpLtdWeight(0.015f),
                               synaptogenesisRate(0.0001f), pruningRate(0.00001f),
                               maxSteps(1000), verbose(false), saveResults(false) {}
};

// Experiment preset configurations
class ExperimentPresets {
public:
    static UnifiedExperimentConfig getMinimal() {
        UnifiedExperimentConfig config;
        config.name = "Minimal Test";
        config.mode = "minimal";
        config.neuronCount = 100;
        config.regionCount = 1;
        config.maxSteps = 100;
        config.stdDevTolerance = 0.01f;
        config.enableSynapticPlasticity = false;
        config.enableDevelopment = false;
        return config;
    }
    
    static UnifiedExperimentConfig getFast() {
        UnifiedExperimentConfig config;
        config.name = "Fast Test";
        config.mode = "fast";
        config.neuronCount = 200;
        config.regionCount = 1;
        config.maxSteps = 500;
        config.timestep = 0.0005;
        config.stdDevTolerance = 0.02f;
        config.enableSynapticPlasticity = true;
        config.enableDevelopment = false;
        return config;
    }
    
    static UnifiedExperimentConfig getComprehensive() {
        UnifiedExperimentConfig config;
        config.name = "Comprehensive Test";
        config.mode = "comprehensive";
        config.neuronCount = 1000;
        config.regionCount = 3;
        config.maxSteps = 2000;
        config.stdDevTolerance = 0.05f;
        config.enableSynapticPlasticity = true;
        config.enableDevelopment = true;
        return config;
    }
    
    static UnifiedExperimentConfig getPhase4() {
        UnifiedExperimentConfig config;
        config.name = "Phase 4 Integration";
        config.mode = "phase4";
        config.neuronCount = 500;
        config.regionCount = 2;
        config.maxSteps = 1500;
        config.stdDevTolerance = 0.03f;
        config.enableSynapticPlasticity = true;
        config.enableDevelopment = true;
        config.enableMemory = true;
        config.enableNeuromodulation = true;
        config.enablePrediction = true;
        config.enableCognition = true;
        return config;
    }
    
    static UnifiedExperimentConfig getPhase6() {
        UnifiedExperimentConfig config;
        config.name = "Phase 6 Full Integration";
        config.mode = "phase6";
        config.neuronCount = 750;
        config.regionCount = 3;
        config.maxSteps = 3000;
        config.stdDevTolerance = 0.02f;
        config.enableSynapticPlasticity = true;
        config.enableDevelopment = true;
        config.enableMemory = true;
        config.enableNeuromodulation = true;
        config.enablePrediction = true;
        config.enableCognition = true;
        config.enableCheckpointing = true;
        config.enableReplay = true;
        return config;
    }
    
private:
    UnifiedExperimentConfig() {}
};

// Unified Experiment Result
struct UnifiedExperimentResult {
    std::string experimentName;
    std::string mode;
    uint64_t seed;
    SimulationStep startStep;
    SimulationStep endStep;
    size_t totalSteps;
    size_t initialNeuronCount;
    size_t finalNeuronCount;
    size_t initialSynapseCount;
    size_t finalSynapseCount;
    float initialAvgFiringRate;
    float finalAvgFiringRate;
    float totalSpikeCount;
    std::map<std::string, double> metrics;
    std::vector<std::string> notes;
    bool success;
    std::chrono::duration<double> wallClockTime;
    
    // For compatibility with existing code
    size_t getMetric(const std::string& name, size_t defaultValue = 0) const {
        auto it = metrics.find(name);
        if (it != metrics.end()) {
            return static_cast<size_t>(it->second);
        }
        return defaultValue;
    }
    
    double getMetric(const std::string& name, double defaultValue = 0.0) const {
        auto it = metrics.find(name);
        if (it != metrics.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    float getMetric(const std::string& name, float defaultValue = 0.0f) const {
        auto it = metrics.find(name);
        if (it != metrics.end()) {
            return static_cast<float>(it->second);
        }
        return defaultValue;
    }
};

// Unified Experiment Runner Class
class UnifiedExperimentRunner {
public:
    UnifiedExperimentRunner() = default;
    ~UnifiedExperimentRunner() = default;
    
    // Configuration management
    void setConfig(const UnifiedExperimentConfig& config) { currentConfig = config; }
    const UnifiedExperimentConfig& getConfig() const { return currentConfig; }
    
    // Run experiment with current configuration
    UnifiedExperimentResult run() {
        return runExperiment(currentConfig);
    }
    
    // Run experiment with specific configuration
    UnifiedExperimentResult runExperiment(const UnifiedExperimentConfig& config);
    
    // Set brain configuration
    void setBrainConfig(std::shared_ptr<Config> config) {
        brainConfig = config;
    }
    
    // Set environment configuration
    void setEnvironmentConfig(std::shared_ptr<Config> config) {
        environmentConfig = config;
    }
    
    // Get results summary
    std::string getResultsSummary(const UnifiedExperimentResult& result) const;
    
    // Save results to file
    bool saveResults(const UnifiedExperimentResult& result, const std::string& filename) const;
    
    // Load configuration from file
    bool loadConfig(const std::string& filename);
    
    // Save configuration to file
    bool saveConfig(const std::string& filename) const;
    
    // Get available presets
    static std::vector<std::string> getAvailablePresets() {
        return {"minimal", "fast", "comprehensive", "phase4", "phase6"};
    }
    
    // Get preset configuration
    static UnifiedExperimentConfig getPreset(const std::string& presetName);
    
    // Print help information
    static void printHelp();
    
    // Print version information
    static void printVersion();

private:
    UnifiedExperimentConfig currentConfig;
    std::shared_ptr<Config> brainConfig;
    std::shared_ptr<Config> environmentConfig;
    
    // Helper methods
    std::shared_ptr<Brain> createBrain() const;
    std::shared_ptr<Environment> createEnvironment() const;
    void applyConfigToBrain(std::shared_ptr<Brain> brain) const;
    void applyConfigToEnvironment(std::shared_ptr<Environment> environment) const;
    void recordInitialMetrics(std::shared_ptr<Brain> brain, UnifiedExperimentResult& result) const;
    void recordFinalMetrics(std::shared_ptr<Brain> brain, UnifiedExperimentResult& result) const;
    void runExperimentSteps(std::shared_ptr<Brain> brain, SimulationStep maxSteps, 
                           UnifiedExperimentResult& result) const;
    void cleanupBrain(std::shared_ptr<Brain> brain) const;
};

// Command-line interface for unified runner
class UnifiedRunnerCLI {
public:
    static int run(int argc, char** argv);
    
private:
    static UnifiedExperimentConfig parseCommandLine(int argc, char** argv);
    static void printUsage(const std::string& programName);
    static void printVersion();
};

} // namespace nlm

#endif // NLM_UNIFIED_EXPERIMENT_RUNNER_HPP

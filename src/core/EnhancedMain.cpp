// Refactored NLM (熙然) - Neural Learning Machine
// Improved Main Demo with Modular Architecture
//
// This version refactors the monolithic main.cpp into modular components
// with better error handling, configuration management, and user experience.
// Addresses bugs and improves code quality for easier maintenance and extension.

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <filesystem>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/DemoFramework.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

namespace nlm {

// Forward declarations
class LearningExperiment;
class DemoManager;

// Learning Experiment results structure (improved version)
struct LearningResult {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    bool learningDetected;
    float weightChangePercentage;
    float learningConfidence;
    
    LearningResult(std::shared_ptr<Brain> b, uint64_t s)
        : brain(b), seed(s), learningDetected(false), weightChangePercentage(0.0f), learningConfidence(0.0f) {}
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
    bool validateResults() const;
};

// Core demo functions (modularized)
namespace demos {
    
    // Basic connectivity test
    bool runBasicConnectivityDemo(std::shared_ptr<Brain> brain, 
                                  SimulationStep steps,
                                  bool verbose = false,
                                  ProgressMonitor* progress = nullptr);
    
    // Plasticity learning experiment
    bool runPlasticityLearningDemo(std::shared_ptr<Brain> brain,
                                   SimulationStep steps,
                                   bool verbose = false,
                                   ProgressMonitor* progress = nullptr);
    
    // STDP verification test
    bool runStdpVerificationDemo(std::shared_ptr<Brain> brain,
                                 SimulationStep steps,
                                 bool verbose = false,
                                 ProgressMonitor* progress = nullptr);
    
    // Phase 6 integration demo
    bool runPhase6IntegrationDemo(std::shared_ptr<Brain> brain,
                                  const DemoOptions& options,
                                  bool verbose = false,
                                  ProgressMonitor* progress = nullptr);
    
    // Text teaching demo
    bool runTextDemo(std::shared_ptr<Brain> brain,
                     const DemoOptions& options,
                     bool verbose = false,
                     ProgressMonitor* progress = nullptr);
    
    // Benchmark demo
    bool runBenchmarkDemo(std::shared_ptr<Brain> brain,
                         const std::vector<std::pair<size_t, size_t>>& benchmarks,
                         bool verbose = false,
                         ProgressMonitor* progress = nullptr);
} // namespace demos

// Configuration management utilities
namespace config {
    
    // Load configuration from file with error handling
    std::shared_ptr<Config> loadConfig(const std::string& filename);
    
    // Parse command line arguments into configuration
    bool parseCommandLineArgs(int argc, char** argv, DemoOptions& options);
    
    // Merge configurations
    void mergeConfigs(std::shared_ptr<Config> base, const DemoOptions& options);
    
    // Validate configuration
    bool validateConfig(const std::shared_ptr<Config>& config);
    
    // Print configuration summary
    void printConfigSummary(const std::shared_ptr<Config>& config);
} // namespace config

// Result management and reporting
namespace results {
    
    // Save results to file with proper formatting
    bool saveResults(const std::string& filename, const std::string& results);
    
    // Load results from file
    bool loadResults(const std::string& filename, std::string& results);
    
    // Generate report string
    std::string generateReport(const LearningResult& result);
    
    // Generate summary report
    std::string generateSummaryReport(const std::vector<LearningResult>& results);
} // namespace results

// Error handling utilities
namespace error {
    
    // Handle configuration errors
    void handleConfigError(const std::string& error);
    
    // Handle demo execution errors
    void handleDemoError(const std::string& error, bool fatal = false);
    
    // Validate file access
    bool validateFileAccess(const std::string& path, bool write = false);
} // namespace error

// Main demo runner class
class DemoManager {
public:
    DemoManager();
    ~DemoManager();
    
    // Initialize demo manager with options
    bool initialize(const DemoOptions& options);
    
    // Run the main demo
    bool run(const DemoOptions& options);
    
    // Run learning experiment with full lifecycle
    bool runLearningExperiment(const DemoOptions& options);
    
    // Run all available demos
    bool runAllDemos(const DemoOptions& options);
    
    // Setup brain for demo
    bool setupBrain(std::shared_ptr<Brain>& brain, const DemoOptions& options);
    
    // Run demo with progress monitoring
    bool runDemoWithProgress(const DemoOptions& options, DemoMode mode);
    
    // Save demo results
    bool saveDemoResults(const std::string& prefix, const std::string& results);
    
    // Load checkpoint
    bool loadCheckpoint(std::shared_ptr<Brain> brain, const std::string& path);
    
    // Save checkpoint
    bool saveCheckpoint(std::shared_ptr<Brain> brain, const std::string& path);
    
    // Get demo statistics
    std::string getDemoStatistics() const;
    
    // Cleanup and reset
    void cleanup();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private helper methods
    bool setupLogging(const DemoOptions& options);
    bool setupEnvironment(const DemoOptions& options);
    bool setupProgressMonitor(const DemoOptions& options);
    bool setupPerformanceProfiler(const DemoOptions& options);
    bool setupCheckpointSystem(const DemoOptions& options);
    
    // Demo execution wrappers
    bool runBasicConnectivityDemo(const DemoOptions& options);
    bool runPlasticityLearningDemo(const DemoOptions& options);
    bool runStdpVerificationDemo(const DemoOptions& options);
    bool runPhase6IntegrationDemo(const DemoOptions& options);
    bool runTextDemo(const DemoOptions& options);
    bool runBenchmarkDemo(const DemoOptions& options);
    bool runCustomConfigDemo(const DemoOptions& options);
    
    // Progress reporting
    void reportProgress(const std::string& message, float progress = -1.0f);
    void reportDemoStatus(DemoMode mode, bool success, const std::string& details = "");
    
    // Result aggregation
    std::string aggregateResults(const std::vector<LearningResult>& learningResults) const;
    
    // Validation
    bool validateOptions(const DemoOptions& options) const;
    bool validateBrainState(std::shared_ptr<Brain> brain) const;
}; // class DemoManager

// Global demo manager instance
DemoManager& getDemoManager();

// Convenience functions
bool runLearningDemo(int argc, char** argv);
bool runIntegrationDemo(int argc, char** argv);
bool runBenchmarkDemo(int argc, char** argv);

} // namespace nlm

// Implementation of LearningExperiment methods
void LearningResult::recordInitialState() {
    if (!brain) {
        error::handleConfigError("Cannot record initial state: brain is null");
        return;
    }
    
    initialSynapseCount = brain->getTotalSynapseCount();
    initialWeights.clear();
    
    // Record initial weights from first region
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    NLM_LOG_INFO("Initial state recorded:");
    NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
    if (!initialWeights.empty()) {
        float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float mean = sum / initialWeights.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningResult::recordFinalState() {
    finalWeights.clear();
    
    // Record final weights from first region
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
    }
    
    mostActiveNeurons.clear();
    if (auto* spikeSystem = brain->getSpikeSystem()) {
        mostActiveNeurons = spikeSystem->getMostActiveNeurons(10);
    }
    
    NLM_LOG_INFO("Final state recorded:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    if (!finalWeights.empty()) {
        float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float mean = sum / finalWeights.size();
        NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
    }
}

void LearningResult::computeStatistics() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Learning Experiment Results ===");
    NLM_LOG_INFO("");
    
    if (initialWeights.empty() || finalWeights.empty()) {
        NLM_LOG_ERROR("ERROR: No weights recorded");
        return;
    }
    
    // Compute weight changes
    float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
    float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
    float initialMean = initialSum / initialWeights.size();
    float finalMean = finalSum / finalWeights.size();
    
    weightChangePercentage = std::abs((finalMean - initialMean) / initialMean * 100.0f);
    learningConfidence = weightChangePercentage > 1.0f ? 0.8f : 0.3f; // Simple confidence metric
    
    NLM_LOG_INFO("Weight Statistics:");
    NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
    NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
    NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
    NLM_LOG_INFO("  Change percentage: " + std::to_string(weightChangePercentage) + "%");
    NLM_LOG_INFO("  Learning confidence: " + std::to_string(learningConfidence));
    
    // Count synapses that changed significantly
    size_t strengthened = 0;
    size_t weakened = 0;
    size_t unchanged = 0;
    
    size_t minSize = std::min(initialWeights.size(), finalWeights.size());
    for (size_t i = 0; i < minSize; ++i) {
        float delta = finalWeights[i] - initialWeights[i];
        if (delta > 0.01f) ++strengthened;
        else if (delta < -0.01f) ++weakened;
        else ++unchanged;
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Synaptic Changes:");
    NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
    NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
    NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Spike Activity:");
    NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
    NLM_LOG_INFO("  Most active neurons: " + std::to_string(mostActiveNeurons.size()));
    
    // Determine if learning occurred (improved logic)
    learningDetected = (std::abs(finalMean - initialMean) > 0.001f) ||
                      (strengthened > 0 || weakened > 0) ||
                      weightChangePercentage > 0.5f; // Lower threshold for detection
    
    NLM_LOG_INFO("");
    if (learningDetected) {
        NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        NLM_LOG_INFO("  Learning magnitude: " + std::to_string(weightChangePercentage) + "%");
    } else {
        NLM_LOG_INFO("✗ NO SIGNIFICANT LEARNING: Weights did not change detectably");
        NLM_LOG_INFO("  This may indicate a problem with the network or plasticity parameters");
    }
}

bool LearningResult::validateResults() const {
    if (!brain) {
        NLM_LOG_ERROR("Validation failed: brain is null");
        return false;
    }
    
    if (initialWeights.empty() && finalWeights.empty()) {
        NLM_LOG_WARNING("Validation: No weights recorded - may indicate empty brain");
        return true; // Not a failure, just informational
    }
    
    if (mostActiveNeurons.size() > brain->getTotalNeuronCount()) {
        NLM_LOG_ERROR("Validation failed: mostActiveNeurons count exceeds total neurons");
        return false;
    }
    
    return true;
}

namespace config {
    
    std::shared_ptr<Config> loadConfig(const std::string& filename) {
        auto config = std::make_shared<Config>();
        
        if (filename.empty() || filename == "default") {
            NLM_LOG_INFO("Using default configuration");
            return config;
        }
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Could not open config file: " + filename + ", using defaults");
            return config;
        }
        
        NLM_LOG_INFO("Loading configuration from: " + filename);
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                NLM_LOG_WARNING("Invalid line in config file " + filename + ": " + line);
                continue;
            }
            
            std::string key = line.substr(0, equalsPos);
            std::string value = line.substr(equalsPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            try {
                if (key == "neuron_count") {
                    config->set("neuron_count", static_cast<int64_t>(std::stoul(value)), ConfigSource::File);
                }
                else if (key == "region_count") {
                    config->set("region_count", static_cast<int64_t>(std::stoul(value)), ConfigSource::File);
                }
                else if (key == "connection_probability") {
                    config->set("connection_probability", std::stof(value), ConfigSource::File);
                }
                else if (key == "random_seed") {
                    config->set("random_seed", static_cast<int64_t>(std::stoll(value)), ConfigSource::File);
                }
                else if (key == "simulation_timestep") {
                    config->set("simulation_timestep", std::stod(value), ConfigSource::File);
                }
                else if (key == "stdp_ltp_weight") {
                    config->set("stdp_ltp_weight", std::stof(value), ConfigSource::File);
                }
                else if (key == "stdp_ltd_weight") {
                    config->set("stdp_ltd_weight", std::stof(value), ConfigSource::File);
                }
                else if (key == "stdp_time_constant") {
                    config->set("stdp_time_constant", std::stof(value), ConfigSource::File);
                }
                else if (key == "plasticity_learning_rate") {
                    config->set("plasticity_learning_rate", std::stof(value), ConfigSource::File);
                }
                else if (key == "development_synaptogenesis_rate") {
                    config->set("synaptogenesis_rate", std::stof(value), ConfigSource::File);
                }
                else if (key == "development_pruning_rate") {
                    config->set("pruning_rate", std::stof(value), ConfigSource::File);
                }
                else if (key == "dopamine_baseline") {
                    config->set("dopamine_baseline", std::stof(value), ConfigSource::File);
                }
                else if (key == "reward_discount_factor") {
                    config->set("reward_discount_factor", std::stof(value), ConfigSource::File);
                }
                else if (key == "environment_name") {
                    config->set("environment_name", value, ConfigSource::File);
                }
                else if (key == "environment_width") {
                    config->set("environment_width", static_cast<int64_t>(std::stoul(value)), ConfigSource::File);
                }
                else if (key == "environment_height") {
                    config->set("environment_height", static_cast<int64_t>(std::stoul(value)), ConfigSource::File);
                }
                else if (key == "log_level") {
                    config->set("log_level", value, ConfigSource::File);
                }
                else if (key == "log_to_file") {
                    config->set("log_to_file", (value == "true" || value == "1" || value == "yes"), ConfigSource::File);
                }
                else if (key == "log_filename") {
                    config->set("log_filename", value, ConfigSource::File);
                }
                else if (key == "max_simulation_steps") {
                    config->set("max_simulation_steps", static_cast<int64_t>(std::stoull(value)), ConfigSource::File);
                }
                else if (key == "visualization_enabled") {
                    config->set("visualization_enabled", (value == "true" || value == "1" || value == "yes"), ConfigSource::File);
                }
                else {
                    NLM_LOG_WARNING("Unknown config key in " + filename + ": " + key);
                }
            }
            catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing config key " + key + " in " + filename + ": " + e.what());
            }
        }
        
        file.close();
        NLM_LOG_INFO("Configuration loaded successfully");
        return config;
    }
    
    bool parseCommandLineArgs(int argc, char** argv, DemoOptions& options) {
        try {
            for (int i = 1; i < argc; ++i) {
                std::string arg(argv[i]);
                
                if (arg == "--help" || arg == "-h") {
                    getDemoRunner().printUsage();
                    return false;
                }
                else if (arg == "--version") {
                    getDemoRunner().printVersion();
                    return false;
                }
                else if (arg == "--list") {
                    DemoRunner::listAvailableDemos();
                    return false;
                }
                else if (arg.substr(0, 10) == "--demo=") {
                    std::string demoName = arg.substr(10);
                    // Convert demo name to DemoMode
                    static const std::map<std::string, DemoMode> demoMap = {
                        {"basic", DemoMode::BASIC_CONNECTIVITY},
                        {"plasticity", DemoMode::PLASTICITY_LEARNING},
                        {"stdp", DemoMode::STDP_VERIFICATION},
                        {"phase6", DemoMode::PHASE6_INTEGRATION},
                        {"text", DemoMode::TEXT_DEMO},
                        {"benchmark", DemoMode::BENCHMARK},
                        {"all", DemoMode::ALL_TESTS},
                        {"custom", DemoMode::CUSTOM_CONFIG}
                    };
                    
                    auto it = demoMap.find(demoName);
                    if (it != demoMap.end()) {
                        options.mode = it->second;
                    } else {
                        std::cerr << "Unknown demo: " << demoName << std::endl;
                        return false;
                    }
                }
                else if (arg.substr(0, 12) == "--config=") {
                    options.configFile = arg.substr(12);
                }
                else if (arg == "--verbose" || arg == "-v") {
                    options.verbose = true;
                }
                else if (arg == "--quiet" || arg == "-q") {
                    options.quiet = true;
                }
                else if (arg == "--progress") {
                    options.progress = true;
                }
                else if (arg.substr(0, 11) == "--output=") {
                    options.outputFile = arg.substr(11);
                }
                else if (arg.substr(0, 15) == "--checkpoint=") {
                    options.checkpointPath = arg.substr(15);
                }
                else if (arg.substr(0, 16) == "--log-level=") {
                    options.logLevel = arg.substr(16);
                }
                else if (arg == "--no-colors") {
                    options.enableColors = false;
                }
                else if (arg.substr(0, 16) == "--profile=") {
                    options.profileOutput = arg.substr(16);
                }
                else if (arg == "--continuous") {
                    options.runContinuous = true;
                }
                else if (arg.substr(0, 12) == "--runtime=") {
                    options.runTime = static_cast<uint64_t>(std::stoul(arg.substr(12)));
                }
                else if (arg.substr(0, 17) == "--neuron-count=") {
                    options.neuronCount = static_cast<size_t>(std::stoul(arg.substr(17)));
                }
                else if (arg.substr(0, 17) == "--region-count=") {
                    options.regionCount = static_cast<size_t>(std::stoul(arg.substr(17)));
                }
                else if (arg.substr(0, 23) == "--connection-prob=") {
                    options.connectionProbability = std::stof(arg.substr(23));
                }
                else if (arg == "--advanced") {
                    options.enableAdvanced = true;
                }
                else if (arg.substr(0, 12) == "--steps=") {
                    options.steps = static_cast<uint64_t>(std::stoul(arg.substr(12)));
                }
                else {
                    // Assume it's a custom argument
                    options.customArgs.push_back(arg);
                }
            }
            
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing command line arguments: " << e.what() << std::endl;
            return false;
        }
    }
    
    void mergeConfigs(std::shared_ptr<Config> base, const DemoOptions& options) {
        if (!base) {
            return;
        }
        
        // Override base config with demo options
        if (options.neuronCount > 0) {
            base->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
        }
        
        if (options.regionCount > 0) {
            base->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
        }
        
        if (options.connectionProbability > 0.0f) {
            base->set("connection_probability", options.connectionProbability, ConfigSource::Default);
        }
        
        if (options.steps > 0) {
            base->set("max_simulation_steps", static_cast<int64_t>(options.steps), ConfigSource::Default);
        }
        
        // Set demo-specific defaults
        base->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        base->set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    
    bool validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) {
            NLM_LOG_ERROR("Config validation failed: config is null");
            return false;
        }
        
        // Check required settings
        try {
            int64_t neuronCount = config->getOr<int64_t>("neuron_count", 1000);
            if (neuronCount <= 0) {
                NLM_LOG_ERROR("Invalid neuron_count in config: " + std::to_string(neuronCount));
                return false;
            }
            
            int64_t regionCount = config->getOr<int64_t>("region_count", 1);
            if (regionCount <= 0) {
                NLM_LOG_ERROR("Invalid region_count in config: " + std::to_string(regionCount));
                return false;
            }
            
            float connectionProb = config->getOr<float>("connection_probability", 0.1f);
            if (connectionProb < 0.0f || connectionProb > 1.0f) {
                NLM_LOG_ERROR("Invalid connection_probability in config: " + std::to_string(connectionProb));
                return false;
            }
            
            double timestep = config->getOr<double>("simulation_timestep", 0.001);
            if (timestep <= 0.0) {
                NLM_LOG_ERROR("Invalid simulation_timestep in config: " + std::to_string(timestep));
                return false;
            }
            
        }
        catch (const std::exception& e) {
            NLM_LOG_ERROR("Config validation error: " + std::string(e.what()));
            return false;
        }
        
        return true;
    }
    
    void printConfigSummary(const std::shared_ptr<Config>& config) {
        if (!config) {
            std::cout << "No configuration available" << std::endl;
            return;
        }
        
        std::cout << "Configuration Summary:" << std::endl;
        std::cout << "====================" << std::endl;
        
        try {
            std::cout << "neuron_count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
            std::cout << "region_count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
            std::cout << "connection_probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
            std::cout << "random_seed: " << config->getOr<int64_t>("random_seed", 0) << std::endl;
            std::cout << "simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.0) << std::endl;
            std::cout << "stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.0f) << std::endl;
            std::cout << "stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.0f) << std::endl;
            std::cout << "synaptogenesis_rate: " << config->getOr<float>("synaptogenesis_rate", 0.0f) << std::endl;
            std::cout << "pruning_rate: " << config->getOr<float>("pruning_rate", 0.0f) << std::endl;
            
        }
        catch (const std::exception& e) {
            std::cout << "Error printing config summary: " << e.what() << std::endl;
        }
    }
    
} // namespace config

namespace results {
    
    bool saveResults(const std::string& filename, const std::string& results) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open results file: " << filename << std::endl;
            return false;
        }
        
        file << results;
        file.close();
        
        std::cout << "Results saved to: " << filename << std::endl;
        return true;
    }
    
    bool loadResults(const std::string& filename, std::string& results) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open results file: " << filename << std::endl;
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        results = buffer.str();
        
        file.close();
        return true;
    }
    
    std::string generateReport(const LearningResult& result) {
        std::stringstream report;
        
        report << "=== NLM Learning Experiment Report ===" << std::endl;
        report << "Experiment ID: " << result.seed << std::endl;
        report << "Initial synapses: " << result.initialSynapseCount << std::endl;
        report << "Final spikes: " << result.brain->getTotalSpikeCount() << std::endl;
        report << "Learning detected: " << (result.learningDetected ? "YES" : "NO") << std::endl;
        report << "Weight change: " << result.weightChangePercentage << "%" << std::endl;
        report << "Learning confidence: " << result.learningConfidence << std::endl;
        report << "" << std::endl;
        
        if (!result.initialWeights.empty()) {
            report << "Initial weights: [";
            for (size_t i = 0; i < std::min<size_t>(10, result.initialWeights.size()); ++i) {
                report << result.initialWeights[i];
                if (i < std::min<size_t>(9, result.initialWeights.size() - 1)) {
                    report << ", ";
                }
            }
            if (result.initialWeights.size() > 10) {
                report << ", ... (" << result.initialWeights.size() << " total)";
            }
            report << "]" << std::endl;
        }
        
        if (!result.finalWeights.empty()) {
            report << "Final weights: [";
            for (size_t i = 0; i < std::min<size_t>(10, result.finalWeights.size()); ++i) {
                report << result.finalWeights[i];
                if (i < std::min<size_t>(9, result.finalWeights.size() - 1)) {
                    report << ", ";
                }
            }
            if (result.finalWeights.size() > 10) {
                report << ", ... (" << result.finalWeights.size() << " total)";
            }
            report << "]" << std::endl;
        }
        
        return report.str();
    }
    
    std::string generateSummaryReport(const std::vector<LearningResult>& results) {
        std::stringstream report;
        
        report << "=== NLM Learning Experiment Summary ===" << std::endl;
        report << "Total experiments: " << results.size() << std::endl;
        
        size_t learningExperiments = 0;
        float totalWeightChange = 0.0f;
        size_t experimentsWithWeights = 0;
        
        for (const auto& result : results) {
            if (result.learningDetected) {
                learningExperiments++;
            }
            if (result.weightChangePercentage > 0.0f) {
                totalWeightChange += result.weightChangePercentage;
                experimentsWithWeights++;
            }
        }
        
        report << "Experiments with learning: " << learningExperiments << "/" << results.size()
               << " (" << (results.size() > 0 ? (learningExperiments * 100.0f / results.size()) : 0.0f) << "%)" << std::endl;
        
        if (experimentsWithWeights > 0) {
            report << "Average weight change: " << (totalWeightChange / experimentsWithWeights) << "%" << std::endl;
        }
        
        report << "" << std::endl;
        return report.str();
    }
    
} // namespace results

namespace error {
    
    void handleConfigError(const std::string& error) {
        std::cerr << "Configuration Error: " << error << std::endl;
        // Could log to logger here
    }
    
    void handleDemoError(const std::string& error, bool fatal) {
        std::cerr << "Demo Error: " << error << std::endl;
        
        if (fatal) {
            std::cerr << "Fatal error - exiting." << std::endl;
            std::exit(1);
        }
    }
    
    bool validateFileAccess(const std::string& path, bool write) {
        if (path.empty()) {
            return false;
        }
        
        std::filesystem::path filePath(path);
        std::filesystem::file_status status = std::filesystem::status(filePath);
        
        if (!std::filesystem::exists(status)) {
            // File doesn't exist, check if we can create it
            if (write) {
                std::filesystem::path parentDir = filePath.parent_path();
                if (!parentDir.empty() && !std::filesystem::exists(parentDir)) {
                    return false;
                }
            }
            return !write; // Allow reading non-existent files
        }
        
        if (write) {
            // Check if file is writable
            return std::filesystem::is_regular_file(status) && 
                   std::filesystem::status_permissions(status) & std::filesystem::perms::owner_write;
        } else {
            // Check if file is readable
            return std::filesystem::is_regular_file(status) &&
                   std::filesystem::status_permissions(status) & std::filesystem::perms::owner_read;
        }
    }
    
} // namespace error

// DemoManager implementation
struct DemoManager::Impl {
    DemoOptions currentOptions;
    DemoRunner runner;
    ProgressMonitor progress;
    PerformanceProfiler profiler;
    std::vector<LearningResult> learningResults;
    std::shared_ptr<Config> currentConfig;
    bool initialized;
    bool cleanupRequired;
};

DemoManager::DemoManager() : pImpl(std::make_unique<Impl>()) {
    pImpl->initialized = false;
    pImpl->cleanupRequired = false;
}

DemoManager::~DemoManager() {
    cleanup();
}

bool DemoManager::initialize(const DemoOptions& options) {
    try {
        pImpl->currentOptions = options;
        
        // Validate options
        if (!validateOptions(options)) {
            return false;
        }
        
        // Load configuration
        if (!options.configFile.empty()) {
            pImpl->currentConfig = config::loadConfig(options.configFile);
        } else {
            pImpl->currentConfig = std::make_shared<Config>();
            config::mergeConfigs(pImpl->currentConfig, options);
        }
        
        if (!config::validateConfig(pImpl->currentConfig)) {
            error::handleConfigError("Invalid configuration");
            return false;
        }
        
        // Setup logging
        if (!setupLogging(options)) {
            error::handleConfigError("Failed to setup logging");
            return false;
        }
        
        // Setup progress monitor if needed
        if (options.progress) {
            setupProgressMonitor(options);
        }
        
        // Setup performance profiler if needed
        if (!options.profileOutput.empty()) {
            setupPerformanceProfiler(options);
        }
        
        pImpl->initialized = true;
        NLM_LOG_INFO("DemoManager initialized successfully");
        
        return true;
        
    } catch (const std::exception& e) {
        error::handleConfigError(std::string("Initialization error: ") + e.what());
        return false;
    }
}

bool DemoManager::run(const DemoOptions& options) {
    if (!pImpl->initialized) {
        if (!initialize(options)) {
            return false;
        }
    }
    
    // Setup brain
    std::shared_ptr<Brain> brain;
    if (!setupBrain(brain, options)) {
        error::handleDemoError("Failed to setup brain");
        return false;
    }
    
    // Run the main demo based on mode
    bool success = false;
    
    switch (options.mode) {
        case DemoMode::BASIC_CONNECTIVITY:
            success = runBasicConnectivityDemo(options);
            break;
        case DemoMode::PLASTICITY_LEARNING:
            success = runPlasticityLearningDemo(options);
            break;
        case DemoMode::STDP_VERIFICATION:
            success = runStdpVerificationDemo(options);
            break;
        case DemoMode::PHASE6_INTEGRATION:
            success = runPhase6IntegrationDemo(options);
            break;
        case DemoMode::TEXT_DEMO:
            success = runTextDemo(options);
            break;
        case DemoMode::BENCHMARK:
            success = runBenchmarkDemo(options);
            break;
        case DemoMode::ALL_TESTS:
            success = runAllDemos(options);
            break;
        case DemoMode::CUSTOM_CONFIG:
            success = runCustomConfigDemo(options);
            break;
        default:
            error::handleDemoError("Unknown demo mode");
            return false;
    }
    
    if (success) {
        NLM_LOG_INFO("Demo completed successfully");
    } else {
        NLM_LOG_ERROR("Demo completed with errors");
    }
    
    return success;
}

bool DemoManager::runLearningExperiment(const DemoOptions& options) {
    NLM_LOG_INFO("=== Starting Learning Experiment ===");
    
    // Create brain
    std::shared_ptr<Brain> brain;
    if (!setupBrain(brain, options)) {
        error::handleDemoError("Failed to setup brain for learning experiment");
        return false;
    }
    
    // Create learning experiment
    LearningResult learningResult(brain, options.customArgs.empty() ? 42 : std::stoll(options.customArgs[0]));
    
    // Record initial state
    learningResult.recordInitialState();
    
    // Enable plasticity for learning
    enablePlasticityForLearning(brain);
    
    // Run learning simulation
    if (!runPlasticityLearningDemo(options, brain, &learningResult)) {
        error::handleDemoError("Learning simulation failed");
        return false;
    }
    
    // Record final state
    learningResult.recordFinalState();
    
    // Validate results
    if (!learningResult.validateResults()) {
        error::handleDemoError("Learning results validation failed");
        return false;
    }
    
    // Store results
    pImpl->learningResults.push_back(learningResult);
    
    // Generate and save report
    std::string report = results::generateReport(learningResult);
    std::string summary = results::generateSummaryReport(pImpl->learningResults);
    
    std::string fullResults = report + "\n" + summary;
    
    if (!options.outputFile.empty()) {
        if (!results::saveResults(options.outputFile, fullResults)) {
            NLM_LOG_WARNING("Failed to save results to file");
        }
    }
    
    // Print summary
    NLM_LOG_INFO("=== Learning Experiment Complete ===");
    NLM_LOG_INFO("Learning detected: " + std::string(learningResult.learningDetected ? "YES" : "NO"));
    NLM_LOG_INFO("Weight change: " + std::to_string(learningResult.weightChangePercentage) + "%");
    
    return true;
}

bool DemoManager::runAllDemos(const DemoOptions& options) {
    NLM_LOG_INFO("=== Running All Demos ===");
    
    bool allSuccess = true;
    std::vector<DemoMode> demoOrder = {
        DemoMode::BASIC_CONNECTIVITY,
        DemoMode::STDP_VERIFICATION,
        DemoMode::PLASTICITY_LEARNING,
        DemoMode::PHASE6_INTEGRATION,
        DemoMode::TEXT_DEMO,
        DemoMode::BENCHMARK
    };
    
    for (DemoMode mode : demoOrder) {
        NLM_LOG_INFO("Running demo: " + DemoRunner::getDemoName(mode));
        
        DemoOptions demoOptions = options;
        demoOptions.mode = mode;
        
        bool success = runDemoWithProgress(demoOptions, mode);
        
        if (!success) {
            allSuccess = false;
            NLM_LOG_WARNING("Demo " + DemoRunner::getDemoName(mode) + " failed, continuing with next");
        }
        
        // Brief pause between demos
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    
    return allSuccess;
}

bool DemoManager::setupBrain(std::shared_ptr<Brain>& brain, const DemoOptions& options) {
    try {
        // Create brain with current configuration
        brain = std::make_shared<Brain>(pImpl->currentConfig);
        
        if (!brain->initialize()) {
            error::handleDemoError("Failed to initialize brain");
            return false;
        }
        
        // Validate brain state
        if (!validateBrainState(brain)) {
            error::handleDemoError("Invalid brain state");
            return false;
        }
        
        NLM_LOG_INFO("Brain setup complete: " + std::to_string(brain->getTotalNeuronCount()) + " neurons,"
                    " " + std::to_string(brain->getTotalSynapseCount()) + " synapses");
        
        return true;
        
    } catch (const std::exception& e) {
        error::handleDemoError(std::string("Brain setup error: ") + e.what());
        return false;
    }
}

bool DemoManager::runDemoWithProgress(const DemoOptions& options, DemoMode mode) {
    // Wrapper to run demo with progress monitoring
    bool success = false;
    
    switch (mode) {
        case DemoMode::BASIC_CONNECTIVITY:
            success = runBasicConnectivityDemo(options);
            break;
        case DemoMode::PLASTICITY_LEARNING:
            success = runPlasticityLearningDemo(options);
            break;
        case DemoMode::STDP_VERIFICATION:
            success = runStdpVerificationDemo(options);
            break;
        case DemoMode::PHASE6_INTEGRATION:
            success = runPhase6IntegrationDemo(options);
            break;
        case DemoMode::TEXT_DEMO:
            success = runTextDemo(options);
            break;
        case DemoMode::BENCHMARK:
            success = runBenchmarkDemo(options);
            break;
        case DemoMode::CUSTOM_CONFIG:
            success = runCustomConfigDemo(options);
            break;
        default:
            error::handleDemoError("Unknown demo mode for progress wrapper");
            return false;
    }
    
    return success;
}

bool DemoManager::saveDemoResults(const std::string& prefix, const std::string& results) {
    // Generate timestamped filename
    std::string timestamp = DemoRunner::getTimestamp();
    std::string filename = prefix + "_results_" + timestamp + ".txt";
    
    return results::saveResults(filename, results);
}

bool DemoManager::loadCheckpoint(std::shared_ptr<Brain> brain, const std::string& path) {
    if (!brain) {
        error::handleDemoError("Cannot load checkpoint: brain is null");
        return false;
    }
    
    if (path.empty()) {
        error::handleDemoError("Cannot load checkpoint: path is empty");
        return false;
    }
    
    if (!error::validateFileAccess(path, false)) {
        error::handleDemoError("Cannot access checkpoint file: " + path);
        return false;
    }
    
    if (!brain->load(path)) {
        error::handleDemoError("Failed to load checkpoint from: " + path);
        return false;
    }
    
    NLM_LOG_INFO("Checkpoint loaded successfully from: " + path);
    return true;
}

bool DemoManager::saveCheckpoint(std::shared_ptr<Brain> brain, const std::string& path) {
    if (!brain) {
        error::handleDemoError("Cannot save checkpoint: brain is null");
        return false;
    }
    
    if (path.empty()) {
        error::handleDemoError("Cannot save checkpoint: path is empty");
        return false;
    }
    
    if (!error::validateFileAccess(path, true)) {
        error::handleDemoError("Cannot write checkpoint to: " + path);
        return false;
    }
    
    if (!brain->save(path)) {
        error::handleDemoError("Failed to save checkpoint to: " + path);
        return false;
    }
    
    NLM_LOG_INFO("Checkpoint saved successfully to: " + path);
    return true;
}

std::string DemoManager::getDemoStatistics() const {
    std::stringstream stats;
    
    stats << "=== Demo Manager Statistics ===" << std::endl;
    stats << "Initialized: " << (pImpl->initialized ? "YES" : "NO") << std::endl;
    stats << "Learning experiments completed: " << pImpl->learningResults.size() << std::endl;
    stats << "Current configuration loaded: " << (pImpl->currentConfig ? "YES" : "NO") << std::endl;
    
    if (!pImpl->learningResults.empty()) {
        stats << "" << std::endl;
        stats << "Learning Experiment Summary:" << std::endl;
        for (size_t i = 0; i < pImpl->learningResults.size(); ++i) {
            const auto& result = pImpl->learningResults[i];
            stats << "  Experiment " << i << ": learning=" << (result.learningDetected ? "YES" : "NO")
                  << ", weight_change=" << result.weightChangePercentage << "%" << std::endl;
        }
    }
    
    return stats.str();
}

void DemoManager::cleanup() {
    if (pImpl->cleanupRequired) {
        // Cleanup resources
        NLM_LOG_INFO("Cleaning up DemoManager resources");
        
        // Clear learning results
        pImpl->learningResults.clear();
        
        // Reset configuration
        pImpl->currentConfig.reset();
        
        // Reset flags
        pImpl->initialized = false;
        pImpl->cleanupRequired = false;
        
        NLM_LOG_INFO("DemoManager cleanup complete");
    }
}

bool DemoManager::setupLogging(const DemoOptions& options) {
    // Use the existing DemoRunner for logging setup
    pImpl->runner.setupLogging(options);
    return true;
}

bool DemoManager::setupEnvironment(const DemoOptions& options) {
    // Setup working directory and environment
    if (!options.outputFile.empty()) {
        std::string dirname = std::filesystem::path(options.outputFile).parent_path();
        if (!dirname.empty() && !std::filesystem::exists(dirname)) {
            std::filesystem::create_directories(dirname);
        }
    }
    
    return true;
}

bool DemoManager::setupProgressMonitor(const DemoOptions& options) {
    // Setup progress monitor if needed
    if (options.progress) {
        pImpl->progress.setUpdateInterval(0.1f); // Update every 100ms
    }
    return true;
}

bool DemoManager::setupPerformanceProfiler(const DemoOptions& options) {
    if (!options.profileOutput.empty()) {
        // Start profiling
        pImpl->profiler.start("demo_run");
    }
    return true;
}

bool DemoManager::setupCheckpointSystem(const DemoOptions& options) {
    if (!options.checkpointPath.empty()) {
        // Setup checkpoint system
        NLM_LOG_INFO("Checkpoint system ready: " + options.checkpointPath);
    }
    return true;
}

void DemoManager::reportProgress(const std::string& message, float progress) {
    if (progress >= 0.0f) {
        pImpl->progress.update(progress, message);
    } else {
        NLM_LOG_INFO(message);
    }
}

void DemoManager::reportDemoStatus(DemoMode mode, bool success, const std::string& details) {
    std::string status = success ? "PASSED" : "FAILED";
    std::string demoName = DemoRunner::getDemoName(mode);
    
    NLM_LOG_INFO("Demo " + demoName + " " + status);
    
    if (!details.empty()) {
        NLM_LOG_INFO("Details: " + details);
    }
}

std::string DemoManager::aggregateResults(const std::vector<LearningResult>& learningResults) const {
    std::stringstream aggregate;
    
    aggregate << "=== Aggregated Learning Results ===" << std::endl;
    aggregate << "Total experiments: " << learningResults.size() << std::endl;
    
    size_t successfulExperiments = 0;
    float totalLearningRate = 0.0f;
    
    for (const auto& result : learningResults) {
        if (result.learningDetected) {
            successfulExperiments++;
            totalLearningRate += result.weightChangePercentage;
        }
    }
    
    aggregate << "Successful learning experiments: " << successfulExperiments << std::endl;
    
    if (successfulExperiments > 0) {
        aggregate << "Average learning rate: " << (totalLearningRate / successfulExperiments) << "%" << std::endl;
    }
    
    return aggregate.str();
}

bool DemoManager::validateOptions(const DemoOptions& options) const {
    if (options.connectionProbability < 0.0f || options.connectionProbability > 1.0f) {
        NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(options.connectionProbability));
        return false;
    }
    
    if (options.neuronCount == 0) {
        NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(options.neuronCount));
        return false;
    }
    
    if (options.regionCount == 0) {
        NLM_LOG_ERROR("Invalid region_count: " + std::to_string(options.regionCount));
        return false;
    }
    
    if (options.steps == 0) {
        NLM_LOG_ERROR("Invalid steps: " + std::to_string(options.steps));
        return false;
    }
    
    return true;
}

bool DemoManager::validateBrainState(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        NLM_LOG_ERROR("Brain validation failed: brain is null");
        return false;
    }
    
    if (brain->getTotalNeuronCount() == 0) {
        NLM_LOG_ERROR("Brain validation failed: no neurons");
        return false;
    }
    
    // Additional validation as needed
    return true;
}

// Convenience function implementations
DemoManager& getDemoManager() {
    static DemoManager manager;
    return manager;
}

bool runLearningDemo(int argc, char** argv) {
    DemoOptions options;
    
    if (!config::parseCommandLineArgs(argc, argv, options)) {
        return false;
    }
    
    // Set default mode if not specified
    if (options.mode == DemoMode::CUSTOM_CONFIG) {
        options.mode = DemoMode::PLASTICITY_LEARNING; // Default to plasticity learning demo
    }
    
    DemoManager& manager = getDemoManager();
    
    if (!manager.initialize(options)) {
        error::handleConfigError("Failed to initialize demo manager");
        return false;
    }
    
    bool success = manager.run(options);
    
    // Save results if requested
    if (!options.outputFile.empty() && manager.pImpl->initialized) {
        std::string stats = manager.getDemoStatistics();
        manager.saveDemoResults("learning", stats);
    }
    
    return success;
}

bool runIntegrationDemo(int argc, char** argv) {
    DemoOptions options;
    
    if (!config::parseCommandLineArgs(argc, argv, options)) {
        return false;
    }
    
    // Force integration demo mode
    options.mode = DemoMode::PHASE6_INTEGRATION;
    
    DemoManager& manager = getDemoManager();
    
    if (!manager.initialize(options)) {
        error::handleConfigError("Failed to initialize integration demo manager");
        return false;
    }
    
    return manager.run(options);
}

bool runBenchmarkDemo(int argc, char** argv) {
    DemoOptions options;
    
    if (!config::parseCommandLineArgs(argc, argv, options)) {
        return false;
    }
    
    // Force benchmark demo mode
    options.mode = DemoMode::BENCHMARK;
    
    DemoManager& manager = getDemoManager();
    
    if (!manager.initialize(options)) {
        error::handleConfigError("Failed to initialize benchmark demo manager");
        return false;
    }
    
    return manager.run(options);
}

// Helper function to enable plasticity for learning
void enablePlasticityForLearning(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false); // Enable Hebbian and STDP
        }
    }
}

bool runPlasticityLearningDemo(const DemoOptions& options, std::shared_ptr<Brain> brain, LearningResult* learningResult) {
    NLM_LOG_INFO("=== Plasticity Learning Demo (Enhanced) ===");
    
    if (!brain) {
        error::handleDemoError("Cannot run plasticity demo: brain is null");
        return false;
    }
    
    // Record initial state
    if (learningResult) {
        learningResult->recordInitialState();
    }
    
    // Enable plasticity
    enablePlasticityForLearning(brain);
    
    // Run learning simulation with enhanced pattern
    std::vector<NeuronId> patternNeurons;
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        for (auto* neuron : region->getAllNeurons()) {
            if (neuron->getType() == NeuronType::Sensory && patternNeurons.size() < 20) {
                patternNeurons.push_back(neuron->getId());
            }
        }
    }
    
    // Enhanced learning simulation with progress reporting
    for (SimulationStep step = 0; step < options.steps; ++step) {
        // Apply enhanced input pattern to stimulate selective learning
        for (NeuronId id : patternNeurons) {
            // Varying input strength for more diverse learning
            float inputStrength = 30.0f + (static_cast<float>(rand()) / RAND_MAX) * 20.0f;
            brain->injectCurrent(id, inputStrength);
        }
        
        brain->step(step, step * 0.001);
        
        // Periodic progress reporting
        if (options.progress && step % 100 == 0) {
            float progress = static_cast<float>(step) / options.steps;
            NLM_LOG_INFO("Learning progress: " + std::to_string(progress * 100.0f) + "%");
        }
    }
    
    // Record final state
    if (learningResult) {
        learningResult->recordFinalState();
    }
    
    NLM_LOG_INFO("Plasticity learning simulation completed");
    
    return true;
}

namespace demos {
    
    // Implement the demo functions (simplified versions)
    bool runBasicConnectivityDemo(std::shared_ptr<Brain> brain, 
                                  SimulationStep steps,
                                  bool verbose,
                                  ProgressMonitor* progress) {
        if (!brain) {
            error::handleDemoError("Cannot run basic connectivity demo: brain is null");
            return false;
        }
        
        NLM_LOG_INFO("Running basic connectivity demo...");
        
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject current into first few neurons
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            error::handleDemoError("No regions found for basic connectivity demo");
            return false;
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            error::handleDemoError("No neurons found for basic connectivity demo");
            return false;
        }
        
        for (size_t i = 0; i < std::min<size_t>(10, neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        // Run simulation
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * 0.001);
            
            if (progress) {
                progress->update(static_cast<float>(step) / steps,
                               "Basic connectivity steps: " + std::to_string(step));
            }
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        NLM_LOG_INFO("Basic connectivity demo completed: " + std::to_string(spikes) + " spikes generated");
        
        return spikes > 0;
    }
    
    bool runStdpVerificationDemo(std::shared_ptr<Brain> brain,
                                 SimulationStep steps,
                                 bool verbose,
                                 ProgressMonitor* progress) {
        if (!brain) {
            error::handleDemoError("Cannot run STDP demo: brain is null");
            return false;
        }
        
        NLM_LOG_INFO("Running STDP verification demo...");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region || region->getSynapses().empty()) {
            error::handleDemoError("No synapses available for STDP test");
            return false;
        }
        
        // Test a single synapse
        auto& synapses = region->getSynapses();
        Synapse* syn = synapses[0].get();
        
        syn->enablePlasticity(false, true, false); // STDP only
        float beforeWeight = syn->getWeight();
        
        NLM_LOG_INFO("Testing STDP on synapse with initial weight: " + std::to_string(beforeWeight));
        
        // Apply correlated pre->post activity
        for (int trial = 0; trial < 50; ++trial) {
            brain->injectCurrent(syn->getSourceNeuron(), 60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            brain->injectCurrent(syn->getDestinationNeuron(), 60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
        
        float afterWeight = syn->getWeight();
        float delta = afterWeight - beforeWeight;
        
        NLM_LOG_INFO("STDP test completed: weight changed from " + std::to_string(beforeWeight) +
                     " to " + std::to_string(afterWeight) + ", delta = " + std::to_string(delta));
        
        return std::abs(delta) > 0.001f;
    }
    
} // namespace demos

} // namespace nlm

// Main entry point for the new demo system
int main(int argc, char** argv) {
    // Use the enhanced demo framework
    DemoOptions options;
    
    // Parse command line arguments
    if (!config::parseCommandLineArgs(argc, argv, options)) {
        return 1;
    }
    
    // Setup logging
    DemoRunner::getDemoRunner().setupLogging(options);
    
    // Create and run demo manager
    DemoManager& manager = getDemoManager();
    
    if (!manager.initialize(options)) {
        std::cerr << "Failed to initialize demo manager" << std::endl;
        return 1;
    }
    
    bool success = manager.run(options);
    
    // Save results if requested
    if (!options.outputFile.empty() && manager.pImpl->initialized) {
        std::string stats = manager.getDemoStatistics();
        manager.saveDemoResults("demo_results", stats);
    }
    
    // Cleanup
    manager.cleanup();
    
    return success ? 0 : 1;
}
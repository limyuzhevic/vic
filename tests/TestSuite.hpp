// NLM Phase 2 Tests - Separate test files for better organization
// This file contains the three test functions extracted from main.cpp

#ifndef NLM_TESTS_H
#define NLM_TESTS_H

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "core/Logger/Logger.hpp"
#include <memory>
#include <vector>
#include <string>
#include <iomanip>
#include <numeric>

using namespace nlm;

// Learning Experiment: Demonstrates measurable synaptic changes through experience
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        // Record initial weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
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
    
    void recordFinalState() {
        finalWeights.clear();
        
        // Record final weights from first region
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        // Compute weight changes
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
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
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        // Determine if learning occurred
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                                (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

// Helper functions for test setup and configuration
namespace test_helpers {
    
    // Parse command line arguments for configuration
    bool parseCommandLineArgs(int argc, char** argv, std::string& configFile) {
        configFile = "configs/default.cfg";
        
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            if (arg.substr(0, 7) == "--config") {
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                } else {
                    NLM_LOG_ERROR("Error: --config requires a filename");
                    return false;
                }
            }
        }
        return true;
    }
    
    // Load configuration with validation
    std::shared_ptr<Config> loadAndValidateConfig(const std::string& configFile) {
        auto config = std::make_shared<Config>();
        
        // Try to load from file if provided
        if (config->loadFromFile(configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        // Set default values for Phase 2
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        
        return config;
    }
    
    // Log configuration summary
    void logConfigurationSummary(const std::shared_ptr<Config>& config) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Configuration:");
        NLM_LOG_INFO("  random_seed: " + std::to_string(config->getOr<int64_t>("random_seed", 42)));
        NLM_LOG_INFO("  simulation_timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.001)) + "s");
        NLM_LOG_INFO("  neuron_count: " + std::to_string(config->getOr<int64_t>("neuron_count", 500)));
        NLM_LOG_INFO("  region_count: " + std::to_string(config->getOr<int64_t>("region_count", 1)));
        NLM_LOG_INFO("  connection_probability: " + std::to_string(config->getOr<float>("connection_probability", 0.15f)));
        NLM_LOG_INFO("");
    }
    
    // Helper function to calculate weight statistics (replaces duplicate calculations)
    struct WeightStats {
        float initialMean;
        float finalMean;
        float change;
        size_t strengthened;
        size_t weakened;
        size_t unchanged;
    };
    
    WeightStats calculateWeightStats(const std::vector<float>& initialWeights, const std::vector<float>& finalWeights) {
        WeightStats stats;
        
        if (initialWeights.empty() || finalWeights.empty()) {
            stats.initialMean = stats.finalMean = stats.change = 0.0f;
            stats.strengthened = stats.weakened = stats.unchanged = 0;
            return stats;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        stats.initialMean = initialSum / initialWeights.size();
        stats.finalMean = finalSum / finalWeights.size();
        stats.change = stats.finalMean - stats.initialMean;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        stats.strengthened = stats.weakened = stats.unchanged = 0;
        
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++stats.strengthened;
            else if (delta < -0.01f) ++stats.weakened;
            else ++stats.unchanged;
        }
        
        return stats;
    }
    
    // Performance timing utilities
    class Timer {
        private:
            double startTime;
        public:
            Timer() { reset(); }
            void reset() { startTime = clock() / (double)CLOCKS_PER_SEC; }
            double elapsed() const { return clock() / (double)CLOCKS_PER_SEC - startTime; }
            double elapsedMs() const { return elapsed() * 1000.0; }
    };
    
    // Configuration presets for different test scenarios
    namespace presets {
        
        // Create a minimal configuration for quick testing
        std::shared_ptr<Config> createMinimalConfig() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.1, ConfigSource::Default);
            config->set("random_seed", 123, ConfigSource::Default);
            return config;
        }
        
        // Create a configuration optimized for learning demonstration
        std::shared_ptr<Config> createLearningConfig() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 500, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.15, ConfigSource::Default);
            config->set("random_seed", 42, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.02f, ConfigSource::Default);
            config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
            config->set("pruning_rate", 0.00002f, ConfigSource::Default);
            return config;
        }
        
        // Create a configuration optimized for stability testing
        std::shared_ptr<Config> createStabilityConfig() {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 200, ConfigSource::Default);
            config->set("region_count", 1, ConfigSource::Default);
            config->set("connection_probability", 0.05, ConfigSource::Default);
            config->set("random_seed", 999, ConfigSource::Default);
            config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.01f, ConfigSource::Default);
            config->set("synaptogenesis_rate", 0.00001f, ConfigSource::Default);
            config->set("pruning_rate", 0.000001f, ConfigSource::Default);
            return config;
        }
    }
}

#endif // NLM_TESTS_H
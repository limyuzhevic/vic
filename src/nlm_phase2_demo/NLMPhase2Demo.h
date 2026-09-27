// NLMPhase2Demo.h - Extracted from main.cpp
// Contains the demo orchestration class and LearningExperiment struct

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/Random/Random.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>

namespace nlm {

// Learning experiment data structure
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

// Constants for configuration and testing
namespace Constants {
    // Simulation parameters
    constexpr double DEFAULT_TIMESTEP = 0.001;
    constexpr double WORLD_TIMESTEP = 0.1;
    constexpr SimulationStep DEFAULT_STEP_COUNT = 1000;
    
    // Neuron configuration
    constexpr int64_t DEFAULT_NEURON_COUNT = 500;
    constexpr float DEFAULT_V_THESHOLD = -50.0f;
    constexpr float DEFAULT_V_REST = -70.0f;
    constexpr float DEFAULT_V_RESET = -75.0f;
    constexpr float DEFAULT_TAU_MEM = 20.0f;
    constexpr float DEFAULT_TAU_REF = 2.0f;
    
    // Connection parameters
    constexpr int64_t DEFAULT_REGION_COUNT = 1;
    constexpr float DEFAULT_CONNECTION_PROBABILITY = 0.15f;
    
    // Plasticity parameters
    constexpr float DEFAULT_STDP_LTP_WEIGHT = 0.02f;
    constexpr float DEFAULT_STDP_LTD_WEIGHT = 0.015f;
    constexpr float DEFAULT_STDP_TAU = 20.0f;
    constexpr float DEFAULT_SYNAPTOGENESIS_RATE = 0.0001f;
    constexpr float DEFAULT_PRUNING_RATE = 0.00001f;
    
    // Test parameters
    constexpr SimulationStep BASIC_CONNECTIVITY_STEPS = 50;
    constexpr SimulationStep PLASTICITY_EXPERIMENT_STEPS = 1000;
    constexpr SimulationStep STDP_VERIFICATION_TRIALS = 50;
    
    // Activity thresholds
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float SIGNIFICANT_WEIGHT_CHANGE = 0.01f;
    
    // File paths
    constexpr const char* DEFAULT_CONFIG_FILE = "configs/default.cfg";
    constexpr const char* CHECKPOINT_FILE = "nlm_checkpoint.bin";
}

// Demonstration class for NLM Phase 2
class NLMPhase2Demo {
public:
    NLMPhase2Demo() = default;
    ~NLMPhase2Demo() = default;
    
    // Initialize with configuration
    bool initialize(int argc, char** argv);
    
    // Run all demonstrations/tests
    void runAllTests();
    
    // Individual test methods
    void runBasicConnectivityTest();
    void runPlasticityExperiment();
    void runStdpVerification();
    
    // Cleanup
    void cleanup();
    
private:
    // Helper methods
    void setupConfiguration(int argc, char** argv);
    void setupLogging();
    void logConfigurationSummary() const;
    
    // Test helper methods
    void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
    void runPlasticityExperiment(std::shared_ptr<Brain> brain);
    void runStdpVerification(std::shared_ptr<Brain> brain);
    
    // Utility methods
    void recordRegionAccessError() const;
    bool validateBrainState() const;
    
    // Configuration
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<SimulationClock> clock_;
    std::shared_ptr<Brain> brain_;
    
    // Constants for this demo instance
    double timestep_ = Constants::DEFAULT_TIMESTEP;
    SimulationStep step_count_ = Constants::DEFAULT_STEP_COUNT;
};

inline void printBanner() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2: Real Neural Computation                         ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║     This phase implements:                                    ║
    ║     - Leaky Integrate-and-Fire (LIF) neuron dynamics                ║
    ║     - Event-driven spike propagation                          ║
    ║     - STDP and Hebbian plasticity                            ║
    ║     - Structural plasticity                                   ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
}

inline bool isValidStepArgument(const std::string& arg) {
    // Validate step argument format
    try {
        size_t pos;
        int value = std::stoi(arg, &pos);
        return pos == arg.length() && value >= 0;
    } catch (const std::exception&) {
        return false;
    }
}

inline bool isValidFloatArgument(const std::string& arg) {
    // Validate float argument format
    try {
        size_t pos;
        float value = std::stof(arg, &pos);
        return pos == arg.length();
    } catch (const std::exception&) {
        return false;
    }
}

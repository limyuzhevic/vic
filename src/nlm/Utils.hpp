// Convenience APIs and high-level interfaces for NLM
// This file provides user-friendly functions that make the neural network system
// easier to use for both beginners and advanced users

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <vector>

namespace nlm {

// ===== CONVENIENCE FACTORY FUNCTIONS =====

// Create a brain with default settings (quick start for beginners)
std::shared_ptr<Brain> createBrain() {
    auto config = std::make_shared<Config>();
    
    // Set up sensible defaults for quick testing
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Enable all integrated systems
    config->set("enable_memory_systems", true, ConfigSource::Default);
    config->set("enable_prediction", true, ConfigSource::Default);
    config->set("enable_cognition", true, ConfigSource::Default);
    config->set("enable_neuromodulation", true, ConfigSource::Default);
    config->set("enable_development", true, ConfigSource::Default);
    config->set("enable_checkpoints", true, ConfigSource::Default);
    
    return std::make_shared<Brain>(config);
}

// Create a brain with custom configuration (for advanced users)
std::shared_ptr<Brain> createBrainWithConfig(const std::string& configPath) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(configPath);
    return std::make_shared<Brain>(config);
}

// Create a brain from command line arguments
std::shared_ptr<Brain> createBrainFromArgs(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    config->loadFromArgs(argc, argv);
    return std::make_shared<Brain>(config);
}

// ===== QUICK START TEMPLATES =====

// Simple test template - creates a small, fast network for quick testing
std::shared_ptr<Brain> createQuickTestBrain() {
    auto brain = createBrain();
    auto config = brain->getConfig();
    
    // Optimized for quick testing
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Runtime);
    config->set("simulation_timestep", 0.001, ConfigSource::Runtime);
    config->set("enable_memory_systems", false, ConfigSource::Runtime);  // Disable for speed
    config->set("enable_checkpoints", false, ConfigSource::Runtime);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Runtime);
    
    brain->initialize();
    return brain;
}

// Learning experiment template - optimized for plasticity testing
std::shared_ptr<Brain> createPlasticityTestBrain() {
    auto brain = createBrain();
    auto config = brain->getConfig();
    
    // Optimized for learning experiments
    config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Runtime);
    config->set("enable_memory_systems", true, ConfigSource::Runtime);
    config->set("enable_neuromodulation", true, ConfigSource::Runtime);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Runtime);
    config->set("synaptogenesis_rate", 0.001f, ConfigSource::Runtime);
    config->set("pruning_rate", 0.0001f, ConfigSource::Runtime);
    
    brain->initialize();
    return brain;
}

// Performance testing template - optimized for speed
std::shared_ptr<Brain> createPerformanceTestBrain() {
    auto brain = createBrain();
    auto config = brain->getConfig();
    
    // Optimized for performance testing
    config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(4), ConfigSource::Runtime);
    config->set("enable_memory_systems", false, ConfigSource::Runtime);  // Disable for max speed
    config->set("enable_prediction", false, ConfigSource::Runtime);
    config->set("enable_cognition", false, ConfigSource::Runtime);
    config->set("enable_neuromodulation", false, ConfigSource::Runtime);
    config->set("enable_development", false, ConfigSource::Runtime);
    config->set("enable_checkpoints", false, ConfigSource::Runtime);
    
    brain->initialize();
    return brain;
}

// ===== HIGH-LEVEL API FUNCTIONS =====

// Run a complete learning experiment with sensory input
void runLearningExperiment(std::shared_ptr<Brain> brain, 
                           const std::vector<float>& inputPattern,
                           size_t steps = 1000) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    // Create and inject sensory input
    auto sensoryInput = std::make_unique<SensoryInput>(inputPattern);
    brain->receiveSensoryInput(*sensoryInput);
    
    // Run the experiment
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i);
        
        // Log progress every 100 steps
        if (i % 100 == 0) {
            auto action = brain->produceAction();
            auto spikes = brain->getTotalSpikeCount();
            NLM_LOG_INFO("Step " + std::to_string(i) + 
                        " | Spikes: " + std::to_string(spikes) +
                        " | Action: " + (action ? "Active" : "Wait"));
        }
    }
}

// Run plasticity verification experiment
void runPlasticityVerification(std::shared_ptr<Brain> brain, size_t steps = 500) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    // Enable plasticity on all synapses
    auto* spikeSystem = brain->getSpikeSystem();
    if (spikeSystem) {
        // Enable STDP and Hebbian plasticity
        brain->getSTDP()->setLTPWeight(0.02f);
        brain->getSTDP()->setLTDWeight(0.015f);
        brain->getHebbian()->enable();
    }
    
    // Apply correlated input to test synaptic strengthening
    for (size_t i = 0; i < steps; ++i) {
        // Inject correlated pre-post activity
        brain->injectCurrentToNeurons(NeuronType::Internal, 30.0f);
        brain->step(i);
        
        if (i % 100 == 0) {
            NLM_LOG_INFO("Plasticity verification step " + std::to_string(i) +
                        " | STDP weight: " + std::to_string(brain->getSTDP()->getLTPWeight()));
        }
    }
}

// Run connectivity test - checks if network can propagate signals
bool runConnectivityTest(std::shared_ptr<Brain> brain, uint32_t steps = 100) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    // Inject strong current into sensory neurons
    brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
    
    // Run for specified steps
    for (uint32_t i = 0; i < steps; ++i) {
        brain->step(i);
    }
    
    size_t finalSpikes = brain->getTotalSpikeCount();
    size_t spikesGenerated = finalSpikes - initialSpikes;
    
    NLM_LOG_INFO("Connectivity test: " + std::to_string(spikesGenerated) + 
                " spikes generated in " + std::to_string(steps) + " steps");
    
    return spikesGenerated > 0;
}

// Generate action sequence from sensory input
size_t generateActionSequence(std::shared_ptr<Brain> brain,
                             const std::vector<std::vector<float>>& inputSequence,
                             size_t maxSteps = 1000) {
    if (!brain) {
        throw std::runtime_error("Brain pointer is null");
    }
    
    size_t actionCount = 0;
    
    for (size_t i = 0; i < maxSteps; ++i) {
        if (i < inputSequence.size()) {
            brain->receiveSensoryInput(SensoryInput(inputSequence[i]));
        }
        
        auto action = brain->produceAction();
        if (action && action->type != ActionType::Wait) {
            actionCount++;
            NLM_LOG_INFO("Step " + std::to_string(i) + ": Action " + std::to_string(static_cast<int>(action->type)));
        }
        
        brain->step(i);
    }
    
    return actionCount;
}

// ===== DIAGNOSTIC AND MONITORING FUNCTIONS =====

// Get brain health summary
std::string getBrainHealthSummary(std::shared_ptr<Brain> brain) {
    if (!brain) {
        return "Brain is null";
    }
    
    std::ostringstream oss;
    oss << "=== NLM Brain Health Report ===\n";
    oss << "Neurons: " << brain->getTotalNeuronCount() << "\n";
    oss << "Synapses: " << brain->getTotalSynapseCount() << "\n";
    oss << "Total spikes: " << brain->getTotalSpikeCount() << "\n";
    oss << "E/I ratio: " << brain->getExcitationInhibitionRatio() << "\n";
    oss << "Average firing rate: " << brain->getAverageFiringRate() << " Hz\n";
    
    // Memory system status
    auto workingMemory = brain->getWorkingMemory();
    if (workingMemory) {
        oss << "Working memory traces: " << workingMemory->getActiveTraces() << "\n";
    }
    
    auto episodicMemory = brain->getEpisodicMemory();
    if (episodicMemory) {
        oss << "Episodic memory episodes: " << episodicMemory->getEpisodeCount() << "\n";
    }
    
    return oss.str();
}

// Check if brain is properly initialized
bool isBrainHealthy(std::shared_ptr<Brain> brain) {
    if (!brain) {
        return false;
    }
    
    // Basic checks
    return brain->getTotalNeuronCount() > 0 &&
           brain->getTotalSynapseCount() > 0 &&
           brain->getRegionCount() > 0;
}

// Get configuration summary
summary_config getConfigSummary(std::shared_ptr<const Config> config) {
    summary_config summary;
    
    if (!config) {
        return summary;
    }
    
    summary.totalEntries = config->getKeys().size();
    
    for (const auto& key : config->getKeys()) {
        auto value = config->getOr<std::string>(key, "");
        if (value.has_value()) {
            summary.entries[key] = value.value();
        }
    }
    
    return summary;
}

// ===== LEGACY COMPATIBILITY FUNCTIONS =====

// Legacy function for backward compatibility
// Creates a simple brain identical to the original Phase 2 implementation
std::shared_ptr<Brain> createPhase2Brain() {
    auto brain = createBrain();
    auto config = brain->getConfig();
    
    // Phase 2 specific settings
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Runtime);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Runtime);
    config->set("stdp_tau", 20.0f, ConfigSource::Runtime);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Runtime);
    config->set("pruning_rate", 0.00001f, ConfigSource::Runtime);
    
    brain->initialize();
    return brain;
}

// Legacy function for quick testing
void quickTestPhase2() {
    auto brain = createPhase2Brain();
    
    NLM_LOG_INFO("=== Quick Phase 2 Test ===");
    
    // Test 1: Basic connectivity
    runConnectivityTest(brain, 50);
    
    // Test 2: Plasticity
    brain->reset();
    brain->initialize();
    runPlasticityVerification(brain, 100);
    
    NLM_LOG_INFO("=== Quick Test Complete ===");
    NLM_LOG_INFO("Brain health: " + getBrainHealthSummary(brain));
}

} // namespace nlm

// Convenience type definitions for easier API usage
using BrainPtr = std::shared_ptr<nlm::Brain>;
using ConfigPtr = std::shared_ptr<nlm::Config>;

// Global convenience functions
inline BrainPtr makeBrain() { return nlm::createBrain(); }
inline BrainPtr makeQuickBrain() { return nlm::createQuickTestBrain(); }
inline BrainPtr makePerformanceBrain() { return nlm::createPerformanceTestBrain(); }

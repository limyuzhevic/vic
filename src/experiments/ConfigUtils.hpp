#pragma once

#include "core/Config/Config.hpp"
#include <memory>

namespace nlm {

/**
 * Utility function to create consistent configuration across all phases
 * This ensures that different demos use the same default parameters,
 * reducing configuration-related bugs and making behavior more predictable.
 */
std::shared_ptr<Config> createConsistentConfig(size_t neuronCount = 500) {
    auto config = std::make_shared<Config>();
    
    // Core neural parameters (consistent across phases)
    config->set("neuron_count", neuronCount);
    config->set("region_count", size_t(1));  // Single region for consistency
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    
    // Plasticity parameters
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    
    // Structural plasticity
    config->set("synaptogenesis_rate", 0.0001f);
    config->set("pruning_rate", 0.00001f);
    
    // Memory system parameters
    config->set("working_memory_capacity", float(neuronCount) * 0.1f);
    config->set("max_episodes", size_t(1000));
    
    // Neuromodulation parameters
    config->set("dopamine_decay_rate", 0.01f);
    config->set("curiosity_threshold", 0.1f);
    config->set("novelty_decay_rate", 0.02f);
    
    // Development parameters
    config->set("development_age_rate", 0.001);
    config->set("critical_period_length", size_t(1000));
    
    return config;
}

/**
 * Configuration for Phase 6 integration experiment
 * Uses consistent settings but allows customization
 */
struct ConsistentPhase6Config {
    size_t neuronCount = 500;
    size_t maxSteps = 2000;
    bool enableCheckpointing = true;
    bool enableReplay = true;
    bool enableDevelopment = true;
    std::string checkpointPath = "./checkpoint_test.bin";
    
    // Convert to brain configuration
    std::shared_ptr<Config> toBrainConfig() const {
        auto config = createConsistentConfig(neuronCount);
        
        // Override with Phase 6 specific settings
        config->set("replay_interval", size_t(100));
        config->set("consolidation_interval", size_t(1000));
        config->set("development_stage_update_interval", size_t(1000));
        
        return config;
    }
};

} // namespace nlm

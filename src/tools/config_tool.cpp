//!/usr/bin/env.cpp
// Configuration management tool for NLM
// Demonstrates advanced configuration features for developers

#include "core/Config/ConfigManager.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

// Example configuration change listener
class ConsoleConfigListener : public nlm::ConfigChangeListener {
public:
    void onConfigChanged(const std::string& key, const nlm::ConfigValue& oldValue, const nlm::ConfigValue& newValue) override {
        std::cout << "[CONFIG CHANGE] " << key << " changed from " << formatValue(oldValue) 
                  << " to " << formatValue(newValue) << std::endl;
    }
    
private:
    std::string formatValue(const nlm::ConfigValue& value) {
        return std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return "\"" + val + "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                return val ? "true" : "false";
            } else {
                return std::to_string(val);
            }
        }, value);
    }
};

// Advanced configuration setup with schema
void setupAdvancedConfig(nlm::ConfigManager& config) {
    // Define schema for advanced configuration
    std::vector<nlm::ConfigSchema> schema = {
        // Neural network parameters
        nlm::ConfigSchema("neuron_count", 1000LL, "Number of neurons in the brain", true),
        nlm::ConfigSchema("region_count", 1LL, "Number of brain regions", true),
        nlm::ConfigSchema("connection_probability", 0.1f, "Probability of synapse formation", true),
        
        // Plasticity parameters
        nlm::ConfigSchema("stdp_ltp_weight", 0.02f, "STDP long-term potentiation weight", true),
        nlm::ConfigSchema("stdp_ltd_weight", 0.015f, "STDP long-term depression weight", true),
        nlm::ConfigSchema("stdp_tau", 20.0f, "STDP time constant in milliseconds", true),
        
        // Structural plasticity
        nlm::ConfigSchema("synaptogenesis_rate", 0.0001f, "Rate of new synapse formation", true),
        nlm::ConfigSchema("pruning_rate", 0.00001f, "Rate of synapse elimination", true),
        
        // Simulation parameters
        nlm::ConfigSchema("simulation_timestep", 0.001, "Simulation time step in seconds", true),
        nlm::ConfigSchema("replay_interval", 100LL, "Memory replay interval in steps", false),
        nlm::ConfigSchema("consolidation_interval", 1000LL, "Memory consolidation interval in steps", false),
        
        // Neuromodulation
        nlm::ConfigSchema("dopamine_baseline", 0.1f, "Baseline dopamine level", false),
        nlm::ConfigSchema("curiosity_sensitivity", 1.0f, "Curiosity signal sensitivity", false),
        
        // Advanced options
        nlm::ConfigSchema("checkpoint_dir", std::string("./checkpoints"), "Directory for checkpoint files", false),
        nlm::ConfigSchema("enable_logging", true, "Enable detailed logging", false),
        nlm::ConfigSchema("random_seed", 42LL, "Random seed for reproducibility", false),
        
        // Performance tuning
        nlm::ConfigSchema("max_memory_usage_mb", 512.0, "Maximum memory usage in MB", false),
        nlm::ConfigSchema("enable_vectorization", true, "Enable SIMD vectorization", false),
    };
    
    config.applySchema(schema);
    
    // Set advanced defaults
    config.set("enable_logging", true, nlm::ConfigSource::Default);
    config.set("checkpoint_dir", std::string("./checkpoints"), nlm::ConfigSource::Default);
    config.set("random_seed", 12345LL, nlm::ConfigSource::Default);
}
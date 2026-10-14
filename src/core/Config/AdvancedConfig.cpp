#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../agent/AgentBrain.hpp"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

namespace nlm {

// Advanced Config implementation

void Config::validate() const {
    // Basic validation implementation
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        std::stringstream ss;
        ss << "Configuration validation failed:";
        for (const auto& error : errors) {
            ss << " " << error;
        }
        NLM_LOG_ERROR(ss.str());
    }
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Check for conflicting settings
    if (has("plasticity.stdp.enable") && has("plasticity.hebbian.enable")) {
        errors.push_back("STDP and Hebbian plasticity both enabled - may cause interference");
    }
    
    // Check for reasonable value ranges
    if (has("brain.neuron_count")) {
        int64_t neuronCount = getOr<int64_t>("brain.neuron_count", 1000);
        if (neuronCount < 10) {
            errors.push_back("Neuron count too low (< 10) - may not support meaningful computation");
        } else if (neuronCount > 100000) {
            errors.push_back("Neuron count too high (> 100,000) - may cause performance issues");
        }
    }
    
    if (has("brain.connection_probability")) {
        float connProb = getOr<float>("brain.connection_probability", 0.1f);
        if (connProb < 0.001f) {
            errors.push_back("Connection probability too low (< 0.001) - sparse network");
        } else if (connProb > 0.5f) {
            errors.push_back("Connection probability too high (> 0.5) - dense network");
        }
    }
    
    return errors;
}

std::shared_ptr<Config> Config::createDefaultAdvancedConfig() {
    auto config = std::make_shared<Config>();
    
    // Set optimized default parameters
    config->set("neuron_count", 1000);
    config->set("region_count", 2);
    config->set("connection_probability", 0.1f);
    config->set("simulation_timestep", 0.001f);
    
    // STDP parameters
    config->set("stdp_ltp_weight", 0.02f);
    config->set("stdp_ltd_weight", 0.015f);
    config->set("stdp_tau", 20.0f);
    
    // Neuromodulation parameters
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    
    // Memory system parameters
    config->set("memory.working_memory.capacity", 100);
    config->set("memory.episodic_memory.max_episodes", 1000);
    config->set("memory.associative.memory_strength", 0.5f);
    
    // Development parameters
    config->set("development.initial_stage", "CriticalPeriod");
    config->set("development.plasticity_rate", 1.0f);
    config->set("development.synapse_growth_rate", 0.001f);
    
    // Performance optimization
    config->set("performance.optimization_level", 2);
    config->set("performance.use_memory_pools", true);
    
    return config;
}

std::shared_ptr<Config> Config::createResearchConfig() {
    auto config = std::make_shared<Config>();
    
    // Research-oriented parameters
    config->set("neuron_count", 5000);
    config->set("region_count", 4);
    config->set("connection_probability", 0.2f);
    config->set("simulation_timestep", 0.0005f);
    
    // Precise STDP for research
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.01f);
    config->set("stdp_tau", 10.0f);
    
    // Enhanced neuromodulation for research
    config->set("neuromod.dopamine.scale", 2.0f);
    config->set("neuromod.curiosity.enable", true);
    config->set("neuromod.novelty.enable", true);
    config->set("neuromod.prediction_error.enable", true);
    
    // Large memory for research
    config->set("memory.working_memory.capacity", 1000);
    config->set("memory.episodic_memory.max_episodes", 10000);
    config->set("memory.semantic_memory.capacity", 500);
    
    // Extended development for research
    config->set("development.initial_stage", "Maturation");
    config->set("development.plasticity_rate", 2.0f);
    config->set("development.synapse_growth_rate", 0.002f);
    
    // Research optimization
    config->set("performance.optimization_level", 3);
    config->set("performance.use_memory_pools", true);
    config->set("performance.enable_vectorization", true);
    
    return config;
}

std::shared_ptr<Config> Config::createProductionConfig() {
    auto config = std::make_shared<Config>();
    
    // Production-optimized parameters
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("connection_probability", 0.05f);
    config->set("simulation_timestep", 0.002f);
    
    // Stable STDP for production
    config->set("stdp_ltp_weight", 0.005f);
    config->set("stdp_ltd_weight", 0.005f);
    config->set("stdp_tau", 50.0f);
    
    // Basic neuromodulation for production
    config->set("neuromod.dopamine.scale", 1.0f);
    config->set("neuromod.curiosity.enable", false);
    config->set("neuromod.novelty.enable", false);
    
    // Adequate memory for production
    config->set("memory.working_memory.capacity", 200);
    config->set("memory.episodic_memory.max_episodes", 100);
    config->set("memory.associative.memory_strength", 0.7f);
    
    // Standard development for production
    config->set("development.initial_stage", "Adult");
    config->set("development.plasticity_rate", 0.5f);
    config->set("development.synapse_growth_rate", 0.0001f);
    
    // Production optimization
    config->set("performance.optimization_level", 2);
    config->set("performance.use_memory_pools", true);
    config->set("performance.enable_vectorization", false); // Safer
    config->set("performance.enable_error_correction", true);
    
    return config;
}

} // namespace nlm

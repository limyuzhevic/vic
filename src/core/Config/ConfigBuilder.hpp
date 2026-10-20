#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <memory>

namespace nlm {

/**
 * Configuration Builder for NLM
 * 
 * Provides a fluent interface for building NLM configurations with validation
 * and convenient presets for common use cases.
 */
class ConfigBuilder {
public:
    // Constructor - creates an empty config
    ConfigBuilder() : config(std::make_shared<Config>()) {}
    
    // Copy constructor (disabled)
    ConfigBuilder(const ConfigBuilder&) = delete;
    ConfigBuilder& operator=(const ConfigBuilder&) = delete;
    
    // Move constructor and assignment (enabled)
    ConfigBuilder(ConfigBuilder&& other) noexcept = default;
    ConfigBuilder& operator=(ConfigBuilder&& other) noexcept = default;
    
    // --- Brain Core Configuration ---
    
    /**
     * Set the number of neurons in the network
     * 
     * @param neuronCount Number of neurons to create
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& neuronCount(size_t count) {
        config->set("neuron_count", static_cast<int64_t>(count), ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Set the number of neural regions
     * 
     * @param regionCount Number of regions to create
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& regionCount(size_t count) {
        config->set("region_count", static_cast<int64_t>(count), ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Set connection probability between neurons
     * 
     * @param probability Probability (0.0 to 1.0) for synaptic connections
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& connectionProbability(float probability) {
        config->set("connection_probability", probability, ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Set the simulation timestep
     * 
     * @param timestep Time step in milliseconds
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& simulationTimestep(double timestep) {
        config->set("simulation_timestep", timestep, ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Set the random seed for reproducible simulations
     * 
     * @param seed Random seed value
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& randomSeed(uint64_t seed) {
        config->set("random_seed", static_cast<int64_t>(seed), ConfigSource::Runtime);
        return *this;
    }
    
    // --- Neural Dynamics Configuration ---
    
    /**
     * Configure neuron dynamics parameters
     * 
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& configureLIF(double restingPotential, double threshold,
                               double resetPotential, double leakConductance) {
        config->set("neuron_resting_potential", restingPotential, ConfigSource::Runtime);
        config->set("neuron_threshold", threshold, ConfigSource::Runtime);
        config->set("neuron_reset_potential", resetPotential, ConfigSource::Runtime);
        config->set("neuron_leak_conductance", leakConductance, ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Configure synaptic plasticity parameters
     * 
     * @param stdpLtp LTP weight for STDP
     * @param stdpLtd LTD weight for STDP
     * @param stdpTau Time constant for STDP
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& configureSTDP(float stdpLtp, float stdpLtd, float stdpTau) {
        config->set("stdp_ltp_weight", stdpLtp, ConfigSource::Runtime);
        config->set("stdp_ltd_weight", stdpLtd, ConfigSource::Runtime);
        config->set("stdp_tau", stdpTau, ConfigSource::Runtime);
        return *this;
    }
    
    // --- Memory System Configuration ---
    
    /**
     * Configure working memory parameters
     * 
     * @param capacity Working memory capacity (number of items)
     * @param decayRate Decay rate for memory items
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& configureWorkingMemory(size_t capacity, float decayRate) {
        config->set("working_memory_capacity", static_cast<int64_t>(capacity), ConfigSource::Runtime);
        config->set("working_memory_decay_rate", decayRate, ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Configure episodic memory parameters
     * 
     * @param maxEpisodes Maximum number of episodic memories to store
     * @param replayInterval Steps between replay events
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& configureEpisodicMemory(size_t maxEpisodes, size_t replayInterval) {
        config->set("episodic_memory_max_episodes", static_cast<int64_t>(maxEpisodes), ConfigSource::Runtime);
        config->set("episodic_memory_replay_interval", static_cast<int64_t>(replayInterval), ConfigSource::Runtime);
        return *this;
    }
    
    // --- Neuromodulation Configuration ---
    
    /**
     * Configure neuromodulation parameters
     * 
     * @param dopamineEffect Effect strength of dopamine on plasticity
     * @param curiosityThreshold Threshold for curiosity-driven exploration
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& configureNeuromodulation(float dopamineEffect, float curiosityThreshold) {
        config->set("dopamine_effect", dopamineEffect, ConfigSource::Runtime);
        config->set("curiosity_threshold", curiosityThreshold, ConfigSource::Runtime);
        return *this;
    }
    
    // --- Development Configuration ---
    
    /**
     * Configure developmental parameters
     * 
     * @param initialPlasticity Initial plasticity rate
     * @param maturationRate Rate at which neural properties mature
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& configureDevelopment(float initialPlasticity, float maturationRate) {
        config->set("development_initial_plasticity", initialPlasticity, ConfigSource::Runtime);
        config->set("development_maturation_rate", maturationRate, ConfigSource::Runtime);
        return *this;
    }
    
    // --- Preset Configurations ---
    
    /**
     * Create a "simple" brain configuration
     * 
     * Optimized for fast testing and basic functionality
     * 
     * @return ConfigBuilder for fluent configuration
     */
    static ConfigBuilder simple() {
        ConfigBuilder builder;
        return builder.neuronCount(100)
                     .regionCount(1)
                     .connectionProbability(0.1f)
                     .simulationTimestep(0.001)
                     .randomSeed(42)
                     .configureLIF(-70.0, -55.0, -70.0, 10.0)
                     .configureSTDP(0.01f, 0.012f, 20.0f);
    }
    
    /**
     * Create a "standard" brain configuration
     * 
     * Balanced configuration for general research purposes
     * 
     * @return ConfigBuilder for fluent configuration
     */
    static ConfigBuilder standard() {
        ConfigBuilder builder;
        return builder.neuronCount(500)
                     .regionCount(2)
                     .connectionProbability(0.15f)
                     .simulationTimestep(0.001)
                     .randomSeed(42)
                     .configureLIF(-70.0, -55.0, -70.0, 10.0)
                     .configureSTDP(0.02f, 0.015f, 20.0f)
                     .configureWorkingMemory(50, 0.01f)
                     .configureNeuromodulation(0.5f, 0.3f);
    }
    
    /**
     * Create a "high-performance" brain configuration
     * 
     * Optimized for simulation speed with fewer neurons
     * 
     * @return ConfigBuilder for fluent configuration
     */
    static ConfigBuilder highPerformance() {
        ConfigBuilder builder;
        return builder.neuronCount(200)
                     .regionCount(1)
                     .connectionProbability(0.05f)
                     .simulationTimestep(0.001)
                     .randomSeed(42)
                     .configureLIF(-70.0, -55.0, -70.0, 10.0)
                     .configureSTDP(0.01f, 0.012f, 20.0f);
    }
    
    /**
     * Create a "research" brain configuration
     * 
     * Comprehensive configuration for detailed research experiments
     * 
     * @return ConfigBuilder for fluent configuration
     */
    static ConfigBuilder research() {
        ConfigBuilder builder;
        return builder.neuronCount(1000)
                     .regionCount(3)
                     .connectionProbability(0.2f)
                     .simulationTimestep(0.001)
                     .randomSeed(42)
                     .configureLIF(-70.0, -55.0, -70.0, 10.0)
                     .configureSTDP(0.03f, 0.035f, 20.0f)
                     .configureWorkingMemory(100, 0.005f)
                     .configureEpisodicMemory(1000, 100)
                     .configureNeuromodulation(1.0f, 0.5f)
                     .configureDevelopment(0.1f, 0.01f);
    }
    
    /**
     * Create a "demo" brain configuration
     * 
     * Optimized for demonstration purposes - small but feature-rich
     * 
     * @return ConfigBuilder for fluent configuration
     */
    static ConfigBuilder demo() {
        ConfigBuilder builder;
        return builder.neuronCount(300)
                     .regionCount(2)
                     .connectionProbability(0.1f)
                     .simulationTimestep(0.001)
                     .randomSeed(42)
                     .configureLIF(-70.0, -55.0, -70.0, 10.0)
                     .configureSTDP(0.015f, 0.018f, 20.0f)
                     .configureWorkingMemory(30, 0.02f)
                     .configureNeuromodulation(0.7f, 0.2f);
    }
    
    // --- Configuration Modification ---
    
    /**
     * Set a custom configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& set(const std::string& key, const std::string& value) {
        config->set(key, value, ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Set a custom configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& set(const std::string& key, double value) {
        config->set(key, value, ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Set a custom configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& set(const std::string& key, int value) {
        config->set(key, value, ConfigSource::Runtime);
        return *this;
    }
    
    /**
     * Set a custom configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @return Reference to this builder for fluent interface
     */
    ConfigBuilder& set(const std::string& key, bool value) {
        config->set(key, value, ConfigSource::Runtime);
        return *this;
    }
    
    // --- Get the built configuration ---
    
    /**
     * Get the built configuration
     * 
     * @return Shared pointer to the configuration
     */
    std::shared_ptr<Config> getConfig() const {
        return config;
    }
    
    /**
     * Get the built configuration as a unique pointer
     * 
     * @return Unique pointer to the configuration
     */
    std::shared_ptr<Config> releaseConfig() {
        return config;
    }
    
    // --- Configuration Validation ---
    
    /**
     * Validate the current configuration
     * 
     * @return true if configuration is valid, false otherwise
     */
    bool validate() const {
        // Basic validation
        if (!config->has("neuron_count")) {
            return false;
        }
        auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0) return false;
        
        if (!config->has("simulation_timestep")) {
            return false;
        }
        auto timestep = config->getOr<double>("simulation_timestep", 0.001);
        if (timestep <= 0.0) return false;
        
        if (config->has("connection_probability")) {
            auto prob = config->getOr<double>("connection_probability", 0.0);
            if (prob < 0.0 || prob > 1.0) return false;
        }
        
        return true;
    }
    
    /**
     * Get validation errors
     * 
     * @return String describing validation errors, empty if valid
     */
    std::string getValidationErrors() const {
        std::string errors;
        
        if (!config->has("neuron_count")) {
            errors += "Missing neuron_count; ";
        } else {
            auto neuronCount = config->getOr<int64_t>("neuron_count", 0);
            if (neuronCount <= 0) errors += "neuron_count must be positive; ";
        }
        
        if (!config->has("simulation_timestep")) {
            errors += "Missing simulation_timestep; ";
        } else {
            auto timestep = config->getOr<double>("simulation_timestep", 0.001);
            if (timestep <= 0.0) errors += "simulation_timestep must be positive; ";
        }
        
        if (config->has("connection_probability")) {
            auto prob = config->getOr<double>("connection_probability", 0.0);
            if (prob < 0.0 || prob > 1.0) errors += "connection_probability must be between 0.0 and 1.0; ";
        }
        
        return errors.empty() ? "" : "Validation errors: " + errors;
    }

private:
    std::shared_ptr<Config> config;
};

} // namespace nlm
#ifndef NLM_CONFIG_UTILS_HPP
#define NLM_CONFIG_UTILS_HPP

#include "core/Config/Config.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * Configuration utilities for NLM experiments
 * Provides centralized configuration management to avoid duplication
 */
class NLMConfigUtils {
public:
    /**
     * Default configuration for all NLM experiments
     */
    static void applyDefaultConfig(std::shared_ptr<Config> config) {
        if (!config) return;
        
        // Core brain parameters
        config->set("neuron_count", 1000LL);
        config->set("region_count", 2LL);
        config->set("connection_probability", 0.1f);
        config->set("random_seed", 42LL);
        config->set("simulation_timestep", 0.001);
        
        // Plasticity parameters (shared across experiments)
        config->set("stdp_ltp_weight", 0.01f);
        config->set("stdp_ltd_weight", 0.012f);
        config->set("stdp_tau", 20.0f);
        config->set("synaptogenesis_rate", 0.0001f);
        config->set("pruning_rate", 0.00001f);
        
        // Development parameters
        config->set("critical_period", 300.0);
        config->set("maturation_period", 900.0);
        
        // Neuromodulation parameters
        config->set("dopamine_scale", 1.0f);
        config->set("curiosity_threshold", 0.3f);
        config->set("novelty_decay_rate", 0.99f);
    }
    
    /**
     * Environment configuration for simple world
     */
    static void applyWorldConfig(std::shared_ptr<Config> config, 
                                int width = 20, int height = 20,
                                int visionWidth = 16, int visionHeight = 16) {
        if (!config) return;
        
        config->set("environment_width", width);
        config->set("environment_height", height);
        config->set("vision_width", visionWidth);
        config->set("vision_height", visionHeight);
        config->set("agent_energy", 100.0);
    }
    
    /**
     * Configure for Phase 3 experiments (world interaction)
     */
    static void applyPhase3Config(std::shared_ptr<Config> config) {
        applyDefaultConfig(config);
        applyWorldConfig(config, 20, 20, 16, 16);
        
        // Phase 3 specific settings
        config->set("neuron_count", 500LL);
        config->set("region_count", 2LL);
    }
    
    /**
     * Configure for Phase 4 experiments (cognition)
     */
    static void applyPhase4Config(std::shared_ptr<Config> config) {
        applyDefaultConfig(config);
        applyWorldConfig(config, 15, 15, 12, 12);
        
        // Phase 4 specific settings
        config->set("neuron_count", 1000LL);
        config->set("region_count", 2LL);
    }
    
    /**
     * Configure for Phase 6 integration
     */
    static void applyPhase6Config(std::shared_ptr<Config> config,
                                 int neuronCount = 500,
                                 int maxSteps = 2000,
                                 bool enableCheckpointing = true,
                                 bool enableReplay = true,
                                 bool enableDevelopment = true) {
        applyDefaultConfig(config);
        applyWorldConfig(config, 20, 20, 16, 16);
        
        // Phase 6 specific settings
        config->set("neuron_count", static_cast<long long>(neuronCount));
        config->set("max_simulation_steps", static_cast<long long>(maxSteps));
        config->set("enable_checkpointing", enableCheckpointing);
        config->set("enable_replay", enableReplay);
        config->set("enable_development", enableDevelopment);
        
        // Integration-specific settings
        config->set("working_memory_capacity", 50);
        config->set("max_episodes", 100);
        config->set("consolidation_interval", 10.0);
    }
    
    /**
     * Validate configuration parameters
     */
    static bool validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) return false;
        
        // Check for required parameters
        if (!config->has("neuron_count")) return false;
        if (!config->has("region_count")) return false;
        if (!config->has("connection_probability")) return false;
        
        // Validate ranges
        int64_t neuronCount = config->getOr<int64_t>("neuron_count", 0);
        if (neuronCount <= 0 || neuronCount > 100000) return false;
        
        float connProb = config->getOr<float>("connection_probability", 0.0f);
        if (connProb < 0.0f || connProb > 1.0f) return false;
        
        return true;
    }
    
    /**
     * Get configuration parameter with safe default
     */
    template<typename T>
    static T getConfig(const std::shared_ptr<Config>& config, 
                       const std::string& key, T defaultValue) {
        if (!config) return defaultValue;
        return config->getOr<T>(key, defaultValue);
    }
    
    /**
     * Print configuration summary for debugging
     */
    static void printConfigSummary(const std::shared_ptr<Config>& config) {
        if (!config) {
            std::cout << "Config: null\n";
            return;
        }
        
        std::cout << "Configuration Summary:\n";
        std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 0) << "\n";
        std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 0) << "\n";
        std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.0f) << "\n";
        std::cout << "  stdp_ltp_weight: " << config->getOr<float>("stdp_ltp_weight", 0.0f) << "\n";
        std::cout << "  stdp_ltd_weight: " << config->getOr<float>("stdp_ltd_weight", 0.0f) << "\n";
        std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.0) << "\n";
        std::cout << "  random_seed: " << config->getOr<int64_t>("random_seed", 0) << "\n";
    }
};

} // namespace nlm

#endif // NLM_CONFIG_UTILS_HPP

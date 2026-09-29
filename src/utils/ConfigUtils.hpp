#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>

namespace nlm {

// Configuration utilities for NLM
class ConfigUtils {
public:
    // Common configuration keys used across NLM
    static const std::string RANDOM_SEED;
    static const std::string SIMULATION_TIMESTEP;
    static const std::string NEURON_COUNT;
    static const std::string REGION_COUNT;
    static const std::string CONNECTION_PROBABILITY;
    static const std::string STDP_LTP_WEIGHT;
    static const std::string STDP_LTD_WEIGHT;
    static const std::string STDP_TAU;
    static const std::string SYNAPTOGENESIS_RATE;
    static const std::string PRUNING_RATE;
    static const std::string CHECKPOINT_DIR;
    static const std::string REPLAY_INTERVAL;
    static const std::string CONSOLIDATION_INTERVAL;
    
    // Validate configuration parameters
    static bool validateConfig(const Config& config);
    
    // Get configuration with validation and defaults
    template<typename T>
    static T getValidated(const Config& config, const std::string& key, T defaultValue,
                         std::function<bool(T)> validator = nullptr);
    
    // Apply NLM-specific configuration defaults
    static void applyNLMDefaults(Config& config);
    
    // Check if configuration is complete for initialization
    static bool isConfigComplete(const Config& config);
    
    // Log configuration in a structured format
    static void logConfig(const Config& config);
    
    // Merge configurations (source overrides destination)
    static void mergeConfigs(Config& destination, const Config& source);
    
    // Create backup of current configuration
    static bool backupConfig(const Config& config, const std::string& backupPath);
    
    // Load configuration with error handling
    static bool safeLoadConfig(Config& config, const std::string& filepath);
};

// Configuration key constants implementation
const std::string ConfigUtils::RANDOM_SEED = "random_seed";
const std::string ConfigUtils::SIMULATION_TIMESTEP = "simulation_timestep";
const std::string ConfigUtils::NEURON_COUNT = "neuron_count";
const std::string ConfigUtils::REGION_COUNT = "region_count";
const std::string ConfigUtils::CONNECTION_PROBABILITY = "connection_probability";
const std::string ConfigUtils::STDP_LTP_WEIGHT = "stdp_ltp_weight";
const std::string ConfigUtils::STDP_LTD_WEIGHT = "stdp_ltd_weight";
const std::string ConfigUtils::STDP_TAU = "stdp_tau";
const std::string ConfigUtils::SYNAPTOGENESIS_RATE = "synaptogenesis_rate";
const std::string ConfigUtils::PRUNING_RATE = "pruning_rate";
const std::string ConfigUtils::CHECKPOINT_DIR = "checkpoint_dir";
const std::string ConfigUtils::REPLAY_INTERVAL = "replay_interval";
const std::string ConfigUtils::CONSOLIDATION_INTERVAL = "consolidation_interval";

} // namespace nlm

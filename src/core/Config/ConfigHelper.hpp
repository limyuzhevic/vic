// ConfigHelper - Standardizes configuration loading across NLM
// Provides consistent interface for accessing configuration values

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <sstream>

namespace nlm {

/**
 * ConfigHelper provides standardized configuration loading patterns
 * and utilities for working with NLM configuration objects.
 */
class ConfigHelper {
public:
    ConfigHelper() = default;
    ~ConfigHelper() = default;
    
    // Standard configuration loading patterns
    struct StandardConfig {
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        float stdpLTPWeight;
        float stdpLTDWeight;
        float stdpTau;
        float synaptogenesisRate;
        float pruningRate;
        uint64_t randomSeed;
        float simulationTimestep;
        std::string checkpointDir;
        
        StandardConfig()
            : neuronCount(1000)
            , regionCount(1)
            , connectionProbability(0.1f)
            , stdpLTPWeight(0.01f)
            , stdpLTDWeight(0.012f)
            , stdpTau(20.0f)
            , synaptogenesisRate(0.0001f)
            , pruningRate(0.00001f)
            , randomSeed(42)
            , simulationTimestep(0.001)
            , checkpointDir("./checkpoints") {}
    };
    
    // Load configuration from config object using standard keys
    static StandardConfig loadStandardConfig(const class Config& config) {
        StandardConfig stdConfig;
        
        // Brain configuration
        if (config.has("neuron_count")) {
            stdConfig.neuronCount = config.get<size_t>("neuron_count");
        }
        
        if (config.has("region_count")) {
            stdConfig.regionCount = config.get<size_t>("region_count");
        }
        
        if (config.has("connection_probability")) {
            stdConfig.connectionProbability = config.get<float>("connection_probability");
        }
        
        // Plasticity configuration
        if (config.has("stdp_ltp_weight")) {
            stdConfig.stdpLTPWeight = config.get<float>("stdp_ltp_weight");
        }
        
        if (config.has("stdp_ltd_weight")) {
            stdConfig.stdpLTDWeight = config.get<float>("stdp_ltd_weight");
        }
        
        if (config.has("stdp_tau")) {
            stdConfig.stdpTau = config.get<float>("stdp_tau");
        }
        
        if (config.has("synaptogenesis_rate")) {
            stdConfig.synaptogenesisRate = config.get<float>("synaptogenesis_rate");
        }
        
        if (config.has("pruning_rate")) {
            stdConfig.pruningRate = config.get<float>("pruning_rate");
        }
        
        // Core configuration
        if (config.has("random_seed")) {
            stdConfig.randomSeed = config.get<uint64_t>("random_seed");
        }
        
        if (config.has("simulation_timestep")) {
            stdConfig.simulationTimestep = config.get<float>("simulation_timestep");
        }
        
        // System configuration
        if (config.has("checkpoint_dir")) {
            stdConfig.checkpointDir = config.get<std::string>("checkpoint_dir");
        }
        
        return stdConfig;
    }
    
    // Set configuration values in config object using standard keys
    static void applyStandardConfig(class Config& config, const StandardConfig& stdConfig) {
        config.set("neuron_count", stdConfig.neuronCount);
        config.set("region_count", stdConfig.regionCount);
        config.set("connection_probability", stdConfig.connectionProbability);
        config.set("stdp_ltp_weight", stdConfig.stdpLTPWeight);
        config.set("stdp_ltd_weight", stdConfig.stdpLTDWeight);
        config.set("stdp_tau", stdConfig.stdpTau);
        config.set("synaptogenesis_rate", stdConfig.synaptogenesisRate);
        config.set("pruning_rate", stdConfig.pruningRate);
        config.set("random_seed", stdConfig.randomSeed);
        config.set("simulation_timestep", stdConfig.simulationTimestep);
        config.set("checkpoint_dir", stdConfig.checkpointDir);
    }
    
    // Get configuration as string summary
    static std::string getConfigSummary(const class Config& config) {
        std::ostringstream oss;
        oss << "=== NLM Configuration ===" << std::endl;
        
        std::vector<std::string> keys = config.getKeys();
        for (const auto& key : keys) {
            std::string valueStr;
            if (config.has(key)) {
                // Try to get appropriate type based on key
                if (key.find("count") != std::string::npos || key == "neuron_count" || key == "region_count") {
                    valueStr = std::to_string(config.get<size_t>(key));
                } else if (key.find("rate") != std::string::npos || key.find("weight") != std::string::npos || 
                          key.find("tau") != std::string::npos || key == "simulation_timestep") {
                    valueStr = std::to_string(config.get<float>(key));
                } else if (key.find("seed") != std::string::npos || key == "random_seed") {
                    valueStr = std::to_string(config.get<uint64_t>(key));
                } else {
                    valueStr = config.get<std::string>(key);
                }
                oss << key << " = " << valueStr << std::endl;
            }
        }
        
        return oss.str();
    }
    
    // Check if configuration has required keys
    static bool hasRequiredKeys(const class Config& config, const std::vector<std::string>& requiredKeys) {
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                return false;
            }
        }
        return true;
    }
    
    // Get configuration value or default
    template<typename T>
    static T getOrDefault(const class Config& config, const std::string& key, const T& defaultValue) {
        if (config.has(key)) {
            return config.get<T>(key);
        }
        return defaultValue;
    }
    
    // Merge two configuration objects
    static class Config mergeConfigs(const class Config& config1, const class Config& config2) {
        class Config merged;
        
        // Copy from config1
        std::vector<std::string> keys1 = config1.getKeys();
        for (const auto& key : keys1) {
            if (config1.has(key)) {
                // Try to get appropriate type
                if (std::is_same<typename std::remove_const<decltype(config1)>::type::value_type, size_t>::value) {
                    merged.set(key, config1.get<size_t>(key));
                } else if (std::is_same<typename std::remove_const<decltype(config1)>::type::value_type, float>::value) {
                    merged.set(key, config1.get<float>(key));
                } else if (std::is_same<typename std::remove_const<decltype(config1)>::type::value_type, std::string>::value) {
                    merged.set(key, config1.get<std::string>(key));
                } else if (std::is_same<typename std::remove_const<decltype(config1)>::type::value_type, uint64_t>::value) {
                    merged.set(key, config1.get<uint64_t>(key));
                }
            }
        }
        
        // Override with values from config2
        std::vector<std::string> keys2 = config2.getKeys();
        for (const auto& key : keys2) {
            if (config2.has(key)) {
                // Use same type detection
                if (std::is_same<typename std::remove_const<decltype(config1)>::type::value_type, size_t>::value) {
                    merged.set(key, config2.get<size_t>(key));
                } else if (std::is_same<typename std::remove_const<decltype(config1)>::type::value_type, float>::value) {
                    merged.set(key, config2.get<float>(key));
                } else if (std::is_same<typename std::remove_const<decltype(config1)>::type::value_type, std::string>::value) {
                    merged.set(key, config2.get<std::string>(key));
                } else if (std::is_same<typename std::remove_const<decltype(config1)>::type::value_type, uint64_t>::value) {
                    merged.set(key, config2.get<uint64_t>(key));
                }
            }
        }
        
        return merged;
    }
    
    // Create configuration from standard config
    static class Config createConfig(const StandardConfig& stdConfig) {
        class Config config;
        applyStandardConfig(config, stdConfig);
        return config;
    }
    
    // Apply environment variables to configuration
    static void applyEnvironmentVariables(class Config& config) {
        // NLM_NEURON_COUNT
        const char* envNeuronCount = std::getenv("NLM_NEURON_COUNT");
        if (envNeuronCount) {
            try {
                size_t neuronCount = std::stoull(envNeuronCount);
                config.set("neuron_count", neuronCount);
            } catch (...) {
                // Ignore conversion errors
            }
        }
        
        // NLM_CONNECTION_PROBABILITY
        const char* envConnProb = std::getenv("NLM_CONNECTION_PROBABILITY");
        if (envConnProb) {
            try {
                float connProb = std::stof(envConnProb);
                config.set("connection_probability", connProb);
            } catch (...) {
                // Ignore conversion errors
            }
        }
        
        // NLM_RANDOM_SEED
        const char* envSeed = std::getenv("NLM_RANDOM_SEED");
        if (envSeed) {
            try {
                uint64_t seed = std::stoull(envSeed);
                config.set("random_seed", seed);
            } catch (...) {
                // Ignore conversion errors
            }
        }
        
        // NLM_CHECKPOINT_DIR
        const char* envCheckpointDir = std::getenv("NLM_CHECKPOINT_DIR");
        if (envCheckpointDir) {
            config.set("checkpoint_dir", std::string(envCheckpointDir));
        }
    }
    
    // Print configuration
    static void printConfig(const class Config& config) {
        std::cout << getConfigSummary(config);
    }
};
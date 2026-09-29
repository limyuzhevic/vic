#include "ConfigUtils.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace nlm {

bool ConfigUtils::validateConfig(const Config& config) {
    try {
        // Check essential parameters
        if (!config.getOr<int64_t>(RANDOM_SEED, 0)) {
            NLM_LOG_WARNING("Missing essential configuration: " + RANDOM_SEED);
            return false;
        }
        
        if (!config.getOr<double>(SIMULATION_TIMESTEP, 0.0)) {
            NLM_LOG_WARNING("Missing essential configuration: " + SIMULATION_TIMESTEP);
            return false;
        }
        
        if (!config.getOr<int64_t>(NEURON_COUNT, 0)) {
            NLM_LOG_WARNING("Missing essential configuration: " + NEURON_COUNT);
            return false;
        }
        
        // Validate parameter ranges
        double timestep = config.getOr<double>(SIMULATION_TIMESTEP, 0.001);
        if (timestep <= 0.0 || timestep > 1.0) {
            NLM_LOG_ERROR("Invalid simulation_timestep: " + std::to_string(timestep));
            return false;
        }
        
        int64_t neuronCount = config.getOr<int64_t>(NEURON_COUNT, 1000);
        if (neuronCount < 100 || neuronCount > 100000) {
            NLM_LOG_ERROR("Invalid neuron_count: " + std::to_string(neuronCount));
            return false;
        }
        
        float connectionProb = config.getOr<float>(CONNECTION_PROBABILITY, 0.1f);
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            NLM_LOG_ERROR("Invalid connection_probability: " + std::to_string(connectionProb));
            return false;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config validation failed: " + std::string(e.what()));
        return false;
    }
}

template<typename T>
T ConfigUtils::getValidated(const Config& config, const std::string& key, T defaultValue,
                          std::function<bool(T)> validator) {
    T value;
    if (config.get(key, value)) {
        if (validator && !validator(value)) {
            NLM_LOG_WARNING("Invalid value for key '" + key + "': " + std::to_string(value) +
                          ". Using default: " + std::to_string(defaultValue));
            return defaultValue;
        }
        return value;
    }
    return defaultValue;
}

void ConfigUtils::applyNLMDefaults(Config& config) {
    // Apply NLM-specific defaults if not already set
    config.setIfNotExists("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config.setIfNotExists("simulation_timestep", 0.001, ConfigSource::Default);
    config.setIfNotExists("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config.setIfNotExists("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config.setIfNotExists("connection_probability", 0.1f, ConfigSource::Default);
    
    // STDP defaults
    config.setIfNotExists("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config.setIfNotExists("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config.setIfNotExists("stdp_tau", 20.0f, ConfigSource::Default);
    
    // Structural plasticity defaults
    config.setIfNotExists("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config.setIfNotExists("pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Integration settings
    config.setIfNotExists("replay_interval", 100, ConfigSource::Default);
    config.setIfNotExists("consolidation_interval", 1000, ConfigSource::Default);
    
    // Checkpointing
    config.setIfNotExists("checkpoint_dir", "./checkpoints", ConfigSource::Default);
}

bool ConfigUtils::isConfigComplete(const Config& config) {
    // Check for all essential parameters
    std::vector<std::string> essentialKeys = {
        RANDOM_SEED, SIMULATION_TIMESTEP, NEURON_COUNT,
        REGION_COUNT, CONNECTION_PROBABILITY
    };
    
    for (const auto& key : essentialKeys) {
        if (!config.get(key).has_value()) {
            NLM_LOG_WARNING("Missing essential configuration key: " + key);
            return false;
        }
    }
    return true;
}

void ConfigUtils::logConfig(const Config& config) {
    NLM_LOG_INFO("=== NLM Configuration ===");
    
    std::vector<std::pair<std::string, std::string>> configItems = {
        {RANDOM_SEED, std::to_string(config.getOr<int64_t>(RANDOM_SEED, 42))},
        {SIMULATION_TIMESTEP, std::to_string(config.getOr<double>(SIMULATION_TIMESTEP, 0.001)) + "s"},
        {NEURON_COUNT, std::to_string(config.getOr<int64_t>(NEURON_COUNT, 1000))},
        {REGION_COUNT, std::to_string(config.getOr<int64_t>(REGION_COUNT, 1))},
        {CONNECTION_PROBABILITY, std::to_string(config.getOr<float>(CONNECTION_PROBABILITY, 0.1f))}
    };
    
    for (const auto& item : configItems) {
        NLM_LOG_INFO("  " + item.first + ": " + item.second);
    }
    
    NLM_LOG_INFO("=======================");
}

void ConfigUtils::mergeConfigs(Config& destination, const Config& source) {
    // This would typically require access to Config's internal data
    // For now, we'll just log a warning
    NLM_LOG_WARNING("Config merging not fully implemented - consider manual merging");
}

bool ConfigUtils::backupConfig(const Config& config, const std::string& backupPath) {
    std::ofstream backupFile(backupPath);
    if (!backupFile.is_open()) {
        NLM_LOG_ERROR("Failed to create backup config file: " + backupPath);
        return false;
    }
    
    // For simplicity, we'll just copy the config object
    // In a real implementation, you'd serialize the config
    backupFile << "# NLM Configuration Backup\n";
    backupFile << "# Generated at: " << std::endl;
    backupFile << "# random_seed: " << config.getOr<int64_t>("random_seed", 42) << std::endl;
    backupFile << "# simulation_timestep: " << config.getOr<double>("simulation_timestep", 0.001) << std::endl;
    
    backupFile.close();
    NLM_LOG_INFO("Configuration backed up to: " + backupPath);
    return true;
}

bool ConfigUtils::safeLoadConfig(Config& config, const std::string& filepath) {
    try {
        if (config.loadFromFile(filepath)) {
            NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
            return true;
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + filepath + ". Using defaults.");
            applyNLMDefaults(config);
            return false;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading configuration from " + filepath + ": " + std::string(e.what()));
        applyNLMDefaults(config);
        return false;
    }
}

} // namespace nlm

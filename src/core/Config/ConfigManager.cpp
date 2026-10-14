#include "Config.hpp"

namespace nlm {

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::registerKey(const std::string& key, const ConfigMetadata& metadata) {
    keyMetadata[key] = metadata;
}

std::optional<ConfigMetadata> ConfigManager::getMetadata(const std::string& key) const {
    auto it = keyMetadata.find(key);
    if (it != keyMetadata.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool ConfigManager::validate(const std::string& key, const ConfigValue& value) const {
    auto metadata = getMetadata(key);
    if (!metadata) {
        return true; // No validation rule for this key
    }
    
    if (metadata->validator) {
        return metadata->validator(value);
    }
    return true; // No custom validator
}

std::vector<std::string> ConfigManager::validateRequired() const {
    std::vector<std::string> missingKeys;
    for (const auto& pair : keyMetadata) {
        if (pair.second.required) {
            missingKeys.push_back(pair.first);
        }
    }
    return missingKeys;
}

std::vector<std::string> ConfigManager::getRegisteredKeys() const {
    std::vector<std::string> keys;
    keys.reserve(keyMetadata.size());
    for (const auto& pair : keyMetadata) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigManager::clear() {
    keyMetadata.clear();
}

void ConfigManager::initializeDefaultConfig() {
    // Initialize with default configuration for NLM
    
    // Core simulation settings
    registerKey("random_seed", ConfigMetadata{
        "Random seed for reproducible experiments",
        "Core",
        true,
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); },
        "42"
    });
    
    registerKey("simulation_timestep", ConfigMetadata{
        "Simulation timestep in seconds",
        "Core",
        true,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; },
        "0.001"
    });
    
    registerKey("neuron_count", ConfigMetadata{
        "Number of neurons in the brain",
        "Core",
        true,
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "500"
    });
    
    registerKey("region_count", ConfigMetadata{
        "Number of brain regions",
        "Core",
        true,
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "1"
    });
    
    registerKey("connection_probability", ConfigMetadata{
        "Connection probability for random connectivity",
        "Core",
        true,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
        "0.15"
    });
    
    // Plasticity settings
    registerKey("plasticity_learning_rate", ConfigMetadata{
        "Learning rate for plasticity",
        "Plasticity",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
        "0.01"
    });
    
    registerKey("stdp_ltp_weight", ConfigMetadata{
        "STDP LTP weight",
        "Plasticity",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
        "0.02"
    });
    
    registerKey("stdp_ltd_weight", ConfigMetadata{
        "STDP LTD weight",
        "Plasticity",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
        "0.015"
    });
    
    registerKey("stdp_tau", ConfigMetadata{
        "STDP time constant (seconds)",
        "Plasticity",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; },
        "20.0"
    });
    
    // Development settings
    registerKey("synaptogenesis_rate", ConfigMetadata{
        "Rate of new synapse formation",
        "Development",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
        "0.0001"
    });
    
    registerKey("pruning_rate", ConfigMetadata{
        "Rate of synapse pruning",
        "Development",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
        "0.00001"
    });
    
    // Neuromodulation settings
    registerKey("dopamine_baseline", ConfigMetadata{
        "Baseline dopamine level",
        "Neuromodulation",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
        "0.1"
    });
    
    registerKey("reward_discount_factor", ConfigMetadata{
        "Discount factor for reward prediction",
        "Neuromodulation",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; },
        "0.99"
    });
    
    // Environment settings
    registerKey("environment_name", ConfigMetadata{
        "Name of the simulation environment",
        "Environment",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); },
        "GridWorld"
    });
    
    registerKey("environment_width", ConfigMetadata{
        "Width of the environment",
        "Environment",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "10"
    });
    
    registerKey("environment_height", ConfigMetadata{
        "Height of the environment",
        "Environment",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "10"
    });
    
    // Logging settings
    registerKey("log_level", ConfigMetadata{
        "Logging level",
        "Logging",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); },
        "INFO"
    });
    
    registerKey("log_to_file", ConfigMetadata{
        "Whether to log to file",
        "Logging",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<bool>(v); },
        "false"
    });
    
    registerKey("log_filename", ConfigMetadata{
        "Log filename",
        "Logging",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); },
        "nlm.log"
    });
    
    // Simulation settings
    registerKey("max_simulation_steps", ConfigMetadata{
        "Maximum number of simulation steps",
        "Simulation",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) > 0; },
        "10000"
    });
    
    registerKey("simulation_time_limit", ConfigMetadata{
        "Simulation time limit (0 = unlimited)",
        "Simulation",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) >= 0.0; },
        "0.0"
    });
    
    // Visualization settings
    registerKey("visualization_enabled", ConfigMetadata{
        "Whether visualization is enabled",
        "Visualization",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<bool>(v); },
        "false"
    });
    
    registerKey("visualization_update_rate", ConfigMetadata{
        "Visualization update rate (Hz)",
        "Visualization",
        false,
        [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0; },
        "30.0"
    });
}

} // namespace nlm

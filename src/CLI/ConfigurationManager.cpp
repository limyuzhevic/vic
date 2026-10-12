// ConfigurationManager.cpp
#include "CLI/ConfigurationManager.h"
#include <iostream>
#include <iomanip>
#include <set>
#include <stdexcept>
#include <regex>

struct ConfigurationManager::Impl {
    struct DefaultValue {
        std::string key;
        std::string value;
        std::string description;
        nlm::ConfigSource source;
    };
    
    std::vector<DefaultValue> defaults;
    
    // Default configuration validation rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const nlm::ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    std::vector<ValidationRule> validationRules;
};

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {
    // Add default configuration values
    std::vector<DefaultConfigValue> defaultValues = {
        // Basic simulation settings
        {"random_seed", "42", "Random seed for reproducible experiments", nlm::ConfigSource::Default},
        {"simulation_timestep", "0.001", "Simulation timestep in seconds", nlm::ConfigSource::Default},
        {"neuron_count", "500", "Number of neurons in the brain", nlm::ConfigSource::Default},
        {"region_count", "1", "Number of brain regions", nlm::ConfigSource::Default},
        {"connection_probability", "0.15", "Connection probability for random connectivity", nlm::ConfigSource::Default},
        
        // Plasticity settings
        {"plasticity_learning_rate", "0.01", "Base learning rate for plasticity", nlm::ConfigSource::Default},
        {"stdp_ltp_weight", "0.02", "STDP long-term potentiation weight", nlm::ConfigSource::Default},
        {"stdp_ltd_weight", "0.015", "STDP long-term depression weight", nlm::ConfigSource::Default},
        {"stdp_tau", "20.0", "STDP time constant", nlm::ConfigSource::Default},
        
        // Structural plasticity
        {"synaptogenesis_rate", "0.0001", "Rate of new synapse formation", nlm::ConfigSource::Default},
        {"pruning_rate", "0.00001", "Rate of synapse elimination", nlm::ConfigSource::Default},
        
        // Neuromodulation settings
        {"dopamine_baseline", "0.1", "Baseline dopamine level", nlm::ConfigSource::Default},
        {"curiosity_threshold", "0.05", "Novelty threshold for curiosity", nlm::ConfigSource::Default},
        {"reward_scaling", "1.0", "Reward signal scaling factor", nlm::ConfigSource::Default},
        
        // Prediction system
        {"prediction_horizon", "10", "Number of steps to look ahead", nlm::ConfigSource::Default},
        {"prediction_error_threshold", "0.5", "Threshold for prediction error detection", nlm::ConfigSource::Default},
        
        // Environment and world settings
        {"environment_name", "GridWorld", "Name of the simulation environment", nlm::ConfigSource::Default},
        {"environment_width", "20", "World width in cells", nlm::ConfigSource::Default},
        {"environment_height", "20", "World height in cells", nlm::ConfigSource::Default},
        {"vision_width", "8", "Agent vision width", nlm::ConfigSource::Default},
        {"vision_height", "8", "Agent vision height", nlm::ConfigSource::Default},
        
        // Logging and performance
        {"log_level", "INFO", "Logging level (DEBUG, INFO, WARN, ERROR)", nlm::ConfigSource::Default},
        {"log_to_file", "false", "Whether to log to file", nlm::ConfigSource::Default},
        {"log_filename", "nlm.log", "Log file name", nlm::ConfigSource::Default},
        {"max_simulation_steps", "10000", "Maximum simulation steps", nlm::ConfigSource::Default},
        {"simulation_time_limit", "0.0", "Time limit in seconds (0 = unlimited)", nlm::ConfigSource::Default},
        
        // Visualization
        {"visualization_enabled", "false", "Enable visualization", nlm::ConfigSource::Default},
        {"visualization_update_rate", "30.0", "Visualization update rate (Hz)", nlm::ConfigSource::Default},
        
        // Development settings
        {"development_synaptogenesis_rate", "0.001", "Developmental synaptogenesis rate", nlm::ConfigSource::Default},
        {"development_pruning_rate", "0.0001", "Developmental pruning rate", nlm::ConfigSource::Default},
        {"critical_period_start", "100", "Start of critical period (steps)", nlm::ConfigSource::Default},
        {"critical_period_end", "1000", "End of critical period (steps)", nlm::ConfigSource::Default},
        
        // Reward and motivation
        {"reward_discount_factor", "0.99", "Reward discount factor for planning", nlm::ConfigSource::Default},
        {"exploration_rate", "0.1", "Random exploration rate", nlm::ConfigSource::Default},
        {"exploitation_rate", "0.9", "Exploitation rate (1 - exploration)", nlm::ConfigSource::Default},
    };
    
    for (const auto& defaultValue : defaultValues) {
        pImpl->defaults.push_back({
            defaultValue.key,
            defaultValue.value,
            defaultValue.description,
            defaultValue.source
        });
    }
    
    // Add validation rules
    pImpl->validationRules.push_back({
        "neuron_count",
        [](const nlm::ConfigValue& val) {
            return std::holds_alternative<int64_t>(val) && std::get<int64_t>(val) > 0;
        },
        "neuron_count must be a positive integer"
    });
    
    pImpl->validationRules.push_back({
        "simulation_timestep",
        [](const nlm::ConfigValue& val) {
            return std::holds_alternative<double>(val) && std::get<double>(val) > 0.0 && std::get<double>(val) <= 0.1;
        },
        "simulation_timestep must be a positive number <= 0.1 seconds"
    });
    
    pImpl->validationRules.push_back({
        "connection_probability",
        [](const nlm::ConfigValue& val) {
            return std::holds_alternative<double>(val) && std::get<double>(val) >= 0.0 && std::get<double>(val) <= 1.0;
        },
        "connection_probability must be between 0.0 and 1.0"
    });
    
    pImpl->validationRules.push_back({
        "random_seed",
        [](const nlm::ConfigValue& val) {
            return std::holds_alternative<int64_t>(val);
        },
        "random_seed must be an integer"
    });
}

void ConfigurationManager::addDefault(const DefaultConfigValue& defaultValue) {
    pImpl->defaults.push_back({
        defaultValue.key,
        defaultValue.value,
        defaultValue.description,
        defaultValue.source
    });
}

bool ConfigurationManager::validateConfig(std::shared_ptr<nlm::Config> config) const {
    std::vector<std::string> errors;
    
    // Check validation rules
    for (const auto& rule : pImpl->validationRules) {
        if (config->has(rule.key)) {
            auto value = config->get<std::string>(rule.key);
            if (value && !validateValue(rule.key, *value)) {
                errors.push_back(rule.errorMessage);
            }
        }
    }
    
    // Check for duplicate keys (if any)
    std::set<std::string> seenKeys;
    for (const auto& entry : config->getKeys()) {
        if (seenKeys.find(entry) != seenKeys.end()) {
            errors.push_back("Duplicate configuration key: " + entry);
        }
        seenKeys.insert(entry);
    }
    
    if (!errors.empty()) {
        std::cout << "Configuration validation failed:\n";
        for (const auto& error : errors) {
            std::cout << "  ERROR: " << error << "\n";
        }
        return false;
    }
    
    return true;
}

void ConfigurationManager::applyDefaults(std::shared_ptr<nlm::Config> config) const {
    for (const auto& defaultVal : pImpl->defaults) {
        if (!config->has(defaultVal.key)) {
            config->set(defaultVal.key, defaultVal.value, defaultVal.source);
        }
    }
}

std::vector<std::string> ConfigurationManager::getDefaultKeys() const {
    std::vector<std::string> keys;
    for (const auto& defaultVal : pImpl->defaults) {
        keys.push_back(defaultVal.key);
    }
    return keys;
}

void ConfigurationManager::showConfigSummary(std::shared_ptr<nlm::Config> config) const {
    std::cout << "\n=== NLM Configuration Summary ===\n";
    
    std::cout << std::left << std::setw(25) << "Key" 
              << std::setw(15) << "Value" 
              << std::setw(12) << "Source" 
              << "Description" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    for (const auto& key : config->getKeys()) {
        // Get the value as string
        auto value = config->get<std::string>(key);
        
        // For source, we can't easily determine from ConfigValue, so we'll use a placeholder
        std::string sourceStr = "UNKNOWN";
        
        std::cout << std::left << std::setw(25) << key
                  << std::setw(15) << (value.value_or("") + " (file)")
                  << std::setw(12) << sourceStr;
        
        // Find description
        bool found = false;
        for (const auto& defaultVal : pImpl->defaults) {
            if (defaultVal.key == key) {
                std::cout << defaultVal.description;
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "(user-defined)";
        }
        std::cout << std::endl;
    }
}

bool ConfigurationManager::validateValue(const std::string& key, const std::string& value) const {
    // Basic type validation based on key name
    if (key == "neuron_count") {
        try {
            int64_t val = std::stoll(value);
            return val > 0;
        } catch (...) { return false; }
    }
    
    if (key == "simulation_timestep") {
        try {
            double val = std::stod(value);
            return val > 0.0 && val <= 0.1;
        } catch (...) { return false; }
    }
    
    if (key == "connection_probability") {
        try {
            double val = std::stod(value);
            return val >= 0.0 && val <= 1.0;
        } catch (...) { return false; }
    }
    
    if (key == "random_seed") {
        try {
            int64_t val = std::stoll(value);
            return true;
        } catch (...) { return false; }
    }
    
    if (key == "log_level") {
        return value == "DEBUG" || value == "INFO" || value == "WARN" || value == "ERROR";
    }
    
    if (key == "log_to_file") {
        return value == "true" || value == "false";
    }
    
    return true; // No validation for unknown keys
}

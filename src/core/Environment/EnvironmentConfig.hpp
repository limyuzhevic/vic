// Environment-based Configuration System
//
// This system provides environment variable support for NLM configuration,
// allowing flexible configuration management across different environments.

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <map>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>

namespace nlm {

// Environment variable mapping configuration keys to environment variable names
struct EnvironmentConfig {
    std::string configKey;
    std::string envVarName;
    std::string envVarPrefix = "NLM_";  // Default prefix
    bool required = false;
    std::string description;
};

// Environment configuration manager
class EnvironmentConfigManager {
public:
    static EnvironmentConfigManager instance;
    
    std::vector<EnvironmentConfig> envConfigs;
    std::map<std::string, std::string> envVars;
    
    EnvironmentConfigManager() {
        initializeEnvironmentConfigs();
        loadEnvironmentVariables();
    }
    
    void initializeEnvironmentConfigs() {
        // Brain configuration environment variables
        envConfigs.push_back({
            "brain.neuron_count", "NEURON_COUNT",
            "NLM_", true,
            "Number of neurons in the brain (can be overridden by NLM_NEURON_COUNT)"
        });
        
        envConfigs.push_back({
            "brain.synapse_density", "SYNAPSE_DENSITY",
            "NLM_", false,
            "Synapse connectivity density (can be overridden by NLM_SYNAPSE_DENSITY)"
        });
        
        envConfigs.push_back({
            "brain.connection_probability", "CONNECTION_PROBABILITY",
            "NLM_", false,
            "Connection probability for random connectivity"
        });
        
        // Plasticity configuration environment variables
        envConfigs.push_back({
            "plasticity.stdp.learning_rate", "STDP_LEARNING_RATE",
            "NLM_", false,
            "STDP learning rate"
        });
        
        envConfigs.push_back({
            "plasticity.stdp.tau_plus", "STDP_TAU_PLUS",
            "NLM_", false,
            "STDP time constant (plus)"
        });
        
        envConfigs.push_back({
            "plasticity.stdp.tau_minus", "STDP_TAU_MINUS",
            "NLM_", false,
            "STDP time constant (minus)"
        });
        
        // Neuromodulation configuration environment variables
        envConfigs.push_back({
            "neuromod.dopamine.scale", "DOPAMINE_SCALE",
            "NLM_", false,
            "Dopamine modulation scale"
        });
        
        // Environment configuration environment variables
        envConfigs.push_back({
            "environment.width", "ENVIRONMENT_WIDTH",
            "NLM_", false,
            "Environment width"
        });
        
        envConfigs.push_back({
            "environment.height", "ENVIRONMENT_HEIGHT",
            "NLM_", false,
            "Environment height"
        });
        
        // Simulation configuration environment variables
        envConfigs.push_back({
            "simulation.timestep", "SIMULATION_TIMESTEP",
            "NLM_", false,
            "Simulation timestep in seconds"
        });
        
        envConfigs.push_back({
            "random_seed", "RANDOM_SEED",
            "NLM_", false,
            "Random seed for reproducible experiments"
        });
    }
    
    void loadEnvironmentVariables() {
        // Clear existing variables
        envVars.clear();
        
        // Get all environment variables
        extern char** environ;
        for (char** env = environ; *env != nullptr; ++env) {
            std::string envStr = *env;
            size_t pos = envStr.find('=');
            if (pos != std::string::npos) {
                std::string key = envStr.substr(0, pos);
                std::string value = envStr.substr(pos + 1);
                envVars[key] = value;
            }
        }
    }
    
    // Get environment variable value
    std::string getEnvVar(const std::string& varName) {
        std::string key = varName;
        // Try with NLM_ prefix first
        std::string prefixedKey = "NLM_" + varName;
        if (envVars.find(prefixedKey) != envVars.end()) {
            return envVars[prefixedKey];
        }
        // Try without prefix
        if (envVars.find(key) != envVars.end()) {
            return envVars[key];
        }
        return "";
    }
    
    // Parse environment variable value to appropriate type
    template<typename T>
    T parseEnvVar(const std::string& varName, const T& defaultValue) {
        std::string value = getEnvVar(varName);
        if (value.empty()) {
            return defaultValue;
        }
        
        // Simple parsing for common types
        if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            try {
                return static_cast<T>(std::stoll(value));
            } catch (...) {
                NLM_LOG_WARNING("Failed to parse environment variable " + varName + " as int64_t");
                return defaultValue;
            }
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            try {
                return static_cast<T>(std::stod(value));
            } catch (...) {
                NLM_LOG_WARNING("Failed to parse environment variable " + varName + " as float");
                return defaultValue;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Parse boolean values
            std::string lowerValue = value;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(),
                          [](unsigned char c) { return std::tolower(c); });
            return (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes");
        } else if constexpr (std::is_same_v<T, std::string>) {
            return value;
        }
        
        return defaultValue;
    }
    
    // Apply environment configuration to config object
    void applyEnvironmentConfig(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("=== Applying Environment Configuration ===");
        
        int appliedCount = 0;
        for (const auto& envConfig : envConfigs) {
            std::string value = getEnvVar(envConfig.envVarName);
            if (!value.empty()) {
                // Parse value based on type
                std::variant<int64_t, float, bool, std::string> parsedValue;
                if (envConfig.configKey.find("neuron_count") != std::string::npos ||
                    envConfig.configKey.find("width") != std::string::npos ||
                    envConfig.configKey.find("height") != std::string::npos ||
                    envConfig.configKey.find("steps") != std::string::npos ||
                    envConfig.configKey.find("timestep") != std::string::npos ||
                    envConfig.configKey.find("seed") != std::string::npos) {
                    // Integer or float
                    try {
                        if (envConfig.configKey.find(".") != std::string::npos) {
                            // Float value
                            parsedValue = static_cast<float>(std::stod(value));
                        } else {
                            // Integer value
                            parsedValue = static_cast<int64_t>(std::stoll(value));
                        }
                    } catch (...) {
                        NLM_LOG_WARNING("Failed to parse environment variable " + envConfig.envVarName + ": " + value);
                        continue;
                    }
                } else if (envConfig.configKey.find("enable") != std::string::npos ||
                           envConfig.configKey.find("random") != std::string::npos) {
                    // Boolean value
                    std::string lowerValue = value;
                    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(),
                                  [](unsigned char c) { return std::tolower(c); });
                    parsedValue = (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes");
                } else {
                    // String value
                    parsedValue = value;
                }
                
                // Apply to configuration
                if (std::holds_alternative<int64_t>(parsedValue)) {
                    config->set(envConfig.configKey, std::get<int64_t>(parsedValue), ConfigSource::Environment);
                } else if (std::holds_alternative<float>(parsedValue)) {
                    config->set(envConfig.configKey, std::get<float>(parsedValue), ConfigSource::Environment);
                } else if (std::holds_alternative<bool>(parsedValue)) {
                    config->set(envConfig.configKey, std::get<bool>(parsedValue), ConfigSource::Environment);
                } else if (std::holds_alternative<std::string>(parsedValue)) {
                    config->set(envConfig.configKey, std::get<std::string>(parsedValue), ConfigSource::Environment);
                }
                
                NLM_LOG_INFO("Applied environment config: " + envConfig.configKey + " = " + value);
                appliedCount++;
            } else if (envConfig.required) {
                NLM_LOG_WARNING("Required environment variable " + envConfig.envVarName + " is not set");
            }
        }
        
        NLM_LOG_INFO("Applied " + std::to_string(appliedCount) + " environment variables to configuration");
    }
}; 

// Environment configuration instance
EnvironmentConfigManager EnvironmentConfigManager::instance;

// Convenience functions
void loadEnvironmentConfiguration(std::shared_ptr<Config> config) {
    EnvironmentConfigManager::instance.applyEnvironmentConfig(config);
}

void refreshEnvironmentVariables() {
    EnvironmentConfigManager::instance.loadEnvironmentVariables();
}

// Get environment variable
std::string getEnvironmentVariable(const std::string& varName) {
    return EnvironmentConfigManager::instance.getEnvVar(varName);
}

// Check if environment variable is set
bool isEnvironmentVariableSet(const std::string& varName) {
    return !EnvironmentConfigManager::instance.getEnvVar(varName).empty();
}

// List all environment variables
void listEnvironmentVariables() {
    NLM_LOG_INFO("=== Environment Variables ===");
    for (const auto& pair : EnvironmentConfigManager::instance.envVars) {
        NLM_LOG_INFO(pair.first + " = " + pair.second);
    }
}

} // namespace nlm

// Usage example:
void example() {
    std::cout << "=== Environment Configuration Example ===" << std::endl;
    
    // Create configuration
    auto config = std::make_shared<Config>();
    
    // Apply environment configuration
    loadEnvironmentConfiguration(config);
    
    // Check what environment variables are available
    listEnvironmentVariables();
    
    // Set some environment variables (for demonstration)
    std::cout << "\nSetting environment variables..." << std::endl;
    
    // Simulate environment variable setting
    putenv("NLM_NEURON_COUNT=2000");
    putenv("NLM_CONNECTION_PROBABILITY=0.15");
    putenv("NLM_STDP_LEARNING_RATE=0.002");
    
    // Refresh environment variables
    refreshEnvironmentVariables();
    
    // Apply environment configuration again
    loadEnvironmentConfiguration(config);
    
    // Check configuration values
    std::cout << "Configuration after environment application:" << std::endl;
    std::cout << "  brain.neuron_count: " << config->getOr<int64_t>("brain.neuron_count", 1000) << std::endl;
    std::cout << "  brain.connection_probability: " << config->getOr<float>("brain.connection_probability", 0.1f) << std::endl;
    std::cout << "  plasticity.stdp.learning_rate: " << config->getOr<float>("plasticity.stdp.learning_rate", 0.001f) << std::endl;
    
    std::cout << "\nEnvironment configuration example complete!" << std::endl;
}

// Example usage in main.cpp
// void main() {
//     auto config = std::make_shared<Config>();
//     config->set("brain.neuron_count", 1000LL, ConfigSource::Default);
//     config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
//     
//     // Apply environment configuration
//     loadEnvironmentConfiguration(config);
//     
//     // Configuration now includes environment variable values
// }
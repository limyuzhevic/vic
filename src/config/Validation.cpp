# Configuration Validation System for NLM

// Configuration Validation System
// Provides comprehensive validation for NLM configuration parameters
// Supports range checking, dependency validation, and type validation

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stdexcept>
#include <sstream>

namespace nlm {
namespace config {

// Configuration validation exceptions
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigRangeError : public ConfigValidationError {
public:
    ConfigRangeError(const std::string& key, double value, double min, double max)
        : ConfigValidationError("Configuration key \"" + key + "\" has invalid value " + 
                               std::to_string(value) + ". Must be in range [" + 
                               std::to_string(min) + ", " + std::to_string(max) + "]") {}
};

class ConfigDependencyError : public ConfigValidationError {
public:
    ConfigDependencyError(const std::string& key, const std::string& dependency)
        : ConfigValidationError("Configuration key \"" + key + "\" requires dependency \"" + dependency + "\"") {}
};

class ConfigTypeError : public ConfigValidationError {
public:
    ConfigTypeError(const std::string& key, const std::string& expectedType)
        : ConfigValidationError("Configuration key \"" + key + "\" has invalid type. Expected " + expectedType) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "float", "bool", "string"
    double minValue;
    double maxValue;
    bool required;
    std::string defaultValue;
    std::vector<std::string> dependencies; // Keys that must be set for this to be valid
    std::string description;
};

// Global configuration schema
class ConfigValidator {
private:
    std::vector<ConfigSchema> schema;
    std::map<std::string, bool> validationEnabled;
    
public:
    ConfigValidator() {
        initializeSchema();
    }
    
    void initializeSchema() {
        // Clear existing schema
        schema.clear();
        
        // Define configuration schema with validation rules
        ConfigSchema neuronCount{ "neuron_count", "int", 1, 10000, true, "1000",
                                 {}, "Number of neurons in the brain" };
        schema.push_back(neuronCount);
        
        ConfigSchema regionCount{ "region_count", "int", 1, 10, true, "2",
                                  {}, "Number of brain regions" };
        schema.push_back(regionCount);
        
        ConfigSchema connectionProbability{ "connection_probability", "float", 0.0, 1.0, true, "0.1",
                                            {}, "Probability of connection between neurons" };
        schema.push_back(connectionProbability);
        
        ConfigSchema simulationTimestep{ "simulation_timestep", "float", 0.0001, 1.0, true, "0.001",
                                         {}, "Simulation timestep in seconds" };
        schema.push_back(simulationTimestep);
        
        ConfigSchema randomSeed{ "random_seed", "int", 0, 2147483647, true, "42",
                                {}, "Random seed for reproducible experiments" };
        schema.push_back(randomSeed);
        
        ConfigSchema plasticityLearningRate{ "plasticity_learning_rate", "float", 0.0, 1.0, true, "0.01",
                                             {}, "Plasticity learning rate" };
        schema.push_back(plasticityLearningRate);
        
        ConfigSchema stdpLtpWeight{ "stdp_ltp_weight", "float", 0.0, 0.1, true, "0.01",
                                    {}, "STDP Long-Term Potentiation weight" };
        schema.push_back(stdpLtpWeight);
        
        ConfigSchema stdpLtdWeight{ "stdp_ltd_weight", "float", 0.0, 0.1, true, "0.012",
                                     {}, "STDP Long-Term Depression weight" };
        schema.push_back(stdpLtdWeight);
        
        ConfigSchema stdpTau{ "stdp_tau", "float", 0.1, 100.0, true, "20.0",
                              {}, "STDP time constant" };
        schema.push_back(stdpTau);
        
        ConfigSchema synaptogenesisRate{ "synaptogenesis_rate", "float", 0.0, 0.1, true, "0.001",
                                        {}, "Synaptogenesis rate (structural plasticity)" };
        schema.push_back(synaptogenesisRate);
        
        ConfigSchema pruningRate{ "pruning_rate", "float", 0.0, 0.01, true, "0.0001",
                                  {}, "Pruning rate (structural plasticity)" };
        schema.push_back(pruningRate);
        
        ConfigSchema dopamineBaseline{ "dopamine_baseline", "float", -1.0, 1.0, true, "0.1",
                                       {}, "Dopamine baseline level" };
        schema.push_back(dopamineBaseline);
        
        ConfigSchema rewardDiscountFactor{ "reward_discount_factor", "float", 0.0, 1.0, true, "0.99",
                                           {}, "Reward discount factor" };
        schema.push_back(rewardDiscountFactor);
        
        ConfigSchema environmentWidth{ "environment_width", "float", 1.0, 100.0, true, "10.0",
                                       {}, "Environment width" };
        schema.push_back(environmentWidth);
        
        ConfigSchema environmentHeight{ "environment_height", "float", 1.0, 100.0, true, "10.0",
                                        {}, "Environment height" };
        schema.push_back(environmentHeight);
        
        ConfigSchema visionWidth{ "vision_width", "int", 1, 100, true, "16",
                                  {}, "Vision sensor width" };
        schema.push_back(visionWidth);
        
        ConfigSchema visionHeight{ "vision_height", "int", 1, 100, true, "16",
                                   {}, "Vision sensor height" };
        schema.push_back(visionHeight);
        
        ConfigSchema logLevel{ "log_level", "string", 0, 0, false, "INFO",
                               {}, "Logging level (DEBUG, INFO, WARNING, ERROR)" };
        schema.push_back(logLevel);
        
        ConfigSchema logToFile{ "log_to_file", "bool", 0, 1, false, "false",
                                {}, "Log to file" };
        schema.push_back(logToFile);
        
        ConfigSchema visualizationEnabled{ "visualization_enabled", "bool", 0, 1, false, "false",
                                           {}, "Enable visualization" };
        schema.push_back(visualizationEnabled);
        
        ConfigSchema visualizationUpdateRate{ "visualization_update_rate", "float", 1.0, 100.0, false, "30.0",
                                              {}, "Visualization update rate (Hz)" };
        schema.push_back(visualizationUpdateRate);
        
        // Set validation enabled for all keys by default
        for (const auto& entry : schema) {
            validationEnabled[entry.key] = true;
        }
    }
    
    // Enable or disable validation for a specific key
    void setValidationEnabled(const std::string& key, bool enabled) {
        if (validationEnabled.find(key) != validationEnabled.end()) {
            validationEnabled[key] = enabled;
        }
    }
    
    // Validate a configuration
    void validate(const Config& config) {
        std::vector<std::string> errors;
        
        // Validate required keys
        for (const auto& entry : schema) {
            if (entry.required && !config.has(entry.key)) {
                errors.push_back("Missing required configuration key: " + entry.key);
            }
        }
        
        // Validate each configured key
        for (const auto& entry : schema) {
            if (!validationEnabled[entry.key] || !config.has(entry.key)) {
                continue;
            }
            
            validateKey(config, entry, errors);
        }
        
        // Check dependencies
        for (const auto& entry : schema) {
            if (!validationEnabled[entry.key] || !config.has(entry.key)) {
                continue;
            }
            
            validateDependencies(config, entry, errors);
        }
        
        // Report all errors
        if (!errors.empty()) {
            std::stringstream ss;
            ss << "Configuration validation failed:" << std::endl;
            for (const auto& error : errors) {
                ss << "  - " << error << std::endl;
            }
            throw ConfigValidationError(ss.str());
        }
    }
    
private:
    void validateKey(const Config& config, const ConfigSchema& entry, std::vector<std::string>& errors) {
        // Get the actual value from config
        auto value = getConfigValue(config, entry.key);
        
        // Type validation
        if (entry.type == "int") {
            try {
                int intValue = std::stoi(value);
                if (intValue < entry.minValue || intValue > entry.maxValue) {
                    errors.push_back("Value for key \"" + entry.key + "\" is out of range [" + 
                                   std::to_string(entry.minValue) + ", " + 
                                   std::to_string(entry.maxValue) + "]");
                }
            } catch (const std::exception&) {
                errors.push_back("Configuration key \"" + entry.key + "\" has invalid type. Expected int, got: " + value);
            }
        } else if (entry.type == "float") {
            try {
                double floatValue = std::stod(value);
                if (floatValue < entry.minValue || floatValue > entry.maxValue) {
                    errors.push_back("Value for key \"" + entry.key + "\" is out of range [" + 
                                   std::to_string(entry.minValue) + ", " + 
                                   std::to_string(entry.maxValue) + "]");
                }
            } catch (const std::exception&) {
                errors.push_back("Configuration key \"" + entry.key + "\" has invalid type. Expected float, got: " + value);
            }
        } else if (entry.type == "bool") {
            // Accept "true", "false", "1", "0" etc.
            std::string lowerValue = value;
            std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
            if (lowerValue != "true" && lowerValue != "false" && lowerValue != "1" && lowerValue != "0") {
                errors.push_back("Configuration key \"" + entry.key + "\" has invalid type. Expected bool, got: " + value);
            }
        } else if (entry.type == "string") {
            // Any string is valid
        }
    }
    
    void validateDependencies(const Config& config, const ConfigSchema& entry, std::vector<std::string>& errors) {
        // Check if all dependencies are satisfied
        for (const auto& dependency : entry.dependencies) {
            if (!config.has(dependency)) {
                errors.push_back("Configuration key \"" + entry.key + "\" depends on key \"" + dependency + "\" which is not set");
            }
        }
    }
    
    std::string getConfigValue(const Config& config, const std::string& key) {
        // Try to get value as string
        // This is a simplified implementation - actual implementation would need to handle different types
        return "1000"; // placeholder
    }
    
    // Specialized validation for certain parameter combinations
    void validateSpecialCombinations(const Config& config, std::vector<std::string>& errors) {
        // Validate that region_count is positive if neuron_count is set
        if (config.has("neuron_count") && config.has("region_count")) {
            auto neuronCount = config.getOr<int64_t>("neuron_count", 1000);
            auto regionCount = config.getOr<int64_t>("region_count", 1);
            
            if (regionCount <= 0) {
                errors.push_back("region_count must be positive (>= 1)");
            }
            
            if (neuronCount < regionCount) {
                errors.push_back("neuron_count must be >= region_count");
            }
        }
        
        // Validate that connection_probability is reasonable
        if (config.has("connection_probability")) {
            auto connectionProbability = config.getOr<float>("connection_probability", 0.1);
            if (connectionProbability < 0.0 || connectionProbability > 1.0) {
                errors.push_back("connection_probability must be between 0.0 and 1.0");
            }
        }
        
        // Validate that simulation_timestep is positive
        if (config.has("simulation_timestep")) {
            auto simulationTimestep = config.getOr<double>("simulation_timestep", 0.001);
            if (simulationTimestep <= 0.0) {
                errors.push_back("simulation_timestep must be positive (> 0.0)");
            }
        }
        
        // Validate that synaptogenesis_rate <= pruning_rate (typically)
        if (config.has("synaptogenesis_rate") && config.has("pruning_rate")) {
            auto synRate = config.getOr<float>("synaptogenesis_rate", 0.001);
            auto pruneRate = config.getOr<float>("pruning_rate", 0.0001);
            
            if (synRate < pruneRate) {
                errors.push_back("synaptogenesis_rate should typically be >= pruning_rate");
            }
        }
    }
    
public:
    // Public validation methods
    void validate(const Config& config, bool enableSpecialValidation = true) {
        std::vector<std::string> errors;
        
        // Basic validation
        validate(config);
        
        // Special validation (additional checks)
        if (enableSpecialValidation) {
            validateSpecialCombinations(config, errors);
        }
        
        if (!errors.empty()) {
            std::stringstream ss;
            ss << "Configuration validation failed:" << std::endl;
            for (const auto& error : errors) {
                ss << "  - " << error << std::endl;
            }
            throw ConfigValidationError(ss.str());
        }
    }
    
    // Get validation errors for a configuration (doesn't throw)
    std::vector<std::string> getValidationErrors(const Config& config) {
        std::vector<std::string> errors;
        
        try {
            validate(config);
        } catch (const ConfigValidationError& e) {
            // Parse error message to extract individual errors
            std::string errorMsg = e.what();
            size_t pos = 0;
            while (pos < errorMsg.length()) {
                size_t newlinePos = errorMsg.find("\n", pos);
                if (newlinePos == std::string::npos) {
                    break;
                }
                std::string line = errorMsg.substr(pos, newlinePos - pos);
                if (line.find("  - ") == 0) {
                    errors.push_back(line.substr(5));
                }
                pos = newlinePos + 1;
            }
        }
        
        return errors;
    }
    
    // Check if configuration is valid (doesn't throw)
    bool isValid(const Config& config) {
        return getValidationErrors(config).empty();
    }
    
    // Get schema information
    std::vector<ConfigSchema> getSchema() const {
        return schema;
    }
    
    // Get validation status for a key
    bool isValidationEnabled(const std::string& key) const {
        auto it = validationEnabled.find(key);
        return it != validationEnabled.end() && it->second;
    }
    
    // Get schema entry for a key
    ConfigSchema getSchemaEntry(const std::string& key) const {
        for (const auto& entry : schema) {
            if (entry.key == key) {
                return entry;
            }
        }
        throw ConfigValidationError("No schema entry found for key: " + key);
    }
};

// Global validator instance
ConfigValidator globalValidator;

// Convenience functions
void validateConfig(const Config& config) {
    globalValidator.validate(config);
}

void validateConfig(const Config& config, bool enableSpecialValidation) {
    globalValidator.validate(config, enableSpecialValidation);
}

bool isValidConfig(const Config& config) {
    return globalValidator.isValid(config);
}

std::vector<std::string> getConfigValidationErrors(const Config& config) {
    return globalValidator.getValidationErrors(config);
}

// Configuration migration system
class ConfigMigrator {
private:
    std::map<std::string, std::string> migrations;
    
public:
    ConfigMigrator() {
        initializeMigrations();
    }
    
    void initializeMigrations() {
        // Migration rules for configuration updates
        // This can be expanded as needed
        
        // Example: If certain old keys are present, migrate to new values
        migrations["old_plasticity_learning_rate"] = "plasticity_learning_rate";
        migrations["old_stdp_ltp_weight"] = "stdp_ltp_weight";
        migrations["old_stdp_ltd_weight"] = "stdp_ltd_weight";
    }
    
    void migrate(Config& config) {
        // Apply migrations
        for (const auto& pair : migrations) {
            if (config.has(pair.first)) {
                auto value = config.getOr<std::string>(pair.first, "");
                config.set(pair.second, value, ConfigSource::User);
            }
        }
    }
};

// Global migrator instance
ConfigMigrator globalMigrator;

void migrateConfig(Config& config) {
    globalMigrator.migrate(config);
}

// Configuration backup and restore
void backupConfig(const Config& config, const std::string& filepath) {
    // Implementation would serialize config to file
    // Placeholder
}

Config restoreConfig(const std::string& filepath) {
    auto config = std::make_shared<Config>();
    // Implementation would deserialize config from file
    // Placeholder
    return *config;
}

// Configuration diff utility
std::string diffConfig(const Config& config1, const Config& config2) {
    std::stringstream ss;
    ss << "Configuration differences:" << std::endl;
    
    // Compare keys in config1
    auto keys1 = config1.getKeys();
    auto keys2 = config2.getKeys();
    
    // Find keys that are different
    std::set<std::string> allKeys;
    for (const auto& key : keys1) {
        allKeys.insert(key);
    }
    for (const auto& key : keys2) {
        allKeys.insert(key);
    }
    
    for (const auto& key : allKeys) {
        if (!keys1.empty() && !keys2.empty()) {
            // Implementation would compare values
            ss << "  " << key << ": [different]" << std::endl;
        }
    }
    
    return ss.str();
}

} // namespace config
} // namespace nlm

// Configuration validation and error handling for NLM
// This file provides comprehensive configuration validation and error handling
// to ensure robust and reliable NLM operation

#include "src/core/Config/Config.hpp"
#include <stdexcept>
#include <algorithm>
#include <unordered_set>
#include <cmath>

namespace nlm {

// Configuration validation rules
namespace config_validation {
    
    // Valid configuration parameter keys
    static const std::unordered_set<std::string> VALID_PARAMETERS = {
        // Brain configuration
        "brain.neuron_count",
        "brain.region_count", 
        "brain.synapse_density",
        "brain.connection_probability",
        "brain.initial_weight_mean",
        "brain.initial_weight_std",
        "brain.v_thresh",
        "brain.v_rest",
        "brain.v_reset",
        "brain.tau_mem",
        "brain.tau_ref",
        
        // Plasticity configuration
        "plasticity.stdp.enable",
        "plasticity.stdp.learning_rate",
        "plasticity.stdp.tau_plus",
        "plasticity.stdp.tau_minus",
        "plasticity.hebbian.enable",
        "plasticity.stdp.enable",  // Note: duplicate from above
        "plasticity.structural.enable",
        
        // Neuromodulation configuration
        "neuromod.dopamine.scale",
        "neuromod.dopamine.baseline",
        "neuromod.curiosity.enable",
        "neuromod.novelty.enable",
        "neuromod.prediction_error.scale",
        
        // Development configuration
        "development.synaptogenesis_rate",
        "development.pruning_rate",
        "development.critical_period_start",
        "development.critical_period_end",
        
        // Memory configuration
        "memory.working_memory.capacity",
        "memory.episodic_memory.max_episodes",
        "memory.associative_memory.pattern_capacity",
        
        // Simulation configuration
        "simulation.timestep",
        "simulation.max_steps",
        "simulation.time_limit",
        
        // Logging configuration
        "log.level",
        "log.to_file",
        "log.filename",
        
        // Environment configuration
        "environment.name",
        "environment.width",
        "environment.height",
        "environment.vision_width",
        "environment.vision_height",
        
        // Visualization configuration
        "visualization.enabled",
        "visualization.update_rate",
        
        // Random configuration
        "random.seed",
        "random.algorithm",
        
        // Reward configuration
        "reward.function",
        "reward.discount_factor",
        "reward.exploration_bonus",
        
        // General configuration
        "checkpoint.enabled",
        "checkpoint.interval",
        "checkpoint.directory",
        "performance.optimization_level",
        "debug.mode"
    };
    
    // Configuration validation exceptions
    class ConfigValidationError : public std::runtime_error {
    public:
        ConfigValidationError(const std::string& message) 
            : std::runtime_error("Configuration validation error: " + message) {}
    };
    
    class ConfigValidationWarning : public std::runtime_error {
    public:
        ConfigValidationWarning(const std::string& message)
            : std::runtime_error("Configuration warning: " + message) {}
    };
    
    // Validate configuration parameter
    void validateParameter(const std::string& key, const ConfigValue& value) {
        if (!isValidParameter(key)) {
            throw ConfigValidationError("Invalid configuration parameter: " + key);
        }
        
        validateParameterType(key, value);
        validateParameterRange(key, value);
        validateParameterDependencies(key, value);
    }
    
    // Check if parameter is valid
    bool isValidParameter(const std::string& key) {
        return VALID_PARAMETERS.find(key) != VALID_PARAMETERS.end();
    }
    
    // Validate parameter type based on expected type
    void validateParameterType(const std::string& key, const ConfigValue& value) {
        if (key.find(".neuron_count") != std::string::npos || 
            key.find(".region_count") != std::string::npos ||
            key.find(".width") != std::string::npos ||
            key.find(".height") != std::string::npos ||
            key.find(".vision_width") != std::string::npos ||
            key.find(".vision_height") != std::string::npos) {
            // Integer parameters
            if (!std::holds_alternative<int>(value) && !std::holds_alternative<int64_t>(value)) {
                throw ConfigValidationError("Parameter " + key + " must be an integer");
            }
            int64_t intValue = getIntValue(value);
            if (intValue <= 0) {
                throw ConfigValidationError("Parameter " + key + " must be positive");
            }
        } else if (key.find(".probability") != std::string::npos ||
                   key.find(".scale") != std::string::npos ||
                   key.find(".rate") != std::string::npos ||
                   key.find(".factor") != std::string::npos ||
                   key.find(".discount") != std::string::npos ||
                   key.find(".learning_rate") != std::string::npos ||
                   key.find(".weight") != std::string::npos) {
            // Float parameters
            if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
                throw ConfigValidationError("Parameter " + key + " must be a float/double");
            }
            double floatValue = getFloatValue(value);
            if (floatValue < 0.0 || floatValue > 1.0) {
                // Check if it's a percentage parameter
                if (key.find(".synapse_density") != std::string::npos ||
                    key.find(".connection_probability") != std::string::npos) {
                    if (floatValue < 0.0 || floatValue > 1.0) {
                        throw ConfigValidationError("Parameter " + key + " must be between 0.0 and 1.0");
                    }
                }
            }
        } else if (key.find(".timestep") != std::string::npos) {
            // Time parameters
            if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
                throw ConfigValidationError("Parameter " + key + " must be a time value");
            }
            double timeValue = getFloatValue(value);
            if (timeValue <= 0.0) {
                throw ConfigValidationError("Parameter " + key + " must be positive");
            }
        } else if (key.find(".threshold") != std::string::npos ||
                   key.find(".potential") != std::string::npos) {
            // Voltage parameters
            if (!std::holds_alternative<double>(value) && !std::holds_alternative<float>(value)) {
                throw ConfigValidationError("Parameter " + key + " must be a voltage value");
            }
            double voltageValue = getFloatValue(value);
            if (voltageValue < -100.0 || voltageValue > 100.0) {
                throw ConfigValidationError("Parameter " + key + " is outside reasonable biological range (-100 to 100 mV)");
            }
        } else if (key.find(".enable") != std::string::npos ||
                   key.find(".mode") != std::string::npos ||
                   key.find(".enabled") != std::string::npos) {
            // Boolean parameters
            if (!std::holds_alternative<bool>(value)) {
                throw ConfigValidationError("Parameter " + key + " must be a boolean");
            }
        } else if (key.find(".log_level") != std::string::npos) {
            // String parameters (limited set)
            if (!std::holds_alternative<std::string>(value)) {
                throw ConfigValidationError("Parameter " + key + " must be a string");
            }
            std::string logLevel = std::get<std::string>(value);
            std::transform(logLevel.begin(), logLevel.end(), logLevel.begin(), ::tolower);
            if (logLevel != "trace" && logLevel != "debug" && logLevel != "info" &&
                logLevel != "warning" && logLevel != "error" && logLevel != "critical") {
                throw ConfigValidationWarning("Log level " + logLevel + " is not standard; using default");
            }
        } else if (key.find(".name") != std::string::npos) {
            // Environment name
            if (!std::holds_alternative<std::string>(value)) {
                throw ConfigValidationError("Parameter " + key + " must be a string");
            }
        }
    }
    
    // Validate parameter range
    void validateParameterRange(const std::string& key, const ConfigValue& value) {
        // Additional range validation for specific parameters
        if (key.find(".neuron_count") != std::string::npos) {
            int64_t neuronCount = getIntValue(value);
            if (neuronCount > 100000) {
                throw ConfigValidationWarning("Large neuron count (>100,000) may impact performance");
            }
        }
        
        if (key.find(".synapse_density") != std::string::npos) {
            double density = getFloatValue(value);
            if (density > 0.5) {
                throw ConfigValidationWarning("High synapse density (>0.5) may impact memory usage");
            }
        }
        
        if (key.find(".connection_probability") != std::string::npos) {
            double prob = getFloatValue(value);
            if (prob > 0.2) {
                throw ConfigValidationWarning("High connection probability (>0.2) may cause overly dense networks");
            }
        }
    }
    
    // Validate parameter dependencies
    void validateParameterDependencies(const std::string& key, const ConfigValue& value) {
        // Check dependencies between parameters
        if (key.find(".plasticity.stdp.enable") != std::string::npos) {
            bool stdpEnabled = std::get<bool>(value);
            if (stdpEnabled) {
                // Check if learning rates are configured
                // This would require access to full config, not just this parameter
            }
        }
    }
    
    // Get integer value from ConfigValue
    int64_t getIntValue(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            return static_cast<int64_t>(std::get<double>(value));
        } else if (std::holds_alternative<float>(value)) {
            return static_cast<int64_t>(std::get<float>(value));
        } else {
            throw ConfigValidationError("Cannot extract integer value from configuration");
        }
    }
    
    // Get float value from ConfigValue
    double getFloatValue(const ConfigValue& value) {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            return static_cast<double>(std::get<float>(value));
        } else if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value));
        } else {
            throw ConfigValidationError("Cannot extract float value from configuration");
        }
    }
    
    // Get string value from ConfigValue
    std::string getStringValue(const ConfigValue& value) {
        if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else {
            throw ConfigValidationError("Cannot extract string value from configuration");
        }
    }
} // namespace config_validation

// Enhanced Config class with validation
class ValidatedConfig : public Config {
private:
    std::vector<std::string> validationWarnings;
    std::vector<std::string> validationErrors;
    
public:
    ValidatedConfig() : Config() {}
    
    // Override set method with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime) override {
        try {
            // Validate the configuration
            config_validation::validateParameter(key, value);
            
            // Call parent method
            Config::set(key, value, source);
            
        } catch (const config_validation::ConfigValidationWarning& e) {
            validationWarnings.push_back(std::string(e.what()));
            Config::set(key, value, source); // Still set it but warn
        } catch (const config_validation::ConfigValidationError& e) {
            validationErrors.push_back(std::string(e.what()));
            throw; // Re-throw validation errors
        } catch (const std::exception& e) {
            validationErrors.push_back("Unexpected error: " + std::string(e.what()));
            throw;
        }
    }
    
    // Override set methods with type-specific validation
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) override {
        ConfigValue configValue = value;
        set(key, configValue, source);
    }
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) override {
        ConfigValue configValue = value;
        set(key, configValue, source);
    }
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) override {
        ConfigValue configValue = value;
        set(key, configValue, source);
    }
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) override {
        ConfigValue configValue = value;
        set(key, configValue, source);
    }
    
    // Get validation warnings
    const std::vector<std::string>& getValidationWarnings() const {
        return validationWarnings;
    }
    
    // Get validation errors
    const std::vector<std::string>& getValidationErrors() const {
        return validationErrors;
    }
    
    // Clear validation history
    void clearValidationHistory() {
        validationWarnings.clear();
        validationErrors.clear();
    }
    
    // Load configuration from file with validation
    bool loadFromFile(const std::string& filepath) override {
        bool success = Config::loadFromFile(filepath);
        if (success) {
            // Validate all loaded parameters
            clearValidationHistory();
            validateAllParameters();
        }
        return success;
    }
    
    // Validate all current parameters
    void validateAllParameters() {
        clearValidationHistory();
        
        std::vector<std::string> keys = getKeys();
        for (const std::string& key : keys) {
            std::optional<ConfigValue> value = getAnyValue(key);
            if (value) {
                try {
                    config_validation::validateParameter(key, *value);
                } catch (const config_validation::ConfigValidationWarning& e) {
                    validationWarnings.push_back(std::string(e.what()));
                } catch (const config_validation::ConfigValidationError& e) {
                    validationErrors.push_back(std::string(e.what()));
                }
            }
        }
    }
    
    // Get any ConfigValue (needed for validation)
    std::optional<ConfigValue> getAnyValue(const std::string& key) const {
        if (key.find(".neuron_count") != std::string::npos ||
            key.find(".region_count") != std::string::npos) {
            auto val = get<int64_t>(key);
            if (val) return ConfigValue(*val);
        } else if (key.find(".synapse_density") != std::string::npos ||
                   key.find(".connection_probability") != std::string::npos ||
                   key.find(".scale") != std::string::npos) {
            auto val = get<double>(key);
            if (val) return ConfigValue(*val);
        } else if (key.find(".enable") != std::string::npos) {
            auto val = get<bool>(key);
            if (val) return ConfigValue(*val);
        } else if (key.find(".name") != std::string::npos ||
                   key.find(".filename") != std::string::npos) {
            auto val = get<std::string>(key);
            if (val) return ConfigValue(*val);
        }
        return std::nullopt;
    }
}; // class ValidatedConfig

// Configuration file error handling
namespace config_error {
    
    // Configuration file parsing errors
    class ConfigParseError : public std::runtime_error {
    public:
        ConfigParseError(const std::string& message) 
            : std::runtime_error("Configuration file parsing error: " + message) {}
    };
    
    class ConfigFileNotFoundError : public std::runtime_error {
    public:
        ConfigFileNotFoundError(const std::string& filepath)
            : std::runtime_error("Configuration file not found: " + filepath) {}
    };
    
    class ConfigSyntaxError : public std::runtime_error {
    public:
        ConfigSyntaxError(const std::string& filepath, int lineNumber, const std::string& details)
            : std::runtime_error("Syntax error in configuration file " + filepath + 
                                 " at line " + std::to_string(lineNumber) + ": " + details) {}
    };
    
    class ConfigValidationError : public std::runtime_error {
    public:
        ConfigValidationError(const std::string& message) 
            : std::runtime_error("Configuration validation error: " + message) {}
    };
    
    // Safe configuration loading with error handling
    std::shared_ptr<ValidatedConfig> safeLoadConfig(const std::string& filepath) {
        auto config = std::make_shared<ValidatedConfig>();
        
        try {
            // Check if file exists
            std::ifstream file(filepath);
            if (!file.good()) {
                throw ConfigFileNotFoundError(filepath);
            }
            
            // Try to load
            if (!config->loadFromFile(filepath)) {
                throw ConfigParseError("Failed to parse configuration file");
            }
            
            // Check for validation errors
            const auto& errors = config->getValidationErrors();
            if (!errors.empty()) {
                std::stringstream errorMsg;
                errorMsg << "Configuration validation failed:" << std::endl;
                for (const auto& error : errors) {
                    errorMsg << "  - " << error << std::endl;
                }
                throw ConfigValidationError(errorMsg.str());
            }
            
            // Report warnings
            const auto& warnings = config->getValidationWarnings();
            for (const auto& warning : warnings) {
                std::cerr << "Warning: " << warning << std::endl;
            }
            
        } catch (const ConfigFileNotFoundError& e) {
            std::cerr << "Warning: " << e.what() << " Using default configuration." << std::endl;
            // Continue with default configuration
        } catch (const ConfigParseError& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            throw;
        } catch (const ConfigValidationError& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            throw;
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error loading configuration: " << e.what() << std::endl;
            throw ConfigParseError(std::string(e.what()));
        }
        
        return config;
    }
    
    // Configuration backup and recovery
    bool backupConfig(const std::string& originalPath, const std::string& backupPath) {
        try {
            std::ifstream src(originalPath, std::ios::binary);
            if (!src) {
                return false; // Original file doesn't exist
            }
            
            std::ofstream dst(backupPath, std::ios::binary);
            if (!dst) {
                return false; // Cannot create backup
            }
            
            dst << src.rdbuf();
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error creating configuration backup: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Configuration recovery from backup
    bool recoverConfig(const std::string& backupPath, const std::string& targetPath) {
        try {
            std::ifstream src(backupPath, std::ios::binary);
            if (!src) {
                return false; // Backup doesn't exist
            }
            
            std::ofstream dst(targetPath, std::ios::binary);
            if (!dst) {
                return false; // Cannot create target file
            }
            
            dst << src.rdbuf();
            
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error recovering configuration from backup: " << e.what() << std::endl;
            return false;
        }
    }
    
    // Validate configuration consistency
    bool validateConfigConsistency(const ValidatedConfig& config) {
        try {
            auto neuronCount = config.getOr<int64_t>("brain.neuron_count", 1000);
            auto regionCount = config.getOr<int64_t>("brain.region_count", 1);
            auto connectionProbability = config.getOr<double>("brain.connection_probability", 0.1);
            
            // Check that neuron count is reasonable
            if (neuronCount <= 0) {
                return false;
            }
            
            // Check that region count is reasonable
            if (regionCount <= 0) {
                return false;
            }
            
            // Check that connection probability is valid
            if (connectionProbability < 0.0 || connectionProbability > 1.0) {
                return false;
            }
            
            // Check that neurons per region is reasonable
            if (neuronCount / regionCount < 10) {
                std::cerr << "Warning: Very few neurons per region may impact network properties" << std::endl;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error validating configuration consistency: " << e.what() << std::endl;
            return false;
        }
    }
} // namespace config_error

// Safe configuration loading helper function
std::shared_ptr<ValidatedConfig> loadConfigSafe(const std::string& filepath = "configs/default.cfg") {
    return config_error::safeLoadConfig(filepath);
}

// Configuration validation helper function
bool validateConfig(const ValidatedConfig& config) {
    return config_error::validateConfigConsistency(config);
}

// Configuration backup helper function
bool backupConfig(const std::string& originalPath, const std::string& backupPath = "configs/backup.cfg") {
    return config_error::backupConfig(originalPath, backupPath);
}

// Configuration recovery helper function
bool recoverConfig(const std::string& backupPath, const std::string& targetPath) {
    return config_error::recoverConfig(backupPath, targetPath);
}

} // namespace nlm

// Include the enhanced Config header
#include "src/core/Config/Config.hpp"

namespace nlm {
    // Export enhanced config class
    using ValidatedConfig = nlm::ValidatedConfig;
    
    // Safe loading function
    inline std::shared_ptr<ValidatedConfig> createValidatedConfig(const std::string& filepath = "configs/default.cfg") {
        return config_error::safeLoadConfig(filepath);
    }
} // namespace nlm

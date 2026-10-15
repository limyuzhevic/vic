#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>
#include <sstream>

namespace nlm {

// Configuration validation errors
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "", 
                         const std::string& value = "", const std::string& expected = "")
        : std::runtime_error(formatMessage(message, key, value, expected)),
          key(key), value(value), expected(expected) {}
    
    const std::string& getKey() const { return key; }
    const std::string& getValue() const { return value; }
    const std::string& getExpected() const { return expected; }
    
private:
    static std::string formatMessage(const std::string& message, const std::string& key,
                                     const std::string& value, const std::string& expected) {
        std::ostringstream oss;
        oss << message;
        if (!key.empty()) oss << " (key: '" << key << "')";
        if (!value.empty()) oss << " (value: '" << value << "')";
        if (!expected.empty()) oss << " (expected: '" << expected << "')";
        return oss.str();
    }
    
    std::string key;
    std::string value;
    std::string expected;
};

// Configuration validation constraints
struct ConfigValidationConstraint {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool isRequired = false;
    std::string defaultValue;
    
    ConfigValidationConstraint(const std::string& k, std::function<bool(const ConfigValue&)> v,
                               const std::string& err, bool req = false, const std::string& def = "")
        : key(k), validator(v), errorMessage(err), isRequired(req), defaultValue(def) {}
};

// Phase 6 configuration schema
class Phase6ConfigSchema {
public:
    Phase6ConfigSchema() {
        // Define configuration constraints for Phase 6
        initializeSchema();
    }
    
    // Validate configuration against schema
    void validate(const Config& config) const {
        std::vector<std::string> errors;
        
        for (const auto& constraint : constraints) {
            bool isValid = true;
            std::string errorMsg = constraint.errorMessage;
            
            if (constraint.isRequired) {
                if (!config.has(constraint.key)) {
                    errors.push_back(
                        "Missing required configuration parameter: '" + constraint.key + "'");
                    continue;
                }
                
                auto value = config.get<ConfigValue>(constraint.key);
                if (!value) {
                    errors.push_back(
                        "Required configuration parameter has invalid type: '" + constraint.key + "'");
                    continue;
                }
                
                if (!constraint.validator(*value)) {
                    errors.push_back(errorMsg);
                }
            } else if (config.has(constraint.key)) {
                auto value = config.get<ConfigValue>(constraint.key);
                if (!value) {
                    errors.push_back(
                        "Configuration parameter has invalid type: '" + constraint.key + "'");
                    continue;
                }
                
                if (!constraint.validator(*value)) {
                    errors.push_back(errorMsg);
                }
            } else if (!constraint.defaultValue.empty()) {
                // Will be set by defaults later
            }
        }
        
        if (!errors.empty()) {
            throw ConfigValidationError("Configuration validation failed:", "", 
                                      "", "\n  " + join(errors, "\n  "));
        }
    }
    
    // Apply default values for missing parameters
    void applyDefaults(Config& config) const {
        for (const auto& constraint : constraints) {
            if (!config.has(constraint.key) && !constraint.defaultValue.empty()) {
                // Set default value based on constraint
                setDefaultValue(config, constraint);
            }
        }
    }
    
    // Get validation error messages for debugging
    std::string getValidationReport(const Config& config) const {
        std::vector<std::string> warnings;
        
        for (const auto& constraint : constraints) {
            if (config.has(constraint.key)) {
                auto value = config.get<ConfigValue>(constraint.key);
                if (value && !constraint.validator(*value)) {
                    warnings.push_back(
                        "Configuration parameter '" + constraint.key + "' is outside recommended range");
                }
            }
        }
        
        std::ostringstream oss;
        oss << "Configuration Validation Report:";
        if (warnings.empty()) {
            oss << "\n  All configuration parameters are valid.";
        } else {
            oss << "\n  Warnings:";
            for (const auto& warning : warnings) {
                oss << "\n    - " << warning;
            }
        }
        return oss.str();
    }
    
private:
    std::vector<ConfigValidationConstraint> constraints;
    
    void initializeSchema() {
        // Configuration constraints for Phase 6 integration
        
        // Basic simulation parameters
        constraints.emplace_back(
            "simulation_timestep",
            [](const ConfigValue& v) { return std::holds_alternative<double>(v) && std::get<double>(v) > 0.0 && std::get<double>(v) <= 1.0; },
            "simulation_timestep must be between 0.0 and 1.0 seconds",
            true, "0.001"
        );
        
        constraints.emplace_back(
            "neuron_count",
            [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 10 && std::get<int64_t>(v) <= 100000; },
            "neuron_count must be between 10 and 100000",
            true, "1000"
        );
        
        constraints.emplace_back(
            "region_count",
            [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v) && std::get<int64_t>(v) >= 1 && std::get<int64_t>(v) <= 1000; },
            "region_count must be between 1 and 1000",
            true, "1"
        );
        
        constraints.emplace_back(
            "connection_probability",
            [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
            "connection_probability must be between 0.0 and 1.0",
            true, "0.1"
        );
        
        // Plasticity parameters
        constraints.emplace_back(
            "stdp_ltp_weight",
            [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
            "stdp_ltp_weight must be between 0.0 and 1.0",
            true, "0.01"
        );
        
        constraints.emplace_back(
            "stdp_ltd_weight",
            [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
            "stdp_ltd_weight must be between 0.0 and 1.0",
            true, "0.012"
        );
        
        constraints.emplace_back(
            "stdp_tau",
            [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) > 0.0f && std::get<float>(v) <= 1000.0f; },
            "stdp_tau must be between 0.0 and 1000.0",
            true, "20.0"
        );
        
        // Structural plasticity
        constraints.emplace_back(
            "synaptogenesis_rate",
            [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
            "synaptogenesis_rate must be between 0.0 and 1.0",
            true, "0.0001"
        );
        
        constraints.emplace_back(
            "pruning_rate",
            [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
            "pruning_rate must be between 0.0 and 1.0",
            true, "0.00001"
        );
        
        // Neuromodulation
        constraints.emplace_back(
            "dopamine_baseline",
            [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 10.0f; },
            "dopamine_baseline must be between 0.0 and 10.0",
            true, "0.1"
        );
        
        constraints.emplace_back(
            "curiosity_threshold",
            [](const ConfigValue& v) { return std::holds_alternative<float>(v) && std::get<float>(v) >= 0.0f && std::get<float>(v) <= 1.0f; },
            "curiosity_threshold must be between 0.0 and 1.0",
            true, "0.5"
        );
        
        // Development
        constraints.emplace_back(
            "replay_interval",
            [](const ConfigValue& v) { return std::holds_alternative<size_t>(v) && std::get<size_t>(v) >= 1 && std::get<size_t>(v) <= 10000; },
            "replay_interval must be between 1 and 10000",
            true, "100"
        );
        
        constraints.emplace_back(
            "consolidation_interval",
            [](const ConfigValue& v) { return std::holds_alternative<size_t>(v) && std::get<size_t>(v) >= 1 && std::get<size_t>(v) <= 100000; },
            "consolidation_interval must be between 1 and 100000",
            true, "1000"
        );
        
        // Boolean flags
        constraints.emplace_back(
            "enable_checkpointing",
            [](const ConfigValue& v) { return std::holds_alternative<bool>(v); },
            "enable_checkpointing must be a boolean",
            true, "true"
        );
        
        constraints.emplace_back(
            "enable_replay",
            [](const ConfigValue& v) { return std::holds_alternative<bool>(v); },
            "enable_replay must be a boolean",
            true, "true"
        );
        
        constraints.emplace_back(
            "enable_development",
            [](const ConfigValue& v) { return std::holds_alternative<bool>(v); },
            "enable_development must be a boolean",
            true, "true"
        );
        
        constraints.emplace_back(
            "enable_curiosity",
            [](const ConfigValue& v) { return std::holds_alternative<bool>(v); },
            "enable_curiosity must be a boolean",
            true, "true"
        );
        
        constraints.emplace_back(
            "enable_reward_modulation",
            [](const ConfigValue& v) { return std::holds_alternative<bool>(v); },
            "enable_reward_modulation must be a boolean",
            true, "true"
        );
        
        constraints.emplace_back(
            "enable_structural_plasticity",
            [](const ConfigValue& v) { return std::holds_alternative<bool>(v); },
            "enable_structural_plasticity must be a boolean",
            true, "true"
        );
    }
    
    void setDefaultValue(Config& config, const ConfigValidationConstraint& constraint) {
        // Convert default value string to appropriate type
        if (constraint.key == "simulation_timestep") {
            config.set(constraint.key, std::stod(constraint.defaultValue), ConfigSource::Default);
        } else if (constraint.key == "neuron_count" || constraint.key == "region_count" || 
                   constraint.key == "replay_interval" || constraint.key == "consolidation_interval") {
            config.set(constraint.key, std::stoll(constraint.defaultValue), ConfigSource::Default);
        } else if (constraint.key == "connection_probability" || constraint.key == "stdp_ltp_weight" ||
                   constraint.key == "stdp_ltd_weight" || constraint.key == "stdp_tau" ||
                   constraint.key == "dopamine_baseline" || constraint.key == "curiosity_threshold" ||
                   constraint.key == "synaptogenesis_rate" || constraint.key == "pruning_rate") {
            config.set(constraint.key, std::stof(constraint.defaultValue), ConfigSource::Default);
        } else if (constraint.key == "enable_checkpointing" || constraint.key == "enable_replay" ||
                   constraint.key == "enable_development" || constraint.key == "enable_curiosity" ||
                   constraint.key == "enable_reward_modulation" || constraint.key == "enable_structural_plasticity") {
            bool defaultBool = constraint.defaultValue == "true";
            config.set(constraint.key, defaultBool, ConfigSource::Default);
        }
    }
    
    static std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
        std::ostringstream oss;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) oss << delimiter;
            oss << vec[i];
        }
        return oss.str();
    }
};

// Global schema instance
extern Phase6ConfigSchema phase6Schema;

// Configuration manager with validation
class ValidatedConfig {
public:
    ValidatedConfig() : config(std::make_shared<Config>()) {}
    
    // Load configuration with validation
    bool load(const std::string& filepath, bool validate = true) {
        if (!config->loadFromFile(filepath)) {
            return false;
        }
        
        if (validate) {
            try {
                phase6Schema.applyDefaults(*config);
                phase6Schema.validate(*config);
            } catch (const ConfigValidationError& e) {
                // Log validation error
                std::cerr << "Configuration validation error: " << e.what() << std::endl;
                return false;
            }
        }
        
        return true;
    }
    
    // Get the underlying config
    std::shared_ptr<Config> getConfig() { return config; }
    
    // Get validation report
    std::string getValidationReport() const {
        return phase6Schema.getValidationReport(*config);
    }
    
    // Apply configuration to brain
    void applyToBrain(std::shared_ptr<Brain> brain) const {
        // Apply all configuration values to brain
        for (const auto& key : config->getKeys()) {
            if (key == "simulation_timestep") {
                double timestep = config->getOr<double>("simulation_timestep", 0.001);
                // Brain would need this interface
            } else if (key == "neuron_count") {
                int64_t neuronCount = config->getOr<int64_t>("neuron_count", 1000);
                // Brain would need this interface
            } else if (key == "region_count") {
                int64_t regionCount = config->getOr<int64_t>("region_count", 1);
                // Brain would need this interface
            } else if (key == "connection_probability") {
                float connProb = config->getOr<float>("connection_probability", 0.15f);
                // Brain would need this interface
            }
            // Add more parameter mappings as needed
        }
    }
    
private:
    std::shared_ptr<Config> config;
};

} // namespace nlm

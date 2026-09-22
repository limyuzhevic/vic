#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <variant>
#include <vector>
#include <stdexcept>
#include "../Config.hpp"

namespace nlm {

// Configuration parameter definition with validation
struct ConfigParameter {
    std::string key;
    ConfigValue defaultValue;
    std::string description;
    bool required = false;
    std::function<bool(const ConfigValue&)> validator;
    std::vector<std::string> allowedValues;  // For string parameters
    std::pair<double, double> numericRange;  // For numeric parameters
    
    ConfigParameter() = default;
    
    ConfigParameter(const std::string& k, const ConfigValue& def, const std::string& desc, bool req = false)
        : key(k), defaultValue(def), description(desc), required(req) {}
    
    bool isValid(const ConfigValue& value) const {
        // Check type matches
        if (!typeMatches(value)) {
            return false;
        }
        
        // Check allowed values for strings
        if (!allowedValues.empty() && std::holds_alternative<std::string>(value)) {
            const auto& strValue = std::get<std::string>(value);
            if (std::find(allowedValues.begin(), allowedValues.end(), strValue) == allowedValues.end()) {
                return false;
            }
        }
        
        // Check numeric range
        if (numericRange.first != numericRange.second) {
            if (std::holds_alternative<int>(value)) {
                int intVal = std::get<int>(value);
                if (intVal < numericRange.first || intVal > numericRange.second) {
                    return false;
                }
            } else if (std::holds_alternative<double>(value)) {
                double doubleVal = std::get<double>(value);
                if (doubleVal < numericRange.first || doubleVal > numericRange.second) {
                    return false;
                }
            }
        }
        
        // Run custom validator if provided
        if (validator && !validator(value)) {
            return false;
        }
        
        return true;
    }
    
    bool typeMatches(const ConfigValue& value) const {
        if (defaultValue.index() != value.index()) {
            return false;
        }
        
        // Check specific type matches for variant types
        if (std::holds_alternative<int>(defaultValue) && std::holds_alternative<int>(value)) {
            return true;
        } else if (std::holds_alternative<int64_t>(defaultValue) && std::holds_alternative<int64_t>(value)) {
            return true;
        } else if (std::holds_alternative<double>(defaultValue) && std::holds_alternative<double>(value)) {
            return true;
        } else if (std::holds_alternative<bool>(defaultValue) && std::holds_alternative<bool>(value)) {
            return true;
        } else if (std::holds_alternative<std::string>(defaultValue) && std::holds_alternative<std::string>(value)) {
            return true;
        } else if (std::holds_alternative<std::vector<int>>(defaultValue) && std::holds_alternative<std::vector<int>>(value)) {
            return true;
        } else if (std::holds_alternative<std::vector<double>>(defaultValue) && std::holds_alternative<std::vector<double>>(value)) {
            return true;
        } else if (std::holds_alternative<std::vector<std::string>>(defaultValue) && std::holds_alternative<std::vector<std::string>>(value)) {
            return true;
        }
        
        return false;
    }
};

// Configuration schema for validation
class ConfigSchema {
public:
    void addParameter(const ConfigParameter& param) {
        parameters_[param.key] = param;
    }
    
    bool isValid(const Config& config) const {
        bool valid = true;
        
        // Check all required parameters are present
        for (const auto& [key, param] : parameters_) {
            if (param.required && !config.has(key)) {
                // Missing required parameter
                valid = false;
            }
        }
        
        // Validate all provided parameters
        for (const auto& [key, param] : parameters_) {
            if (config.has(key)) {
                auto value = config.get<ConfigValue>(key);
                if (value && !param.isValid(*value)) {
                    valid = false;
                }
            }
        }
        
        return valid;
    }
    
    std::vector<std::string> getMissingRequiredParameters(const Config& config) const {
        std::vector<std::string> missing;
        
        for (const auto& [key, param] : parameters_) {
            if (param.required && !config.has(key)) {
                missing.push_back(key);
            }
        }
        
        return missing;
    }
    
    std::string getValidationError(const Config& config) const {
        std::string errors;
        
        // Check required parameters
        auto missing = getMissingRequiredParameters(config);
        if (!missing.empty()) {
            errors += "Missing required parameters: ";
            for (const auto& key : missing) {
                errors += key + " ";
            }
        }
        
        // Check invalid parameter types/values
        for (const auto& [key, param] : parameters_) {
            if (config.has(key)) {
                auto value = config.get<ConfigValue>(key);
                if (value && !param.isValid(*value)) {
                    if (!errors.empty()) errors += "\n";
                    errors += "Invalid value for parameter '" + key + "': ";
                    
                    if (!param.description.empty()) {
                        errors += param.description;
                    }
                }
            }
        }
        
        return errors;
    }
    
    const ConfigParameter* getParameter(const std::string& key) const {
        auto it = parameters_.find(key);
        if (it != parameters_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    std::vector<std::string> getAllParameterKeys() const {
        std::vector<std::string> keys;
        keys.reserve(parameters_.size());
        for (const auto& pair : parameters_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::string getParameterDescription(const std::string& key) const {
        const auto* param = getParameter(key);
        if (param) {
            return param->description;
        }
        return "";
    }
    
    std::string summary() const {
        std::string summary;
        summary += "Configuration Schema (" + std::to_string(parameters_.size()) + " parameters):\n";
        
        for (const auto& [key, param] : parameters_) {
            summary += "  " + key + " (" + (param.required ? "required" : "optional") + ")";
            if (!param.description.empty()) {
                summary += ": " + param.description;
            }
            summary += "\n";
        }
        
        return summary;
    }
    
private:
    std::unordered_map<std::string, ConfigParameter> parameters_;
};

// Configuration manager with schema validation
class ConfigManager {
public:
    static ConfigManager& getInstance() {
        static ConfigManager instance;
        return instance;
    }
    
    void registerSchema(const std::string& name, const ConfigSchema& schema) {
        schemas_[name] = schema;
    }
    
    bool validateConfig(const std::string& schemaName, const Config& config) const {
        auto it = schemas_.find(schemaName);
        if (it == schemas_.end()) {
            return false;
        }
        return it->second.isValid(config);
    }
    
    std::string getValidationError(const std::string& schemaName, const Config& config) const {
        auto it = schemas_.find(schemaName);
        if (it == schemas_.end()) {
            return "Schema not found: " + schemaName;
        }
        return it->second.getValidationError(config);
    }
    
    const ConfigSchema* getSchema(const std::string& schemaName) const {
        auto it = schemas_.find(schemaName);
        if (it != schemas_.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    std::vector<std::string> getSchemaNames() const {
        std::vector<std::string> names;
        names.reserve(schemas_.size());
        for (const auto& pair : schemas_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::string getSchemaSummary(const std::string& schemaName) const {
        const auto* schema = getSchema(schemaName);
        if (schema) {
            return schema->summary();
        }
        return "Schema not found: " + schemaName;
    }
    
private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    std::unordered_map<std::string, ConfigSchema> schemas_;
};

// Predefined configuration schemas for NLM
namespace ConfigSchemas {
    
    // Core neural network configuration
    inline ConfigSchema createCoreSchema() {
        ConfigSchema schema;
        
        // Neuron configuration
        schema.addParameter(ConfigParameter(
            "neuron_count", ConfigValue(static_cast<int64_t>(1000)),
            "Total number of neurons in the network",
            true
        ));
        
        schema.addParameter(ConfigParameter(
            "region_count", ConfigValue(static_cast<int64_t>(1)),
            "Number of brain regions",
            true
        ));
        
        schema.addParameter(ConfigParameter(
            "connection_probability", ConfigValue(0.1f),
            "Probability of synaptic connection between neurons",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "mean_synaptic_weight", ConfigValue(0.2f),
            "Mean synaptic weight (positive for excitatory, negative for inhibitory)",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "weight_variance", ConfigValue(0.1f),
            "Variance of synaptic weight distribution",
            false
        ));
        
        // Simulation configuration
        schema.addParameter(ConfigParameter(
            "simulation_timestep", ConfigValue(0.001),
            "Simulation timestep in seconds",
            true
        ));
        
        schema.addParameter(ConfigParameter(
            "max_sim_steps", ConfigValue(static_cast<int64_t>(100000)),
            "Maximum number of simulation steps before stopping",
            false
        ));
        
        // Plasticity configuration
        schema.addParameter(ConfigParameter(
            "stdp_ltp_weight", ConfigValue(0.01f),
            "STDP learning rate for long-term potentiation",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "stdp_ltd_weight", ConfigValue(0.012f),
            "STDP learning rate for long-term depression",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "stdp_time_constant", ConfigValue(20.0f),
            "STDP time constant in milliseconds",
            false
        ));
        
        // Development configuration
        schema.addParameter(ConfigParameter(
            "synaptogenesis_rate", ConfigValue(0.0001f),
            "Rate of new synapse formation during development",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "pruning_rate", ConfigValue(0.00001f),
            "Rate of synapse pruning during development",
            false
        ));
        
        // Random seed for reproducibility
        schema.addParameter(ConfigParameter(
            "random_seed", ConfigValue(static_cast<uint64_t>(42)),
            "Random seed for reproducible simulations",
            false
        ));
        
        // Memory configuration
        schema.addParameter(ConfigParameter(
            "working_memory_capacity", ConfigValue(static_cast<size_t>(100)),
            "Capacity of working memory in number of neurons",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "max_episodes", ConfigValue(static_cast<size_t>(1000)),
            "Maximum number of episodic memory entries",
            false
        ));
        
        return schema;
    }
    
    // Full Phase 6 configuration (integration-focused)
    inline ConfigSchema createPhase6Schema() {
        ConfigSchema schema = createCoreSchema();
        
        // Add Phase 6 specific configurations
        schema.addParameter(ConfigParameter(
            "replay_interval", ConfigValue(static_cast<size_t>(100)),
            "Number of simulation steps between memory replays",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "consolidation_interval", ConfigValue(static_cast<size_t>(1000)),
            "Number of simulation steps between memory consolidations",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "checkpoint_interval", ConfigValue(static_cast<size_t>(1000)),
            "Number of simulation steps between checkpoints",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "enable_neuromodulation", ConfigValue(true),
            "Whether to enable neuromodulation systems",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "enable_prediction", ConfigValue(true),
            "Whether to enable predictive processing",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "enable_cognition", ConfigValue(true),
            "Whether to enable cognitive processes",
            false
        ));
        
        schema.addParameter(ConfigParameter(
            "enable_development", ConfigValue(true),
            "Whether to enable developmental processes",
            false
        ));
        
        return schema;
    }
    
    // Lightweight configuration for performance testing
    inline ConfigSchema createPerformanceSchema() {
        ConfigSchema schema = createCoreSchema();
        
        // Reduced configuration for faster testing
        schema.getParameter("neuron_count")->defaultValue = ConfigValue(static_cast<int64_t>(100));
        schema.getParameter("region_count")->defaultValue = ConfigValue(static_cast<int64_t>(1));
        schema.getParameter("connection_probability")->defaultValue = ConfigValue(0.05f);
        schema.getParameter("max_sim_steps")->defaultValue = ConfigValue(static_cast<int64_t>(1000));
        schema.getParameter("synaptogenesis_rate")->defaultValue = ConfigValue(0.00001f);
        schema.getParameter("pruning_rate")->defaultValue = ConfigValue(0.000001f);
        
        return schema;
    }
}

} // namespace nlm

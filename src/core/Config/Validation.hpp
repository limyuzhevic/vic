// NLM Configuration Schema and Validation System
// Comprehensive configuration validation with schema support

#pragma once

#include <string>
#include <vector>
#include <map>
#include <any>
#include <functional>
#include <stdexcept>
#include <variant>
#include <optional>

namespace nlm {

// Configuration value types for schema validation
enum class ConfigValueType {
    Integer,
    Float,
    String,
    Boolean,
    StringList,
    FloatList,
    IntList,
    Any
};

// Schema validation rules
struct ConfigValidationRule {
    std::string key;
    ConfigValueType type;
    bool required = false;
    std::any defaultValue;
    std::vector<std::any> allowedValues;
    std::function<bool(const std::any&)> validator;
    std::string description;
    
    ConfigValidationRule(const std::string& k, ConfigValueType t, bool req = false, const std::string& desc = "")
        : key(k), type(t), required(req), description(desc) {}
    
    bool validate(const std::any& value) const {
        if (!validator) return true; // No custom validator, type check only
        return validator(value);
    }
};

// Configuration schema for validation
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add validation rule
    void addRule(const ConfigValidationRule& rule) {
        rules_[rule.key] = rule;
    }
    
    // Add integer rule
    void addIntRule(const std::string& key, bool required = false, 
                   int64_t min = std::numeric_limits<int64_t>::min(),
                   int64_t max = std::numeric_limits<int64_t>::max(),
                   const std::string& description = "") {
        rules_[key] = ConfigValidationRule(
            key, ConfigValueType::Integer, required, description,
            [min, max](const std::any& val) {
                try {
                    int64_t value = std::any_cast<int64_t>(val);
                    return value >= min && value <= max;
                } catch (...) {
                    return false;
                }
            });
    }
    
    // Add float rule
    void addFloatRule(const std::string& key, bool required = false,
                     float min = std::numeric_limits<float>::min(),
                     float max = std::numeric_limits<float>::max(),
                     const std::string& description = "") {
        rules_[key] = ConfigValidationRule(
            key, ConfigValueType::Float, required, description,
            [min, max](const std::any& val) {
                try {
                    float value = std::any_cast<float>(val);
                    return value >= min && value <= max;
                } catch (...) {
                    return false;
                }
            });
    }
    
    // Add string rule
    void addStringRule(const std::string& key, bool required = false,
                      size_t minLength = 0, size_t maxLength = std::numeric_limits<size_t>::max(),
                      const std::string& description = "") {
        rules_[key] = ConfigValidationRule(
            key, ConfigValueType::String, required, description,
            [minLength, maxLength](const std::any& val) {
                try {
                    std::string value = std::any_cast<std::string>(val);
                    return value.length() >= minLength && value.length() <= maxLength;
                } catch (...) {
                    return false;
                }
            });
    }
    
    // Add boolean rule
    void addBoolRule(const std::string& key, bool required = false,
                    const std::string& description = "") {
        rules_[key] = ConfigValidationRule(
            key, ConfigValueType::Boolean, required, description,
            [](const std::any& val) {
                return val.type() == typeid(bool);
            });
    }
    
    // Add enum-like validation with allowed values
    template<typename T>
    void addEnumRule(const std::string& key, const std::vector<T>& allowedValues,
                    bool required = false, const std::string& description = "") {
        rules_[key] = ConfigValidationRule(
            key, ConfigValueType::Any, required, description,
            [allowedValues](const std::any& val) {
                return std::find(allowedValues.begin(), allowedValues.end(), std::any_cast<T>(val)) != allowedValues.end();
            });
    }
    
    // Get validation rule for key
    std::optional<ConfigValidationRule> getRule(const std::string& key) const {
        auto it = rules_.find(key);
        if (it != rules_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
    
    // Check if schema is complete (all required rules present)
    bool isComplete(const std::map<std::string, std::any>& values) const {
        for (const auto& rule : rules_) {
            if (rule.second.required) {
                if (values.find(rule.first) === values.end()) {
                    return false;
                }
            }
        }
        return true;
    }
    
    // Validate all values against schema
    bool validate(const std::map<std::string, std::any>& values) const {
        for (const auto& value : values) {
            auto ruleIt = rules_.find(value.first);
            if (ruleIt === rules_.end()) {
                // Unknown key, but not necessarily an error unless we want strict validation
                continue;
            }
            
            const auto& rule = ruleIt->second;
            if (!rule.validate(value.second)) {
                return false;
            }
        }
        return true;
    }
    
    // Get all required keys
    std::vector<std::string> getRequiredKeys() const {
        std::vector<std::string> requiredKeys;
        for (const auto& rule : rules_) {
            if (rule.second.required) {
                requiredKeys.push_back(rule.first);
            }
        }
        return requiredKeys;
    }
    
    // Get description for key
    std::string getDescription(const std::string& key) const {
        auto it = rules_.find(key);
        if (it !== rules_.end()) {
            return it->second.description;
        }
        return "";
    }
    
private:
    std::map<std::string, ConfigValidationRule> rules_;
};

// Default configuration schemas for NLM
class DefaultSchemas {
public:
    static ConfigSchema getPhase2Schema() {
        ConfigSchema schema;
        
        // Core simulation parameters
        schema.addIntRule("random_seed", true, "0", "2147483647", "Random seed for reproducible experiments");
        schema.addFloatRule("simulation_timestep", true, "0.0", "1.0", "Simulation timestep in seconds");
        schema.addIntRule("neuron_count", true, "1", "100000", "Number of neurons in the brain");
        schema.addIntRule("region_count", true, "1", "100", "Number of brain regions");
        schema.addFloatRule("connection_probability", true, "0.0", "1.0", 
                          "Connection probability for random connectivity");
        
        // Plasticity settings
        schema.addFloatRule("plasticity_learning_rate", true, "0.0", "1.0", "Plasticity learning rate");
        schema.addFloatRule("stdp_ltp_weight", true, "0.0", "1.0", "STDP LTP weight");
        schema.addFloatRule("stdp_ltd_weight", true, "0.0", "1.0", "STDP LTD weight");
        schema.addFloatRule("stdp_time_constant", true, "0.0", "100.0", "STDP time constant");
        
        // Development settings
        schema.addFloatRule("development_synaptogenesis_rate", true, "0.0", "0.01", "Development synaptogenesis rate");
        schema.addFloatRule("development_pruning_rate", true, "0.0", "0.01", "Development pruning rate");
        
        // Neuromodulation settings
        schema.addFloatRule("dopamine_baseline", true, "0.0", "1.0", "Dopamine baseline level");
        schema.addFloatRule("reward_discount_factor", true, "0.0", "1.0", "Reward discount factor");
        
        // Environment settings
        schema.addStringRule("environment_name", true, "0", "100", "Environment name");
        schema.addIntRule("environment_width", true, "1", "1000", "Environment width");
        schema.addIntRule("environment_height", true, "1", "1000", "Environment height");
        
        // Logging settings
        schema.addStringRule("log_level", true, "", "", "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)");
        schema.addBoolRule("log_to_file", false, "Log to file instead of console");
        schema.addStringRule("log_filename", false, "", "", "Log filename");
        
        // Simulation settings
        schema.addIntRule("max_simulation_steps", true, "1", "1000000", "Maximum simulation steps");
        schema.addFloatRule("simulation_time_limit", true, "0.0", "10000.0", "Simulation time limit");
        
        // Visualization settings
        schema.addBoolRule("visualization_enabled", false, "Enable visualization");
        schema.addFloatRule("visualization_update_rate", false, "0.0", "60.0", "Visualization update rate (Hz)");
        
        return schema;
    }
    
    static ConfigSchema getAgentBrainSchema() {
        ConfigSchema schema;
        
        // Agent brain specific parameters
        schema.addBoolRule("enable_reward_modulation", false, "Enable reward modulation");
        schema.addBoolRule("enable_structural_plasticity", false, "Enable structural plasticity");
        schema.addBoolRule("enable_development", false, "Enable agent development");
        schema.addBoolRule("enable_curiosity", false, "Enable curiosity-driven exploration");
        schema.addFloatRule("curiosity_threshold", false, "0.0", "1.0", "Curiosity threshold");
        schema.addIntRule("max_planning_depth", false, "0", "20", "Maximum planning depth");
        
        return schema;
    }
    
    static ConfigSchema getPythonBindingsSchema() {
        ConfigSchema schema;
        
        // Python bindings specific parameters
        schema.addStringRule("python_module_name", false, "", "", "Python module name");
        schema.addBoolRule("export_all_classes", false, "Export all available classes");
        schema.addStringRule("additional_bindings", false, "", "", "Additional bindings to export");
        
        return schema;
    }
};

// Configuration validation exception
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "", const std::string& schema = "")
        : std::runtime_error(message), key_(key), schema_(schema) {}
    
    const std::string& getKey() const { return key_; }
    const std::string& getSchema() const { return schema_; }
    
private:
    std::string key_;
    std::string schema_;
};

} // namespace nlm
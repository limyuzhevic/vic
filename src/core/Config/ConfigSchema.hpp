// NLM Configuration Schema Support
// Configuration validation and schema definitions

#pragma once

#include "Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <functional>
#include <sstream>

namespace nlm {

// Configuration schema field definition
struct ConfigSchemaField {
    std::string name;
    ConfigValue defaultValue;
    std::string type; // "int", "int64_t", "double", "bool", "string", "vector<int>", etc.
    std::string description;
    bool required;
    
    // Validation constraints
    double minValue;  // For numeric types
    double maxValue;
    std::set<std::string> allowedValues; // For string types
    size_t minSize;    // For vector types
    size_t maxSize;
    
    ConfigSchemaField() : required(false), minValue(0.0), maxValue(0.0), 
                         minSize(0), maxSize(0) {}
};

// Configuration validation result
struct ConfigValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration schema
class ConfigSchema {
private:
    std::map<std::string, ConfigSchemaField> fields_;
    std::string schemaName_;
    std::string version_;
    
public:
    ConfigSchema(const std::string& name = "NLM Configuration", 
                 const std::string& version = "1.0.0") 
        : schemaName_(name), version_(version) {}
    
    // Add a schema field
    void addField(const ConfigSchemaField& field) {
        fields_[field.name] = field;
    }
    
    // Add field with parameters
    void addField(const std::string& name, const ConfigValue& defaultValue,
                  const std::string& type, const std::string& description,
                  bool required = false, double minVal = 0.0, double maxVal = 0.0,
                  const std::set<std::string>& allowedValues = {},
                  size_t minSize = 0, size_t maxSize = 0) {
        ConfigSchemaField field;
        field.name = name;
        field.defaultValue = defaultValue;
        field.type = type;
        field.description = description;
        field.required = required;
        field.minValue = minVal;
        field.maxValue = maxVal;
        field.allowedValues = allowedValues;
        field.minSize = minSize;
        field.maxSize = maxSize;
        
        fields_[name] = field;
    }
    
    // Validate configuration against schema
    ConfigValidationResult validate(const Config& config) const {
        ConfigValidationResult result;
        
        // Check required fields
        for (const auto& pair : fields_) {
            const auto& field = pair.second;
            if (field.required && !config.has(field.name)) {
                result.addError("Missing required field: " + field.name);
            }
        }
        
        // Validate field values
        for (const auto& pair : fields_) {
            const auto& field = pair.second;
            if (!config.has(field.name)) continue;
            
            auto value = config.get(field.name);
            if (!value) continue;
            
            validateFieldValue(field, *value, result);
        }
        
        return result;
    }
    
    // Validate a single field value
    void validateFieldValue(const ConfigSchemaField& field, const ConfigValue& value,
                           ConfigValidationResult& result) const {
        // Check type
        if (!valueMatchesType(value, field.type)) {
            result.addError("Field '" + field.name + "' has wrong type. Expected: " + field.type);
            return;
        }
        
        // Type-specific validation
        if (field.type == "int" || field.type == "int64_t" || field.type == "double") {
            double numVal = getNumber(value);
            if (numVal < field.minValue || numVal > field.maxValue) {
                result.addError("Field '" + field.name + "' value " + std::to_string(numVal) + 
                              " is outside valid range [" + std::to_string(field.minValue) +
                              ", " + std::to_string(field.maxValue) + "]");
            }
        } else if (field.type == "string") {
            std::string strVal = getString(value);
            if (!field.allowedValues.empty() && field.allowedValues.find(strVal) == field.allowedValues.end()) {
                result.addError("Field '" + field.name + "' value '" + strVal + 
                              " is not in allowed values: " + vectorToString(field.allowedValues));
            }
            if (strVal.size() < field.minSize || strVal.size() > field.maxSize) {
                result.addError("Field '" + field.name + "' string length " + std::to_string(strVal.size()) +
                              " is outside valid range [" + std::to_string(field.minSize) +
                              ", " + std::to_string(field.maxSize) + "]");
            }
        } else if (field.type.find("vector<") == 0) {
            auto vecVal = getVector(value);
            if (vecVal.size() < field.minSize || vecVal.size() > field.maxSize) {
                result.addError("Field '" + field.name + "' vector size " + std::to_string(vecVal.size()) +
                              " is outside valid range [" + std::to_string(field.minSize) +
                              ", " + std::to_string(field.maxSize) + "]");
            }
        }
    }
    
    // Check if value matches expected type
    bool valueMatchesType(const ConfigValue& value, const std::string& type) const {
        if (type == "int") return std::holds_alternative<int>(value);
        if (type == "int64_t") return std::holds_alternative<int64_t>(value);
        if (type == "double") return std::holds_alternative<double>(value);
        if (type == "bool") return std::holds_alternative<bool>(value);
        if (type == "string") return std::holds_alternative<std::string>(value);
        if (type == "vector<int>") return std::holds_alternative<std::vector<int>>(value);
        if (type == "vector<double>") return std::holds_alternative<std::vector<double>>(value);
        if (type == "vector<string>") return std::holds_alternative<std::vector<std::string>>(value);
        return false;
    }
    
    // Extract numeric value from ConfigValue
    double getNumber(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return static_cast<double>(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return static_cast<double>(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::get<double>(value);
        return 0.0;
    }
    
    // Extract string value from ConfigValue
    std::string getString(const ConfigValue& value) const {
        if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
        return std::to_string(getNumber(value));
    }
    
    // Extract vector value from ConfigValue
    std::vector<ConfigValue> getVector(const ConfigValue& value) const {
        if (std::holds_alternative<std::vector<int>>(value)) {
            std::vector<ConfigValue> result;
            for (int v : std::get<std::vector<int>>(value)) {
                result.push_back(v);
            }
            return result;
        }
        if (std::holds_alternative<std::vector<double>>(value)) {
            std::vector<ConfigValue> result;
            for (double v : std::get<std::vector<double>>(value)) {
                result.push_back(v);
            }
            return result;
        }
        if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::vector<ConfigValue> result;
            for (const std::string& v : std::get<std::vector<std::string>>(value)) {
                result.push_back(v);
            }
            return result;
        }
        return {};
    }
    
    // Get field description
    std::string getFieldDescription(const std::string& name) const {
        auto it = fields_.find(name);
        if (it != fields_.end()) {
            return it->second.description;
        }
        return "";
    }
    
    // Get default value for field
    ConfigValue getDefaultValue(const std::string& name) const {
        auto it = fields_.find(name);
        if (it != fields_.end()) {
            return it->second.defaultValue;
        }
        throw std::runtime_error("Unknown field in schema: " + name);
    }
    
    // Get all field names
    std::vector<std::string> getFieldNames() const {
        std::vector<std::string> names;
        names.reserve(fields_.size());
        for (const auto& pair : fields_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Generate configuration summary
    std::string generateSummary(const Config& config) const {
        std::ostringstream oss;
        oss << "Configuration Schema: " << schemaName_ << " v" << version_ << "\n";
        oss << "Fields: " << fields_.size() << "\n";
        
        for (const auto& pair : fields_) {
            const auto& field = pair.second;
            bool hasValue = config.has(field.name);
            oss << "  " << field.name << " (" << field.type << ")";
            if (hasValue) {
                oss << " = " << getConfigValueString(config.get(field.name).value_or(ConfigValue()));
            } else if (field.required) {
                oss << " = <REQUIRED>";
            } else {
                oss << " = <DEFAULT: " << getConfigValueString(field.defaultValue) << ">";
            }
            oss << " - " << field.description << "\n";
        }
        
        return oss.str();
    }
    
private:
    std::string getConfigValueString(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
        if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
        if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
        if (std::holds_alternative<std::string>(value)) return "\"" + std::get<std::string>(value) + "\"";
        if (std::holds_alternative<std::vector<int>>(value)) {
            std::string result = "[";
            for (size_t i = 0; i < std::get<std::vector<int>>(value).size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(std::get<std::vector<int>>(value)[i]);
            }
            result += "]";
            return result;
        }
        if (std::holds_alternative<std::vector<double>>(value)) {
            std::string result = "[";
            for (size_t i = 0; i < std::get<std::vector<double>>(value).size(); ++i) {
                if (i > 0) result += ", ";
                result += std::to_string(std::get<std::vector<double>>(value)[i]);
            }
            result += "]";
            return result;
        }
        if (std::holds_alternative<std::vector<std::string>>(value)) {
            std::string result = "[";
            for (size_t i = 0; i < std::get<std::vector<std::string>>(value).size(); ++i) {
                if (i > 0) result += ", ";
                result += "\"" + std::get<std::vector<std::string>>(value)[i] + "\"";
            }
            result += "]";
            return result;
        }
        return "<unknown>";
    }
    
    std::string vectorToString(const std::set<std::string>& values) const {
        std::string result;
        bool first = true;
        for (const auto& v : values) {
            if (!first) result += ", ";
            result += "\"" + v + "\"";
            first = false;
        }
        return result;
    }
};

// Default NLM configuration schema
inline ConfigSchema createDefaultConfigSchema() {
    ConfigSchema schema("NLM Configuration", "2.0.0");
    
    // Core simulation parameters
    schema.addField("random_seed", 42, "int64_t", 
                   "Random seed for reproducible simulations", true, -1, 2147483647);
    
    schema.addField("simulation_timestep", 0.001, "double",
                   "Simulation timestep in seconds", true, 1e-6, 0.1);
    
    schema.addField("neuron_count", 500, "int64_t",
                   "Number of neurons in the network", true, 1, 100000);
    
    schema.addField("region_count", 1, "int64_t",
                   "Number of neural regions", true, 1, 1000);
    
    schema.addField("connection_probability", 0.15, "double",
                   "Probability of forming connections between neurons", true, 0.0, 1.0);
    
    // STDP parameters
    schema.addField("stdp_ltp_weight", 0.02, "double",
                   "STDP long-term potentiation weight", true, 0.0, 1.0);
    
    schema.addField("stdp_ltd_weight", 0.015, "double",
                   "STDP long-term depression weight", true, 0.0, 1.0);
    
    schema.addField("stdp_tau", 20.0, "double",
                   "STDP time constant for synaptic changes", true, 1.0, 1000.0);
    
    // Structural plasticity
    schema.addField("synaptogenesis_rate", 0.0001, "double",
                   "Rate of new synapse formation", true, 0.0, 0.01);
    
    schema.addField("pruning_rate", 0.00001, "double",
                   "Rate of synapse elimination", true, 0.0, 0.001);
    
    // Memory systems
    schema.addField("working_memory_capacity", 1000, "int64_t",
                   "Maximum number of items in working memory", false, 0, 10000);
    
    schema.addField("episodic_memory_capacity", 5000, "int64_t",
                   "Maximum number of episodes in episodic memory", false, 0, 100000);
    
    // Development
    schema.addField("development_enabled", true, "bool",
                   "Enable developmental stages and plasticity changes", false);
    
    schema.addField("critical_period_duration", 100.0, "double",
                   "Duration of critical period for learning enhancement", false, 0.0, 1000.0);
    
    // Neuromodulation
    schema.addField("reward_sensitivity", 1.0, "double",
                   "Sensitivity to reward signals", false, 0.0, 10.0);
    
    schema.addField("curiosity_sensitivity", 1.0, "double",
                   "Sensitivity to novelty and curiosity signals", false, 0.0, 10.0);
    
    // Prediction and cognition
    schema.addField("prediction_accuracy_threshold", 0.8, "double",
                   "Threshold for acceptable prediction accuracy", false, 0.0, 1.0);
    
    schema.addField("planning_confidence_threshold", 0.7, "double",
                   "Threshold for planning confidence", false, 0.0, 1.0);
    
    // Environment and behavior
    schema.addField("environment_width", 20, "int64_t",
                   "Width of the simulated environment", false, 1, 1000);
    
    schema.addField("environment_height", 20, "int64_t",
                   "Height of the simulated environment", false, 1, 1000);
    
    // Output and logging
    schema.addField("log_level", "INFO", "string",
                   "Logging level (DEBUG, INFO, WARNING, ERROR, CRITICAL)", false);
    
    schema.addField("output_results", true, "bool",
                   "Whether to save simulation results to file", false);
    
    schema.addField("results_file_path", "results.csv", "string",
                   "Path to results output file", false);
    
    return schema;
}

// Global default schema instance
inline ConfigSchema& getDefaultConfigSchema() {
    static ConfigSchema schema = createDefaultConfigSchema();
    return schema;
}

} // namespace nlm
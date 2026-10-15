// Configuration Validator Implementation

#include "Config.hpp"
#include <sstream>
#include <algorithm>

namespace nlm {

// Basic configuration validator with common validation rules
class BasicConfigValidator : public ConfigValidator {
public:
    BasicConfigValidator() {
        // Initialize default validation rules
        initializeDefaultRules();
    }
    
    bool validate(const std::string& key, const ConfigValue& value, ConfigValidationError& error) override {
        error = ConfigValidationError::None;
        
        // Check if key exists in schema
        auto schemaIt = std::find_if(schema.begin(), schema.end(),
            [&key](const ConfigSchemaEntry& e) { return e.key == key; });
        
        if (schemaIt == schema.end()) {
            // Key not in schema - may be allowed depending on configuration
            return true; // Allow non-schema keys
        }
        
        const ConfigSchemaEntry& schema = *schemaIt;
        
        // Type validation
        if (!validateType(key, value, schema.type, error)) {
            return false;
        }
        
        // Range validation for numeric types
        if (!validateRange(key, value, schema, error)) {
            return false;
        }
        
        // Custom validation rules
        if (!validateCustomRules(key, value, schema, error)) {
            return false;
        }
        
        return true;
    }
    
    std::string getErrorMessage(ConfigValidationError error) const override {
        switch (error) {
            case ConfigValidationError::InvalidType:
                return "Invalid type for configuration value";
            case ConfigValidationError::OutOfRange:
                return "Value is out of valid range";
            case ConfigValidationError::MissingRequired:
                return "Required configuration key is missing";
            case ConfigValidationError::InvalidValue:
                return "Invalid configuration value";
            case ConfigValidationError::None:
                return "Validation passed";
            default:
                return "Unknown validation error";
        }
    }
    
    void setSchema(const std::vector<ConfigSchemaEntry>& schema) {
        this->schema = schema;
    }
    
    void addSchemaEntry(const ConfigSchemaEntry& entry) {
        schema.push_back(entry);
    }
    
protected:
    virtual bool validateType(const std::string& key, const ConfigValue& value,
                           const std::type_index& expectedType, ConfigValidationError& error) {
        if (expectedType == typeid(int)) {
            return std::holds_alternative<int>(value);
        }
        else if (expectedType == typeid(int64_t)) {
            return std::holds_alternative<int64_t>(value);
        }
        else if (expectedType == typeid<double>) {
            return std::holds_alternative<double>(value);
        }
        else if (expectedType == typeid<bool>) {
            return std::holds_alternative<bool>(value);
        }
        else if (expectedType == typeid(std::string)) {
            return std::holds_alternative<std::string>(value);
        }
        else if (expectedType == typeid(std::vector<int>)) {
            return std::holds_alternative<std::vector<int>>(value);
        }
        else if (expectedType == typeid(std::vector<double>)) {
            return std::holds_alternative<std::vector<double>>(value);
        }
        else if (expectedType == typeid(std::vector<std::string>)) {
            return std::holds_alternative<std::vector<std::string>>(value);
        }
        
        error = ConfigValidationError::InvalidType;
        return false;
    }
    
    virtual bool validateRange(const std::string& key, const ConfigValue& value,
                             const ConfigSchemaEntry& schema, ConfigValidationError& error) {
        // Range validation for numeric types
        if (std::holds_alternative<int>(value)) {
            int val = std::get<int>(value);
            if (schema.defaultValue.type() == typeid(int)) {
                int defaultVal = std::any_cast<int>(schema.defaultValue);
                if (val < defaultVal * 0.5 || val > defaultVal * 2.0) {
                    error = ConfigValidationError::OutOfRange;
                    return false;
                }
            }
        }
        else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (schema.defaultValue.type() == typeid(double)) {
                double defaultVal = std::any_cast<double>(schema.defaultValue);
                if (val < defaultVal * 0.5 || val > defaultVal * 2.0) {
                    error = ConfigValidationError::OutOfRange;
                    return false;
                }
            }
        }
        
        return true;
    }
    
    virtual bool validateCustomRules(const std::string& key, const ConfigValue& value,
                                   const ConfigSchemaEntry& schema, ConfigValidationError& error) {
        // Custom validation rules based on key
        if (key == "brain.neuron_count") {
            if (std::holds_alternative<int>(value)) {
                int count = std::get<int>(value);
                if (count < 10 || count > 10000) {
                    error = ConfigValidationError::InvalidValue;
                    return false;
                }
            }
        }
        else if (key == "brain.connection_probability") {
            if (std::holds_alternative<double>(value)) {
                double prob = std::get<double>(value);
                if (prob < 0.0 || prob > 1.0) {
                    error = ConfigValidationError::InvalidValue;
                    return false;
                }
            }
        }
        else if (key == "plasticity.stdp.learning_rate") {
            if (std::holds_alternative<double>(value)) {
                double rate = std::get<double>(value);
                if (rate < 0.0 || rate > 1.0) {
                    error = ConfigValidationError::InvalidValue;
                    return false;
                }
            }
        }
        
        return true;
    }
    
private:
    std::vector<ConfigSchemaEntry> schema;
    
    void initializeDefaultRules() {
        // Initialize with common NLM configuration validation rules
        schema.clear();
        
        // Add common configuration validation rules
        addSchemaEntry(ConfigSchemaEntry(
            "brain.neuron_count", std::any(1000), 
            "Total number of neurons in the brain", true
        ));
        
        addSchemaEntry(ConfigSchemaEntry(
            "brain.connection_probability", std::any(0.1),
            "Probability of connection between neurons"
        ));
        
        addSchemaEntry(ConfigSchemaEntry(
            "plasticity.stdp.learning_rate", std::any(0.01),
            "STDP learning rate"
        ));
        
        addSchemaEntry(ConfigSchemaEntry(
            "plasticity.hebbian.learning_rate", std::any(0.01),
            "Hebbian learning rate"
        ));
        
        addSchemaEntry(ConfigSchemaEntry(
            "neuromodulation.dopamine.scale", std::any(1.0),
            "Dopamine modulation scale"
        ));
    }
};

} // namespace nlm
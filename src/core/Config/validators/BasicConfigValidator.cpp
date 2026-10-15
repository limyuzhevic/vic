// Basic Configuration Validator Implementation

#include "BasicConfigValidator.hpp"
#include <sstream>
#include <algorithm>
#include <any>

namespace nlm {

BasicConfigValidator::BasicConfigValidator() {
    // Initialize default validation rules
    initializeDefaultRules();
}

bool BasicConfigValidator::validate(const std::string& key, const ConfigValue& value, ConfigValidationError& error) {
    error = ConfigValidationError::None;
    
    // Check if key exists in schema
    auto schemaIt = std::find_if(schema.begin(), schema.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    
    if (schemaIt == schema.end()) {
        // Key not in schema - may be allowed depending on configuration
        return true; // Allow non-schema keys
    }
    
    const ConfigSchemaEntry& schemaEntry = *schemaIt;
    
    // Type validation
    if (!validateType(key, value, schemaEntry.type, error)) {
        return false;
    }
    
    // Range validation for numeric types
    if (!validateRange(key, value, schemaEntry, error)) {
        return false;
    }
    
    // Custom validation rules
    if (!validateCustomRules(key, value, schemaEntry, error)) {
        return false;
    }
    
    return true;
}

std::string BasicConfigValidator::getErrorMessage(ConfigValidationError error) const {
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

void BasicConfigValidator::setSchema(const std::vector<ConfigSchemaEntry>& schemaEntries) {
    schema = schemaEntries;
}

void BasicConfigValidator::addSchemaEntry(const ConfigSchemaEntry& entry) {
    schema.push_back(entry);
}

bool BasicConfigValidator::validateType(const std::string& key, const ConfigValue& value,
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

bool BasicConfigValidator::validateRange(const std::string& key, const ConfigValue& value,
                                       const ConfigSchemaEntry& schemaEntry, ConfigValidationError& error) {
    // Range validation for numeric types
    if (std::holds_alternative<int>(value)) {
        int val = std::get<int>(value);
        if (schemaEntry.defaultValue.type() == typeid(int)) {
            int defaultVal = std::any_cast<int>(schemaEntry.defaultValue);
            if (val < defaultVal * 0.5 || val > defaultVal * 2.0) {
                error = ConfigValidationError::OutOfRange;
                return false;
            }
        }
    }
    else if (std::holds_alternative<double>(value)) {
        double val = std::get<double>(value);
        if (schemaEntry.defaultValue.type() == typeid(double)) {
            double defaultVal = std::any_cast<double>(schemaEntry.defaultValue);
            if (val < defaultVal * 0.5 || val > defaultVal * 2.0) {
                error = ConfigValidationError::OutOfRange;
                return false;
            }
        }
    }
    
    return true;
}

bool BasicConfigValidator::validateCustomRules(const std::string& key, const ConfigValue& value,
                                             const ConfigSchemaEntry& schemaEntry, ConfigValidationError& error) {
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

void BasicConfigValidator::initializeDefaultRules() {
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

} // namespace nlm
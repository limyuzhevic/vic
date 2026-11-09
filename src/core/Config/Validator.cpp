// Configuration Validator Implementation
// Validates configuration values against schema rules

#include "Validator.hpp"
#include <sstream>
#include <iomanip>

namespace nlm {

// Default validation error messages
static const std::string kMissingRequiredKeyError = "Missing required configuration key: ";
static const std::string kInvalidTypeError = "Invalid type for configuration key: ";
static const std::string kOutOfRangeError = "Value out of range for configuration key: ";
static const std::string kInvalidEnumError = "Invalid value for configuration key: ";

// ConfigValidator class implementation
ConfigValidator::ConfigValidator() {
    setupDefaultSchemas();
}

void ConfigValidator::setupDefaultSchemas() {
    phase2_schema = DefaultSchemas::getPhase2Schema();
    agent_brain_schema = DefaultSchemas::getAgentBrainSchema();
    python_bindings_schema = DefaultSchemas::getPythonBindingsSchema();
}

void ConfigValidator::validateConfig(const Config& config, ConfigType type) {
    std::map<std::string, std::any> configMap;
    for (const auto& key : config.getKeys()) {
        std::optional<std::any> value = config.getInternalValue(key);
        if (value) {
            configMap[key] = *value;
        }
    }
    
    const ConfigSchema* schema = nullptr;
    switch (type) {
        case ConfigType::Phase2:
            schema = &phase2_schema;
            break;
        case ConfigType::AgentBrain:
            schema = &agent_brain_schema;
            break;
        case ConfigType::PythonBindings:
            schema = &python_bindings_schema;
            break;
    }
    
    if (schema) {
        schema->validate(configMap);
    }
}

void ConfigValidator::validateRequiredKeys(const Config& config, ConfigType type) {
    const ConfigSchema* schema = nullptr;
    switch (type) {
        case ConfigType::Phase2:
            schema = &phase2_schema;
            break;
        case ConfigType::AgentBrain:
            schema = &agent_brain_schema;
            break;
        case ConfigType::PythonBindings:
            schema = &python_bindings_schema;
            break;
    }
    
    if (schema) {
        std::vector<std::string> requiredKeys = schema->getRequiredKeys();
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                throw ConfigValidationError("Missing required key: " + key, key, "phase2");
            }
        }
    }
}

std::string ConfigValidator::getValidationError(const Config& config, ConfigType type) {
    std::vector<std::string> errors;
    
    try {
        validateRequiredKeys(config, type);
    } catch (const ConfigValidationError& e) {
        errors.push_back(e.what());
    }
    
    try {
        validateConfig(config, type);
    } catch (const ConfigValidationError& e) {
        errors.push_back(e.what());
    }
    
    if (errors.empty()) {
        return "Validation passed";
    }
    
    std::ostringstream oss;
    oss << "Validation errors:\n";
    for (const auto& error : errors) {
        oss << "  - " << error << "\n";
    }
    return oss.str();
}

} // namespace nlm
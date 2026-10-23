// Enhanced Configuration Implementation
// Implementation of the enhanced configuration system with validation and JSON support

#include "EnhancedConfig.hpp"
#include <fstream>
#include <algorithm>
#include <sstream>

namespace nlm {

// Config implementation
Config::Config() : validator_(createDefaultSchema()) {
    NLM_LOG_INFO("Enhanced configuration system initialized");
}

Config::Config(const ConfigSchema& schema) : validator_(schema) {
    NLM_LOG_INFO("Enhanced configuration system initialized with custom schema");
}

void Config::createDefaultEntries() {
    // Create entries from default schema fields
    const auto& fieldNames = schema_.getFieldNames();
    for (const auto& fieldName : fieldNames) {
        const ConfigSchema::FieldInfo* field = schema_.getField(fieldName);
        if (field) {
            ConfigValue defaultValue;
            if (field->type == "int") {
                defaultValue = 0;
            } else if (field->type == "float") {
                defaultValue = 0.0f;
            } else if (field->type == "bool") {
                defaultValue = false;
            } else if (field->type == "string") {
                defaultValue = "";
            } else if (field->type == "int_list") {
                defaultValue = std::vector<int>();
            } else if (field->type == "float_list") {
                defaultValue = std::vector<float>();
            } else if (field->type == "string_list") {
                defaultValue = std::vector<std::string>();
            }
            
            set(fieldName, defaultValue, field->description, field->isRequired);
        }
    }
}

void Config::set(const std::string& key, const ConfigValue& value, const std::string& description, bool isRequired) {
    ConfigEntry entry;
    entry.key = key;
    entry.value = value;
    entry.description = description;
    entry.isRequired = isRequired;
    
    configData_[key] = entry;
    
    // Validate immediately
    ValidationResult validation = schema_.validate(key, value);
    entry.validationResult = validation;
    configData_[key] = entry;
    
    if (validation.hasErrors()) {
        NLM_LOG_ERROR("Invalid configuration key '" + key + "': " + validation.errors[0]);
    } else if (validation.hasWarnings()) {
        NLM_LOG_WARNING("Configuration key '" + key + "' warnings:");
        for (const auto& warning : validation.warnings) {
            NLM_LOG_WARNING("  " + warning);
        }
    }
}

std::string Config::getTypeName() const {
    return "ConfigValue";
}

} // namespace nlm

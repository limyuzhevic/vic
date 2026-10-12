#pragma once

#include "ConfigSchema.hpp"
#include "ConfigValidator.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

// Template function implementations for ConfigSchema

template<typename T>
void ConfigSchema::add(const std::string& name,
                       const std::any& defaultValue,
                       bool required,
                       const std::string& description,
                       const std::string& group,
                       int priority,
                       bool readOnly) {
    Field field;
    field.name = name;
    field.description = description;
    field.defaultValue = defaultValue;
    field.required = required;
    field.group = group;
    field.priority = priority;
    field.readOnly = readOnly;
    
    fields.push_back(field);
}

template<typename T>
void ConfigSchema::add(const std::string& name,
                       const std::any& defaultValue,
                       std::shared_ptr<ConfigValidator> validator,
                       const std::string& description,
                       const std::string& group,
                       int priority,
                       bool readOnly) {
    Field field;
    field.name = name;
    field.description = description;
    field.defaultValue = defaultValue;
    field.required = false; // Will be checked by validator
    field.group = group;
    field.priority = priority;
    field.readOnly = readOnly;
    
    if (validator) {
        field.validators.push_back(validator);
    }
    
    fields.push_back(field);
}

const ConfigSchema::Field* ConfigSchema::getField(const std::string& name) const {
    auto it = std::find_if(fields.begin(), fields.end(),
                          [&name](const Field& field) { return field.name == name; });
    if (it != fields.end()) {
        return &(*it);
    }
    return nullptr;
}

ConfigSchema::Field* ConfigSchema::getField(const std::string& name) {
    auto it = std::find_if(fields.begin(), fields.end(),
                          [&name](const Field& field) { return field.name == name; });
    if (it != fields.end()) {
        return &(*it);
    }
    return nullptr;
}

std::vector<const ConfigSchema::Field*> ConfigSchema::getFieldsByGroup(const std::string& group) const {
    std::vector<const Field*> groupFields;
    for (const auto& field : fields) {
        if (field.group == group) {
            groupFields.push_back(&field);
        }
    }
    return groupFields;
}

bool ConfigSchema::validate(const Config& config) const {
    for (const auto& field : fields) {
        // Check if field exists in config
        if (field.required && !config.has(field.name)) {
            return false; // Required field is missing
        }
        
        // Validate field value
        std::optional<ConfigValue> valueOpt = config.get<ConfigValue>(field.name);
        if (valueOpt) {
            const ConfigValue& value = *valueOpt;
            
            // Validate against validators
            for (const auto& validator : field.validators) {
                ValidationResult result = validator->validate(field.name, value);
                if (!result.valid) {
                    return false; // Validation failed
                }
            }
        }
    }
    
    return true; // All validations passed
}

std::vector<std::string> ConfigSchema::getRequiredFieldNames() const {
    std::vector<std::string> requiredFields;
    for (const auto& field : fields) {
        if (field.required) {
            requiredFields.push_back(field.name);
        }
    }
    return requiredFields;
}

std::vector<std::string> ConfigSchema::getFieldNamesByGroup(const std::string& group) const {
    std::vector<std::string> fieldNames;
    for (const auto& field : fields) {
        if (field.group == group) {
            fieldNames.push_back(field.name);
        }
    }
    return fieldNames;
}

} // namespace nlm
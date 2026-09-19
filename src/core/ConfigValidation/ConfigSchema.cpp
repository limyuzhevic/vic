#include "ConfigSchema.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {
    
    // Implementation of ConfigSchema methods
    
    // Field registration methods
    void ConfigSchema::addField(const std::string& name,
                               const FieldType& defaultValue,
                               const std::string& description,
                               bool required,
                               const std::string& typeName) {
        fields_[name] = FieldDefinition(name, defaultValue, description, {}, required, typeName);
        requiredFields_.insert(name);
    }
    
    void ConfigSchema::addField(const std::string& name,
                               const FieldType& defaultValue,
                               const std::vector<ValidationRules::ValidationFunction>& validators,
                               const std::string& description,
                               bool required,
                               const std::string& typeName) {
        fields_[name] = FieldDefinition(name, defaultValue, description, validators, required, typeName);
        fieldValidators_[name] = validators;
        requiredFields_.insert(name);
    }
    
    void ConfigSchema::addField(const std::string& name,
                               const FieldType& defaultValue,
                               ValidationRules::ValidationFunction validator,
                               const std::string& description,
                               bool required,
                               const std::string& typeName) {
        fields_[name] = FieldDefinition(name, defaultValue, description, {validator}, required, typeName);
        fieldValidators_[name] = {validator};
        requiredFields_.insert(name);
    }
    
    void ConfigSchema::addFieldWithType(const std::string& name,
                                       const FieldType& defaultValue,
                                       const std::string& typeName,
                                       const std::string& description,
                                       bool required) {
        fields_[name] = FieldDefinition(name, defaultValue, description, {}, required, typeName);
        fieldTypes_[name] = typeName;
        requiredFields_.insert(name);
    }
    
    void ConfigSchema::addCustomValidator(const std::string& fieldName,
                                         std::function<ValidationResult(const ConfigEntry&, const ConfigEntry*)> validator) {
        customValidators_[fieldName] = validator;
    }
    
    // Get field definition
    const FieldDefinition* ConfigSchema::getField(const std::string& name) const {
        auto it = fields_.find(name);
        return (it != fields_.end()) ? &it->second : nullptr;
    }
    
    // Check if field exists
    bool ConfigSchema::hasField(const std::string& name) const {
        return fields_.find(name) != fields_.end();
    }
    
    // Get all field names
    std::vector<std::string> ConfigSchema::getFieldNames() const {
        std::vector<std::string> names;
        names.reserve(fields_.size());
        for (const auto& pair : fields_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get all required fields
    std::set<std::string> ConfigSchema::getRequiredFields() const {
        return requiredFields_;
    }
    
    // Get field type name
    std::string ConfigSchema::getFieldTypeName(const std::string& name) const {
        auto it = fieldTypes_.find(name);
        return (it != fieldTypes_.end()) ? it->second : "";
    }
    
    // Validate configuration entry against this schema
    ValidationResult ConfigSchema::validateEntry(const ConfigEntry& entry, const std::map<std::string, ConfigEntry>& allEntries) const {
        // First validate against field-specific validators
        auto fieldIt = fields_.find(entry.key);
        if (fieldIt != fields_.end()) {
            const auto& fieldDef = fieldIt->second;
            
            // Check if required field is present
            if (fieldDef.required && !hasField(entry.key)) {
                return ValidationResult::failure("Required field '" + entry.key + "' is missing");
            }
            
            // Run field validators
            for (const auto& validator : fieldDef.validators) {
                ValidationResult result = validator(entry.value, &entry);
                if (!result.isValid) {
                    return result;
                }
            }
        } else {
            // Field not in schema - if strict validation is enabled, reject it
            if (validateAgainstSchema_) {
                return ValidationResult::failure("Unknown configuration field: '" + entry.key + "'");
            }
        }
        
        // Run custom validators for this field
        auto customIt = customValidators_.find(entry.key);
        if (customIt != customValidators_.end()) {
            ValidationResult result = customIt->second(entry, nullptr);
            if (!result.isValid) {
                return result;
            }
        }
        
        return ValidationResult::success();
    }
    
    // Validate entire configuration
    ValidationResult ConfigSchema::validate(const std::shared_ptr<Config>& config) const {
        if (!config) {
            return ValidationResult::failure("Configuration is null");
        }
        
        std::map<std::string, ConfigEntry> allEntries;
        for (const auto& key : config->getKeys()) {
            auto it = std::find_if(config->pImpl->entries.begin(), config->pImpl->entries.end(),
                                 [&key](const ConfigEntry& e) { return e.key == key; });
            if (it != config->pImpl->entries.end()) {
                allEntries[key] = *it;
            }
        }
        
        std::vector<ValidationResult> results;
        
        // Validate all entries
        for (const auto& pair : allEntries) {
            ValidationResult result = validateEntry(pair.second, allEntries);
            if (!result.isValid) {
                results.push_back(result);
            }
        }
        
        // Check for missing required fields
        for (const auto& fieldName : requiredFields_) {
            if (allEntries.find(fieldName) == allEntries.end()) {
                const auto* fieldDef = getField(fieldName);
                std::string fieldType = fieldDef ? fieldDef->typeName : "field";
                results.push_back(ValidationResult::failure(
                    "Required field '" + fieldName + "' of type '" + fieldType + "' is missing"
                ));
            }
        }
        
        if (results.empty()) {
            return ValidationResult::success();
        }
        
        // Combine all validation errors
        std::ostringstream oss;
        oss << "Configuration validation failed with " << results.size() << " error(s):";
        for (size_t i = 0; i < results.size(); ++i) {
            oss << "\n  " << (i + 1) << ". " << results[i].errorMessage;
            if (!results[i].details.empty()) {
                oss << " (Details: ";
                for (const auto& detail : results[i].details) {
                    oss << detail.first << "=" << detail.second << "; ";
                }
                oss << ")";
            }
        }
        
        ValidationResult combined;
        combined.isValid = false;
        combined.errorMessage = oss.str();
        
        return combined;
    }
    
    // Get schema name
    std::string ConfigSchema::getName() const {
        return name_;
    }
    
    // Enable/disable strict validation
    void ConfigSchema::setStrictValidation(bool strict) {
        validateAgainstSchema_ = strict;
    }
    
    // Check if strict validation is enabled
    bool ConfigSchema::isStrictValidation() const {
        return validateAgainstSchema_;
    }
    
    // Merge another schema (fields from other schema take precedence)
    void ConfigSchema::merge(const ConfigSchema& other) {
        for (const auto& pair : other.fields_) {
            fields_[pair.first] = pair.second;
        }
        for (const auto& pair : other.fieldValidators_) {
            fieldValidators_[pair.first] = pair.second;
        }
        for (const auto& pair : other.fieldTypes_) {
            fieldTypes_[pair.first] = pair.second;
        }
        requiredFields_.insert(other.requiredFields_.begin(), other.requiredFields_.end());
        for (const auto& pair : other.customValidators_) {
            customValidators_[pair.first] = pair.second;
        }
    }
    
    // Clear all fields
    void ConfigSchema::clear() {
        fields_.clear();
        fieldValidators_.clear();
        fieldTypes_.clear();
        customValidators_.clear();
        requiredFields_.clear();
    }
    
} // namespace nlm
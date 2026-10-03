// Validation framework implementation
#include "Validation.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace nlm {

// ValidationFramework implementation
ValidationFramework::ValidationFramework() : strategy_(ValidationStrategy::Strict),
                                            schemaLoadingEnabled_(true) {}

ValidationFramework::~ValidationFramework() {}

void ValidationFramework::registerValidator(std::unique_ptr<ConfigValidator> validator) {
    if (validator) {
        validators_[validator->getName()] = std::move(validator);
    }
}

ConfigValidator* ValidationFramework::getValidator(const std::string& name) const {
    auto it = validators_.find(name);
    return it != validators_.end() ? it->second.get() : nullptr;
}

ValidationResult ValidationFramework::validate(const std::unordered_map<std::string, ConfigValue>& config,
                                               const ConfigSchema& schema,
                                               ValidationStrategy strategy) const {
    ValidationResult result;
    setValidationStrategy(strategy);
    
    // Validate all fields in schema
    for (const auto& field : schema.fields) {
        auto it = config.find(field.key);
        if (it == config.end()) {
            // Field is missing
            if (field.required) {
                result.addError("Required field \"" + field.key + "\" is missing");
            }
            continue;
        }
        
        // Validate field value
        ValidationResult fieldResult = validateField(field.key, it->second, field, strategy);
        if (!fieldResult.valid) {
            for (const auto& error : fieldResult.errors) {
                result.addError("Field \"" + field.key + "\": " + error);
            }
        }
        for (const auto& warning : fieldResult.warnings) {
            result.addWarning("Field \"" + field.key + "\": " + warning);
        }
    }
    
    return result;
}

ValidationResult ValidationFramework::validateField(const std::string& key,
                                                    const ConfigValue& value,
                                                    const ConfigSchema::Field& field,
                                                    ValidationStrategy strategy) const {
    ValidationResult result;
    
    // Get validators for this field based on type
    std::vector<std::string> validatorNames;
    
    // Type validation is always performed
    validatorNames.push_back("TypeValidator");
    
    // Range validation for numeric types
    if (isNumericType(field.type)) {
        validatorNames.push_back("RangeValidator");
    }
    
    // Format validation for string types
    if (field.type == "string") {
        validatorNames.push_back("FormatValidator");
    }
    
    // Validate against each registered validator
    for (const auto& validatorName : validatorNames) {
        ConfigValidator* validator = getValidator(validatorName);
        if (validator) {
            ValidationResult validatorResult = validator->validate(value, key, field);
            
            if (strategy == ValidationStrategy::Strict) {
                for (const auto& error : validatorResult.errors) {
                    result.addError(error);
                }
            } else if (strategy == ValidationStrategy::Lenient) {
                for (const auto& error : validatorResult.errors) {
                    if (isCriticalError(error)) {
                        result.addError(error);
                    } else {
                        result.addWarning(error);
                    }
                }
            } else { // Report
                for (const auto& error : validatorResult.errors) {
                    result.addError(error);
                }
                for (const auto& warning : validatorResult.warnings) {
                    result.addWarning(warning);
                }
            }
        }
    }
    
    return result;
}

std::vector<std::string> ValidationFramework::findSchemas() const {
    std::vector<std::string> schemas;
    
    for (const auto& directory : schemaDirectories_) {
        // Implementation would search for schema files
        // For now, just return empty vector
    }
    
    return schemas;
}

std::string ValidationFramework::typeToString(const ConfigValue& value) const {
    std::string typeName;
    
    if (std::holds_alternative<int>(value)) {
        typeName = "int";
    } else if (std::holds_alternative<int64_t>(value)) {
        typeName = "int64_t";
    } else if (std::holds_alternative<double>(value)) {
        typeName = "double";
    } else if (std::holds_alternative<bool>(value)) {
        typeName = "bool";
    } else if (std::holds_alternative<std::string>(value)) {
        typeName = "string";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        typeName = "vector<int>";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        typeName = "vector<double>";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        typeName = "vector<string>";
    } else if (std::holds_alternative<std::unordered_map<std::string, std::string>>(value)) {
        typeName = "map";
    }
    
    return typeName;
}

bool ValidationFramework::isNumericType(const std::string& type) const {
    return type == "int" || type == "int64_t" || type == "double";
}

bool ValidationFramework::isVectorType(const std::string& type) const {
    return type == "vector<int>" || type == "vector<double>" || type == "vector<string>";
}

bool ValidationFramework::isCriticalError(const std::string& error) const {
    // Basic heuristic to determine if an error is critical
    // Critical errors contain keywords like "missing", "invalid", "out of range"
    std::string lowerError = error;
    std::transform(lowerError.begin(), lowerError.end(), lowerError.begin(), ::tolower);
    
    return lowerError.find("missing") != std::string::npos ||
           lowerError.find("invalid") != std::string::npos ||
           lowerError.find("out of range") != std::string::npos;
}

// Global validation framework instance
namespace {
    ValidationFramework globalValidationFramework;
}

ValidationFramework& getValidationFramework() {
    return globalValidationFramework;
}

// TypeValidator implementation
ValidationResult TypeValidator::validate(const ConfigValue& value,
                                        const std::string& key,
                                        const ConfigSchema::Field& field) const {
    ValidationResult result;
    std::string actualType = typeToString(value);
    
    if (field.type != actualType) {
        result.addError("Type mismatch: expected '" + field.type + "', got '" + actualType + "'");
    }
    
    return result;
}

// RangeValidator implementation
ValidationResult RangeValidator::validate(const ConfigValue& value,
                                          const std::string& key,
                                          const ConfigSchema::Field& field) const {
    ValidationResult result;
    
    // Helper function to check if a value is within range
    auto checkRange = [&](double val) {
        if (field.min.has_value()) {
            double minVal = std::holds_alternative<int>(*field.min) ? static_cast<double>(std::get<int>(*field.min)) : std::get<double>(*field.min);
            if (val < minVal) {
                std::stringstream ss;
                ss << "Value " << val << " is less than minimum " << minVal;
                result.addError(ss.str());
            }
        }
        if (field.max.has_value()) {
            double maxVal = std::holds_alternative<int>(*field.max) ? static_cast<double>(std::get<int>(*field.max)) : std::get<double>(*field.max);
            if (val > maxVal) {
                std::stringstream ss;
                ss << "Value " << val << " exceeds maximum " << maxVal;
                result.addError(ss.str());
            }
        }
    };
    
    if (std::holds_alternative<int>(value)) {
        checkRange(static_cast<double>(std::get<int>(value)));
    } else if (std::holds_alternative<int64_t>(value)) {
        checkRange(static_cast<double>(std::get<int64_t>(value)));
    } else if (std::holds_alternative<double>(value)) {
        checkRange(std::get<double>(value));
    }
    
    return result;
}

// FormatValidator implementation
ValidationResult FormatValidator::validate(const ConfigValue& value,
                                           const std::string& key,
                                           const ConfigSchema::Field& field) const {
    ValidationResult result;
    
    if (std::holds_alternative<std::string>(value)) {
        const std::string& strValue = std::get<std::string>(value);
        
        // Basic format validation based on field description
        std::string lowerDesc = field.description;
        std::transform(lowerDesc.begin(), lowerDesc.end(), lowerDesc.begin(), ::tolower);
        
        // Path validation if description mentions path
        if (lowerDesc.find("path") != std::string::npos) {
            if (!strValue.empty() && strValue[0] != '/' && strValue[0] != '\' &&
                strValue.find(':') == std::string::npos) {
                // Not an absolute path, check if it exists relative to current directory
                // For now, just warn
                result.addWarning("Path \"" + strValue + "\" may not be absolute");
            }
        }
        
        // Email validation if description mentions email
        if (lowerDesc.find("email") != std::string::npos) {
            // Simple email validation
            size_t atPos = strValue.find('@');
            if (atPos == std::string::npos || atPos == 0 || atPos == strValue.length() - 1) {
                result.addError("Invalid email format: \"" + strValue + "\"");
            }
        }
        
        // UUID validation if description mentions uuid
        if (lowerDesc.find("uuid") != std::string::npos) {
            // Simple UUID format check (8-4-4-4-12 hex digits)
            std::string uuidPattern = "[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}";
            std::regex uuidRegex(uuidPattern);
            if (!std::regex_match(strValue, uuidRegex)) {
                result.addError("Invalid UUID format: \"" + strValue + "\"");
            }
        }
    }
    
    return result;
}

// DependencyValidator implementation
ValidationResult DependencyValidator::validate(const ConfigValue& value,
                                                const std::string& key,
                                                const ConfigSchema::Field& field) const {
    ValidationResult result;
    
    // Check if this key has any dependencies
    auto it = dependencies_.find(key);
    if (it != dependencies_.end()) {
        std::string requiredKey = it->second;
        
        // Note: This validation assumes we have access to the full configuration
        // In practice, dependency validation would need the entire config
        // For now, we'll just validate the value itself
    }
    
    return result;
}

void DependencyValidator::addDependency(const std::string& dependentKey,
                                         const std::string& requiredKey) {
    dependencies_[dependentKey] = requiredKey;
}

// LogicalValidator implementation
ValidationResult LogicalValidator::validate(const ConfigValue& value,
                                            const std::string& key,
                                            const ConfigSchema::Field& field) const {
    ValidationResult result;
    
    // Check mutually exclusive groups
    for (const auto& group : mutuallyExclusiveGroups_) {
        int presentCount = 0;
        for (const auto& groupKey : group) {
            if (groupKey == key) {
                presentCount++;
            }
        }
        if (presentCount > 1) {
            result.addError("Key \"" + key + "\" is in mutually exclusive group with multiple keys present");
        }
    }
    
    // Check required combinations
    for (const auto& combination : requiredCombinations_) {
        bool allPresent = true;
        for (const auto& comboKey : combination) {
            if (comboKey != key && !has(comboKey)) {
                allPresent = false;
                break;
            }
        }
        
        if (allPresent) {
            // All required keys are present, but we can't validate the value itself
        }
    }
    
    return result;
}

void LogicalValidator::addMutuallyExclusiveGroup(const std::vector<std::string>& keys) {
    mutuallyExclusiveGroups_.push_back(keys);
}

void LogicalValidator::addRequiredCombination(const std::vector<std::string>& keys) {
    requiredCombinations_.push_back(keys);
}

// Convenience functions
ValidationResult validateConfig(const Config& config,
                               const ConfigSchema& schema,
                               ValidationStrategy strategy) {
    ValidationResult result;
    
    // Convert config entries to map for validation
    std::unordered_map<std::string, ConfigValue> configMap;
    for (const auto& entry : config.getKeys()) {
        auto value = config.get<ConfigValue>(entry);
        if (value.has_value()) {
            configMap[entry] = *value;
        }
    }
    
    return getValidationFramework().validate(configMap, schema, strategy);
}

std::string getValidationErrors(const Config& config,
                               const ConfigSchema& schema,
                               ValidationStrategy strategy) {
    ValidationResult result = validateConfig(config, schema, strategy);
    return result.getFullMessage();
}

} // namespace nlm

#include "Validation.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

// Implementation of validation functions

std::vector<ValidationError> ValidationRegistry::validate(const Config& config, 
                                                         ValidationSeverity minSeverity) const {
    std::vector<ValidationError> errors;
    
    // First, validate based on registered rules
    for (const auto& rule : rules) {
        for (const auto& entry : config.pImpl->entries) {
            ValidationSeverity severity = rule->validate(entry.key, entry.value, config);
            
            if (static_cast<int>(severity) >= static_cast<int>(minSeverity)) {
                std::string errorMsg = "Validation failed for key: " + entry.key + " - " + 
                                     rule->getDescription();
                
                ValidationError error(entry.key, errorMsg, severity);
                
                // Add more specific error information based on rule type
                if (typeid(*rule) == typeid(TypeValidationRule)) {
                    error.expected = "Expected type: " + 
                                   dynamic_cast<const TypeValidationRule*>(rule.get())->expectedType;
                    // Note: getting actual type from variant is complex, omit for now
                } else if (typeid(*rule) == typeid(RangeValidationRule)) {
                    const auto& rangeRule = dynamic_cast<const RangeValidationRule&>(*rule);
                    error.expected = "Expected range: [" + std::to_string(rangeRule.min) + 
                                   ", " + std::to_string(rangeRule.max) + "]";
                    
                    // Get actual value
                    double actualValue = 0.0;
                    if (rangeRule.type == "int" && std::holds_alternative<int>(entry.value)) {
                        actualValue = static_cast<double>(std::get<int>(entry.value));
                    } else if (rangeRule.type == "int64_t" && std::holds_alternative<int64_t>(entry.value)) {
                        actualValue = static_cast<double>(std::get<int64_t>(entry.value));
                    } else if (rangeRule.type == "double" && std::holds_alternative<double>(entry.value)) {
                        actualValue = std::get<double>(entry.value);
                    }
                    
                    if (actualValue > 0) {  // Avoid division by zero
                        error.actual = "Actual value: " + std::to_string(actualValue);
                    }
                }
                
                errors.push_back(error);
            }
        }
    }
    
    // Validate based on metadata
    for (const auto& entry : config.pImpl->entries) {
        auto it = configMetadata.find(entry.key);
        if (it != configMetadata.end()) {
            const auto& metadata = it->second;
            
            // Check if value matches metadata type
            if (!metadata.type.empty()) {
                std::string actualType;
                if (std::holds_alternative<int>(entry.value)) actualType = "int";
                else if (std::holds_alternative<int64_t>(entry.value)) actualType = "int64_t";
                else if (std::holds_alternative<double>(entry.value)) actualType = "double";
                else if (std::holds_alternative<bool>(entry.value)) actualType = "bool";
                else if (std::holds_alternative<std::string>(entry.value)) actualType = "string";
                else if (std::holds_alternative<std::vector<int>>(entry.value)) actualType = "std::vector<int>";
                else if (std::holds_alternative<std::vector<double>>(entry.value)) actualType = "std::vector<double>";
                else if (std::holds_alternative<std::vector<std::string>>(entry.value)) actualType = "std::vector<std::string>";
                else actualType = "unknown";
                
                if (actualType != metadata.type) {
                    errors.push_back(ValidationError(
                        entry.key,
                        "Type mismatch: expected '" + metadata.type + "', got '" + actualType + "'",
                        ValidationSeverity::ERROR,
                        "Expected type: '" + metadata.type + "'",
                        "Actual type: '" + actualType + "'"
                    ));
                }
            }
            
            // Check range constraints
            if (metadata.range.has_value()) {
                const auto& range = metadata.range.value();
                double actualValue = 0.0;
                
                if (metadata.type == "int" && std::holds_alternative<int>(entry.value)) {
                    actualValue = static_cast<double>(std::get<int>(entry.value));
                } else if (metadata.type == "int64_t" && std::holds_alternative<int64_t>(entry.value)) {
                    actualValue = static_cast<double>(std::get<int64_t>(entry.value));
                } else if (metadata.type == "double" && std::holds_alternative<double>(entry.value)) {
                    actualValue = std::get<double>(entry.value);
                }
                
                if (actualValue < range.first || actualValue > range.second) {
                    errors.push_back(ValidationError(
                        entry.key,
                        "Value out of range: " + std::to_string(actualValue) + 
                        " not in [" + std::to_string(range.first) + ", " + std::to_string(range.second) + "]",
                        ValidationSeverity::ERROR,
                        "Range: [" + std::to_string(range.first) + ", " + std::to_string(range.second) + "]",
                        "Actual: " + std::to_string(actualValue)
                    ));
                } else if (actualValue < range.first * 1.1 || actualValue > range.second * 0.9) {
                    errors.push_back(ValidationError(
                        entry.key,
                        "Value outside optimal range: " + std::to_string(actualValue),
                        ValidationSeverity::WARNING,
                        "Optimal range: [" + std::to_string(range.first * 1.1) + ", " + 
                                       std::to_string(range.second * 0.9) + "]",
                        "Actual: " + std::to_string(actualValue)
                    ));
                }
            }
            
            // Check if required
            if (metadata.required && !config.has(entry.key)) {
                errors.push_back(ValidationError(
                    entry.key,
                    "Required configuration key is missing",
                    ValidationSeverity::ERROR,
                    "Key must be present in configuration",
                    "Key not found"
                ));
            }
            
            // Check dependencies
            for (const auto& depKey : metadata.dependsOn) {
                if (!config.has(depKey)) {
                    errors.push_back(ValidationError(
                        entry.key,
                        "Configuration key depends on missing key: " + depKey,
                        ValidationSeverity::ERROR,
                        "Key '" + depKey + "' must be present",
                        "Missing: " + depKey
                    ));
                }
            }
        }
    }
    
    // Sort errors by key for consistent output
    std::sort(errors.begin(), errors.end(), 
              [](const ValidationError& a, const ValidationError& b) {
                  return a.key < b.key;
              });
    
    return errors;
}

} // namespace nlm

// src/core/Config/ConfigurationValidator.hpp
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>
#include "ConfigurationSchema.hpp"

namespace nlm {

// Validation result structure
struct ValidationResult {
    bool success;
    std::string errorMessage;
    std::string warningMessage;
    std::vector<std::string> warnings;
    
    ValidationResult() : success(true), errorMessage(""), warningMessage("") {}
    ValidationResult(bool s, const std::string& error, const std::string& warning = "")
        : success(s), errorMessage(error), warningMessage(warning) {}
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration validator class
class ConfigurationValidator {
private:
    std::unique_ptr<ConfigurationSchema> schema;
    std::unordered_map<std::string, std::function<bool(const nlohmann::json&, std::string&)>> customValidators;
    bool autoFixEnabled;
    
public:
    ConfigurationValidator();
    ConfigurationValidator(std::unique_ptr<ConfigurationSchema> s);
    
    // Set the schema to use for validation
    void setSchema(std::unique_ptr<ConfigurationSchema> s) {
        schema = std::move(s);
    }
    
    // Add a custom validator for a specific parameter
    void addCustomValidator(const std::string& paramName, std::function<bool(const nlohmann::json&, std::string&)> validator) {
        customValidators[paramName] = validator;
    }
    
    // Enable or disable auto-fix mode
    void setAutoFix(bool enabled) {
        autoFixEnabled = enabled;
    }
    
    // Validate configuration against schema
    ValidationResult validate(const nlohmann::json& config) const;
    
    // Validate a specific parameter
    ValidationResult validateParameter(const std::string& paramName, const nlohmann::json& value) const;
    
    // Apply default values from schema
    nlohmann::json applyDefaults(const nlohmann::json& config) const;
    
    // Fix invalid values based on schema rules
    nlohmann::json autoFix(const nlohmann::json& config) const;
    
    // Get validation errors and warnings
    std::string getValidationReport(const nlohmann::json& config) const;
    
    // Check if validator has a schema
    bool hasSchema() const { return schema && !schema->isEmpty(); }
    
    // Get schema description
    std::string getSchemaDescription() const {
        return schema ? schema->getDescription() : "No schema defined";
    }
};

// Validation utility functions
namespace Validation {
    // Common validation functions
    bool validatePositive(const nlohmann::json& value, std::string& error);
    bool validateNonNegative(const nlohmann::json& value, std::string& error);
    bool validateInteger(const nlohmann::json& value, std::string& error);
    bool validateRange(const nlohmann::json& value, std::string& error, double min, double max);
    bool validateArraySize(const nlohmann::json& value, std::string& error, size_t minSize, size_t maxSize);
    bool validateStringPattern(const nlohmann::json& value, std::string& error, const std::string& pattern);
    
    // Type conversion helpers
    std::string jsonToString(const nlohmann::json& value);
    double jsonToDouble(const nlohmann::json& value);
    int64_t jsonToInt64(const nlohmann::json& value);
    bool jsonToBool(const nlohmann::json& value);
    
    // JSON merging
    void mergeJson(nlohmann::json& target, const nlohmann::json& source);
    
    // Schema building helpers
    std::unique_ptr<ConfigurationSchema> createDefaultNLMConfigSchema();
}
}
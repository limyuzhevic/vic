#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "core/Config/Config.hpp"

namespace nlm {

class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Validate a configuration
    std::vector<std::string> validate(const Config& config) const;
    
    // Validate a specific key
    std::vector<std::string> validateKey(const std::string& key, const ConfigValue& value) const;
    
    // Add validation rule for a key
    void addValidationRule(const std::string& key, 
                          const std::string& type,
                          const std::string& validation,
                          const std::string& category = "",
                          double minValue = std::numeric_limits<double>::lowest(),
                          double maxValue = std::numeric_limits<double>::max(),
                          bool required = false,
                          const std::string& description = "");
    
    // Get validation rules for a key
    std::vector<std::string> getValidationRules(const std::string& key) const;
    
    // Get all validation rules
    std::unordered_map<std::string, std::vector<std::string>> getAllValidationRules() const;
    
    // Get error message for validation failure
    std::string getErrorMessage(const std::string& key, const std::string& errorType,
                               const ConfigValue& value) const;
    
    // Check if key has validation rules
    bool hasValidationRules(const std::string& key) const;
    
    // Get categories
    std::vector<std::string> getCategories() const;
    
    // Get configuration descriptions
    std::string getDescription(const std::string& key) const;
    
    // Initialize default validation rules
    void initializeDefaultRules();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper functions
    bool validateType(const std::string& key, const ConfigValue& value, std::string& error) const;
    bool validateRange(const std::string& key, const ConfigValue& value, std::string& error) const;
    bool validateFormat(const std::string& key, const ConfigValue& value, std::string& error) const;
    bool validateDependencies(const Config& config, std::string& error) const;
    
    // Rule structure
    struct ValidationRule {
        std::string type;              // "int", "double", "string", "bool", "range", "regex"
        std::string validation;        // Pattern, range format, etc.
        std::string category;          // "brain", "plasticity", "neuromodulation", etc.
        double minValue;               // For numeric ranges
        double maxValue;               // For numeric ranges
        bool required;                 // Whether this setting is required
        std::string description;       // Human-readable description
    };
};

} // namespace nlm

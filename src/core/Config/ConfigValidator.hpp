#pragma once

#include "Config.hpp"
#include <string>
#include <unordered_map>
#include <functional>
#include <stdexcept>

namespace nlm {

// Configuration validation errors
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key, 
                         const ConfigValue& value, const std::string& expected)
        : std::runtime_error(message), key(key), value(value), expected(expected) {}
    
    const std::string& getKey() const { return key; }
    const ConfigValue& getValue() const { return value; }
    const std::string& getExpected() const { return expected; }
    
private:
    std::string key;
    ConfigValue value;
    std::string expected;
};

// Configuration validator
class ConfigValidator {
public:
    // Validation result
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        ValidationResult() : isValid(true) {}
        
        void addError(const std::string& error) {
            isValid = false;
            errors.push_back(error);
        }
        
        void addWarning(const std::string& warning) {
            warnings.push_back(warning);
        }
        
        bool hasErrors() const { return !errors.empty(); }
        bool hasWarnings() const { return !warnings.empty(); }
    };

    // Configuration validation rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
        std::string warningMessage;
        bool required;
    };

    ConfigValidator();
    ~ConfigValidator() = default;
    
    // Add validation rule
    void addRule(const std::string& key, 
                 std::function<bool(const ConfigValue&)> validator,
                 const std::string& errorMessage = "",
                 const std::string& warningMessage = "",
                 bool required = false);
    
    // Add validation rule with type checking
    void addTypeRule(const std::string& key, 
                    std::function<bool(const ConfigValue&)> validator,
                    const std::string& errorMessage = "",
                    const std::string& warningMessage = "",
                    bool required = false);
    
    // Add validation rule with range checking
    void addRangeRule(const std::string& key,
                     std::function<bool(const ConfigValue&)> validator,
                     const std::string& errorMessage = "",
                     const std::string& warningMessage = "",
                     bool required = false);
    
    // Validate configuration
    ValidationResult validate(const Config& config) const;
    
    // Validate configuration value
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    
    // Clear all validation rules
    void clearRules();
    
    // Get number of validation rules
    size_t getRuleCount() const { return rules.size(); }
    
    // Get validation rules for specific key
    std::vector<ValidationRule> getRulesForKey(const std::string& key) const;
    
    // Create default validation rules
    static ConfigValidator createDefaultValidator();
    
private:
    std::vector<ValidationRule> rules;
    
    // Helper functions for type checking
    static bool isInt(const ConfigValue& value);
    static bool isInt64(const ConfigValue& value);
    static bool isDouble(const ConfigValue& value);
    static bool isBool(const ConfigValue& value);
    static bool isString(const ConfigValue& value);
    static bool isIntVector(const ConfigValue& value);
    static bool isDoubleVector(const ConfigValue& value);
    static bool isStringVector(const ConfigValue& value);
};

} // namespace nlm
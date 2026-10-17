#pragma once

#include "Config.hpp"
#include <vector>
#include <string>
#include <functional>

namespace nlm {

/**
 * Configuration validation rules
 */
struct ConfigValidationRule {
    std::string key;
    std::string type;  // "int", "double", "bool", "string", etc.
    bool required;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ConfigValidationRule() : required(false) {}
    ConfigValidationRule(const std::string& k, const std::string& t, bool req, const std::string& error)
        : key(k), type(t), required(req), errorMessage(error) {
        // Set up default validator based on type
        if (type == "int" || type == "int64") {
            validator = [](const ConfigValue& v) { return std::holds_alternative<int>(v) || std::holds_alternative<int64_t>(v); };
        } else if (type == "double") {
            validator = [](const ConfigValue& v) { return std::holds_alternative<double>(v); };
        } else if (type == "bool") {
            validator = [](const ConfigValue& v) { return std::holds_alternative<bool>(v); };
        } else if (type == "string") {
            validator = [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); };
        } else if (type == "float") {
            validator = [](const ConfigValue& v) { return std::holds_alternative<double>(v); }; // Use double for float
        } else {
            validator = [](const ConfigValue& v) { return true; };  // No validation for unknown types
        }
    }
};

/**
 * Configuration validator
 */
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    /**
     * Add a validation rule
     */
    void addRule(const ConfigValidationRule& rule);
    
    /**
     * Add a rule with simple type validation
     */
    void addRule(const std::string& key, const std::string& type, bool required, const std::string& errorMessage);
    
    /**
     * Add a custom validation rule
     */
    void addCustomRule(const std::string& key, std::function<bool(const ConfigValue&)> validator, const std::string& errorMessage);
    
    /**
     * Validate configuration
     */
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    
    /**
     * Get all validation errors without collecting them
     */
    bool isValid(const Config& config) const;
    
    /**
     * Get validation errors for a specific key
     */
    std::vector<std::string> getErrorsForKey(const Config& config, const std::string& key) const;
    
    /**
     * Get all validation errors
     */
    std::vector<std::string> getAllErrors(const Config& config) const;
    
    /**
     * Get rule count
     */
    size_t getRuleCount() const;
    
    /**
     * Clear all rules
     */
    void clearRules();
    
    /**
     * Get default Phase 2 validation rules
     */
    static std::vector<ConfigValidationRule> getDefaultPhase2Rules();
    
    /**
     * Get default Phase 3/4/5/6 validation rules
     */
    static std::vector<ConfigValidationRule> getDefaultFullPhaseRules();
    
    /**
     * Print validation report
     */
    static void printValidationReport(const Config& config, const std::vector<std::string>& errors);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Configuration validation error
 */
class ConfigValidationError {
public:
    std::string key;
    std::string errorMessage;
    
    ConfigValidationError(const std::string& k, const std::string& msg) : key(k), errorMessage(msg) {}
};

} // namespace nlm
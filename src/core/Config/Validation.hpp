#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <stdexcept>
#include <optional>
#include <any>
#include <sstream>
#include <unordered_map>
#include <functional>

// Include common configuration types
#include "ConfigTypes.hpp"

namespace nlm {

// Validation exception
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Validation result structure
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, std::string> validationReport;
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
        validationReport["ERRORS"] += "\n" + error;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
        validationReport["WARNINGS"] += "\n" + warning;
    }
    
    std::string getDetailedReport() const {
        std::ostringstream oss;
        oss << "=== Configuration Validation Report ===\n";
        oss << "Overall: " << (valid ? "PASS" : "FAIL") << "\n\n";
        
        if (!validationReport.empty()) {
            for (const auto& entry : validationReport) {
                if (entry.first == "ERRORS" && !entry.second.empty()) {
                    oss << "ERRORS:" << entry.second.substr(1) << "\n";
                } else if (entry.first == "WARNINGS" && !entry.second.empty()) {
                    oss << "WARNINGS:" << entry.second.substr(1) << "\n";
                }
            }
        }
        
        if (!errors.empty()) {
            oss << "\nDetailed Errors:";
            for (const auto& error : errors) {
                oss << "\n  - " << error;
            }
        }
        
        if (!warnings.empty()) {
            oss << "\n\nDetailed Warnings:";
            for (const auto& warning : warnings) {
                oss << "\n  - " << warning;
            }
        }
        
        return oss.str();
    }
};

// Configuration schema definition
struct ConfigurationSchema {
    enum class Type {
        INT64_T,
        INT,
        DOUBLE,
        FLOAT,
        BOOL,
        STRING,
        VECTOR_INT,
        VECTOR_DOUBLE,
        VECTOR_STRING
    };
    
    enum class ValidationType {
        RANGE_MIN,
        RANGE_MAX,
        RANGE_MIN_MAX,
        POSITIVE,
        NON_NEGATIVE,
        ZERO_TO_ONE,
        GREATER_THAN,
        LESS_THAN,
        EQUAL_TO,
        CONSTRAINT,
        CUSTOM
    };
    
    struct ValidationRule {
        ValidationType type;
        std::any min;
        std::any max;
        std::any exact;
        std::function<bool(const std::any&)> customValidator;
        std::string description;
    };
    
    struct ParameterInfo {
        std::string name;
        Type type;
        std::string description;
        bool required;
        std::any defaultValue;
        std::vector<ValidationRule> validationRules;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    // Constructor and destructor
    ConfigurationSchema();
    ~ConfigurationSchema();
    
    // Disable copying, enable moving
    ConfigurationSchema(const ConfigurationSchema&) = delete;
    ConfigurationSchema& operator=(const ConfigurationSchema&) = delete;
    ConfigurationSchema(ConfigurationSchema&&) noexcept;
    ConfigurationSchema& operator=(ConfigurationSchema&&) noexcept;
    
    // Singleton pattern
    static ConfigurationSchema& getInstance() {
        static ConfigurationSchema instance;
        return instance;
    }
    
    // Add parameter definition
    void addParameter(const std::string& name, Type type, const std::string& description,
                     bool required = false, const std::any& defaultValue = std::any(),
                     const std::vector<ValidationRule>& validationRules = {});
    
    // Get parameter info
    const ParameterInfo* getParameter(const std::string& name) const;
    
    // Validate configuration value
    ValidationResult validate(const std::string& key, const ConfigValue& value, ConfigSource source) const;
    
    // Validate all configuration entries
    ValidationResult validateAll(const std::vector<ConfigEntry>& entries) const;
    
    // Create validation report from result
    static std::string createValidationReport(const ValidationResult& result);
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Clear all parameters
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor
    ConfigurationSchema(std::unique_ptr<Impl>&& impl);
    
    // Validation helper methods
    bool validateType(const std::string& key, const ConfigValue& value, Type expectedType) const;
    bool validateRange(const std::any& value, const ValidationRule& rule) const;
    bool validateConstraint(const std::string& key, const std::vector<ConfigEntry>& entries) const;
};

// Global schema instance
inline ConfigurationSchema& getConfigurationSchema() {
    return ConfigurationSchema::getInstance();
}

} // namespace nlm

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <any>
#include <memory>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <set>

namespace nlm {

// Forward declarations to avoid circular includes
class NLMException;

// Validation result types
enum class ValidationResult {
    Success,
    Failure,
    Warning,
    Skipped
};

// Validation severity levels
enum class ValidationSeverity {
    Information,
    Warning,
    Error,
    Critical
};

// Validation error details
struct ValidationErrorDetail {
    std::string message;
    std::string parameterName;
    ValidationSeverity severity;
    std::string suggestedFix;
    std::map<std::string, std::any> metadata;
    
    ValidationErrorDetail() : severity(ValidationSeverity::Error) {}
    
    ValidationErrorDetail(const std::string& msg, const std::string& param = "",
                         ValidationSeverity sev = ValidationSeverity::Error,
                         const std::string& fix = "")
        : message(msg), parameterName(param), severity(sev), suggestedFix(fix) {}
};

// Validation result container
struct ValidationResult {
    ValidationResult() : passed(true), severity(ValidationSeverity::Information) {}
    
    bool passed;
    ValidationSeverity severity;
    std::vector<ValidationErrorDetail> errors;
    std::vector<ValidationErrorDetail> warnings;
    std::chrono::milliseconds validationTime;
    
    ValidationResult& operator+=(const ValidationResult& other) {
        passed = passed && other.passed;
        severity = std::max(severity, other.severity);
        errors.insert(errors.end(), other.errors.begin(), other.errors.end());
        warnings.insert(warnings.end(), other.warnings.begin(), other.warnings.end());
        validationTime = std::max(validationTime, other.validationTime);
        return *this;
    }
    
    bool hasErrors() const {
        return !errors.empty();
    }
    
    bool hasWarnings() const {
        return !warnings.empty();
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Validation Result: " << (passed ? "PASSED" : "FAILED");
        oss << " (severity: " << static_cast<int>(severity) << ")";
        if (!errors.empty()) {
            oss << "\nErrors: " << errors.size();
            for (const auto& error : errors) {
                oss << "\n  - " << error.message;
                if (!error.parameterName.empty()) {
                    oss << " (param: " << error.parameterName << ")";
                }
            }
        }
        if (!warnings.empty()) {
            oss << "\nWarnings: " << warnings.size();
            for (const auto& warning : warnings) {
                oss << "\n  - " << warning.message;
            }
        }
        oss << "\nValidation time: " << validationTime.count() << "ms";
        return oss.str();
    }
};

// Configuration validation framework
class ConfigValidator {
public:
    struct ConfigValidationRule {
        std::string key;
        std::string description;
        std::string type;
        bool required = false;
        std::string defaultValue;
        std::string minValue;
        std::string maxValue;
        std::string allowedValues;
        std::function<ValidationResult(const std::string&)> customValidator;
        
        ConfigValidationRule() : required(false) {}
        
        ConfigValidationRule(const std::string& k, const std::string& d, bool req = false)
            : key(k), description(d), required(req) {}
    };
    
    struct ConfigTemplate {
        std::string name;
        std::string description;
        std::vector<ConfigValidationRule> rules;
        std::map<std::string, std::string> defaults;
        
        ConfigTemplate(const std::string& n, const std::string& d) : name(n), description(d) {}
    };
    
    ConfigValidator() = default;
    
    // Add validation rule for a key
    void addRule(const ConfigValidationRule& rule) {
        rules[rule.key] = rule;
    }
    
    // Add template
    void addTemplate(const ConfigTemplate& template) {
        templates[template.name] = template;
        // Add individual rules from template
        for (const auto& rule : template.rules) {
            addRule(rule);
        }
    }
    
    // Validate configuration values
    ValidationResult validate(const std::map<std::string, std::string>& config) const {
        ValidationResult result;
        
        for (const auto& [key, rule] : rules) {
            auto it = config.find(key);
            if (it == config.end()) {
                if (rule.required) {
                    ValidationErrorDetail error("Required configuration key is missing", key);
                    error.severity = ValidationSeverity::Error;
                    error.suggestedFix = "Add this key with an appropriate value";
                    result.errors.push_back(error);
                    result.passed = false;
                }
                continue;
            }
            
            // Validate the value
            ValidationResult valueResult = validateValue(rule, it->second);
            result += valueResult;
        }
        
        return result;
    }
    
    // Validate against a template
    ValidationResult validateWithTemplate(const std::string& templateName,
                                          const std::map<std::string, std::string>& config) const {
        auto it = templates.find(templateName);
        if (it == templates.end()) {
            ValidationResult result;
            result.passed = false;
            ValidationErrorDetail error("Template not found", templateName);
            error.severity = ValidationSeverity::Error;
            error.suggestedFix = "Check template name spelling";
            result.errors.push_back(error);
            return result;
        }
        
        return validate(it->second.rules, config, it->second.defaults);
    }
    
    // Validate with custom template rules
    ValidationResult validate(const std::vector<ConfigValidationRule>& rules,
                             const std::map<std::string, std::string>& config,
                             const std::map<std::string, std::string>& defaults) const {
        ValidationResult result;
        
        for (const auto& rule : rules) {
            std::string value;
            auto it = config.find(rule.key);
            if (it != config.end()) {
                value = it->second;
            } else if (defaults.find(rule.key) != defaults.end()) {
                value = defaults.at(rule.key);
            } else if (rule.required) {
                ValidationErrorDetail error("Required configuration key is missing", rule.key);
                error.severity = ValidationSeverity::Error;
                error.suggestedFix = "Provide value for this key";
                result.errors.push_back(error);
                result.passed = false;
                continue;
            } else {
                // Optional key with no value - skip validation
                continue;
            }
            
            ValidationResult valueResult = validateValue(rule, value);
            result += valueResult;
        }
        
        return result;
    }
    
private:
    ValidationResult validateValue(const ConfigValidationRule& rule, const std::string& value) const {
        ValidationResult result;
        
        // Type validation
        if (!rule.type.empty()) {
            ValidationResult typeResult = validateType(rule.type, value);
            if (!typeResult.passed) {
                result += typeResult;
                return result;
            }
        }
        
        // Range validation
        if (!rule.minValue.empty()) {
            ValidationResult rangeResult = validateRange(rule.minValue, rule.maxValue, value);
            if (!rangeResult.passed) {
                result += rangeResult;
                return result;
            }
        }
        
        // Allowed values validation
        if (!rule.allowedValues.empty()) {
            ValidationResult allowedResult = validateAllowed(rule.allowedValues, value);
            if (!allowedResult.passed) {
                result += allowedResult;
                return result;
            }
        }
        
        // Custom validator
        if (rule.customValidator) {
            ValidationResult customResult = rule.customValidator(value);
            if (!customResult.passed) {
                result += customResult;
                return result;
            }
        }
        
        // All validations passed
        return result;
    }
    
    ValidationResult validateType(const std::string& type, const std::string& value) const {
        ValidationResult result;
        
        if (type == "int") {
            try {
                std::stoi(value);
            } catch (...) {
                result.passed = false;
                ValidationErrorDetail error("Value must be an integer", "type");
                error.severity = ValidationSeverity::Error;
                error.suggestedFix = "Provide a valid integer value";
                result.errors.push_back(error);
            }
        } else if (type == "float") {
            try {
                std::stod(value);
            } catch (...) {
                result.passed = false;
                ValidationErrorDetail error("Value must be a floating-point number", "type");
                error.severity = ValidationSeverity::Error;
                error.suggestedFix = "Provide a valid floating-point value";
                result.errors.push_back(error);
            }
        } else if (type == "bool") {
            if (value != "true" && value != "false" && value != "1" && value != "0") {
                result.passed = false;
                ValidationErrorDetail error("Value must be a boolean (true/false or 1/0)", "type");
                error.severity = ValidationSeverity::Error;
                error.suggestedFix = "Provide true/false or 1/0";
                result.errors.push_back(error);
            }
        } else if (type == "string") {
            // String validation passed
        }
        
        return result;
    }
    
    ValidationResult validateRange(const std::string& min, const std::string& max,
                                   const std::string& value) const {
        ValidationResult result;
        
        try {
            double val = std::stod(value);
            double minVal = std::stod(min);
            
            if (!max.empty()) {
                double maxVal = std::stod(max);
                if (val < minVal || val > maxVal) {
                    result.passed = false;
                    ValidationErrorDetail error("Value " + value + " is outside valid range [" + 
                                              min + ", " + (max.empty() ? min : max) + "]", "range");
                    error.severity = ValidationSeverity::Error;
                    error.suggestedFix = "Provide a value within the valid range";
                    result.errors.push_back(error);
                }
            } else if (val < minVal) {
                result.passed = false;
                ValidationErrorDetail error("Value " + value + " is less than minimum " + min, "range");
                error.severity = ValidationSeverity::Error;
                error.suggestedFix = "Provide a value greater than or equal to " + min;
                result.errors.push_back(error);
            }
        } catch (...) {
            result.passed = false;
            ValidationErrorDetail error("Invalid range or value format", "range");
            error.severity = ValidationSeverity::Error;
            error.suggestedFix = "Check range and value formats";
            result.errors.push_back(error);
        }
        
        return result;
    }
    
    ValidationResult validateAllowed(const std::string& allowedList, const std::string& value) const {
        ValidationResult result;
        
        // Parse allowed values (comma-separated list)
        std::stringstream ss(allowedList);
        std::string allowedItem;
        std::vector<std::string> allowedValues;
        
        while (std::getline(ss, allowedItem, ',')) {
            allowedItem.erase(0, allowedItem.find_first_not_of(" \t"));
            allowedItem.erase(allowedItem.find_last_not_of(" \t") + 1);
            if (!allowedItem.empty()) {
                allowedValues.push_back(allowedItem);
            }
        }
        
        if (std::find(allowedValues.begin(), allowedValues.end(), value) == allowedValues.end()) {
            result.passed = false;
            std::string allowedStr = "";
            for (size_t i = 0; i < allowedValues.size(); ++i) {
                if (i > 0) allowedStr += ", ";
                allowedStr += "\"" + allowedValues[i] + "\"";
            }
            
            ValidationErrorDetail error("Value \"" + value + "\" is not in allowed values: " + allowedStr, "allowed");
            error.severity = ValidationSeverity::Error;
            error.suggestedFix = "Choose one of the allowed values";
            result.errors.push_back(error);
        }
        
        return result;
    }
    
    std::map<std::string, ConfigValidationRule> rules;
    std::map<std::string, ConfigTemplate> templates;
};

} // namespace nlm

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <variant>
#include <functional>
#include <memory>
#include <chrono>
#include <set>
#include <any>
#include <numeric> // For std::numeric_limits
#include "Config.hpp"

namespace nlm {

// Configuration schema validation types
enum class ValidationType {
    None,           // No validation
    Range,          // Value must be within a range
    Enum,           // Value must be one of allowed values
    Format,         // Value must match a format pattern
    Required,       // Field must be present
    Dependency,     // Field presence depends on other fields
    Custom          // Custom validation function
};

enum class ValidationSeverity {
    Warning,        // Non-blocking, just a warning
    Error,          // Blocking, prevents configuration from being valid
    Critical        // Critical error, prevents system from functioning
};

struct ValidationRule {
    std::string fieldName;
    ValidationType type;
    ValidationSeverity severity;
    std::string description;
    std::any params;  // Parameters specific to validation type
    std::function<bool(const ConfigValue&, const std::string&, const Config&)> customValidator;
    
    ValidationRule() : type(ValidationType::None), severity(ValidationSeverity::Warning) {}
    
    ValidationRule(const std::string& field, ValidationType t, ValidationSeverity sev = ValidationSeverity::Warning,
                   const std::string& desc = "")
        : fieldName(field), type(t), severity(sev), description(desc) {}
};

struct ValidationError {
    std::string fieldName;
    std::string path;  // Path in nested configuration
    std::string message;
    ValidationSeverity severity;
    std::chrono::system_clock::time_point timestamp;
    std::string code;
    
    ValidationError() : severity(ValidationSeverity::Warning) {}
    
    ValidationError(const std::string& field, const std::string& msg, ValidationSeverity sev = ValidationSeverity::Warning)
        : fieldName(field), message(msg), severity(sev), timestamp(std::chrono::system_clock::now()) {}
    
    bool operator<(const ValidationError& other) const {
        return static_cast<int>(severity) < static_cast<int>(other.severity);
    }
};

class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a validation rule
    void addRule(const ValidationRule& rule) {
        rules.push_back(rule);
    }
    
    // Add a range validation rule
    void addRangeRule(const std::string& field, double min, double max,
                      ValidationSeverity severity = ValidationSeverity::Error,
                      const std::string& description = "") {
        RangeParams params{min, max};
        ValidationRule rule(field, ValidationType::Range, severity, description);
        rule.params = params;
        addRule(rule);
    }
    
    // Add enum validation rule
    void addEnumRule(const std::string& field, const std::vector<std::string>& allowedValues,
                     ValidationSeverity severity = ValidationSeverity::Error,
                     const std::string& description = "") {
        EnumParams params{allowedValues};
        ValidationRule rule(field, ValidationType::Enum, severity, description);
        rule.params = params;
        addRule(rule);
    }
    
    // Add format validation rule
    void addFormatRule(const std::string& field, const std::string& pattern,
                       ValidationSeverity severity = ValidationSeverity::Error,
                       const std::string& description = "") {
        FormatParams params{pattern};
        ValidationRule rule(field, ValidationType::Format, severity, description);
        rule.params = params;
        addRule(rule);
    }
    
    // Add dependency validation rule
    void addDependencyRule(const std::string& field, const std::string& dependsOn,
                           bool requiredWhenPresent = true,
                           ValidationSeverity severity = ValidationSeverity::Error,
                           const std::string& description = "") {
        DependencyParams params{dependsOn, requiredWhenPresent};
        ValidationRule rule(field, ValidationType::Dependency, severity, description);
        rule.params = params;
        addRule(rule);
    }
    
    // Add custom validation rule
    void addCustomRule(const std::string& field,
                       std::function<bool(const ConfigValue&, const std::string&, const Config&)> validator,
                       ValidationSeverity severity = ValidationSeverity::Error,
                       const std::string& description = "") {
        ValidationRule rule(field, ValidationType::Custom, severity, description);
        rule.customValidator = validator;
        addRule(rule);
    }
    
    // Get all validation rules
    const std::vector<ValidationRule>& getRules() const { return rules; }
    
    // Clear all rules
    void clear() { rules.clear(); }
    
    // Check if schema is empty
    bool empty() const { return rules.empty(); }
    
    // Merge with another schema
    void merge(const ConfigSchema& other) {
        rules.insert(rules.end(), other.rules.begin(), other.rules.end());
    }
    
private:
    // Parameter structures for different validation types
    struct RangeParams {
        double min;
        double max;
    };
    
    struct EnumParams {
        std::vector<std::string> allowedValues;
    };
    
    struct FormatParams {
        std::string pattern;
    };
    
    struct DependencyParams {
        std::string dependsOn;
        bool requiredWhenPresent;
    };
    
    std::vector<ValidationRule> rules;
};

class ConfigValidator {
public:
    ConfigValidator() = default;
    
    // Validate configuration against a schema
    std::vector<ValidationError> validate(const Config& config, const ConfigSchema& schema,
                                          bool isStrict = true) const {
        std::vector<ValidationError> errors;
        
        for (const auto& rule : schema.getRules()) {
            validateField(config, rule, errors, isStrict);
        }
        
        // Sort errors by severity (Critical > Error > Warning)
        std::sort(errors.begin(), errors.end(),
                  [](const ValidationError& a, const ValidationError& b) {
                      return static_cast<int>(a.severity) > static_cast<int>(b.severity);
                  });
        
        return errors;
    }
    
    // Validate individual field
    bool validateField(const Config& config, const ValidationRule& rule,
                       std::vector<ValidationError>& errors, bool isStrict) const {
        if (!config.has(rule.fieldName)) {
            if (rule.type == ValidationType::Required) {
                errors.emplace_back(rule.fieldName, "Required field is missing", rule.severity);
                return rule.severity == ValidationSeverity::Critical || !isStrict;
            }
            // Not required and missing - skip other validations
            return true;
        }
        
        // Get the field value
        auto valueOpt = getFieldValue(config, rule.fieldName);
        if (!valueOpt.has_value()) {
            errors.emplace_back(rule.fieldName, "Cannot read field value", ValidationSeverity::Error);
            return !isStrict;
        }
        
        const ConfigValue& value = valueOpt.value();
        
        // Apply validation based on rule type
        switch (rule.type) {
            case ValidationType::Range:
                validateRange(value, rule, errors, isStrict);
                break;
            case ValidationType::Enum:
                validateEnum(value, rule, errors, isStrict);
                break;
            case ValidationType::Format:
                validateFormat(value, rule, errors, isStrict);
                break;
            case ValidationType::Dependency:
                validateDependency(config, value, rule, errors, isStrict);
                break;
            case ValidationType::Custom:
                validateCustom(value, rule, errors, isStrict);
                break;
            case ValidationType::None:
            case ValidationType::Required:
                // Already handled
                break;
        }
        
        return !isStrict || !hasCriticalErrors(errors);
    }
    
    // Validate entire configuration
    bool validate(const Config& config, const ConfigSchema& schema, bool isStrict = true) const {
        std::vector<ValidationError> errors = validate(config, schema, isStrict);
        return errors.empty() || !hasCriticalErrors(errors);
    }
    
    // Validate individual key
    bool validateKey(const Config& config, const std::string& key, const ConfigSchema& schema,
                     bool isStrict = true) const {
        // Create a temporary schema with only the key
        ConfigSchema keySchema;
        for (const auto& rule : schema.getRules()) {
            if (rule.fieldName == key) {
                keySchema.addRule(rule);
            }
        }
        
        return validate(config, keySchema, isStrict);
    }
    
    // Get validation errors
    std::vector<ValidationError> getValidationErrors(const Config& config, const ConfigSchema& schema,
                                                     bool isStrict = true) const {
        return validate(config, schema, isStrict);
    }
    
    // Check if there are critical errors
    bool hasCriticalErrors(const std::vector<ValidationError>& errors) const {
        for (const auto& error : errors) {
            if (error.severity == ValidationSeverity::Critical) {
                return true;
            }
        }
        return false;
    }
    
    // Check if there are any errors (including warnings)
    bool hasErrors(const std::vector<ValidationError>& errors) const {
        return !errors.empty();
    }
    
    // Get errors
    const std::vector<ValidationError>& getErrors() const {
        return errors;
    }
    
private:
    std::vector<ValidationError> errors;
    
    // Helper to get field value from config
    std::optional<ConfigValue> getFieldValue(const Config& config, const std::string& fieldName) const {
        // Try different types based on field name convention or try all types
        // This is a simplified version - real implementation would be more sophisticated
        auto stringVal = config.get<std::string>(fieldName);
        if (stringVal.has_value()) {
            return ConfigValue(stringVal.value());
        }
        
        auto intVal = config.get<int>(fieldName);
        if (intVal.has_value()) {
            return ConfigValue(intVal.value());
        }
        
        auto doubleVal = config.get<double>(fieldName);
        if (doubleVal.has_value()) {
            return ConfigValue(doubleVal.value());
        }
        
        auto boolVal = config.get<bool>(fieldName);
        if (boolVal.has_value()) {
            return ConfigValue(boolVal.value());
        }
        
        return std::nullopt;
    }
    
    // Validate range
    void validateRange(const ConfigValue& value, const ValidationRule& rule,
                       std::vector<ValidationError>& errors, bool isStrict) const {
        if (!rule.params.has_value()) return;
        
        try {
            auto params = std::any_cast<ConfigSchema::RangeParams>(rule.params);
            double val = std::visit([](auto&& arg) -> double {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) return static_cast<double>(arg);
                else if constexpr (std::is_same_v<T, int64_t>) return static_cast<double>(arg);
                else if constexpr (std::is_same_v<T, double>) return arg;
                else if constexpr (std::is_same_v<T, bool>) return arg ? 1.0 : 0.0;
                else if constexpr (std::is_same_v<T, std::string>) {
                    try {
                        return std::stod(arg);
                    } catch (...) { return std::numeric_limits<double>::quiet_NaN(); }
                }
                else return std::numeric_limits<double>::quiet_NaN();
            }, value);
            
            if (val < params.min || val > params.max) {
                errors.emplace_back(rule.fieldName, 
                    "Value " + std::to_string(val) + " is outside valid range [" + 
                    std::to_string(params.min) + ", " + std::to_string(params.max) + "]",
                    rule.severity);
            }
        } catch (...) {
            errors.emplace_back(rule.fieldName, "Range validation failed for field", ValidationSeverity::Error);
        }
    }
    
    // Validate enum
    void validateEnum(const ConfigValue& value, const ValidationRule& rule,
                      std::vector<ValidationError>& errors, bool isStrict) const {
        if (!rule.params.has_value()) return;
        
        try {
            auto params = std::any_cast<ConfigSchema::EnumParams>(rule.params);
            std::string strValue;
            
            // Convert value to string for comparison
            strValue = std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) return arg;
                else if constexpr (std::is_same_v<T, bool>) return arg ? "true" : "false";
                else return std::to_string(arg);
            }, value);
            
            bool isValid = false;
            for (const auto& allowed : params.allowedValues) {
                if (strValue == allowed) {
                    isValid = true;
                    break;
                }
            }
            
            if (!isValid) {
                std::string allowedList = "";
                for (size_t i = 0; i < params.allowedValues.size(); ++i) {
                    if (i > 0) allowedList += ", ";
                    allowedList += "\"" + params.allowedValues[i] + "\"";
                }
                errors.emplace_back(rule.fieldName,
                    "Value '" + strValue + "' is not in allowed values: " + allowedList,
                    rule.severity);
            }
        } catch (...) {
            errors.emplace_back(rule.fieldName, "Enum validation failed for field", ValidationSeverity::Error);
        }
    }
    
    // Validate format (simple regex implementation)
    void validateFormat(const ConfigValue& value, const ValidationRule& rule,
                        std::vector<ValidationError>& errors, bool isStrict) const {
        if (!rule.params.has_value()) return;
        
        try {
            auto params = std::any_cast<ConfigSchema::FormatParams>(rule.params);
            std::string strValue;
            
            // Convert value to string
            strValue = std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) return arg;
                else if constexpr (std::is_same_v<T, bool>) return arg ? "true" : "false";
                else return std::to_string(arg);
            }, value);
            
            // Simple format validation - check for pattern
            // This is a simplified implementation - real version would use proper regex
            bool isValid = true;
            if (params.pattern.find('*') != std::string::npos) {
                // Wildcard pattern - simple matching
                std::string pattern = params.pattern;
                std::replace(pattern.begin(), pattern.end(), '*', ' ');
                isValid = (strValue.find(pattern) != std::string::npos);
            } else {
                // Exact match or simple pattern
                isValid = (strValue == params.pattern);
            }
            
            if (!isValid) {
                errors.emplace_back(rule.fieldName,
                    "Value '" + strValue + "' does not match format: " + params.pattern,
                    rule.severity);
            }
        } catch (...) {
            errors.emplace_back(rule.fieldName, "Format validation failed for field", ValidationSeverity::Error);
        }
    }
    
    // Validate dependency
    void validateDependency(const Config& config, const ConfigValue& value,
                           const ValidationRule& rule, std::vector<ValidationError>& errors,
                           bool isStrict) const {
        if (!rule.params.has_value()) return;
        
        try {
            auto params = std::any_cast<ConfigSchema::DependencyParams>(rule.params);
            
            bool dependsOnPresent = config.has(params.dependsOn);
            
            if (params.requiredWhenPresent && dependsOnPresent) {
                // Field is required when dependency is present
                if (!config.has(rule.fieldName)) {
                    errors.emplace_back(rule.fieldName,
                        "Field is required when '" + params.dependsOn + "' is present",
                        rule.severity);
                }
            } else if (!params.requiredWhenPresent && !dependsOnPresent) {
                // Field is not required when dependency is absent
                if (config.has(rule.fieldName)) {
                    errors.emplace_back(rule.fieldName,
                        "Field is not allowed when '" + params.dependsOn + "' is absent",
                        rule.severity);
                }
            }
        } catch (...) {
            errors.emplace_back(rule.fieldName, "Dependency validation failed for field", ValidationSeverity::Error);
        }
    }
    
    // Validate custom rule
    void validateCustom(const ConfigValue& value, const ValidationRule& rule,
                        std::vector<ValidationError>& errors, bool isStrict) const {
        if (!rule.customValidator) {
            errors.emplace_back(rule.fieldName, "Custom validator not set", ValidationSeverity::Error);
            return;
        }
        
        try {
            errors.emplace_back(rule.fieldName, "Custom validation requires full config context", ValidationSeverity::Error);
        } catch (...) {
            errors.emplace_back(rule.fieldName, "Custom validation failed for field", ValidationSeverity::Error);
        }
    }
    
    // Validation cache for performance
    struct CacheKey {
        std::hash<std::string> hasher;
        size_t operator()(const Config& config) const {
            // Simplified hash - real implementation would be more sophisticated
            return hasher("config_cache");
        }
    };
    
    std::unordered_map<size_t, std::vector<ValidationError>> validationCache;
};

} // namespace nlm
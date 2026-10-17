#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <functional>
#include <regex>
#include <chrono>
#include <memory>
#include <set>
#include <algorithm>

namespace nlm {

// Validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata;
    
    ValidationResult() : valid(true) {}
    
    bool isValid() const { return valid; }
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getErrorSummary() const {
        if (errors.empty()) return "";
        return errors[0] + (errors.size() > 1 ? " (and " + std::to_string(errors.size() - 1) + " more)" : "");
    }
};

// Validation constraint type
using ValidationConstraint = std::function<bool(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&, std::string& errorMessage)>;

// Range validation parameters
struct RangeValidation {
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    bool includeMin = true;
    bool includeMax = true;
    std::string errorMessage = "Value out of range";
};

// Pattern validation parameters
struct PatternValidation {
    std::regex pattern;
    std::string errorMessage = "Pattern validation failed";
};

// Type validation parameters
struct TypeValidation {
    std::string expectedType;
    std::string errorMessage = "Type validation failed";
};

// Constraint validation parameters
struct ConstraintValidation {
    std::function<bool(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&)> constraint;
    std::string errorMessage = "Constraint validation failed";
};

// Format validation parameters
struct FormatValidation {
    std::string format;
    std::string errorMessage = "Format validation failed";
    std::map<std::string, std::string> formatParameters;
};

// Dependency validation parameters
struct DependencyValidation {
    std::string requiredKey;
    std::function<bool(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&,
                      const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&)> validator;
    std::string errorMessage = "Dependency validation failed";
};

// Cross-field validation
struct CrossFieldValidation {
    std::vector<std::string> fieldNames;
    std::function<bool(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&, std::string& errorMessage)> validator;
    std::string errorMessage = "Cross-field validation failed";
};

// Validation rule
struct ValidationRule {
    std::string key;
    ValidationConstraint customConstraint;
    RangeValidation rangeValidation;
    PatternValidation patternValidation;
    TypeValidation typeValidation;
    ConstraintValidation constraintValidation;
    FormatValidation formatValidation;
    DependencyValidation dependencyValidation;
    std::vector<CrossFieldValidation> crossFieldValidations;
    bool enabled = true;
    std::string description;
    
    ValidationRule() = default;
    
    ValidationRule(const std::string& key, const std::string& desc = "") 
        : key(key), description(desc) {}
    
    void addRangeValidation(double min, double max, bool includeMin = true, bool includeMax = true, 
                           const std::string& errorMessage = "Value out of range") {
        rangeValidation.min = min;
        rangeValidation.max = max;
        rangeValidation.includeMin = includeMin;
        rangeValidation.includeMax = includeMax;
        rangeValidation.errorMessage = errorMessage;
    }
    
    void addPatternValidation(const std::string& pattern, const std::string& errorMessage = "Pattern validation failed") {
        patternValidation.pattern = std::regex(pattern);
        patternValidation.errorMessage = errorMessage;
    }
    
    void addTypeValidation(const std::string& typeName, const std::string& errorMessage = "Type validation failed") {
        typeValidation.expectedType = typeName;
        typeValidation.errorMessage = errorMessage;
    }
    
    void addConstraint(const std::function<bool(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&)>& constraint,
                      const std::string& errorMessage = "Constraint validation failed") {
        constraintValidation.constraint = constraint;
        constraintValidation.errorMessage = errorMessage;
    }
    
    void addFormatValidation(const std::string& format, const std::map<std::string, std::string>& params = {},
                            const std::string& errorMessage = "Format validation failed") {
        formatValidation.format = format;
        formatValidation.formatParameters = params;
        formatValidation.errorMessage = errorMessage;
    }
    
    void addDependencyValidation(const std::string& requiredKey,
                                const std::function<bool(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&,
                                                       const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&)>& validator,
                                const std::string& errorMessage = "Dependency validation failed") {
        dependencyValidation.requiredKey = requiredKey;
        dependencyValidation.validator = validator;
        dependencyValidation.errorMessage = errorMessage;
    }
    
    void addCrossFieldValidation(const std::vector<std::string>& fieldNames,
                                const std::function<bool(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&, std::string& errorMessage)>& validator,
                                const std::string& errorMessage = "Cross-field validation failed") {
        CrossFieldValidation cfv;
        cfv.fieldNames = fieldNames;
        cfv.validator = validator;
        cfv.errorMessage = errorMessage;
        crossFieldValidations.push_back(cfv);
    }
};

// Validation engine interface
class IValidationEngine {
public:
    virtual ~IValidationEngine() = default;
    
    // Add validation rule
    virtual void addValidationRule(const ValidationRule& rule) = 0;
    
    // Remove validation rule
    virtual void removeValidationRule(const std::string& key) = 0;
    
    // Validate a single value
    virtual ValidationResult validate(const std::string& key, 
                                     const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value) = 0;
    
    // Validate multiple values
    virtual ValidationResult validateAll(const std::map<std::string, 
                                 std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>& values) = 0;
    
    // Get validation errors for a key
    virtual std::vector<std::string> getErrors(const std::string& key) const = 0;
    
    // Get validation warnings for a key
    virtual std::vector<std::string> getWarnings(const std::string& key) const = 0;
    
    // Clear all validation rules
    virtual void clear() noexcept = 0;
    
    // Get all validation rules
    virtual std::vector<ValidationRule> getValidationRules() const = 0;
    
    // Enable/disable validation rule
    virtual void setRuleEnabled(const std::string& key, bool enabled) = 0;
    
    // Check if validation rule is enabled
    virtual bool isRuleEnabled(const std::string& key) const = 0;
};

// Input validation system
class ValidationEngine {
public:
    ValidationEngine();
    ~ValidationEngine();
    
    // Disable copying, enable moving
    ValidationEngine(const ValidationEngine&) = delete;
    ValidationEngine& operator=(const ValidationEngine&) = delete;
    ValidationEngine(ValidationEngine&&) noexcept;
    ValidationEngine& operator=(ValidationEngine&&) noexcept;
    
    // Get validation engine instance
    static ValidationEngine& getInstance();
    
    // Add validation rule
    void addValidationRule(const ValidationRule& rule);
    
    // Remove validation rule
    void removeValidationRule(const std::string& key);
    
    // Validate a single value
    ValidationResult validate(const std::string& key, 
                             const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value);
    
    // Validate multiple values
    ValidationResult validateAll(const std::map<std::string, 
                           std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>& values);
    
    // Get validation errors for a key
    std::vector<std::string> getErrors(const std::string& key) const;
    
    // Get validation warnings for a key
    std::vector<std::string> getWarnings(const std::string& key) const;
    
    // Clear all validation rules
    void clear() noexcept;
    
    // Get all validation rules
    std::vector<ValidationRule> getValidationRules() const;
    
    // Enable/disable validation rule
    void setRuleEnabled(const std::string& key, bool enabled);
    
    // Check if validation rule is enabled
    bool isRuleEnabled(const std::string& key) const;
    
    // Validate type
    static bool validateType(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                            const std::string& expectedType, std::string& errorMessage);
    
    // Validate range
    static bool validateRange(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                             double min, double max, bool includeMin = true, bool includeMax = true, std::string& errorMessage = "Value out of range");
    
    // Validate pattern
    static bool validatePattern(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                               const std::regex& pattern, std::string& errorMessage = "Pattern validation failed");
    
    // Validate constraint
    static bool validateConstraint(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                                   const std::function<bool(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&)>& constraint,
                                   std::string& errorMessage = "Constraint validation failed");
    
    // Validate format
    static bool validateFormat(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                              const std::string& format, const std::map<std::string, std::string>& formatParameters = {},
                              std::string& errorMessage = "Format validation failed");
    
    // Validate format string (static helper)
    static std::string formatValue(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                                  const std::string& format, const std::map<std::string, std::string>& formatParameters = {});
    
    // Validate dependency
    static bool validateDependency(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& dependentValue,
                                   const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& requiredValue,
                                   const std::function<bool(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&,
                                                         const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&)>& validator,
                                   std::string& errorMessage = "Dependency validation failed");
    
    // Validate cross-field
    static bool validateCrossField(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>& values,
                                  const std::vector<std::string>& fieldNames,
                                  const std::function<bool(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>&, std::string& errorMessage)>& validator,
                                  std::string& errorMessage = "Cross-field validation failed");
    
    // Validate email
    static bool validateEmail(const std::string& value, std::string& errorMessage = "Invalid email format");
    
    // Validate URL
    static bool validateURL(const std::string& value, std::string& errorMessage = "Invalid URL format");
    
    // Validate phone number
    static bool validatePhoneNumber(const std::string& value, std::string& errorMessage = "Invalid phone number format");
    
    // Validate date
    static bool validateDate(const std::string& value, const std::string& format = "YYYY-MM-DD", std::string& errorMessage = "Invalid date format");
    
    // Validate numeric range
    static bool validateNumericRange(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                                    double min, double max, std::string& errorMessage = "Value out of range");
    
    // Validate string length
    static bool validateStringLength(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                                    size_t minLength, size_t maxLength, std::string& errorMessage = "String length out of range");
    
    // Validate vector size
    static bool validateVectorSize(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                                  size_t minSize, size_t maxSize, std::string& errorMessage = "Vector size out of range");
    
    // Validate vector element type
    static bool validateVectorElementType(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value,
                                         const std::string& elementType, std::string& errorMessage = "Vector element type mismatch");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods
    std::string variantToString(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value) const;
    std::string getTypeName(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value) const;
    bool isNumericType(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value) const;
    bool isStringType(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value) const;
    bool isVectorType(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value) const;
    bool isBooleanType(const std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& value) const;
};
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <variant>
#include <regex>
#include <chrono>
#include <mutex>
#include <algorithm>
#include <map>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declarations
class Validator;
class SchemaValidator;
class Config;

// Validation Error System
class ValidationError : public std::runtime_error {
public:
    enum class ErrorSeverity {
        Warning,
        Error,
        Critical
    };
    
    enum class ErrorCategory {
        TypeError,
        ConstraintError,
        SchemaError,
        FormatError,
        DependencyError,
        CustomError
    };
    
    ValidationError(const std::string& message, 
                    ErrorSeverity severity = ErrorSeverity::Error,
                    ErrorCategory category = ErrorCategory::CustomError,
                    const std::string& fieldPath = "",
                    const std::string& suggestion = "")
        : std::runtime_error(message), 
          severity(severity), 
          category(category),
          fieldPath(fieldPath),
          suggestion(suggestion) {}
    
    ErrorSeverity getSeverity() const { return severity; }
    ErrorCategory getCategory() const { return category; }
    const std::string& getFieldPath() const { return fieldPath; }
    const std::string& getSuggestion() const { return suggestion; }
    
    std::string getFullErrorMessage() const {
        std::string msg = std::runtime_error::what();
        if (!fieldPath.empty()) msg += " at ";
        msg += fieldPath;
        if (!suggestion.empty()) msg += " - Suggestion: " + suggestion;
        return msg;
    }
    
private:
    ErrorSeverity severity;
    ErrorCategory category;
    std::string fieldPath;
    std::string suggestion;
};

// Validation Result - collects all errors at once
class ValidationResult {
public:
    ValidationResult() : valid(true), partialValidation(false), failFast(false) {}
    
    ValidationResult& addError(const ValidationError& error) {
        errors.push_back(error);
        valid = false;
        if (failFast) {
            stopProcessing();
        }
        return *this;
    }
    
    ValidationResult& addError(const std::string& message, 
                              ValidationError::ErrorSeverity severity = ValidationError::ErrorSeverity::Error,
                              ValidationError::ErrorCategory category = ValidationError::ErrorCategory::CustomError,
                              const std::string& fieldPath = "",
                              const std::string& suggestion = "") {
        return addError(ValidationError(message, severity, category, fieldPath, suggestion));
    }
    
    ValidationResult& setPartialValidation(bool partial) {
        partialValidation = partial;
        return *this;
    }
    
    ValidationResult& setFailFast(bool fail) {
        failFast = fail;
        return *this;
    }
    
    ValidationResult& setStopProcessing(bool stop) {
        stopProcessing = stop;
        return *this;
    }
    
    bool isValid() const { return valid && !stopProcessing; }
    bool isPartial() const { return partialValidation; }
    bool shouldFailFast() const { return failFast; }
    bool shouldStopProcessing() const { return stopProcessing; }
    size_t errorCount() const { return errors.size(); }
    const std::vector<ValidationError>& getErrors() const { return errors; }
    
    std::string getSummary() const {
        std::string result = "Validation " + (isValid() ? "PASSED" : "FAILED");
        if (errors.size() > 0) {
            result += " (" + std::to_string(errors.size()) + " errors)";
        }
        return result;
    }
    
    std::string getDetailedReport() const {
        std::string report = getSummary() + "\n";
        for (const auto& error : errors) {
            report += "  [" + errorSeverityToString(error.getSeverity()) + "] " + error.getFullErrorMessage() + "\n";
        }
        return report;
    }
    
    static std::string errorSeverityToString(ValidationError::ErrorSeverity severity) {
        switch (severity) {
            case ValidationError::ErrorSeverity::Warning: return "WARNING";
            case ValidationError::ErrorSeverity::Error: return "ERROR";
            case ValidationError::ErrorSeverity::Critical: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
private:
    bool valid;
    bool partialValidation;
    bool failFast;
    bool stopProcessing;
    std::vector<ValidationError> errors;
};

// Base validator interface
class Validator {
public:
    virtual ~Validator() = default;
    virtual std::string getName() const = 0;
    virtual ValidationResult validate(const ConfigValue& value, const std::string& fieldPath = "") = 0;
    virtual bool isActive() const { return true; }
    virtual void setActive(bool active) {}
};

// Type validation utilities
class TypeValidator {
public:
    static bool isInt(const ConfigValue& value) {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    }
    
    static bool isDouble(const ConfigValue& value) {
        return std::holds_alternative<double>(value);
    }
    
    static bool isString(const ConfigValue& value) {
        return std::holds_alternative<std::string>(value);
    }
    
    static bool isBool(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }
    
    static bool isArray(const ConfigValue& value) {
        return std::holds_alternative<std::vector<int>>(value) ||
               std::holds_alternative<std::vector<double>>(value) ||
               std::holds_alternative<std::vector<std::string>>(value);
    }
    
    static bool isNested(const ConfigValue& value) {
        return std::holds_alternative<std::map<std::string, nlohmann::json>>(value);
    }
    
    static std::string getTypeName(const ConfigValue& value);
    
    // Type coercion
    static ConfigValue coerceToType(const ConfigValue& value, const std::type_info& targetType);
    
    // Nested extraction
    static std::optional<ConfigValue> getNestedValue(const ConfigValue& nestedConfig, const std::string& path);
    static void setNestedValue(ConfigValue& nestedConfig, const std::string& path, const ConfigValue& value);
};

// Advanced type validation with coercion
class AdvancedTypeValidator : public Validator {
public:
    AdvancedTypeValidator(const std::string& typeName) : name(typeName) {}
    
    std::string getName() const override { return name; }
    ValidationResult validate(const ConfigValue& value, const std::string& fieldPath = "") override {
        ValidationResult result;
        result.setFailFast(true);
        
        if (!TypeValidator::isInt(value) && !TypeValidator::isDouble(value) && !TypeValidator::isString(value) && 
            !TypeValidator::isBool(value) && !TypeValidator::isArray(value) && !TypeValidator::isNested(value)) {
            result.addError("Invalid type for field", 
                          ValidationError::ErrorSeverity::Error,
                          ValidationError::ErrorCategory::TypeError,
                          fieldPath + ".type");
        }
        
        return result;
    }
    
private:
    std::string name;
};

// Constraint validation system
class ConstraintValidator : public Validator {
public:
    ConstraintValidator(const std::string& name) : name(name) {}
    
    std::string getName() const override { return name; }
    
    ValidationResult validateRange(const ConfigValue& value, const std::string& fieldPath = "",
                                  double min = std::numeric_limits<double>::lowest(),
                                  double max = std::numeric_limits<double>::max(),
                                  bool minInclusive = true, bool maxInclusive = true);
    
    ValidationResult validateStringLength(const ConfigValue& value, const std::string& fieldPath = "",
                                         size_t minLength = 0, size_t maxLength = std::string::max_size());
    
    ValidationResult validatePattern(const ConfigValue& value, const std::string& fieldPath = "",
                                    const std::string& pattern);
    
    ValidationResult validateArrayConstraints(const ConfigValue& value, const std::string& fieldPath = "",
                                             size_t minSize = 0, size_t maxSize = std::string::max_size(),
                                             bool uniqueElements = false);
    
    ValidationResult validateEnum(const ConfigValue& value, const std::string& fieldPath = "",
                                 const std::vector<std::string>& allowedValues);
    
    ValidationResult validateFormat(const ConfigValue& value, const std::string& fieldPath = "",
                                   const std::string& format);
    
    ValidationResult validate(const ConfigValue& value, const std::string& fieldPath = "") override {
        ValidationResult result;
        // Apply all constraint validations
        return result;
    }
    
private:
    std::string name;
};

// Schema-based validation
class SchemaValidator : public Validator {
public:
    SchemaValidator(const std::string& name) : name(name) {}
    
    std::string getName() const override { return name; }
    
    // JSON Schema validation
    ValidationResult validateJSONSchema(const ConfigValue& value, const std::string& fieldPath = "",
                                       const nlohmann::json& schema);
    
    // Custom validator registration
    ValidationResult registerCustomValidator(const std::string& fieldPath, 
                                            std::function<bool(const ConfigValue&)> validator,
                                            const std::string& errorMessage = "Validation failed");
    
    // Conditional validation (if-then rules)
    ValidationResult addConditionalValidator(const std::string& conditionField,
                                            const std::string& conditionValue,
                                            const std::string& targetField,
                                            std::function<bool(const ConfigValue&)> validator);
    
    // Cross-field validation
    ValidationResult addCrossFieldValidator(const std::string& dependentField,
                                           const std::string& referencedField,
                                           std::function<bool(const ConfigValue&, const ConfigValue&)> validator);
    
    ValidationResult validate(const ConfigValue& value, const std::string& fieldPath = "") override;
    
private:
    std::string name;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> customValidators;
    std::vector<std::tuple<std::string, std::string, std::function<bool(const ConfigValue&)>>> conditionalValidators;
    std::vector<std::tuple<std::string, std::string, std::function<bool(const ConfigValue&, const ConfigValue&)>>> crossFieldValidators;
};

// Dynamic validation system
class DynamicValidator : public Validator {
public:
    DynamicValidator(const std::string& name) : name(name) {}
    
    std::string getName() const override { return name; }
    
    // Runtime validator registration
    ValidationResult registerValidator(const std::string& fieldPath, 
                                       std::function<ValidationResult(const ConfigValue&)> validator,
                                       ValidationError::ErrorSeverity severity = ValidationError::ErrorSeverity::Error);
    
    // Context-aware validation
    ValidationResult validateWithContext(const ConfigValue& value, const std::string& fieldPath = "",
                                        const std::unordered_map<std::string, ConfigValue>& context = {});
    
    // Validation rule inheritance
    ValidationResult addInheritedValidator(const std::string& parentField,
                                          const std::string& childField,
                                          std::function<bool(const ConfigValue&, const ConfigValue&)> validator);
    
    ValidationResult validate(const ConfigValue& value, const std::string& fieldPath = "") override;
    
private:
    std::string name;
    std::vector<std::tuple<std::string, std::function<ValidationResult(const ConfigValue&)>, ValidationError::ErrorSeverity>> runtimeValidators;
    std::vector<std::tuple<std::string, std::string, std::function<bool(const ConfigValue&, const ConfigValue&)>>> inheritedValidators;
};

// Neural Network Configuration Validator
class NeuralNetworkValidator : public Validator {
public:
    NeuralNetworkValidator(const std::string& name) : name(name) {}
    
    std::string getName() const override { return name; }
    
    ValidationResult validate(const ConfigValue& value, const std::string& fieldPath = "") override;
    
    ValidationResult validateArchitecture(const ConfigValue& value, const std::string& fieldPath = "");
    ValidationResult validateTrainingParams(const ConfigValue& value, const std::string& fieldPath = "");
    ValidationResult validateInferenceParams(const ConfigValue& value, const std::string& fieldPath = "");
    
private:
    std::string name;
};

// Main Validation Manager - orchestrates all validators
class ValidationManager {
public:
    ValidationManager() : threadSafe(true), enabled(true) {}
    
    // Validator management
    ValidationResult registerValidator(const std::string& fieldPath, 
                                       std::unique_ptr<Validator> validator,
                                       bool override = false);
    
    ValidationResult unregisterValidator(const std::string& fieldPath, const std::string& validatorName);
    
    std::vector<std::string> getRegisteredFields() const;
    std::vector<std::string> getValidatorsForField(const std::string& fieldPath) const;
    
    // Validation execution
    ValidationResult validateField(const ConfigValue& value, const std::string& fieldPath = "",
                                  bool continueOnError = false);
    
    ValidationResult validateAll(const std::unordered_map<std::string, ConfigValue>& config,
                                bool continueOnError = false);
    
    // Configuration validation
    ValidationResult validateConfiguration(const Config& config, 
                                           bool continueOnError = false,
                                           const std::unordered_map<std::string, ConfigValue>& context = {});
    
    // Schema validation
    ValidationResult validateWithSchema(const std::unordered_map<std::string, ConfigValue>& config,
                                       const nlohmann::json& schema);
    
    // Dynamic updates
    ValidationResult addDynamicValidator(const std::string& fieldPath,
                                         std::function<ValidationResult(const ConfigValue&)> validator);
    
    ValidationResult removeDynamicValidator(const std::string& fieldPath, const std::string& validatorName);
    
    // Performance monitoring
    struct ValidationMetrics {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        size_t totalFieldsValidated;
        size_t totalErrors;
        size_t totalWarnings;
        size_t totalCriticalErrors;
        std::unordered_map<std::string, size_t> errorsByField;
        
        double getDurationMs() const {
            return std::chrono::duration<double, std::milli>(endTime - startTime).count();
        }
        
        std::string getSummary() const {
            std::string result = "Validation Metrics:\n";
            result += "  Duration: " + std::to_string(getDurationMs()) + " ms\n";
            result += "  Fields validated: " + std::to_string(totalFieldsValidated) + "\n";
            result += "  Errors: " + std::to_string(totalErrors) + "\n";
            result += "  Warnings: " + std::to_string(totalWarnings) + "\n";
            result += "  Critical: " + std::to_string(totalCriticalErrors) + "\n";
            return result;
        }
    };
    
    void resetMetrics() {
        metrics.startTime = std::chrono::steady_clock::now();
        metrics.totalFieldsValidated = 0;
        metrics.totalErrors = 0;
        metrics.totalWarnings = 0;
        metrics.totalCriticalErrors = 0;
        metrics.errorsByField.clear();
    }
    
    ValidationMetrics getMetrics() const { return metrics; }
    
    // Configuration management
    void setThreadSafe(bool threadSafe) { this->threadSafe = threadSafe; }
    void setEnabled(bool enabled) { this->enabled = enabled; }
    
    bool isThreadSafe() const { return threadSafe; }
    bool isEnabled() const { return enabled; }
    
private:
    bool threadSafe;
    bool enabled;
    ValidationMetrics metrics;
    
    // Field-specific validators
    std::unordered_map<std::string, std::vector<std::unique_ptr<Validator>>> fieldValidators;
    
    // Dynamic validators
    std::unordered_map<std::string, std::vector<std::tuple<std::string, std::function<ValidationResult(const ConfigValue&)>, ValidationError::ErrorSeverity>>> dynamicValidators;
    
    mutable std::mutex mutex;
    
    ValidationResult validateWithMutex(const ConfigValue& value, const std::string& fieldPath,
                                       bool continueOnError);
};

} // namespace nlm
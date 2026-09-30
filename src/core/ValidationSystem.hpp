#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>
#include <functional>
#include <optional>
#include <format>
#include "ErrorHandling.hpp"

namespace nlm {

// Validation error codes
enum class ValidationErrorCode {
    NoError = 0,
    InvalidType,
    OutOfRange,
    MissingRequired,
    InvalidFormat,
    ConstraintViolated,
    DependencyError,
    CircularReference,
    InvalidCombination
};

// Parameter type definitions
using ParameterValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>
>;

// Parameter metadata for validation
struct ParameterInfo {
    std::string name;
    std::string description;
    ParameterValue defaultValue;
    bool required;
    std::string type;
    
    // Validation constraints
    struct RangeConstraint {
        double min;
        double max;
    };
    
    struct ChoiceConstraint {
        std::vector<ParameterValue> allowedValues;
    };
    
    struct DependencyConstraint {
        std::string dependentParameter;
        std::function<bool(const ParameterValue&, const ParameterValue&)> condition;
    };
    
    struct CustomConstraint {
        std::function<bool(const ParameterValue&)> validator;
        std::string errorMessage;
    };
    
    RangeConstraint range;
    ChoiceConstraint choice;
    DependencyConstraint dependency;
    CustomConstraint custom;
    
    ParameterInfo() : required(false) {}
    ParameterInfo(const std::string& name, const std::string& description, 
                  const ParameterValue& defaultVal, bool req = false)
        : name(name), description(description), defaultValue(defaultVal), required(req) {}
};

// Validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getSummary() const {
        std::stringstream ss;
        if (valid) {
            ss << "Validation passed";
            if (!warnings.empty()) {
                ss << " (with " << warnings.size() << " warnings)";
            }
            return ss.str();
        }
        ss << "Validation failed with " << errors.size() << " error(s): ";
        for (size_t i = 0; i < errors.size() && i < 3; ++i) {
            if (i > 0) ss << ", ";
            ss << errors[i];
        }
        if (errors.size() > 3) {
            ss << " ... and " << (errors.size() - 3) << " more";
        }
        if (!warnings.empty()) {
            ss << ". Also " << warnings.size() << " warning(s): ";
            for (size_t i = 0; i < warnings.size() && i < 2; ++i) {
                if (i > 0) ss << ", ";
                ss << warnings[i];
            }
        }
        return ss.str();
    }
};

// Validation schema for configuration
class ValidationSchema {
public:
    ValidationSchema() = default;
    ~ValidationSchema() = default;
    
    // Disable copying, enable moving
    ValidationSchema(const ValidationSchema&) = delete;
    ValidationSchema& operator=(const ValidationSchema&) = delete;
    ValidationSchema(ValidationSchema&&) noexcept = default;
    ValidationSchema& operator=(ValidationSchema&&) noexcept = default;
    
    // Add parameter to schema
    void addParameter(const std::string& name, const ParameterInfo& info);
    
    // Add parameter with validation constraints
    template<typename T>
    void addParameter(const std::string& name, const std::string& description, 
                     const T& defaultValue, bool required = false);
    
    // Add range constraint
    void addRangeConstraint(const std::string& paramName, double min, double max);
    
    // Add choice constraint
    void addChoiceConstraint(const std::string& paramName, 
                           const std::vector<ParameterValue>& allowedValues);
    
    // Add dependency constraint
    void addDependencyConstraint(const std::string& paramName, 
                                const std::string& dependentParam,
                                std::function<bool(const ParameterValue&, const ParameterValue&)> condition);
    
    // Add custom constraint
    void addCustomConstraint(const std::string& paramName,
                            std::function<bool(const ParameterValue&)> validator,
                            const std::string& errorMessage);
    
    // Validate parameter value
    ValidationResult validateParameter(const std::string& name, const ParameterValue& value) const;
    
    // Validate complete parameter set
    ValidationResult validate(const std::unordered_map<std::string, ParameterValue>& parameters) const;
    
    // Get parameter info
    const ParameterInfo* getParameterInfo(const std::string& name) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Get required parameter names
    std::vector<std::string> getRequiredParameters() const;
    
    // Check if all required parameters are present
    ValidationResult checkRequiredParameters(const std::unordered_map<std::string, ParameterValue>& parameters) const;
    
    // Merge two schemas (child overrides parent)
    void merge(const ValidationSchema& other);
    
    // Get schema summary
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Parameter types helper functions
namespace ParameterTypes {
    template<typename T>
    std::string getTypeName();
    
    ParameterValue convertToParameterValue(const std::string& value, const std::string& type);
    std::string parameterValueToString(const ParameterValue& value);
    
    // Type checking
    template<typename T>
    bool isType(const ParameterValue& value);
    
    // Range checking
    bool inRange(const ParameterValue& value, double min, double max);
    
    // Choice checking
    bool isInChoices(const ParameterValue& value, const std::vector<ParameterValue>& choices);
}

// Configuration validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    
    virtual ValidationResult validate(const std::string& configPath) = 0;
    virtual ValidationResult validate(const std::unordered_map<std::string, ParameterValue>& parameters) = 0;
    
    // Set validation schema
    virtual void setSchema(const ValidationSchema& schema) = 0;
    
    // Get current schema
    virtual const ValidationSchema& getSchema() const = 0;
    
    // Add custom validator
    virtual void addCustomValidator(std::function<ValidationResult(const std::unordered_map<std::string, ParameterValue>&)> validator) = 0;
};

// Default configuration validator implementation
class DefaultConfigValidator : public ConfigValidator {
public:
    DefaultConfigValidator() = default;
    ~DefaultConfigValidator() override = default;
    
    ValidationResult validate(const std::string& configPath) override;
    ValidationResult validate(const std::unordered_map<std::string, ParameterValue>& parameters) override;
    
    void setSchema(const ValidationSchema& schema) override;
    const ValidationSchema& getSchema() const override;
    
    void addCustomValidator(std::function<ValidationResult(const std::unordered_map<std::string, ParameterValue>&)> validator) override;
    
    // Load schema from file (JSON/YAML format)
    bool loadSchemaFromFile(const std::string& filepath);
    
    // Save schema to file
    bool saveSchemaToFile(const std::string& filepath) const;
    
    // Get validation statistics
    struct ValidationStats {
        size_t totalValidations;
        size_t successfulValidations;
        size_t failedValidations;
        std::unordered_map<ValidationErrorCode, size_t> errorCounts;
    };
    
    ValidationStats getValidationStats() const;
    
    // Reset statistics
    void resetStats();
    
private:
    ValidationSchema schema;
    std::vector<std::function<ValidationResult(const std::unordered_map<std::string, ParameterValue>&)>> customValidators;
    mutable ValidationStats stats;
};

// Validation utilities
namespace ValidationUtils {
    // Convert value to string for error messages
    template<typename T>
    std::string toString(const T& value);
    
    // Generate error messages
    std::string generateRangeError(const std::string& paramName, double expectedMin, double expectedMax, double actual);
    std::string generateChoiceError(const std::string& paramName, const std::string& actual, const std::string& expectedChoices);
    std::string generateDependencyError(const std::string& paramName, const std::string& dependentParam, const std::string& reason);
    std::string generateCustomError(const std::string& paramName, const std::string& errorMessage);
    
    // Parameter type conversion
    std::string parameterValueToString(const ParameterValue& value);
    
    // File I/O utilities
    bool saveSchemaToJSON(const ValidationSchema& schema, const std::string& filepath);
    bool loadSchemaFromJSON(ValidationSchema& schema, const std::string& filepath);
    
    // Logging utilities
    void logValidationResult(const ValidationResult& result, const std::string& context = "");
}

// Global validation system
class ValidationSystem {
public:
    ValidationSystem() = default;
    ~ValidationSystem() = default;
    
    // Get global validator instance
    static ConfigValidator& getValidator();
    
    // Initialize with default schema
    static void initializeWithDefaults();
    
    // Validate configuration
    static ValidationResult validateConfig(const std::string& configPath);
    static ValidationResult validateParameters(const std::unordered_map<std::string, ParameterValue>& parameters);
    
    // Set global validation schema
    static void setGlobalSchema(const ValidationSchema& schema);
    
    // Get global validation schema
    static const ValidationSchema& getGlobalSchema();
    
private:
    static ConfigValidator* globalValidator;
};

// Convenient validation macros
#define NLM_VALIDATE_PARAM(param_name, value, schema) \
    do { \
        auto result = ValidationSystem::getValidator().getSchema().validateParameter(param_name, value); \
        if (!result.valid) { \
            getGlobalErrorReporter().warn(std::format("Parameter validation failed for '{}': {}", param_name, result.getSummary())); \
        } \
    } while(0)

#define NLM_VALIDATE_CONFIG(config) \
    do { \
        auto result = ValidationSystem::validateParameters(config); \
        if (!result.valid) { \
            getGlobalErrorReporter().warn(std::format("Configuration validation failed: {}", result.getSummary())); \
        } \
    } while(0)

} // namespace nlm

// Implementation details
#include "ValidationSystem.cpp" // Include implementation

// Template implementations
namespace nlm {
    template<typename T>
    void ValidationSchema::addParameter(const std::string& name, const std::string& description, 
                                        const T& defaultValue, bool required) {
        ParameterInfo info(name, description, ParameterValue(defaultValue), required);
        addParameter(name, info);
    }
    
    template<typename T>
    std::string ParameterTypes::getTypeName() {
        return typeid(T).name();
    }
    
    template<typename T>
    bool ParameterTypes::isType(const ParameterValue& value) {
        return std::holds_alternative<T>(value);
    }
    
    template<typename T>
    std::string ValidationUtils::toString(const T& value) {
        return std::to_string(value);
    }
    
    std::string ValidationUtils::toString(const std::string& value) {
        return value;
    }
    
    std::string ValidationUtils::toString(bool value) {
        return value ? "true" : "false";
    }
} // namespace nlm
// IConfigValidator.hpp
// Abstract interface for configuration validation
// Validates configuration parameters and settings
// Ensures configuration correctness and consistency

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <set>

namespace nlm {

// Validation result
struct ValidationResult {
    bool isValid;
    std::string message;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true), message("Valid configuration") {}
    ValidationResult(bool valid, const std::string& msg)
        : isValid(valid), message(msg) {}
};

// Configuration parameter definition
struct ConfigParameter {
    std::string name;
    std::string type;
    std::string description;
    bool isRequired;
    std::string defaultValue;
    std::vector<std::string> allowedValues;
    float minValue;
    float maxValue;
    bool isAdvanced;  // Hidden from basic UI
    
    ConfigParameter() : isRequired(false), isAdvanced(false), minValue(-FLT_MAX), maxValue(FLT_MAX) {}
    ConfigParameter(const std::string& n, const std::string& t, const std::string& desc,
                    bool required = false, const std::string& def = "",
                    const std::vector<std::string>& allowed = {})
        : name(n), type(t), description(desc), isRequired(required), defaultValue(def),
          allowedValues(allowed), isAdvanced(false), minValue(-FLT_MAX), maxValue(FLT_MAX) {}
};

// Configuration constraint
struct ConfigConstraint {
    std::string type;  // "range", "enum", "dependency", "format"
    std::string sourceParam;
    std::string targetParam;
    std::vector<std::string> allowedValues;
    float minValue;
    float maxValue;
    std::string message;
    
    ConfigConstraint() : minValue(-FLT_MAX), maxValue(FLT_MAX) {}
};

class IConfigValidator {
public:
    virtual ~IConfigValidator() = default;

    // Validate complete configuration
    virtual ValidationResult validate(const std::map<std::string, std::string>& config) const = 0;

    // Validate single parameter
    virtual ValidationResult validateParameter(const std::string& name,
                                               const std::string& value) const = 0;

    // Get list of all supported parameters
    virtual std::vector<ConfigParameter> getSupportedParameters() const = 0;

    // Get parameter metadata
    virtual ConfigParameter getParameterMetadata(const std::string& name) const = 0;

    // Get validation rules
    virtual std::vector<ConfigConstraint> getConstraints() const = 0;

    // Update validator configuration
    virtual void update(const std::map<std::string, std::string>& config) = 0;

    // Reset validator to default state
    virtual void reset() = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IConfigValidator"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for validation events
    using ValidationCompletedCallback = std::function<void(const ValidationResult& result)>;
    virtual void setValidationCompletedCallback(const ValidationCompletedCallback& callback) = 0;

    using ParameterChangedCallback = std::function<void(const std::string& paramName, const std::string& value)>;
    virtual void setParameterChangedCallback(const ParameterChangedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Validation parameters
    virtual void setStrictMode(bool strict) = 0;
    virtual bool isStrictMode() const = 0;
    virtual void setAutoFix(bool autoFix) = 0;
    virtual bool isAutoFixEnabled() const = 0;

    // Performance metrics
    virtual uint64_t getTotalValidations() const = 0;
    virtual uint64_t getValidConfigurations() const = 0;
    virtual float getValidationSpeed() const = 0;  // Validations per second

protected:
    IConfigValidator() = default;
};

} // namespace nlm

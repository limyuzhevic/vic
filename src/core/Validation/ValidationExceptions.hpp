// Configuration Exception Classes
#pragma once

#include <string>
#include <stdexcept>
#include <vector>
#include <map>

namespace nlm {

// Base configuration validation error
class ConfigurationValidationError : public std::runtime_error {
public:
    ConfigurationValidationError(const std::string& message, 
                                const std::string& parameterName = "",
                                const std::string& expectedValue = "",
                                const std::string& actualValue = "")
        : std::runtime_error(message), parameterName_(parameterName),
          expectedValue_(expectedValue), actualValue_(actualValue) {}
    
    const std::string& getParameterName() const { return parameterName_; }
    const std::string& getExpectedValue() const { return expectedValue_; }
    const std::string& getActualValue() const { return actualValue_; }
    
private:
    std::string parameterName_;
    std::string expectedValue_;
    std::string actualValue_;
};

// Invalid configuration value error
class InvalidConfigurationValue : public ConfigurationValidationError {
public:
    InvalidConfigurationValue(const std::string& message,
                             const std::string& parameterName,
                             const std::string& expectedType,
                             const std::string& actualType)
        : ConfigurationValidationError(message, parameterName, expectedType, actualType) {}
};

// Missing required parameter error
class MissingRequiredParameter : public ConfigurationValidationError {
public:
    MissingRequiredParameter(const std::string& parameterName)
        : ConfigurationValidationError("Required parameter is missing", parameterName) {}
};

// Validation result structure for reporting
struct ValidationResult {
    bool isValid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string toString() const {
        std::string result;
        if (!errors.empty()) {
            result += "Validation Errors:\n";
            for (const auto& error : errors) {
                result += "  - " + error + "\n";
            }
        }
        if (!warnings.empty()) {
            if (!result.empty()) result += "\n";
            result += "Warnings:\n";
            for (const auto& warning : warnings) {
                result += "  - " + warning + "\n";
            }
        }
        if (isValid && !errors.empty() && !warnings.empty()) {
            result += "Valid (with warnings)\n";
        } else if (isValid) {
            result += "Valid\n";
        } else {
            result += "Invalid\n";
        }
        return result;
    }
};

} // namespace nlm
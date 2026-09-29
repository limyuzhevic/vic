#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <stdexcept>

namespace nlm {

// Configuration validation error types
enum class ValidationErrorType {
    InvalidValue,
    OutOfRange,
    MissingRequired,
    TypeMismatch,
    InvalidCombination,
    Custom
};

struct ValidationError {
    ValidationErrorType type;
    std::string key;
    std::string message;
    std::string value;
    
    ValidationError(ValidationErrorType t, const std::string& k, const std::string& msg, const std::string& val = "")
        : type(t), key(k), message(msg), value(val) {}
};

class ConfigValidator {
public:
    struct ParameterConstraints {
        std::string name;
        std::string description;
        bool required = false;
        std::string type;  // "int", "double", "string", "bool", "array", "object"
        std::string defaultValue;
        std::vector<std::pair<std::string, std::string>> allowedValues;  // key, display name
        std::string minValue;
        std::string maxValue;
        std::string minItems;
        std::string maxItems;
        std::string pattern;  // For strings
        std::function<bool(const std::string&, const std::string&)> customValidator;
        std::string customValidatorName;
    };
    
    ConfigValidator() = default;
    
    // Add parameter constraints
    void addParameter(const std::string& key, const ParameterConstraints& constraints);
    
    // Validate configuration
    std::vector<ValidationError> validate(const class Config& config) const;
    
    // Check if key exists
    bool hasKey(const std::string& key) const;
    
    // Get parameter constraints
    const ParameterConstraints* getParameter(const std::string& key) const;
    
    // Get all parameter keys
    std::vector<std::string> getParameterKeys() const;
    
    // Clear all constraints
    void clear();
    
    // Get validation summary
    std::string getSummary() const;
    
    // Common parameter sets
    void addBrainParameters();
    void addSensoryParameters();
    void addMotorParameters();
    void addPlasticityParameters();
    void addNeuromodulationParameters();
    void addDevelopmentParameters();
    
private:
    std::map<std::string, ParameterConstraints> parameters_;
    
    std::vector<ValidationError> validateValue(const ParameterConstraints& param, 
                                               const std::string& value) const;
    
    bool isValueInRange(const ParameterConstraints& param, const std::string& value) const;
    
    bool validateStringPattern(const std::string& value, const std::string& pattern) const;
    
    std::string typeToString(const ConfigValue& value) const;
    
    bool compareValues(const std::string& val1, const std::string& val2, 
                      const std::string& type) const;
};

// Global validator instance
ConfigValidator& getConfigValidator();

// Convenience functions for common validation scenarios
inline bool validateConfig(const class Config& config) {
    return getConfigValidator().validate(config).empty();
}

inline std::vector<ValidationError> getConfigErrors(const class Config& config) {
    return getConfigValidator().validate(config);
}

} // namespace nlm

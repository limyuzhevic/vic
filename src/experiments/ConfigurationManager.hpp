#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <variant>
#include <optional>
#include <stdexcept>
#include <regex>

namespace nlm {

// Type definitions for configuration validation
using ConfigValue = std::variant<int64_t, double, float, bool, std::string>;

// Validation error types
enum class ValidationErrorType {
    MISSING_REQUIRED_PARAMETER,
    INVALID_TYPE,
    INVALID_VALUE,
    CONSTRAINT_VIOLATION,
    SYNTAX_ERROR
};

// Validation error structure
struct ValidationError {
    ValidationErrorType type;
    std::string parameterName;
    std::string message;
    std::string expectedValue;
    std::string actualValue;
    
    ValidationError(ValidationErrorType t, const std::string& param, const std::string& msg)
        : type(t), parameterName(param), message(msg) {}
    
    ValidationError(ValidationErrorType t, const std::string& param, const std::string& msg,
                   const std::string& expected, const std::string& actual)
        : type(t), parameterName(param), message(msg), expectedValue(expected), actualValue(actual) {}
};

// Parameter constraints
struct ParameterConstraints {
    std::string type; // "int", "double", "string", "bool", "enum"
    std::vector<ConfigValue> allowedValues;
    std::optional<std::pair<double, double>> range; // min, max for numeric types
    std::regex pattern; // for string type validation
    bool required = false;
    std::string description;
    
    // Validation methods
    bool validate(const ConfigValue& value) const;
    bool matchesPattern(const std::string& value) const;
    std::string typeToString() const;
};

// Configuration schema for experiments
struct ExperimentSchema {
    std::string experimentType;
    std::string description;
    std::map<std::string, ParameterConstraints> parameters;
    std::vector<std::string> dependencies; // Other experiment types this depends on
    std::string version;
    
    // Validation methods
    std::vector<ValidationError> validate(const std::map<std::string, ConfigValue>& config) const;
    bool hasParameter(const std::string& name) const;
    const ParameterConstraints* getParameter(const std::string& name) const;
    std::string getParameterType(const std::string& name) const;
    std::vector<std::string> getRequiredParameters() const;
};

// Configuration manager for experiments
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Schema management
    void registerSchema(const ExperimentSchema& schema);
    bool hasSchema(const std::string& experimentType) const;
    const ExperimentSchema* getSchema(const std::string& experimentType) const;
    
    // Configuration validation
    std::vector<ValidationError> validate(const std::string& experimentType, 
                                          const std::map<std::string, ConfigValue>& config) const;
    
    // Configuration merging and inheritance
    std::map<std::string, ConfigValue> mergeConfigurations(
        const std::string& experimentType,
        const std::map<std::string, ConfigValue>& baseConfig,
        const std::map<std::string, ConfigValue>& userConfig) const;
    
    // Configuration from string parsing
    std::map<std::string, ConfigValue> parseConfiguration(const std::string& configString) const;
    std::string configToString(const std::map<std::string, ConfigValue>& config) const;
    
    // Default configuration
    std::map<std::string, ConfigValue> getDefaultConfiguration(const std::string& experimentType) const;
    
    // Configuration templates
    void registerConfigurationTemplate(const std::string& name, 
                                       const std::map<std::string, ConfigValue>& config);
    std::map<std::string, ConfigValue> getConfigurationTemplate(const std::string& name) const;
    
    // Experiment type relationships
    void registerExperimentType(const std::string& experimentType, const std::string& description);
    bool isExperimentTypeAvailable(const std::string& experimentType) const;
    std::vector<std::string> getAvailableExperimentTypes() const;
    std::string getExperimentTypeDescription(const std::string& experimentType) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global configuration manager instance
ConfigurationManager& getConfigurationManager();

// Configuration validation utilities
namespace ConfigValidation {
    // Parameter validation functions
    bool validateInt(const ConfigValue& value, const ParameterConstraints& constraints);
    bool validateDouble(const ConfigValue& value, const ParameterConstraints& constraints);
    bool validateFloat(const ConfigValue& value, const ParameterConstraints& constraints);
    bool validateBool(const ConfigValue& value, const ParameterConstraints& constraints);
    bool validateString(const ConfigValue& value, const ParameterConstraints& constraints);
    
    // Type conversion and validation
    std::string configValueToString(const ConfigValue& value);
    ConfigValue stringToConfigValue(const std::string& value);
    
    // Error message formatting
    std::string formatValidationError(const ValidationError& error);
    std::string getConstraintErrorMessage(const ParameterConstraints& constraints);
}

} // namespace nlm

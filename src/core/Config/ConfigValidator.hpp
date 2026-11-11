// Configuration Validation System
// Defines validation rules for configuration parameters

#pragma once

#include <string>
#include <vector>
#include <optional>
#include <functional>
#include <stdexcept>
#include <unordered_map>

namespace nlm {

enum class ValidationSeverity {
    Error,
    Warning
};

class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, const std::string& parameter = "", 
                   ValidationSeverity severity = ValidationSeverity::Error)
        : std::runtime_error(message), parameter_(parameter), severity_(severity) {}
    
    const std::string& getParameter() const { return parameter_; }
    ValidationSeverity getSeverity() const { return severity_; }
    
private:
    std::string parameter_;
    ValidationSeverity severity_;
};

class ConfigValidator {
public:
    // Validation function type: takes value as string and returns optional error message
    using ValidationFunc = std::function<std::optional<std::string>(const std::string& value)>;
    
    ConfigValidator();
    
    // Add validation rule for a parameter
    void addRule(const std::string& parameter, 
                 ValidationFunc validator,
                 const std::string& description = "",
                 ValidationSeverity severity = ValidationSeverity::Error);
    
    // Add validation rule for integer parameter
    void addIntRule(const std::string& parameter, 
                   std::function<bool(int64_t)> validator,
                   const std::string& description = "",
                   ValidationSeverity severity = ValidationSeverity::Error,
                   int64_t minValue = std::numeric_limits<int64_t>::min(),
                   int64_t maxValue = std::numeric_limits<int64_t>::max());
    
    // Add validation rule for float parameter
    void addFloatRule(const std::string& parameter,
                     std::function<bool(double)> validator,
                     const std::string& description = "",
                     ValidationSeverity severity = ValidationSeverity::Error,
                     double minValue = std::numeric_limits<double>::min(),
                     double maxValue = std::numeric_limits<double>::max());
    
    // Add validation rule for boolean parameter
    void addBoolRule(const std::string& parameter,
                    const std::string& description = "",
                    ValidationSeverity severity = ValidationSeverity::Error);
    
    // Add validation rule for probability (0.0 to 1.0)
    void addProbabilityRule(const std::string& parameter,
                           const std::string& description = "",
                           ValidationSeverity severity = ValidationSeverity::Error);
    
    // Validate a configuration parameter
    void validate(const std::string& parameter, const std::string& valueStr) const;
    
    // Validate multiple parameters
    std::vector<ValidationError> validateAll(const std::unordered_map<std::string, std::string>& config) const;
    
    // Get all parameter names
    std::vector<std::string> getParameterNames() const;
    
    // Get description for a parameter
    std::string getDescription(const std::string& parameter) const;
    
    // Get validation rules for a parameter
    std::optional<ValidationFunc> getRule(const std::string& parameter) const;
    
    // Remove validation rule
    void removeRule(const std::string& parameter);
    
    // Clear all rules
    void clearRules();
    
    // Standard predefined rules
    static ConfigValidator& getInstance();
    
private:
    struct ValidationRule {
        std::string parameter;
        ValidationFunc validator;
        std::string description;
        ValidationSeverity severity;
    };
    
    std::vector<ValidationRule> rules_;
};

// Standard validation rules for NLM configuration
class NLMConfigValidator {
public:
    static void initializeRules(ConfigValidator& validator);
    
private:
    static void addCoreRules(ConfigValidator& validator);
    static void addSTDPParameterRules(ConfigValidator& validator);
    static void addStructuralPlasticityRules(ConfigValidator& validator);
    static void addEnvironmentRules(ConfigValidator& validator);
    static void addVisionRules(ConfigValidator& validator);
    static void addAgentRules(ConfigValidator& validator);
    
    static bool validatePositive(const std::string& param, const std::string& valueStr);
    static bool validateNonNegative(const std::string& param, const std::string& valueStr);
    static bool validateProbability(const std::string& param, const std::string& valueStr);
    static bool validateFloatRange(const std::string& param, const std::string& valueStr, double min, double max);
    static bool validateIntRange(const std::string& param, const std::string& valueStr, int64_t min, int64_t max);
};

// Convenience function for validating config from string map
class ConfigValidationHelper {
public:
    static bool validateConfig(const std::unordered_map<std::string, std::string>& config);
    static std::vector<ValidationError> validateConfigWithErrors(const std::unordered_map<std::string, std::string>& config);
    
    static void setStrictValidation(bool strict) { strictValidation_ = strict; }
    static bool getStrictValidation() { return strictValidation_; }
    
private:
    static bool strictValidation_;
};
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>
#include <any>
#include <algorithm>
#include "core/Config/Config.hpp"

namespace nlm {

// Mode-specific validation schemas for different execution modes
enum class ValidationMode {
    Strict,
    Lenient,
    Development,
    Production,
    Custom
};

// Advanced validation rules
struct ValidationRule {
    std::string name;
    std::function<bool(const Config&)> rule;
    ValidationSeverity severity;
    std::string description;
    
    ValidationRule(const std::string& n, std::function<bool(const Config&)> r,
                   ValidationSeverity sev = ValidationSeverity::Error,
                   const std::string& desc = "")
        : name(n), rule(r), severity(sev), description(desc) {}
};

// Parameter type validation with detailed constraints
struct TypeValidation {
    std::string parameter;
    ParameterType expectedType;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::optional<size_t> minLength;
    std::optional<size_t> maxLength;
    std::vector<std::string> allowedValues;
    bool allowNull;
    
    TypeValidation(const std::string& param, ParameterType type = ParameterType::Float)
        : parameter(param), expectedType(type), allowNull(false) {}
    
    bool validate(const ConfigValue& value) const {
        // Type check
        if (!allowNull && value.index() == std::variant_npos) {
            return false; // null value not allowed
        }
        
        // Check value based on expected type
        switch (expectedType) {
            case ParameterType::Integer:
                if (std::holds_alternative<int>(value)) {
                    int intVal = std::get<int>(value);
                    if (minValue && intVal < minValue) return false;
                    if (maxValue && intVal > maxValue) return false;
                } else if (std::holds_alternative<int64_t>(value)) {
                    int64_t intVal = std::get<int64_t>(value);
                    if (minValue && static_cast<double>(intVal) < *minValue) return false;
                    if (maxValue && static_cast<double>(intVal) > *maxValue) return false;
                } else {
                    return false;
                }
                break;
                
            case ParameterType::Float:
                if (std::holds_alternative<double>(value)) {
                    double val = std::get<double>(value);
                    if (minValue && val < *minValue) return false;
                    if (maxValue && val > *maxValue) return false;
                } else if (std::holds_alternative<int>(value)) {
                    double val = static_cast<double>(std::get<int>(value));
                    if (minValue && val < *minValue) return false;
                    if (maxValue && val > *maxValue) return false;
                } else {
                    return false;
                }
                break;
                
            case ParameterType::Boolean:
                if (!std::holds_alternative<bool>(value)) return false;
                break;
                
            case ParameterType::String:
                if (!std::holds_alternative<std::string>(value)) return false;
                break;
                
            case ParameterType::IntegerArray:
                if (std::holds_alternative<std::vector<int>>(value)) {
                    auto& arr = std::get<std::vector<int>>(value);
                    if (minLength && arr.size() < *minLength) return false;
                    if (maxLength && arr.size() > *maxLength) return false;
                } else {
                    return false;
                }
                break;
                
            case ParameterType::FloatArray:
                if (std::holds_alternative<std::vector<double>>(value)) {
                    auto& arr = std::get<std::vector<double>>(value);
                    if (minLength && arr.size() < *minLength) return false;
                    if (maxLength && arr.size() > *maxLength) return false;
                } else {
                    return false;
                }
                break;
                
            case ParameterType::StringArray:
                if (std::holds_alternative<std::vector<std::string>>(value)) {
                    auto& arr = std::get<std::vector<std::string>>(value);
                    if (minLength && arr.size() < *minLength) return false;
                    if (maxLength && arr.size() > *maxLength) return false;
                } else {
                    return false;
                }
                break;
                
            case ParameterType::Object:
                // For object validation, we could use any non-primitive type
                return !std::holds_alternative<int>(value) &&
                       !std::holds_alternative<double>(value) &&
                       !std::holds_alternative<bool>(value) &&
                       !std::holds_alternative<std::string>(value) &&
                       !std::holds_alternative<std::vector<int>>(value) &&
                       !std::holds_alternative<std::vector<double>>(value) &&
                       !std::holds_alternative<std::vector<std::string>>(value);
        }
        
        return true;
    }
};

// Parameter dependency validation
struct ParameterDependency {
    std::string requiredParameter;
    std::function<bool(const ConfigValue&)> dependencyValidator;
    std::string errorMessage;
    
    ParameterDependency(const std::string& req, std::function<bool(const ConfigValue&)> validator,
                       const std::string& msg = "")
        : requiredParameter(req), dependencyValidator(validator), errorMessage(msg) {}
};

// ConfigValidator class with comprehensive validation capabilities
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Disable copying, enable moving
    ConfigValidator(const ConfigValidator&) = delete;
    ConfigValidator& operator=(const ConfigValidator&) = delete;
    ConfigValidator(ConfigValidator&&) noexcept;
    ConfigValidator& operator=(ConfigValidator&&) noexcept;
    
    // Mode management
    void setValidationMode(ValidationMode mode) { mode_ = mode; }
    ValidationMode getValidationMode() const { return mode_; }
    
    // Schema management
    void registerSchema(const ConfigSchema& schema);
    void registerTypeValidation(const TypeValidation& validation);
    void registerParameterDependency(const ParameterDependency& dependency);
    void registerValidationRule(const ValidationRule& rule);
    
    // Validation methods
    std::vector<ValidationError> validate(const Config& config) const;
    std::vector<ValidationError> validate(const Config& config, 
                                         const std::string& group) const;
    bool validateParameter(const Config& config, const std::string& key,
                          const std::string& group = "") const;
    
    // Mode-specific validation
    std::vector<ValidationError> validateForMode(const Config& config, 
                                                ValidationMode mode) const;
    std::vector<ValidationError> validateForProfile(const Config& config,
                                                   const std::string& profile) const;
    
    // Parameter group validation
    std::vector<ValidationError> validateGroup(const Config& config,
                                              const std::string& group) const;
    
    // Neural parameter validation (specialized)
    std::vector<ValidationError> validateNeuralParameters(const Config& config) const;
    std::vector<ValidationError> validatePlasticityParameters(const Config& config) const;
    std::vector<ValidationError> validateNeuromodulationParameters(const Config& config) const;
    std::vector<ValidationError> validateDevelopmentParameters(const Config& config) const;
    
    // Range validation for specific parameters
    std::vector<ValidationError> validateNeuralRanges(const Config& config) const;
    std::vector<ValidationError> validatePlasticityRanges(const Config& config) const;
    std::vector<ValidationError> validateNeuromodulationRanges(const Config& config) const;
    
    // Dependency validation
    std::vector<ValidationError> validateDependencies(const Config& config) const;
    std::vector<ValidationError> validateParameterDependencies(const Config& config,
                                                              const std::string& group) const;
    
    // Type validation for all parameter types
    std::vector<ValidationError> validateAllTypes(const Config& config) const;
    std::vector<ValidationError> validateType(const Config& config,
                                             const std::string& key,
                                             const std::string& group = "") const;
    
    // Schema validation
    std::vector<ValidationError> validateAgainstSchema(const Config& config,
                                                     const std::string& schemaName) const;
    
    // Specialized validation for configuration categories
    void setupNeuralValidation();
    void setupPlasticityValidation();
    void setupNeuromodulationValidation();
    void setupDevelopmentValidation();
    void setupEnvironmentValidation();
    void setupLoggingValidation();
    void setupSimulationValidation();
    
    // Helper methods
    std::string getParameterCategoryDescription(ParameterCategory category) const;
    std::string getErrorMessageForParameter(const std::string& key) const;
    bool isValidParameterType(const ConfigValue& value, ParameterType type) const;
    
    // Profile management
    void addProfile(const std::string& name, const Config& configTemplate);
    std::vector<std::string> getProfileNames() const;
    std::optional<Config> getProfile(const std::string& name) const;
    
    // Validation statistics
    struct ValidationStats {
        int totalValidations = 0;
        int passedValidations = 0;
        int failedValidations = 0;
        std::vector<ValidationError> errors;
        
        double getPassRate() const {
            return totalValidations > 0 ? (static_cast<double>(passedValidations) / totalValidations) * 100.0 : 0.0;
        }
    };
    
    ValidationStats getValidationStats() const { return stats_; }
    
    // Reset and cleanup
    void resetStats() { stats_ = ValidationStats(); }
    void clearAllRules() {
        typeValidations_.clear();
        parameterDependencies_.clear();
        validationRules_.clear();
        registeredSchemas_.clear();
        profiles_.clear();
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // State management
    ValidationMode mode_;
    std::unordered_map<std::string, ConfigSchema> registeredSchemas_;
    std::vector<TypeValidation> typeValidations_;
    std::vector<ParameterDependency> parameterDependencies_;
    std::vector<ValidationRule> validationRules_;
    std::unordered_map<std::string, Config> profiles_;
    ValidationStats stats_;
    
    // Helper methods
    std::vector<ValidationError> validateConfigEntry(const ConfigEntry& entry) const;
    std::vector<ValidationError> validateValue(const ConfigValue& value, 
                                               const ParameterConstraint& constraint) const;
    bool checkDependency(const Config& config, const ParameterDependency& dep) const;
    void updateStats(bool passed) const;
};

} // namespace nlm
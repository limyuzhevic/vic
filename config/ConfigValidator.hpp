#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <variant>
#include <optional>
#include <stdexcept>
#include <any>
#include <functional>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <sstream>
#include "core/Config/Config.hpp"

namespace nlm {

// Configuration validation and management system

// Forward declarations
class ConfigValidator;
class ConfigManager;

// Validation result structure
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::vector<std::string>> parameterErrors;
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    void addParameterError(const std::string& param, const std::string& error) {
        valid = false;
        parameterErrors[param].push_back(error);
    }
    
    std::string getSummary() const {
        std::ostringstream oss;
        if (valid) {
            oss << "Configuration is valid";
        } else {
            oss << "Configuration is invalid with " << errors.size() << " errors";
            if (!parameterErrors.empty()) {
                oss << " (" << parameterErrors.size() << " parameter-specific errors)";
            }
        }
        if (!warnings.empty()) {
            oss << " (with " << warnings.size() << " warnings)";
        }
        return oss.str();
    }
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> defaultValues;
    std::map<std::string, std::function<bool(const ConfigValue&, const Config&)>> validators;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::pair<double, double>> ranges;
    std::map<std::string, std::set<std::string>> enumValues;
    bool required = false;
    std::string format;
    
    ConfigSchema(const std::string& n = "", const std::string& desc = "")
        : name(n), description(desc) {}
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Validation setup
    void addSchema(const ConfigSchema& schema);
    void setParameterValidator(const std::string& parameter,
                               std::function<bool(const ConfigValue&, const Config&)> validator);
    void setRangeValidator(const std::string& parameter, double min, double max);
    void setEnumValidator(const std::string& parameter, const std::set<std::string>& values);
    void setDependencyValidator(const std::string& parameter, const std::string& dependsOn);
    
    // Validation execution
    ValidationResult validate(const Config& config) const;
    bool isValid(const Config& config) const;
    ValidationResult validateAgainstSchema(const Config& config, const ConfigSchema& schema) const;
    
    // Parameter validation
    ValidationResult validateParameter(const std::string& parameter, const ConfigValue& value,
                                       const Config& config) const;
    ValidationResult validateAllParameters(const Config& config) const;
    
    // Special validation rules
    ValidationResult validateBrainParameters(const Config& config) const;
    ValidationResult validatePlasticityParameters(const Config& config) const;
    ValidationResult validateNeuromodulationParameters(const Config& config) const;
    ValidationResult validateDevelopmentParameters(const Config& config) const;
    ValidationResult validateMemoryParameters(const Config& config) const;
    ValidationResult validatePredictionParameters(const Config& config) const;
    
    // Configuration comparison
    bool compareConfigurations(const Config& config1, const Config& config2) const;
    std::map<std::string, std::pair<std::string, std::string>> getDifferences(
        const Config& config1, const Config& config2) const;
    
    // Configuration normalization
    std::shared_ptr<Config> normalizeConfig(const Config& config) const;
    std::shared_ptr<Config> applyTemplate(const Config& config, const ConfigTemplate& template) const;
    
    // Error management
    std::vector<std::string> getErrors() const { return currentErrors; }
    void clearErrors() { currentErrors.clear(); }
    
    // Built-in schema creation
    static ConfigSchema createBrainSchema();
    static ConfigSchema createPlasticitySchema();
    static ConfigSchema createNeuromodulationSchema();
    static ConfigSchema createDevelopmentSchema();
    static ConfigSchema createMemorySchema();
    static ConfigSchema createPredictionSchema();
    
    // Environment variable substitution (placeholder)
    std::shared_ptr<Config> resolveEnvironmentVariables(const Config& config) const;
    
    // File inclusion and macro support (placeholder)
    std::shared_ptr<Config> resolveIncludes(const Config& config) const;
    
    // Conditional configuration (placeholder)
    std::shared_ptr<Config> evaluateConditions(const Config& config) const;
    
    // Override management (placeholder)
    std::shared_ptr<Config> applyOverrides(const Config& config, const Config& overrides) const;
    
    // Hot configuration reload (placeholder)
    bool reloadConfiguration(const std::string& filepath, std::shared_ptr<Config>& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<std::string> currentErrors;
    
    void collectValidationErrors(const ValidationResult& result);
    bool validateParameterType(const ConfigValue& value, const std::string& expectedType) const;
    bool validateParameterRange(const ConfigValue& value, double min, double max) const;
    bool validateParameterEnum(const ConfigValue& value, const std::set<std::string>& allowed) const;
    bool validateParameterDependency(const std::string& parameter, const ConfigValue& value,
                                    const Config& config) const;
};

// Configuration manager (forward declaration for now)
class ConfigManager;

// Global configuration management functions
extern std::unique_ptr<ConfigValidator> createDefaultConfigValidator();
extern std::unique_ptr<ConfigManager> createDefaultConfigManager();

} // namespace nlm

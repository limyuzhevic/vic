#pragma once

#include <string>
#include <map>
#include <functional>
#include <vector>
#include <unordered_set>
#include "Config.hpp"

namespace nlm {

class ConfigValidator {
public:
    // Validation result
    struct ValidationResult {
        bool valid;
        std::string message;
        std::string key;
        ConfigSource source;
        
        ValidationResult(bool v, const std::string& msg, const std::string& k, ConfigSource s = ConfigSource::Default)
            : valid(v), message(msg), key(k), source(s) {}
    };
    
    ConfigValidator();
    ~ConfigValidator() = default;
    
    // Register validation rule for a parameter
    void registerValidator(const std::string& key,
                          std::function<bool(const ConfigValue&)> validator,
                          const std::string& errorMessage);
    
    // Register dependency rule between parameters
    void registerDependency(const std::string& dependentKey,
                           const std::string& requiredKey,
                           const std::string& errorMessage);
    
    // Register range validation for numeric parameters
    void registerRangeValidator(const std::string& key,
                               double min, double max,
                               const std::string& errorMessage);
    
    // Validate a single key-value pair
    ValidationResult validate(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Default) const;
    
    // Validate entire configuration
    std::vector<ValidationResult> validateAll(const Config& config) const;
    
    // Check parameter consistency
    std::vector<ValidationResult> checkConsistency(const Config& config) const;
    
    // Get validation error message for a key
    std::string getErrorMessage(const std::string& key) const;
    
    // Get all registered keys
    std::vector<std::string> getRegisteredKeys() const;
    
private:
    // Individual validation rules
    struct ValidationRule {
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    // Dependency rules
    struct DependencyRule {
        std::string requiredKey;
        std::string errorMessage;
    };
    
    // Range validation rules
    struct RangeRule {
        double min;
        double max;
        std::string errorMessage;
    };
    
    std::map<std::string, ValidationRule> validators_;
    std::map<std::string, DependencyRule> dependencies_;
    std::map<std::string, RangeRule> rangeValidators_;
    
    // Get value as double for range checking
    bool getValueAsDouble(const ConfigValue& value, double& out) const;
};

} // namespace nlm
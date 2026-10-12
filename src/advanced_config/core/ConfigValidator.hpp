#pragma once

#include <string>
#include <memory>
#include <any>
#include <vector>
#include <functional>
#include "Config.hpp"

namespace nlm {

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string fieldName;
    
    ValidationResult() : valid(true), errorMessage(), fieldName() {}
    ValidationResult(bool v, const std::string& error, const std::string& field)
        : valid(v), errorMessage(error), fieldName(field) {}
};

// Base configuration validator
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    
    // Validate a configuration value
    virtual ValidationResult validate(const std::string& fieldName, const ConfigValue& value) const = 0;
    
    // Get validator type name
    virtual std::string getTypeName() const = 0;
    
    // Check if validator has dependencies
    virtual bool hasDependencies() const { return false; }
    
    // Get dependency field names
    virtual std::vector<std::string> getDependencies() const { return {}; }
};

// Range validator for numeric types
class ConfigRangeValidator : public ConfigValidator {
public:
    ConfigRangeValidator(const std::any& minValue, const std::any& maxValue,
                       bool inclusiveMin = true, bool inclusiveMax = true);
    
    ValidationResult validate(const std::string& fieldName, const ConfigValue& value) const override;
    std::string getTypeName() const override { return "RangeValidator"; }
    bool hasDependencies() const override { return false; }

private:
    std::any minValue;
    std::any maxValue;
    bool inclusiveMin;
    bool inclusiveMax;
    
    template<typename T>
    bool validateRange(const T& value) const;
};

// Format validator for string values
class ConfigFormatValidator : public ConfigValidator {
public:
    ConfigFormatValidator(const std::string& pattern, const std::string& errorMessage = "");
    
    ValidationResult validate(const std::string& fieldName, const ConfigValue& value) const override;
    std::string getTypeName() const override { return "FormatValidator"; }
    bool hasDependencies() const override { return false; }

private:
    std::string pattern;
    std::string errorMessage;
};

// Dependency validator
class ConfigDependencyValidator : public ConfigValidator {
public:
    ConfigDependencyValidator(const std::vector<std::string>& requiredFields,
                             const std::string& errorMessage = "");
    
    ValidationResult validate(const std::string& fieldName, const ConfigValue& value) const override;
    std::string getTypeName() const override { return "DependencyValidator"; }
    bool hasDependencies() const override { return true; }
    std::vector<std::string> getDependencies() const override { return requiredFields; }

private:
    std::vector<std::string> requiredFields;
    std::string errorMessage;
};

// Custom validation callback
class ConfigValidationCallback : public ConfigValidator {
public:
    ConfigValidationCallback(const std::string& fieldName,
                            std::function<ValidationResult(const ConfigValue&)> callback,
                            const std::string& errorMessage = "");
    
    ValidationResult validate(const std::string& fieldName, const ConfigValue& value) const override;
    std::string getTypeName() const override { return "ValidationCallback"; }
    bool hasDependencies() const override { return false; }

private:
    std::string fieldName;
    std::function<ValidationResult(const ConfigValue&)> callback;
    std::string errorMessage;
};

} // namespace nlm
// Validation framework for NLM configuration system
#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <any>
#include <optional>
#include <variant>
#include <typeindex>
#include "Config.hpp"

namespace nlm {

// Forward declarations
class ConfigValidator;
class ConfigSchema;

// Validation result structure
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs = {},
                    const std::vector<std::string>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getFullMessage() const {
        std::string result;
        for (const auto& error : errors) {
            if (!result.empty()) result += "\n";
            result += "ERROR: " + error;
        }
        for (const auto& warning : warnings) {
            if (!result.empty()) result += "\n";
            result += "WARNING: " + warning;
        }
        return result;
    }
};

// Base validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    
    virtual ValidationResult validate(const ConfigValue& value, 
                                     const std::string& key,
                                     const ConfigSchema::Field& field) const = 0;
    
    virtual bool validate(const ConfigValue& value, 
                         const std::string& key,
                         const ConfigSchema::Field& field,
                         std::vector<std::string>& errors) const = 0;
};

// Type validators
class TypeValidator : public ConfigValidator {
public:
    std::string getName() const override { return "TypeValidator"; }
    std::string getDescription() const override { return "Validates that value matches expected type"; }
    
    ValidationResult validate(const ConfigValue& value, 
                             const std::string& key,
                             const ConfigSchema::Field& field) const override;
};

class RangeValidator : public ConfigValidator {
public:
    std::string getName() const override { return "RangeValidator"; }
    std::string getDescription() const override { return "Validates that numeric values are within allowed ranges"; }
    
    ValidationResult validate(const ConfigValue& value, 
                             const std::string& key,
                             const ConfigSchema::Field& field) const override;
};

class FormatValidator : public ConfigValidator {
public:
    std::string getName() const override { return "FormatValidator"; }
    std::string getDescription() const override { return "Validates string formats (paths, emails, etc.)"; }
    
    ValidationResult validate(const ConfigValue& value, 
                             const std::string& key,
                             const ConfigSchema::Field& field) const override;
};

class DependencyValidator : public ConfigValidator {
public:
    std::string getName() const override { return "DependencyValidator"; }
    std::string getDescription() const override { return "Validates that dependencies between keys are satisfied"; }
    
    ValidationResult validate(const ConfigValue& value, 
                             const std::string& key,
                             const ConfigSchema::Field& field) const override;
    
    void addDependency(const std::string& dependentKey, const std::string& requiredKey);
    
private:
    std::map<std::string, std::string> dependencies_;
};

class LogicalValidator : public ConfigValidator {
public:
    std::string getName() const override { return "LogicalValidator"; }
    std::string getDescription() const override { return "Validates logical constraints (mutually exclusive options, required combinations)"; }
    
    ValidationResult validate(const ConfigValue& value, 
                             const std::string& key,
                             const ConfigSchema::Field& field) const override;
    
    void addMutuallyExclusiveGroup(const std::vector<std::string>& keys);
    void addRequiredCombination(const std::vector<std::string>& keys);
    
private:
    std::vector<std::vector<std::string>> mutuallyExclusiveGroups_;
    std::vector<std::vector<std::string>> requiredCombinations_;
};

// Validation strategy
enum class ValidationStrategy {
    Strict,    // Fail on any validation error
    Lenient,   // Warn on conflicts, fail on required errors
    Report     // Report all errors without failing
};

// Main validation framework
class ValidationFramework {
public:
    ValidationFramework();
    ~ValidationFramework();
    
    // Register a validator
    void registerValidator(std::unique_ptr<ConfigValidator> validator);
    
    // Get validator by name
    ConfigValidator* getValidator(const std::string& name) const;
    
    // Validate configuration against schema
    ValidationResult validate(const std::unordered_map<std::string, ConfigValue>& config,
                             const ConfigSchema& schema,
                             ValidationStrategy strategy = ValidationStrategy::Strict) const;
    
    // Validate single field
    ValidationResult validateField(const std::string& key, 
                                  const ConfigValue& value,
                                  const ConfigSchema::Field& field,
                                  ValidationStrategy strategy = ValidationStrategy::Strict) const;
    
    // Set validation strategy
    void setValidationStrategy(ValidationStrategy strategy) { strategy_ = strategy; }
    
    // Get validation strategy
    ValidationStrategy getValidationStrategy() const { return strategy_; }
    
    // Enable/disable schema loading
    void setSchemaLoadingEnabled(bool enabled) { schemaLoadingEnabled_ = enabled; }
    
    // Add schema file path
    void addSchemaDirectory(const std::string& directory) { schemaDirectories_.push_back(directory); }
    
    // Get all schemas in directory
    std::vector<std::string> findSchemas() const;
    
private:
    std::map<std::string, std::unique_ptr<ConfigValidator>> validators_;
    ValidationStrategy strategy_;
    bool schemaLoadingEnabled_;
    std::vector<std::string> schemaDirectories_;
    
    // Helper methods
    std::string typeToString(const ConfigValue& value) const;
    bool isNumericType(const std::string& type) const;
    bool isVectorType(const std::string& type) const;
};

// Global validation framework instance
ValidationFramework& getValidationFramework();

// Convenience functions for validation
ValidationResult validateConfig(const Config& config, 
                               const ConfigSchema& schema,
                               ValidationStrategy strategy = ValidationStrategy::Strict);

std::string getValidationErrors(const Config& config,
                               const ConfigSchema& schema,
                               ValidationStrategy strategy = ValidationStrategy::Strict);

} // namespace nlm

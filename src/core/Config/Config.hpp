#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <regex>

// JSON support using nlohmann
#ifdef NLM_USE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

// YAML support using yaml-cpp
#ifdef NLM_USE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

// File format enumeration
enum class FileFormat {
    UNKNOWN,
    SIMPLE,
    JSON,
    YAML
};

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Validation severity levels
enum class ValidationSeverity {
    WARNING,
    ERROR,
    CRITICAL
};

// Validation result
struct ValidationResult {
    ValidationSeverity severity;
    std::string message;
    std::string suggestion;
    
    ValidationResult() : severity(ValidationSeverity::WARNING), message(), suggestion() {}
    ValidationResult(ValidationSeverity sev, const std::string& msg, const std::string& sug = "")
        : severity(sev), message(msg), suggestion(sug) {}
    
    bool isError() const { return severity >= ValidationSeverity::ERROR; }
    bool isWarning() const { return severity == ValidationSeverity::WARNING; }
    bool isCritical() const { return severity == ValidationSeverity::CRITICAL; }
};

// Validation types
enum class ValidationType {
    TYPE,
    RANGE,
    FORMAT,
    DEPENDENCY,
    SEMANTIC,
    CUSTOM
};

// Validation rule base class
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual ValidationResult validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual ValidationType getType() const = 0;
    virtual std::string getDescription() const = 0;
};

// Type validation rule
class TypeValidationRule : public ValidationRule {
public:
    TypeValidationRule(std::type_index type) : expectedType(type) {}
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override {
        std::type_index actualType = value.index();
        if (actualType == expectedType) {
            return ValidationResult();
        }
        return ValidationResult(ValidationSeverity::ERROR, 
            "Key '" + key + "' has type " + typeName(actualType) + ", expected " + typeName(expectedType));
    }
    
    ValidationType getType() const override { return ValidationType::TYPE; }
    std::string getDescription() const override { return "Type validation for " + typeName(expectedType); }
    
    static std::type_index getTypeIndex(const std::string& typeName) {
        if (typeName == "int") return typeid(int);
        if (typeName == "int64_t") return typeid(int64_t);
        if (typeName == "double") return typeid(double);
        if (typeName == "bool") return typeid(bool);
        if (typeName == "string") return typeid(std::string);
        if (typeName == "int_array") return typeid(std::vector<int>);
        if (typeName == "int64_array") return typeid(std::vector<int64_t>);
        if (typeName == "double_array") return typeid(std::vector<double>);
        if (typeName == "string_array") return typeid(std::vector<std::string>);
        return typeid(void);
    }
    
private:
    static std::string typeName(std::type_index type) {
        if (type == typeid(int)) return "int";
        if (type == typeid(int64_t)) return "int64_t";
        if (type == typeid(double)) return "double";
        if (type == typeid(bool)) return "bool";
        if (type == typeid(std::string)) return "string";
        if (type == typeid(std::vector<int>)) return "int_array";
        if (type == typeid(std::vector<int64_t>)) return "int64_array";
        if (type == typeid(std::vector<double>)) return "double_array";
        if (type == typeid(std::vector<std::string>)) return "string_array";
        return "unknown";
    }
    
    std::type_index expectedType;
};

// Range validation rule
class RangeValidationRule : public ValidationRule {
public:
    RangeValidationRule(double min, double max, bool inclusive = true) 
        : min(min), max(max), inclusive(inclusive) {}
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override {
        if (!isNumericType(value.index())) return ValidationResult();
        
        double val = getNumericValue(value);
        if (inclusive) {
            if (val < min || val > max) {
                return ValidationResult(ValidationSeverity::WARNING,
                    "Key '" + key + "' has value " + std::to_string(val) + " outside range [" + 
                    std::to_string(min) + ", " + std::to_string(max) + "]");
            }
        } else {
            if (val <= min || val >= max) {
                return ValidationResult(ValidationSeverity::WARNING,
                    "Key '" + key + "' has value " + std::to_string(val) + " outside range (" + 
                    std::to_string(min) + ", " + std::to_string(max) + ")");
            }
        }
        return ValidationResult();
    }
    
    ValidationType getType() const override { return ValidationType::RANGE; }
    std::string getDescription() const override { 
        return "Range validation for [" + std::to_string(min) + ", " + std::to_string(max) + "]"; 
    }
    
private:
    static bool isNumericType(size_t index) {
        // Check if index corresponds to numeric types
        return index == typeid(int).hash_code() || 
               index == typeid(int64_t).hash_code() || 
               index == typeid(double).hash_code();
    }
    
    static double getNumericValue(const ConfigValue& value) {
        if (auto ptr = std::get_if<int>(&value)) return *ptr;
        if (auto ptr = std::get_if<int64_t>(&value)) return *ptr;
        if (auto ptr = std::get_if<double>(&value)) return *ptr;
        return 0.0;
    }
    
    double min, max;
    bool inclusive;
};

// Format validation rule
class FormatValidationRule : public ValidationRule {
public:
    FormatValidationRule(const std::regex& pattern, const std::string& errorMessage = "")
        : pattern(pattern), errorMessage(errorMessage) {}
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override {
        if (auto ptr = std::get_if<std::string>(&value)) {
            if (!pattern.match(*ptr)) {
                std::string msg = errorMessage.empty() ? "does not match expected format" : errorMessage;
                return ValidationResult(ValidationSeverity::ERROR,
                    "Key '" + key + "' with value '" + *ptr + "' " + msg);
            }
        }
        return ValidationResult();
    }
    
    ValidationType getType() const override { return ValidationType::FORMAT; }
    std::string getDescription() const override { return "Format validation for " + (errorMessage.empty() ? "pattern" : errorMessage); }
    
private:
    std::regex pattern;
    std::string errorMessage;
};

// Dependency validation rule
class DependencyValidationRule : public ValidationRule {
public:
    DependencyValidationRule(const std::string& dependsOn, const std::string& relationship)
        : dependsOn(dependsOn), relationship(relationship) {}
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override {
        // Note: This is a base implementation - actual validation requires access to config state
        return ValidationResult(ValidationSeverity::WARNING,
            "Dependency validation for " + key + " -> " + dependsOn + " (" + relationship + ")");
    }
    
    ValidationType getType() const override { return ValidationType::DEPENDENCY; }
    std::string getDescription() const override { return "Dependency validation: " + key + " depends on " + dependsOn; }
    
private:
    std::string dependsOn;
    std::string relationship;
    std::string key; // Will be set during configuration
};

// Semantic validation rule
class SemanticValidationRule : public ValidationRule {
public:
    SemanticValidationRule(const std::function<ValidationResult(const std::string&, const ConfigValue&, const std::unordered_map<std::string, ConfigValue>&)> func,
                          const std::string& description)
        : validationFunc(func), description(description) {}
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override {
        // Note: This is a base implementation - actual validation requires access to config state
        return validationFunc(key, value, configState);
    }
    
    ValidationType getType() const override { return ValidationType::SEMANTIC; }
    std::string getDescription() const override { return description; }
    
    static void setConfigState(const std::unordered_map<std::string, ConfigValue>& state) {
        configState = state;
    }
    
private:
    std::function<ValidationResult(const std::string&, const ConfigValue&, const std::unordered_map<std::string, ConfigValue>&)> validationFunc;
    std::string description;
    static std::unordered_map<std::string, ConfigValue> configState;
};

// Custom validation function type
using CustomValidationFunc = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Validation configuration
struct ValidationConfig {
    std::string key;
    std::string description;
    ValidationType type;
    std::string errorMessage;
    bool enabled;
    CustomValidationFunc customValidator;
    
    ValidationConfig(const std::string& k, const std::string& desc, ValidationType t = ValidationType::TYPE)
        : key(k), description(desc), type(t), errorMessage(), enabled(true), customValidator(nullptr) {}
    
    ValidationConfig(const std::string& k, const std::string& desc, CustomValidationFunc validator)
        : key(k), description(desc), type(ValidationType::CUSTOM), errorMessage(), enabled(true), customValidator(validator) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add validation rule
    void addValidationRule(const std::shared_ptr<ValidationRule>& rule) {
        rules.push_back(rule);
    }
    
    // Add validation config
    void addValidationConfig(const ValidationConfig& config) {
        configs.push_back(config);
    }
    
    // Validate a configuration value
    std::vector<ValidationResult> validate(const std::string& key, const ConfigValue& value) const {
        std::vector<ValidationResult> results;
        
        // Check built-in rules
        for (const auto& rule : rules) {
            ValidationResult result = rule->validate(key, value);
            if (result.severity != ValidationSeverity::WARNING || !result.message.empty()) {
                results.push_back(result);
            }
        }
        
        // Check custom configs
        for (const auto& config : configs) {
            if (config.key == key && config.enabled) {
                if (config.customValidator) {
                    ValidationResult result = config.customValidator(key, value);
                    if (result.severity != ValidationSeverity::WARNING || !result.message.empty()) {
                        results.push_back(result);
                    }
                }
            }
        }
        
        return results;
    }
    
    // Check if key has any validation
    bool hasValidation(const std::string& key) const {
        for (const auto& rule : rules) {
            if (rule->getType() == ValidationType::TYPE) {
                // Simple type check - would need refinement in practice
            }
        }
        for (const auto& config : configs) {
            if (config.key == key && config.enabled) {
                return true;
            }
        }
        return false;
    }
    
    // Get all keys with validation
    std::vector<std::string> getValidatedKeys() const {
        std::vector<std::string> keys;
        for (const auto& config : configs) {
            if (config.enabled) {
                keys.push_back(config.key);
            }
        }
        return keys;
    }
    
private:
    std::vector<std::shared_ptr<ValidationRule>> rules;
    std::vector<ValidationConfig> configs;
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator() = default;
    
    // Add schema to validator
    void addSchema(const std::string& section, const ConfigSchema& schema) {
        schemas[section] = schema;
    }
    
    // Validate configuration value
    std::vector<ValidationResult> validate(const std::string& section, const std::string& key, 
                                         const ConfigValue& value, const std::unordered_map<std::string, ConfigValue>& fullConfig) {
        auto it = schemas.find(section);
        if (it == schemas.end()) {
            return {};
        }
        
        SemanticValidationRule::setConfigState(fullConfig);
        
        return it->second.validate(key, value);
    }
    
    // Validate all entries in config
    std::vector<ValidationResult> validateAll(const std::string& section, 
                                            const std::vector<ConfigEntry>& entries) const {
        std::vector<ValidationResult> allResults;
        
        auto it = schemas.find(section);
        if (it == schemas.end()) return allResults;
        
        for (const auto& entry : entries) {
            std::vector<ValidationResult> results = it->second.validate(entry.key, entry.value);
            allResults.insert(allResults.end(), results.begin(), results.end());
        }
        
        return allResults;
    }
    
    // Get all configured sections
    std::vector<std::string> getSections() const {
        std::vector<std::string> sections;
        for (const auto& pair : schemas) {
            sections.push_back(pair.first);
        }
        return sections;
    }
    
    // Check if section has validation
    bool hasValidation(const std::string& section) const {
        return schemas.find(section) != schemas.end();
    }
    
    // Disable validation for a section
    void disableSection(const std::string& section) {
        if (schemas.find(section) != schemas.end()) {
            schemas[section] = ConfigSchema(); // Clear schema
        }
    }
    
private:
    std::unordered_map<std::string, ConfigSchema> schemas;
};

// Configuration entry with validation
struct ValidatedConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<ValidationResult> validationResults;
    bool isValid() const {
        for (const auto& result : validationResults) {
            if (result.isError()) return false;
        }
        return true;
    }
    
    ValidatedConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ValidatedConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Validation methods
    bool validate(bool skipWarnings = false) const;
    std::vector<ValidationResult> getValidationErrors() const;
    std::vector<ValidationResult> getValidationWarnings() const;
    void clearValidationResults();
    
    // Configuration validation API
    void setValidationEnabled(bool enabled);
    bool isValidationEnabled() const;
    void enableSectionValidation(const std::string& section);
    void disableSectionValidation(const std::string& section);
    bool isSectionValidationEnabled(const std::string& section) const;
    
    // Get validation summary
    std::string getValidationSummary() const;
    
    // Set current configuration as baseline for dependency validation
    void setBaselineConfig(const std::unordered_map<std::string, ConfigValue>& baseline);
    
    // Configuration schema management
    void addValidationSchema(const std::string& section, const ConfigSchema& schema);
    void addValidationConfig(const std::string& section, const ValidationConfig& config);
    void addTypeValidation(const std::string& section, const std::string& key, 
                          const std::string& typeName, const std::string& description = "");
    void addRangeValidation(const std::string& section, const std::string& key, 
                           double min, double max, const std::string& description = "",
                           bool inclusive = true);
    void addFormatValidation(const std::string& section, const std::string& key, 
                            const std::regex& pattern, const std::string& description = "",
                            const std::string& errorMessage = "");
    void addDependencyValidation(const std::string& section, const std::string& key,
                                const std::string& dependsOn, const std::string& relationship,
                                const std::string& description = "");
    void addSemanticValidation(const std::string& section, const std::string& key,
                              const std::function<ValidationResult(const std::string&, const ConfigValue&, 
                                                                const std::unordered_map<std::string, ConfigValue>&)>& validator,
                              const std::string& description = "");

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validation components
    ConfigValidator validator;
    bool validationEnabled;
    std::unordered_map<std::string, ConfigValue> baselineConfig;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML parsing helpers
#ifdef NLM_USE_NLOHMANN_JSON
    static ConfigValue jsonValueToConfigValue(const nlohmann::json& j);
#endif
#ifdef NLM_USE_YAML_CPP
    static ConfigValue yamlValueToConfigValue(const YAML::Node& node);
#endif
    static ConfigValue stringToConfigValue(const std::string& value);
    static std::string configValueToString(const ConfigValue& value);
    static FileFormat detectFileFormat(const std::string& filepath);
    
    // JSON/YAML parsing methods
#ifdef NLM_USE_NLOHMANN_JSON
    bool parseJsonFile(const std::string& filepath);
#endif
#ifdef NLM_USE_YAML_CPP
    bool parseYamlFile(const std::string& filepath);
#endif
    bool parseSimpleFile(const std::string& filepath);
    
    // Template instantiations for get()
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Template instantiations for getOr()
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // set() implementations (delegating to validation)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source);
    void set(const std::string& key, const std::string& value, ConfigSource source);
    void set(const std::string& key, int value, ConfigSource source);
    void set(const std::string& key, int64_t value, ConfigSource source);
    void set(const std::string& key, double value, ConfigSource source);
    void set(const std::string& key, bool value, ConfigSource source);
    
    // Internal validation helpers
    void validateEntry(const std::string& section, ConfigEntry& entry);
    std::string determineSection(const std::string& key) const;
};

} // namespace nlm

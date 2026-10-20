#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
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

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration validation error
class ConfigValidationError {
public:
    enum class ErrorCode {
        InvalidType,
        OutOfRange,
        MissingRequired,
        InvalidFormat,
        CircularDependency
    };
    
    ConfigValidationError(ErrorCode code, const std::string& message, const std::string& key = "")
        : code(code), message(message), key(key) {}
    
    ErrorCode code;
    std::string message;
    std::string key;
};

// Configuration validation rules
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual std::vector<ConfigValidationError> validate(const Config& config) const = 0;
    virtual std::string getDescription() const = 0;
};

// Validation rule for required keys
class RequiredKeysRule : public ValidationRule {
public:
    RequiredKeysRule(const std::vector<std::string>& keys) : requiredKeys(keys) {}
    
    std::vector<ConfigValidationError> validate(const Config& config) const override {
        std::vector<ConfigValidationError> errors;
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                errors.emplace_back(
                    ConfigValidationError::ErrorCode::MissingRequired,
                    "Required configuration key is missing: " + key,
                    key
                );
            }
        }
        return errors;
    }
    
    std::string getDescription() const override {
        return "Validates presence of required configuration keys";
    }
    
private:
    std::vector<std::string> requiredKeys;
};

// Validation rule for type constraints
class TypeConstraintRule : public ValidationRule {
public:
    TypeConstraintRule(const std::string& key, const std::type_info& type, 
                      double min = std::numeric_limits<double>::lowest(),
                      double max = std::numeric_limits<double>::max())
        : key(key), type(type), min(min), max(max) {}
    
    std::vector<ConfigValidationError> validate(const Config& config) const override {
        std::vector<ConfigValidationError> errors;
        
        if (!config.has(key)) {
            return errors; // Missing key is handled by RequiredKeysRule
        }
        
        auto value = config.get(key);
        if (!value) {
            errors.emplace_back(
                ConfigValidationError::ErrorCode::InvalidType,
                "Configuration value has invalid type: " + key,
                key
            );
            return errors;
        }
        
        // Check type
        bool typeMatches = false;
        double numericValue = 0.0;
        
        if (type == typeid(int)) {
            typeMatches = std::holds_alternative<int>(value.value());
            if (typeMatches) numericValue = static_cast<double>(std::get<int>(value.value()));
        } else if (type == typeid(int64_t)) {
            typeMatches = std::holds_alternative<int64_t>(value.value());
            if (typeMatches) numericValue = static_cast<double>(std::get<int64_t>(value.value()));
        } else if (type == typeid(double)) {
            typeMatches = std::holds_alternative<double>(value.value());
            if (typeMatches) numericValue = std::get<double>(value.value());
        } else if (type == typeid(bool)) {
            typeMatches = std::holds_alternative<bool>(value.value());
        } else if (type == typeid(std::string)) {
            typeMatches = std::holds_alternative<std::string>(value.value());
        }
        
        if (!typeMatches) {
            errors.emplace_back(
                ConfigValidationError::ErrorCode::InvalidType,
                "Configuration value has invalid type: " + key,
                key
            );
            return errors;
        }
        
        // Check numeric range if applicable
        if (std::isfinite(min) || std::isfinite(max)) {
            if (std::holds_alternative<int>(value.value())) {
                double val = static_cast<double>(std::get<int>(value.value()));
                if ((min != std::numeric_limits<double>::lowest() && val < min) ||
                    (max != std::numeric_limits<double>::max() && val > max)) {
                    errors.emplace_back(
                        ConfigValidationError::ErrorCode::OutOfRange,
                        "Configuration value out of range for key: " + key,
                        key
                    );
                }
            } else if (std::holds_alternative<int64_t>(value.value())) {
                double val = static_cast<double>(std::get<int64_t>(value.value()));
                if ((min != std::numeric_limits<double>::lowest() && val < min) ||
                    (max != std::numeric_limits<double>::max() && val > max)) {
                    errors.emplace_back(
                        ConfigValidationError::ErrorCode::OutOfRange,
                        "Configuration value out of range for key: " + key,
                        key
                    );
                }
            } else if (std::holds_alternative<double>(value.value())) {
                double val = std::get<double>(value.value());
                if ((min != std::numeric_limits<double>::lowest() && val < min) ||
                    (max != std::numeric_limits<double>::max() && val > max)) {
                    errors.emplace_back(
                        ConfigValidationError::ErrorCode::OutOfRange,
                        "Configuration value out of range for key: " + key,
                        key
                    );
                }
            }
        }
        
        return errors;
    }
    
    std::string getDescription() const override {
        return "Validates type and range constraints for configuration keys";
    }
    
private:
    std::string key;
    std::type_info type;
    double min;
    double max;
};

// Validation rule for format constraints
class FormatConstraintRule : public ValidationRule {
public:
    FormatConstraintRule(const std::string& key, const std::string& pattern,
                        bool isRegex = true)
        : key(key), pattern(pattern), isRegex(isRegex) {}
    
    std::vector<ConfigValidationError> validate(const Config& config) const override {
        std::vector<ConfigValidationError> errors;
        
        if (!config.has(key)) {
            return errors; // Missing key is handled by RequiredKeysRule
        }
        
        auto value = config.get<std::string>(key);
        if (!value) {
            errors.emplace_back(
                ConfigValidationError::ErrorCode::InvalidFormat,
                "Configuration value format validation failed for key: " + key,
                key
            );
            return errors;
        }
        
        if (isRegex) {
            std::regex regex(pattern);
            if (!std::regex_match(*value, regex)) {
                errors.emplace_back(
                    ConfigValidationError::ErrorCode::InvalidFormat,
                    "Configuration value format validation failed for key: " + key,
                    key
                );
            }
        } else {
            if (*value != pattern) {
                errors.emplace_back(
                    ConfigValidationError::ErrorCode::InvalidFormat,
                    "Configuration value format validation failed for key: " + key,
                    key
                );
            }
        }
        
        return errors;
    }
    
    std::string getDescription() const override {
        return "Validates format constraints for configuration keys";
    }
    
private:
    std::string key;
    std::string pattern;
    bool isRegex;
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
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation
    void addValidationRule(std::unique_ptr<ValidationRule> rule);
    bool validate() const;
    std::vector<ConfigValidationError> getValidationErrors() const;
    std::string getValidationSummary() const;
    
    // Configuration merging
    void merge(const Config& other);
    static Config mergeConfigs(const std::vector<Config>& configs);
    
    // Get configuration by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static double stringToDouble(const std::string& str);
    
    // Validation helpers
    static bool validateType(const ConfigValue& value, const std::type_info& type);
    static std::vector<ConfigValidationError> validateAllRules(const Config& config);
};

} // namespace nlm

// Include implementation
#include "Config.cpp"


} // namespace nlm

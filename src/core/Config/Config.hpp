#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <nlohmann/json.hpp>
#include "core/Validation/Validation.hpp"

namespace nlm {

// Forward declarations
class Config;
class ValidationManager;

// Error types
class ConfigError : public std::runtime_error {
public:
    enum class ErrorType {
        InvalidFormat,
        MissingKey,
        TypeMismatch,
        ValidationFailed,
        FileIOError,
        EnvironmentVariableNotFound,
        OutOfRange
    };
    
    ConfigError(const std::string& message, ErrorType type = ErrorType::InvalidFormat)
        : std::runtime_error(message), errorType(type) {}
    
    ErrorType getErrorType() const { return errorType; }
    
private:
    ErrorType errorType;
};

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::map<std::string, nlohmann::json> // Nested config
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    EnvironmentVariable
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

// Type alias for nested configuration
using NestedConfig = std::map<std::string, ConfigValue>;

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
    
    // Load from file (JSON/YAML format, with backward compatibility for key=value)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
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
    
    // Environment variable support
    void setFromEnvironment(const std::string& key, const std::string& envVarName);
    bool isFromEnvironment(const std::string& key) const;
    
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
    
    // Validation (extended with advanced validation system)
    bool validate(const std::string& key) const;
    void addValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator);
    
    // Advanced validation system integration
    ValidationResult validateAdvanced(const std::string& key, ValidationError::ErrorSeverity minSeverity = ValidationError::ErrorSeverity::Error) const;
    ValidationResult validateAll(ValidationError::ErrorSeverity minSeverity = ValidationError::ErrorSeverity::Error) const;
    ValidationResult validateWithSchema(const nlohmann::json& schema, ValidationError::ErrorSeverity minSeverity = ValidationError::ErrorSeverity::Error) const;
    
    void addAdvancedValidator(const std::string& key, std::unique_ptr<Validator> validator, bool override = false);
    void removeAdvancedValidator(const std::string& key, const std::string& validatorName);
    
    // Configuration validation with context
    ValidationResult validateWithContext(const std::unordered_map<std::string, ConfigValue>& context, 
                                         ValidationError::ErrorSeverity minSeverity = ValidationError::ErrorSeverity::Error) const;
    
    // Validation management
    void setValidationEnabled(bool enabled);
    void setValidationThreadSafe(bool threadSafe);
    bool isValidationEnabled() const;
    bool isValidationThreadSafe() const;
    
    // Get validation metrics
    ValidationManager::ValidationMetrics getValidationMetrics() const;
    void resetValidationMetrics() const;
    
    // Expert validation features
    ValidationResult validatePartial(bool continueOnError = true, ValidationError::ErrorSeverity minSeverity = ValidationError::ErrorSeverity::Error) const;
    ValidationResult validateWithSeverity(ValidationError::ErrorSeverity minSeverity) const;
    ValidationResult validateCritical() const;
    
// Parse configuration from a file with format detection
    bool parseFromFile(const std::string& filepath, const std::string& format = "auto");
    
    // Parse configuration from string (JSON/YAML/key=value)
    bool parseConfigString(const std::string& configString, const std::string& format = "auto");
    
    // Get raw value for debugging
    ConfigValue getRawValue(const std::string& key) const;
    
    // Convert to JSON/YAML for serialization
    nlohmann::json toJSON() const;
    void toYAML(YAML::Emitter& emitter) const;
    
    // Parse JSON/YAML for deserialization
    static ConfigValue jsonToConfigValue(const nlohmann::json& j);
    static ConfigValue yamlToConfigValue(const YAML::Node& node);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

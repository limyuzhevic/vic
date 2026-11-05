#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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

// Validation error codes
enum class ValidationErrorCode {
    Success,
    OutOfRange,
    InvalidFormat,
    MissingRequired,
    ConflictingValues
};

// Validation severity
enum class ValidationSeverity {
    Warning,
    Error
};

// Validation error details
struct ConfigValidationError {
    std::string key;
    ValidationErrorCode code;
    ValidationSeverity severity;
    std::string message;
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

// Validation function type
using ValidationFunc = std::function<bool(const ConfigEntry&, std::vector<ConfigValidationError>&)>;

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
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Validation methods
    static bool validateConfig(const Config& config, std::vector<ConfigValidationError>& errors);
    static bool validateEntry(const ConfigEntry& entry, std::vector<ConfigValidationError>& errors);
    static bool validateIntValue(const std::string& key, int64_t value, ConfigValidationError& error);
    static bool validateFloatValue(const std::string& key, float value, ConfigValidationError& error);
    static bool validateDoubleValue(const std::string& key, double value, ConfigValidationError& error);
    static bool validateStringValue(const std::string& key, const std::string& value, ConfigValidationError& error);
    static bool validateBoolValue(const std::string& key, bool value, ConfigValidationError& error);
    static bool validateRangeBasedOnKey(const std::string& key, const ConfigValue& value, ConfigValidationError& error);
    static std::string getValidationErrorMessage(ValidationErrorCode code);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Configuration validation
    bool loadFromFileWithValidation(const std::string& filepath);
    bool loadFromArgsWithValidation(int argc, char** argv);
    
    friend class ConfigValidationHelper;
};

// Validation helper class for friend access
class ConfigValidationHelper {
public:
    static bool validateConfigEntry(const Config::ConfigEntry& entry, std::vector<Config::ConfigValidationError>& errors);
};

} // namespace nlm

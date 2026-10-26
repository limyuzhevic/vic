#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_set>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <sstream>

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

// Configuration validation exceptions
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) 
        : std::runtime_error("Configuration validation error: " + message) {}
};

class ConfigValidationWarning : public std::runtime_error {
public:
    ConfigValidationWarning(const std::string& message)
        : std::runtime_error("Configuration warning: " + message) {}
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Enhanced configuration class with validation
class ValidatedConfig : public Config {
private:
    std::vector<std::string> validationWarnings;
    std::vector<std::string> validationErrors;
    
public:
    ValidatedConfig();
    
    // Disable copying, enable moving
    ValidatedConfig(const ValidatedConfig&) = delete;
    ValidatedConfig& operator=(const ValidatedConfig&) = delete;
    ValidatedConfig(ValidatedConfig&&) noexcept;
    ValidatedConfig& operator=(ValidatedConfig&&) noexcept;
    
    // Override set method with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) override;
    
    // Override set methods with type-specific validation
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) override;
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) override;
    
    // Load from file with validation
    bool loadFromFile(const std::string& filepath) override;
    
    // Get validation warnings
    const std::vector<std::string>& getValidationWarnings() const;
    
    // Get validation errors
    const std::vector<std::string>& getValidationErrors() const;
    
    // Clear validation history
    void clearValidationHistory();
    
    // Validate all current parameters
    void validateAllParameters();
    
    // Get any ConfigValue
    std::optional<ConfigValue> getAnyValue(const std::string& key) const;
    
private:
    // Configuration validation
    static bool isValidParameter(const std::string& key);
    static void validateParameterType(const std::string& key, const ConfigValue& value);
    static void validateParameterRange(const std::string& key, const ConfigValue& value);
    static void validateParameterDependencies(const std::string& key, const ConfigValue& value);
    
    // Value extraction helpers
    static int64_t getIntValue(const ConfigValue& value);
    static double getFloatValue(const ConfigValue& value);
    static std::string getStringValue(const ConfigValue& value);
};

// Configuration file error handling
class ConfigFileError : public std::runtime_error {
public:
    ConfigFileError(const std::string& message) 
        : std::runtime_error("Configuration file error: " + message) {}
};

class ConfigFileNotFoundError : public ConfigFileError {
public:
    ConfigFileNotFoundError(const std::string& filepath)
        : ConfigFileError("Configuration file not found: " + filepath) {}
};

class ConfigParseError : public ConfigFileError {
public:
    ConfigParseError(const std::string& message)
        : ConfigFileError("Configuration file parsing error: " + message) {}
};

// Safe configuration loading function
std::shared_ptr<ValidatedConfig> loadConfigSafe(const std::string& filepath = "configs/default.cfg");

// Configuration validation helper function
bool validateConfig(const ValidatedConfig& config);

// Configuration backup helper function
bool backupConfig(const std::string& originalPath, const std::string& backupPath = "configs/backup.cfg");

// Configuration recovery helper function
bool recoverConfig(const std::string& backupPath, const std::string& targetPath);

// Export enhanced config class as default
using Config = ValidatedConfig;

} // namespace nlm

// Include implementation headers
#include "src/core/Config/Config.cpp"
#include "src/core/Config/ValidatedConfig.cpp"

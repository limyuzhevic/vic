#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <nlohmann/json.hpp>  // JSON parser library

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
    
    // Convert config to JSON
    nlohmann::json toJson() const;
    
    // Load from JSON
    static std::shared_ptr<Config> fromJson(const nlohmann::json& json);
    
    // Configuration validation
    
    // Validate a configuration key
    static bool validateKey(const std::string& key, std::string& errorMessage);
    
    // Validate a configuration value for a specific key
    static bool validateValue(const std::string& key, const ConfigValue& value, std::string& errorMessage);
    
    // Check if configuration value is within acceptable range
    static bool isValidRange(const std::string& key, const ConfigValue& value);
    
    // Get validation warning for a specific key/value combination
    static std::string getValidationWarning(const std::string& key, const ConfigValue& value);
    
    // Validate all current configuration entries
    std::vector<std::string> validateAll() const;
    
    // Get configuration parameter descriptions
    static std::string getParameterDescription(const std::string& key);
    
    // Get all valid parameter keys
    static std::vector<std::string> getValidParameterKeys();
    
    // Check if key is a valid configuration parameter
    static bool isValidParameterKey(const std::string& key);
    
    // Get parameter type for validation
    static std::string getParameterType(const std::string& key);
    
    // Validate parameter-specific constraints
    static bool validateParameterConstraints(const std::string& key, const ConfigValue& value);
    
    // Get warning if parameter value is suspicious but still valid
    static std::string getSuspicionWarning(const std::string& key, const ConfigValue& value);
    
    // Validate and log warnings for all parameters
    void validateAndWarn() const;
    
    // Clear all validation warnings
    void clearValidationWarnings();
    
    // Get all validation warnings
    std::vector<std::string> getAllValidationWarnings() const;
};

} // namespace nlm
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Configuration validation system
struct ConfigValidationError {
    std::string key;
    std::string message;
    enum class ErrorType { Value, Range, Type, Missing } type;
};

class ConfigValidation {
public:
    // Validate a configuration key's value
    static bool validateKey(const std::string& key, const ConfigValue& value, 
                          std::vector<ConfigValidationError>& errors);
    
    // Validate all configuration values
    static bool validateAll(const Config& config, std::vector<ConfigValidationError>& errors);
    
    // Check if key exists with required type
    static bool requiresKey(const std::string& key, const std::type_index& type);
    
    // Check value range
    static bool withinRange(const ConfigValue& value, double min, double max);
    
    // Get list of all valid configuration keys
    static std::vector<std::string> getValidKeys();
    
    // Get key type constraints
    static std::type_index getKeyType(const std::string& key);
    
    // Get key validation constraints
    static bool hasRangeConstraint(const std::string& key, double& min, double& max);
    static bool hasValueConstraint(const std::string& key, const std::string& constraint);
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::type_index type;
    bool required;
    double minValue;
    double maxValue;
    std::string description;
    bool experimental;
};

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

} // namespace nlm

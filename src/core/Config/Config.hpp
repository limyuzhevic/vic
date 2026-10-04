#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
#include <functional>
#include <limits>
#include <sstream>

namespace nlm {

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

// Configuration validation exceptions
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigTypeError : public ConfigValidationError {
public:
    ConfigTypeError(const std::string& key, const std::string& expectedType) 
        : ConfigValidationError("Configuration key \"" + key + "\" has wrong type. Expected: " + expectedType) {}
};

class ConfigRangeError : public ConfigValidationError {
public:
    ConfigRangeError(const std::string& key, double value, double min, double max)
        : ConfigValidationError("Configuration key \"" + key + "\" has invalid value " + std::to_string(value) + ". Must be between " + 
                              std::to_string(min) + " and " + std::to_string(max)) {}
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

// Configuration parameter metadata
struct ConfigParameter {
    std::string key;
    std::string description;
    ConfigSource source;
    bool required;
    bool hasRange;
    double minValue;
    double maxValue;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigParameter(const std::string& k = "", const std::string& desc = "", ConfigSource src = ConfigSource::Default,
                   bool req = false, double min = std::numeric_limits<double>::lowest(),
                   double max = std::numeric_limits<double>::max())
        : key(k), description(desc), source(src), required(req), hasRange(false),
          minValue(min), maxValue(max) {
        validator = nullptr;
    }
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation methods
    bool validateValue(const ConfigEntry& entry) const;
    bool validateType(const std::string& key, const ConfigValue& value, const ConfigParameter& param) const;
    bool validateRange(const std::string& key, const ConfigValue& value, const ConfigParameter& param) const;
    void validateAndSet(const std::string& key, const ConfigValue& value, ConfigSource source);
    
    // Initialize default parameter schema
    void initializeDefaultParameters();
};

} // namespace nlm

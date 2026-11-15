#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include <sstream>
#include <fstream>

namespace nlm {

// Forward declarations
class Config;

// Configuration exception classes
class ConfigException : public std::runtime_error {
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileError : public ConfigException {
    explicit ConfigFileError(const std::string& message) : ConfigException("Config file error: " + message) {}
};

class ConfigParsingError : public ConfigException {
    explicit ConfigParsingError(const std::string& message) : ConfigException("Config parsing error: " + message) {}
};

class ConfigValidationError : public ConfigException {
    explicit ConfigValidationError(const std::string& message) : ConfigException("Config validation error: " + message) {}
};

// Configuration validation helper functions
namespace ConfigValidation {
    // Validate key name
    void validateKey(const std::string& key);
    
    // Validate value for key
    template<typename T>
    void validateValue(const std::string& key, const T& value);
    
    // Validate range for numeric values
    template<typename T>
    void validateRange(const std::string& key, const T& value, T min, T max);
    
    // Convert string to value with validation
    template<typename T>
    T parseValue(const std::string& str);
}

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
    void loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    void loadFromArgs(int argc, char** argv);
    
    // Save to file
    void saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    T get(const std::string& key) const;
    
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
    static bool fileExists(const std::string& filepath);
    
    // Validation helpers
    static void validateKey(const std::string& key);
    static void validateKeyName(const std::string& key);
};

} // namespace nlm
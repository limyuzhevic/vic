#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include "../Errors/Errors.hpp"

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

// Main configuration class with comprehensive error handling and validation
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
    bool loadFromFile(const std::string& filepath) throw(ConfigLoadException);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv) throw(ConfigLoadException);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const throw(ConfigLoadException);
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const throw(ConfigValidationException);
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) throw(ConfigValidationException);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) throw(ConfigValidationException);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) throw(ConfigValidationException);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) throw(ConfigValidationException);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) throw(ConfigValidationException);
    
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
    void validateKey(const std::string& key) const throw(ConfigValidationException);
    void validateValue(const std::string& key, const ConfigValue& value) const throw(ConfigValidationException);
    void validateFilePath(const std::string& filepath) const throw(ConfigValidationException);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidFilePath(const std::string& filepath);
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value);
};

} // namespace nlm

} // namespace nlm

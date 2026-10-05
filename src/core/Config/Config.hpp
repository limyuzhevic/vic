#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Configuration source enum with documentation:
// - Default: Built-in default configuration
// - File: Loaded from configuration file
// - CommandLine: Loaded from command line arguments  
// - Runtime: Set during runtime via API

// Validation helper functions
static bool isValidKey(const std::string& key);
static bool isValidValue(const ConfigValue& value);

// Configuration entry with source tracking and validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // Validate entry consistency
    bool isValid() const {
        return !key.empty() && isValidValue(value);
    }
};

// Configuration source enum documentation:
// - Default: Built-in default configuration
// - File: Loaded from configuration file
// - CommandLine: Loaded from command line arguments
// - Runtime: Set during runtime via API

// Configuration value types:
// - Basic types: int, int64_t, double, bool, std::string
// - Collections: std::vector<int>, std::vector<double>, std::vector<std::string>
// All values stored in std::variant for type flexibility

// Main configuration class with improved validation and error handling
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving with exception safety
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format) with validation and error handling
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with parsing validation
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with proper serialization
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety and error handling
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence with validation
    bool has(const std::string& key) const;
    
    // Remove key with error handling
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all entries
    void clear();
    
    // Get configuration summary with error handling
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers with error handling
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value);
};

} // namespace nlm

// Configuration validation utility functions
namespace nlm {
namespace ConfigValidation {
    
    // Validate configuration key
    // Rules: non-empty, reasonable length, no illegal characters
    bool isValidKey(const std::string& key);
    
    // Validate configuration value based on type
    bool isValidValue(const ConfigValue& value);
    
    // Helper to check if value matches expected type
    template<typename T>
    bool isValueOfType(const ConfigValue& value);
    
    // Get error message for invalid configuration
    std::string getValidationError(const std::string& key, const ConfigValue& value);
}
}

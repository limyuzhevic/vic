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

// Configuration value types - more comprehensive
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::map<std::string, std::string>  // JSON-like object support
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry with enhanced features
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable bool validated;  // Track validation state
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validated(false) {}
    
    // Validation function
    std::function<bool(const ConfigEntry&)> validator;
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    ValidationResult(bool v = true, const std::string& msg = "") 
        : valid(v), errorMessage(msg) {}
    
    static ValidationResult success() { return ValidationResult(true); }
    static ValidationResult error(const std::string& msg) { return ValidationResult(false, msg); }
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced file loading with multiple format support
    bool loadFromFile(const std::string& filepath);
    
    // Enhanced argument loading with better parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Enhanced save to file with proper formatting
    bool saveToFile(const std::string& filepath) const;
    
    // Enhanced getter with type validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced setter with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation and type conversion methods
    ValidationResult validate() const;
    ValidationResult validateKey(const std::string& key) const;
    bool has(const std::string& key) const;
    
    // Remove key and all its aliases
    void remove(const std::string& key);
    
    // Get all keys (with optional filter)
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(const std::string& prefix) const;
    
    // Clear all configuration
    void clear();
    
    // Get configuration summary with validation status
    std::string summary() const;
    
    // Get source information for a key
    ConfigSource getSource(const std::string& key) const;
    bool isOverridden(const std::string& key) const;
    
    // Validation and type conversion helpers
    template<typename T>
    static std::optional<T> tryConvert(const ConfigValue& value);
    
    template<typename T>
    static bool validateType(const ConfigValue& value);
    
    // Bulk operations for performance
    void apply(const std::map<std::string, ConfigValue>& values);
    void apply(const Config& other);
    
    // Copy configuration with source filtering
    Config copy(ConfigSource source) const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isBoolean(const std::string& str);
    static ConfigValue parseValue(const std::string& str, ConfigSource source);
    
    // Validation helpers
    ValidationResult validateEntry(const ConfigEntry& entry) const;
    void markAllUnvalidated() const;
};

} // namespace nlm

} // namespace nlm

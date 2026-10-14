#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>

namespace nlm {

// Configuration error types
enum class ConfigError {
    Success,
    FileNotFound,
    PermissionDenied,
    InvalidFormat,
    InvalidKey,
    InvalidValue,
    MissingRequiredKey,
    ValueOutOfRange,
    MemoryAllocationFailed,
    UnknownError
};

// Configuration exception
class ConfigException : public std::exception {
public:
    explicit ConfigException(const std::string& message) : message_(message) {}
    explicit ConfigException(ConfigError error, const std::string& context = "") 
        : error_(error), context_(context) {}
    
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    ConfigError error() const noexcept { return error_; }
    const std::string& context() const noexcept { return context_; }
    
private:
    std::string message_;
    ConfigError error_ = ConfigError::UnknownError;
    std::string context_;
};

// Input validation utilities
namespace ConfigValidation {
    // Validate key name format
    bool isValidKey(const std::string& key);
    
    // Validate numeric value ranges
    bool isValidValue(const std::string& key, const std::string& value);
    bool isValidValue(const std::string& key, int value);
    bool isValidValue(const std::string& key, double value);
    
    // Validate vector values
    bool isValidVector(const std::string& key, const std::vector<std::string>& values);
    
    // Get validation error message for specific key/value
    std::string getValidationError(const std::string& key, const std::string& value);
}

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

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <string>

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

// Configuration file format
enum class ConfigFormat {
    Unknown,
    INI,
    JSON,
    YAML
};

// Error codes for configuration operations
enum class ConfigError {
    None,
    FileNotFound,
    ParseError,
    InvalidFormat,
    KeyNotFound,
    TypeMismatch,
    InvalidValue
};

// Configuration result with error information
struct ConfigResult {
    bool success;
    ConfigError error;
    std::string errorMessage;
    
    ConfigResult() : success(true), error(ConfigError::None), errorMessage() {}
    ConfigResult(bool s, ConfigError e, const std::string& msg = "") 
        : success(s), error(e), errorMessage(msg) {}
    
    static ConfigResult Success() { return ConfigResult(true, ConfigError::None); }
    static ConfigResult Error(ConfigError e, const std::string& msg) { 
        return ConfigResult(false, e, msg); }
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
    
    // Load from file with format detection
    ConfigResult loadFromFile(const std::string& filepath);
    ConfigResult loadFromFile(const std::string& filepath, ConfigFormat format);
    
    // Load from string
    ConfigResult loadFromString(const std::string& content, ConfigFormat format);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    ConfigResult saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with error handling
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
    
    // Get last error
    const std::string& getLastError() const;
    
    // Get file format from extension
    static ConfigFormat detectFormat(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigError parseValue(const std::string& valueStr, ConfigValue& outValue);
    static std::string configValueToString(const ConfigValue& value) const;
};

} // namespace nlm

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Logger;

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
    
    // Validate configuration entry
    bool isValid() const {
        return !key.empty();
    }
};

// Configuration exceptions
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileException : public ConfigException {
public:
    ConfigFileException(const std::string& filepath, const std::string& message) 
        : ConfigException("Config file error in '" + filepath + "': " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& key, const std::string& message)
        : ConfigException("Configuration validation error for key '" + key + "': " + message) {}
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
    
    // Get values with error checking
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get values with validation
    template<typename T>
    T getValidated(const std::string& key, const T& defaultValue,
                   const std::function<bool(const T&)>& validator = nullptr,
                   const std::string& errorMessage = "Invalid configuration value") const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values with validation
    void setValidated(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
                      const std::function<bool(const std::string&)>& validator = nullptr,
                      const std::string& errorMessage = "Invalid configuration value");
    
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
    
    // Validate entire configuration
    std::vector<std::string> validate() const;
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t fileEntries;
        size_t commandLineEntries;
        size_t runtimeEntries;
        std::vector<std::string> warnings;
    };
    
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isBoolean(const std::string& str);
};

} // namespace nlm

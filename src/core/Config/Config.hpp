#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <expected>
#include <any>
#include <functional>
#include <stdexcept>

namespace nlm {

// Configuration exceptions
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message)
        : std::runtime_error("Config Error: " + message) {}
};

class ConfigFileError : public ConfigException {
public:
    explicit ConfigFileError(const std::string& message)
        : ConfigException("File Error: " + message) {}
};

class ConfigValidationError : public ConfigException {
public:
    explicit ConfigValidationError(const std::string& message)
        : ConfigException("Validation Error: " + message) {}
};

class ConfigParseError : public ConfigException {
public:
    explicit ConfigParseError(const std::string& message)
        : ConfigException("Parse Error: " + message) {}
};

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
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
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
    std::expected<bool, std::string> loadFromFileWithError(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get any configuration value
    std::optional<ConfigValue> getAny(const std::string& key) const;
    
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
    
    // Get configuration value with error handling
    template<typename T>
    std::expected<T, std::string> getWithError(const std::string& key) const {
        auto value = get<T>(key);
        if (!value.has_value()) {
            return std::unexpected("Configuration key \"" + key + "\" not found");
        }
        return *value;
    }
    
    // Set configuration value with validation
    template<typename T>
    bool setWithValidation(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime,
                           std::function<bool(const T&)> validator = nullptr) {
        if (validator && !validator(value)) {
            return false;
        }
        set(key, value, source);
        return true;
    }
    
    // Configuration schema validation
    struct ConfigSchema {
        std::string key;
        std::string type; // "int", "double", "string", "bool", "vector"
        std::string description;
        std::any defaultValue;
        bool required = false;
        std::function<bool(const std::any&)> validator;
    };
    
    // Validate configuration against schema
    bool validateSchema(const std::vector<ConfigSchema>& schema) const;
    
    // Utility methods
    std::string summary() const;
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};
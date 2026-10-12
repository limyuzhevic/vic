#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <map>
#include <regex>

namespace nlm {

// Forward declarations
class Config;

// JSON Schema for configuration validation
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "float", "string", "bool", "array", "object"
    std::string description;
    bool required = false;
    std::string defaultValue;
    std::map<std::string, std::string> validationRules;
    
    ConfigSchema() : key(), type(), description(), required(false) {}
    ConfigSchema(const std::string& k, const std::string& t, const std::string& d = "", bool req = false)
        : key(k), type(t), description(d), required(req) {}
};

// Configuration error hierarchy
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileError : public ConfigError {
public:
    explicit ConfigFileError(const std::string& filepath, const std::string& reason)
        : ConfigError("Config file error: " + filepath + " - " + reason) {}
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& key, const std::string& value, const std::string& reason)
        : ConfigError("Config parse error for key \"" + key + "\": value \"" + value + "\" - " + reason) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& key, const std::string& reason)
        : ConfigError("Config validation error for key \"" + key + "\": " + reason) {}
};

class ConfigIOError : public ConfigError {
public:
    explicit ConfigIOError(const std::string& filepath, const std::string& operation, const std::string& reason)
        : ConfigError("Config IO error: " + filepath + " - " + operation + " - " + reason) {}
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
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format with schema validation)
    void loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    void loadFromArgs(int argc, char** argv);
    
    // Save to file
    void saveToFile(const std::string& filepath) const;
    
    // Get values
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
    
    // Enhanced validation and error handling
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    bool validateAgainstSchema(const std::string& key, const ConfigValue& value) const;
    void validateSchema() const;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    void clearSchemas();
    bool hasSchema(const std::string& key) const;
    std::vector<ConfigSchema> getSchemas() const;
    std::string getSchemaSummary() const;
    
    // Configuration types
    enum class ConfigType {
        INTEGER,
        FLOAT,
        STRING,
        BOOLEAN,
        INT_ARRAY,
        FLOAT_ARRAY,
        STRING_ARRAY,
        UNKNOWN
    };
    
    ConfigType getConfigType(const std::string& key) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::map<ConfigSource, size_t> getSourceCounts() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static bool isBoolean(const std::string& str);
    static ConfigType inferType(const std::string& value);
    void validateKeyUniqueness(const std::string& key) const;
    void validateValueType(const std::string& key, const ConfigValue& value) const;
    std::string configValueToString(const ConfigValue& value) const;
    void validateSchema(const ConfigSchema& schema) const;
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <variant>
#include <nlohmann/json.hpp>

namespace nlm {

// Custom exception classes for configuration errors
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message)
        : std::runtime_error(message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message, const std::string& filepath = "")
        : ConfigException("ConfigParseError: " + message + (filepath.empty() ? "" : " (file: " + filepath + ")")) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message)
        : ConfigException("ConfigValidationError: " + message) {}
};

class ConfigFileException : public ConfigException {
public:
    explicit ConfigFileException(const std::string& message, const std::string& filepath = "")
        : ConfigException("ConfigFileError: " + message + (filepath.empty() ? "" : " (file: " + filepath + ")")) {}
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
    mutable std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), timestamp() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()) {}
};

// Validation error
struct ValidationError {
    std::string key;
    std::string message;
    
    ValidationError() : key(), message() {}
    ValidationError(const std::string& k, const std::string& m) : key(k), message(m) {}
};

// Main configuration class with thread safety and full JSON/YAML support
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
    
    // Set values with thread safety
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
    
    // Configuration validation
    void validate() const;
    bool hasErrors() const;
    const std::vector<ValidationError>& getErrors() const;
    
    // File format detection and parsing
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLFile(const std::string& filepath);
    static std::string detectFileFormat(const std::string& filepath);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigValue parseJSONValue(const nlohmann::json& jsonValue);
    static nlohmann::json convertConfigValueToJSON(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Parse JSON file
    bool parseJSON(const std::string& filepath);
    
    // Parse YAML file (fallback implementation)
    bool parseYAML(const std::string& filepath);
    
    // Parse key=value format (backward compatibility)
    bool parseKeyValue(const std::string& content);
    
    // Utility functions for value conversion
    static bool tryConvertStringToValue(const std::string& str, ConfigValue& outValue);
    static std::string configValueToString(const ConfigValue& value);
    
    // Internal helper methods
    void rebuildIndex();
    std::string timePointToString(const std::chrono::system_clock::time_point& tp) const;
};

} // namespace nlm

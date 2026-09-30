#include "core/ErrorHandling/ErrorHandling.hpp"
#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <filesystem>

namespace nlm {

// Forward declarations for JSON and YAML
class ConfigValidator;

// Enhanced configuration value types
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

// Configuration source tracking
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
    std::string type;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type("auto") {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& t = "auto")
        : key(k), value(v), source(s), description(desc), type(t) {}
};

// Exception classes for configuration errors
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigLoadException : public ConfigException {
public:
    ConfigLoadException(const std::string& message, const std::string& filepath)
        : ConfigException("Failed to load config from " + filepath + ": " + message) {}
};

class ConfigSaveException : public ConfigException {
public:
    ConfigSaveException(const std::string& message, const std::string& filepath)
        : ConfigException("Failed to save config to " + filepath + ": " + message) {}
};

class ConfigValidationException : public ConfigException {
public:
    ConfigValidationException(const std::string& message)
        : ConfigException("Configuration validation failed: " + message) {}
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
    
    // Load from file with automatic format detection (JSON, YAML, or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Save to file with automatic format detection (JSON, YAML, or simple key=value format)
    bool saveToFile(const std::string& filepath) const;
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Get values with type conversion
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
    
    // Get configuration summary
    std::string summary() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Add entry with type information
    void addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, 
                  const std::string& description = "", const std::string& type = "auto");
    
    // Get last error message
    std::string getLastError() const;
    
    // Get type of value
    std::string getType(const std::string& key) const;
    
    // Set value with automatic type detection from string
    void setAuto(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get value as string
    std::string getString(const std::string& key) const;
    
    // Configuration validation (basic implementation)
    bool validate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string lastError;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string getFileFormat(const std::string& filepath);
    
    // Type conversion utilities
    static std::string configValueToString(const ConfigValue& value);
    static std::string detectType(const std::string& str);
};
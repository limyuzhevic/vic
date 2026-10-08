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
    
    // Load from file (supports simple key=value format and JSON/YAML when available)
    // Returns true if file was successfully loaded, false otherwise
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    // Supports --key=value and -key value formats
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    // Returns true if file was successfully saved, false otherwise
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    // Returns std::nullopt if key doesn't exist or type doesn't match
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get value with default fallback
    // Returns defaultValue if key doesn't exist or type doesn't match
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with source tracking
    // source indicates where the value came from (Default, File, CommandLine, Runtime)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management
    // Check if a key exists
    bool has(const std::string& key) const;
    
    // Remove a key from configuration
    void remove(const std::string& key);
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration entries
    void clear();
    
    // Get configuration summary for logging/debugging
    std::string summary() const;
    
    // Validate configuration
    // Returns true if configuration is valid, false otherwise
    // Can be extended with specific validation rules
    bool validate() const;
    
    // Get configuration value type
    // Returns string representation of the value type for debugging
    std::string getValueType(const std::string& key) const;
    
    // Check configuration source
    // Returns the source of a configuration value
    ConfigSource getSource(const std::string& key) const;
    
    // List all configuration keys with their sources
    std::string listWithSources() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value);
    static std::string valueToString(const ConfigValue& value);
};

} // namespace nlm

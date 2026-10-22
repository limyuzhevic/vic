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
    
    // Helper functions for enhanced configuration management
    
    // Get configuration as specific type with validation
    template<typename T>
    T getValidated(const std::string& key, const T& defaultValue) const;
    
    // Check if configuration key exists with minimum source priority
    bool isType(const std::string& key, ConfigSource minSource) const;
    
    // Get all configuration values as strings for easier processing
    std::map<std::string, std::string> getAllAsStrings() const;
    
    // Check if configuration was modified from defaults
    bool isModified(const std::string& key) const;
    
    // Get configuration entries with specific source
    std::vector<std::pair<std::string, ConfigValue>> getBySource(ConfigSource source) const;
    
    // Validate critical configuration values
    bool validateCriticalValues() const;
    
    // Additional convenience methods for common use cases
    
    // Get configuration as string with default
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Get configuration as integer with default
    int getInt(const std::string& key, int defaultValue = 0) const;
    
    // Get configuration as float with default
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    
    // Get configuration as boolean with default
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    // Enhanced command line argument parsing with validation
    bool loadFromArgs(int argc, char** argv) override;

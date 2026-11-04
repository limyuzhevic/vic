#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <cstdint>

namespace nlm {

// Forward declarations
class Config;

// Type aliases for configuration values for better readability and maintainability
using ConfigInt = int;
using ConfigInt64 = int64_t;
using ConfigDouble = double;
using ConfigFloat = float;
using ConfigBool = bool;
using ConfigString = std::string;
using ConfigStringVector = std::vector<std::string>;
using ConfigIntVector = std::vector<int>;
using ConfigDoubleVector = std::vector<double>;

// Configuration value types - centralized for maintainability
using ConfigValue = std::variant<
    ConfigInt,
    ConfigInt64,
    ConfigDouble,
    ConfigFloat,
    ConfigBool,
    ConfigString,
    ConfigIntVector,
    ConfigDoubleVector,
    ConfigStringVector
>;

// Configuration source enum for tracking value origins
enum class ConfigSource {
    Default,    // Set as default value
    File,       // Loaded from configuration file
    CommandLine, // Loaded from command line arguments
    Runtime     // Set at runtime via API
};

// Configuration entry structure - contains value and metadata
struct ConfigEntry {
    std::string key;           // Configuration key/name
    ConfigValue value;         // The actual configuration value
    ConfigSource source;       // How this value was obtained
    std::string description;   // Human-readable description of the config option
    
    // Default constructor
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    // Full constructor with all fields
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Helper functions for configuration management
namespace config_helpers {
    // Check if a key exists in configuration entries
    inline bool keyExists(const std::string& key, const std::vector<ConfigEntry>& entries) {
        return std::any_of(entries.begin(), entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    }
    
    // Convert ConfigSource to string for debugging/logging
    inline const char* sourceToString(ConfigSource source) {
        switch (source) {
            case ConfigSource::Default: return "Default";
            case ConfigSource::File: return "File";
            case ConfigSource::CommandLine: return "CommandLine";
            case ConfigSource::Runtime: return "Runtime";
            default: return "Unknown";
        }
    }
    
    // Check if configuration entry is valid
    inline bool isValidEntry(const ConfigEntry& entry) {
        return !entry.key.empty();
    }
}

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
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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

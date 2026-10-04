#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <set>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,                    // Integer values (32-bit)
    int64_t,                // 64-bit integer values
    double,                 // Floating point values
    bool,                   // Boolean values
    std::string,            // String values
    std::vector<int>,       // Integer arrays
    std::vector<double>,    // Float arrays
    std::vector<std::string> // String arrays
>;

// Configuration source enumeration
enum class ConfigSource {
    Default,    // Default configuration value
    File,       // Loaded from configuration file
    CommandLine,// Set via command line arguments
    Runtime     // Set at runtime via API
};

// Configuration entry structure
struct ConfigEntry {
    std::string key;        // Configuration key name
    ConfigValue value;      // Configuration value
    ConfigSource source;    // How the value was set
    std::string description; // Human-readable description of the parameter
    
    // Default constructor
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    // Parameterized constructor
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
    
    // Validation methods
    bool validateEntry(const std::string& key, const ConfigValue& value);
    void registerValidator(const std::string& key, 
                           std::function<bool(const ConfigValue&)> validator);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

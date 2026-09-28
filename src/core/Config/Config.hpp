#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <type_traits>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types - stores various data types commonly used in neural simulation
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

// Configuration source indicates how a configuration value was set
enum class ConfigSource {
    Default,        // Built-in default values
    File,           // Loaded from configuration file
    CommandLine,    // Set via command line arguments
    Runtime        // Set dynamically during runtime
};

// Configuration entry stores a key-value pair with metadata
struct ConfigEntry {
    std::string key;           // Configuration key name
    ConfigValue value;         // Configuration value (variant type)
    ConfigSource source;       // How the value was obtained
    std::string description;   // Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class - manages neural simulation parameters and settings
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying to prevent unintended side effects, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from file (supports simple key=value format)
    // Returns true if file was successfully loaded, false otherwise
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Supports --key=value and -key value formats
    bool loadFromArgs(int argc, char** argv);
    
    // Save current configuration to file
    // Note: Currently writes placeholder values for all settings
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration value with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default fallback
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value using ConfigValue variant
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters for common types - reduces boilerplate
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management methods
    bool has(const std::string& key) const;                    // Check if key exists
    void remove(const std::string& key);                       // Remove key-value pair
    std::vector<std::string> getKeys() const;                  // Get all configuration keys
    void clear();                                              // Clear all configuration
    
    // Generate human-readable configuration summary
    std::string summary() const;
    
private:
    struct Impl;                                              // Private implementation
    std::unique_ptr<Impl> pImpl;                               // PImpl idiom for forward declarations
    
    // Internal helper methods
    static std::string trim(const std::string& str);          // Remove whitespace from string
    static std::string toLower(const std::string& str);       // Convert string to lowercase
    
    // Private constructor for implementation details
    Config(std::unique_ptr<Impl> impl) : pImpl(std::move(impl)) {}
};

} // namespace nlm

// Include Config.cpp for template instantiations
#include "Config.cpp"

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
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration source enumeration indicating how the value was set
enum class ConfigSource {
    Default,    // Default value set by the application
    File,       // Loaded from configuration file (JSON/YAML)
    CommandLine, // Set via command line arguments
    Runtime     // Set at runtime via API calls
};

// Configuration entry structure storing a single configuration key-value pair
struct ConfigEntry {
    std::string key;            // Configuration key
    ConfigValue value;          // Configuration value (variant supports multiple types)
    ConfigSource source;        // Source of this configuration value
    std::string description;    // Human-readable description of the configuration option
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class for NLM (Neural Learning Machine)
// Provides persistent storage and management of configuration parameters
// Supports multiple file formats (JSON, YAML) and command-line arguments
// Thread-safe for concurrent access
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying to prevent shallow copies of configuration
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Enable moving for efficient transfers
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from file (supports JSON, YAML, and simple key=value formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from simple key=value format (fallback method)
    bool loadFromFileSimple(const std::string& filepath);
    
    // Load configuration from command-line arguments (supports --key=value and -key value formats)
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file in human-readable key=value format
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration value with type checking
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default fallback
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value with source tracking
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check if configuration key exists
    bool has(const std::string& key) const;
    
    // Remove configuration key
    void remove(const std::string& key);
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration entries
    void clear();
    
    // Generate human-readable configuration summary
    std::string summary() const;
    
private:
    struct Impl;  // Pimpl pattern for forward declaration
    std::unique_ptr<Impl> pImpl;  // Pointer to implementation
    
    // Internal string processing utilities
    static std::string trim(const std::string& str);  // Remove leading/trailing whitespace
    static std::string toLower(const std::string& str);  // Convert to lowercase
};

} // namespace nlm

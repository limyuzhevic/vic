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

// Configuration entry
// Key-value pair for brain simulation parameters
struct ConfigEntry {
    std::string key;           // Configuration key (e.g., "brain.neuron_count")
    ConfigValue value;         // Configuration value (variant type)
    ConfigSource source;       // Where this configuration came from
    std::string description;   // Human-readable description of the parameter
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration source
// Enum indicating where configuration values originate from
enum class ConfigSource {
    Default,    // Hardcoded defaults from source code
    File,       // Loaded from configuration file
    CommandLine, // Parsed from command line arguments
    Runtime     // Set at runtime via API
};

// Main configuration class for brain simulation
// Manages simulation parameters including neuron count, connectivity, timing,
// plasticity settings, and neuromodulation parameters
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    // Prevents accidental copies of configuration state
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Configuration loading
    // Load configuration from JSON file with validation
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Common patterns: --brain.neuron_count 1000, --plasticity.enable true
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Configuration value access
    // Get configuration value with type checking
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default fallback
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration value modification
    // Set configuration value with source tracking
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration inspection
    // Check if key exists in configuration
    bool has(const std::string& key) const;
    
    // Remove configuration key
    void remove(const std::string& key);
    
    // Get all configuration keys
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration values
    void clear();
    
    // Configuration summary and debugging
    // Generate human-readable summary of configuration
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

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
// Defines the set of types that can be stored as configuration values
// Each configuration key-value pair uses this variant type, allowing flexibility
// while maintaining type safety through the get() and set() methods.
using ConfigValue = std::variant<
    int,                    // 32-bit integer (e.g., neuron_count, region_count)
    int64_t,                // 64-bit integer (e.g., random_seed)
    double,                 // Double-precision floating point (e.g., simulation_timestep)
    float,                  // Single-precision floating point (e.g., connection_probability)
    bool,                   // Boolean value (e.g., enable_logging)
    std::string,            // String value (e.g., log_filename, environment_name)
    std::vector<int>,       // Vector of integers (e.g., list of neuron IDs)
    std::vector<double>,    // Vector of doubles (e.g., parameter ranges)
    std::vector<std::string> // Vector of strings (e.g., list of concept names)
>;

// Configuration source
// Tracks where configuration values originated from, enabling priority resolution
// when the same key is set from multiple sources (File > CommandLine > Default)
enum class ConfigSource {
    Default,    // Built-in default values (lowest priority)
    File,       // Loaded from configuration file
    CommandLine, // Set via command-line arguments
    Runtime     // Set programmatically at runtime (highest priority)
};

// Configuration entry
// Represents a single key-value pair in the configuration system
struct ConfigEntry {
    std::string key;           // Configuration key (e.g., "neuron_count")
    ConfigValue value;         // Stored value (type depends on key)
    ConfigSource source;       // Source of this value
    std::string description;   // Human-readable description for documentation
    
    // Default constructor creates an empty entry
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    // Constructor with description parameter
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
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
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

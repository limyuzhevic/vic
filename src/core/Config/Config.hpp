#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types supported by the system
// Can hold primitive types, containers, or string values
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

// Configuration source priority
// Determines the origin of configuration values and their precedence
enum class ConfigSource {
    Default,      // Built-in defaults in the code
    File,         // Loaded from configuration file
    CommandLine,  // Set via command line arguments
    Runtime       // Set at runtime via API calls
};

// Individual configuration entry with metadata
// Contains the key-value pair plus source information for precedence handling
struct ConfigEntry {
    std::string key;                    // Configuration key (dot-notation for nested values)
    ConfigValue value;                  // The actual configuration value
    ConfigSource source;                // Source of this configuration value
    std::string description;            // Human-readable description for documentation
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration management class
// Provides centralized configuration storage with support for multiple sources,
// type-safe access, and persistence. Uses a key-value style with dot-notation
// for nested configuration (e.g., "brain.neuron_count" or "plasticity.stdp.enable")
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying for memory safety, enable moving semantics
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Configuration persistence
    // Load configuration from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Parses arguments like "--key=value" or "--key value"
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Type-safe value retrieval
    // Returns nullopt if key doesn't exist
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Convenience method with default value
    // Returns defaultValue if key doesn't exist
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration value setting
    // Supports all ConfigValue types with automatic type conversion
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration existence and management
    // Check if configuration key exists
    bool has(const std::string& key) const;
    
    // Remove configuration key
    void remove(const std::string& key);
    
    // Get all configuration keys (for documentation or debugging)
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration (removes file, command line, and runtime settings)
    void clear();
    
    // Generate human-readable configuration summary
    std::string summary() const;
    
private:
    // Pimpl idiom for implementation hiding
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal utility functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

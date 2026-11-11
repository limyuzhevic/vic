#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Configuration value types
// The ConfigValue variant stores different types of configuration values,
// allowing flexible configuration of various parameters throughout the system.
// Supported types include:
// - Fundamental types: int, int64_t, double, bool, std::string
// - Container types: std::vector<int>, std::vector<double>, std::vector<std::string>
// - Each ConfigEntry stores the actual value, its source, and optional description
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
// Enum indicating how a configuration parameter was set, used for precedence
// resolution when multiple sources provide the same key:
// - Default: Built-in default values (lowest precedence)
// - File: Loaded from configuration file
// - CommandLine: Set via command line arguments
// - Runtime: Set programmatically at runtime (highest precedence)
enum class ConfigSource {
    Default,      // Built-in default values
    File,         // Loaded from configuration file
    CommandLine,  // Set via command line arguments  
    Runtime       // Set programmatically at runtime
};

// Configuration entry
// Represents a single configuration parameter with its value, source, and optional
// description for documentation and debugging purposes.
struct ConfigEntry {
    std::string key;              // Configuration parameter name (case-sensitive)
    ConfigValue value;            // The actual configuration value
    ConfigSource source;          // How this value was set (for precedence resolution)
    std::string description;      // Human-readable description of the parameter
    
    // Default constructor
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    // Parameterized constructor
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
// Provides a centralized configuration system for all NLM components, supporting
// multiple sources of configuration values with precedence resolution, type-safe
// access, and persistence to disk.
//
// Configuration system features:
// 1. Multiple configuration sources with precedence (Runtime > CommandLine > File > Default)
// 2. Type-safe value access with automatic conversion between numeric types
// 3. File-based configuration (Phase 2 placeholder supports simple key=value format)
// 4. Command-line configuration via --key=value or -key value syntax
// 5. Programmatic configuration via set() methods
// 6. Configuration persistence to disk (Phase 2 placeholder saves default values only)
// 7. Configuration validation and documentation support
// 8. Summary and debugging capabilities
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from file (Phase 2 supports simple key=value format)
    // 
    // File format: Simple key=value pairs, optionally with comments starting with # or /
    // Quoted values are automatically stripped of quotes during parsing
    // Returns true if file was successfully loaded, false if file doesn't exist or cannot be opened
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Supports both --key=value and -key value syntax for command-line overrides
    // Command line arguments override file and default values when conflicts occur
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    // Phase 2 placeholder - currently saves default values only, with placeholder values
    // In Phase 3+, this would save actual configured values with descriptions
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety (defined here for template instantiation)
    // The configuration system supports automatic conversion between compatible numeric types
    // (e.g., stored double can be retrieved as float). Non-arithmetic types require exact match.
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration values with explicit source for precedence control
    // Multiple overloads support different value types and allow specifying the source
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check if a configuration key exists
    bool has(const std::string& key) const;
    
    // Remove a configuration key
    void remove(const std::string& key);
    
    // Get all configuration keys (useful for documentation and debugging)
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration entries
    void clear();
    
    // Get configuration summary for debugging
    // Provides a human-readable summary of all configuration entries including
    // their values and sources, useful for configuration debugging
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

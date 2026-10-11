#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Configuration value types
// Defines the variant types that can be stored in configuration entries
// Supports simple types (int, bool, string) and collections (vectors)
// Used for flexible configuration management with type safety
using ConfigValue = std::variant<
    int,                     // 32-bit signed integer
    int64_t,                 // 64-bit signed integer
    double,                  // Floating-point number
    bool,                    // Boolean flag
    std::string,             // Text string
    std::vector<int>,        // Integer array
    std::vector<double>,     // Floating-point array
    std::vector<std::string> // String array
>;

// Configuration source enumeration - tracks how configuration values were set
// Helps resolve conflicts when the same key is set from multiple sources
enum class ConfigSource {
    Default,     // Value from default configuration
    File,        // Value loaded from configuration file
    CommandLine, // Value from command-line arguments
    Runtime      // Value set at runtime via API
};

// Configuration entry - represents a single configuration parameter
// Contains the key, value, source, and description for documentation
struct ConfigEntry {
    std::string key;              // Configuration key (e.g., "neuron.lif.tau")
    ConfigValue value;            // Configuration value
    ConfigSource source;          // How the value was obtained
    std::string description;      // Human-readable description of the parameter
    
    // Default constructor creates an empty configuration entry
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    // Parameterized constructor for creating configuration entries
    // Parameters: k - Configuration key, v - Value, s - Source, desc - Description (optional)
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class - manages key-value configuration with multiple sources
// Supports loading from files, command-line arguments, and runtime API
// Handles conflicts by allowing later sources to override earlier ones
// Provides type-safe access to configuration values with sensible defaults
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying - Configuration contains internal state
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Enable moving - Allows transfer of configuration ownership
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Configuration loading operations
    
    // Load configuration from JSON/YAML file
    // Parameters: filepath - Path to configuration file
    // Returns: true if file was successfully read and parsed
    // Note: Currently supports simple key=value format, JSON/YAML planned for Phase 2
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command-line arguments
    // Parameters: argc, argv - Standard command-line parameters
    // Supports --key=value and -key value formats
    // Returns: true if parsing succeeded
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file
    // Parameters: filepath - Path where configuration will be saved
    // Returns: true if file was successfully written
    // Note: Currently saves all entries with placeholder values
    bool saveToFile(const std::string& filepath) const;
    
    // Value access operations
    
    // Get configuration value with type checking
    // Template parameter T - Type of value to retrieve (must match stored type)
    // Parameters: key - Configuration key to look up
    // Returns: Optional containing value if key exists and type matches, std::nullopt otherwise
    // Usage: auto tau = config.get<double>("neuron.lif.tau");
    //        if (tau) { use_tau = *tau; }
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default fallback
    // Template parameter T - Type of value to retrieve
    // Parameters: key - Configuration key, defaultValue - Fallback if key not found
    // Returns: Configuration value if key exists, otherwise defaultValue
    // Usage: auto max_weight = config.getOr("synapse.weight.max", 1.0);
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration modification operations
    
    // Set configuration value with specified source
    // Template parameter T - Type of value (must be compatible with ConfigValue)
    // Parameters: key - Configuration key, value - New value, source - How value was set
    // Note: Different sources have different priority (File > CommandLine > Runtime)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience overloads for common types
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration management operations
    
    // Check if configuration key exists
    // Parameters: key - Configuration key to check
    // Returns: true if key has been set, false otherwise
    bool has(const std::string& key) const;
    
    // Remove configuration key
    // Parameters: key - Configuration key to remove
    void remove(const std::string& key);
    
    // Get all configuration keys
    // Returns: Vector of all configured keys
    std::vector<std::string> getKeys() const;
    
    // Clear all configuration
    // Removes all entries regardless of source
    void clear();
    
    // Get configuration summary for debugging
    // Returns: Formatted string showing all configuration entries and their sources
    // Useful for logging and debugging configuration issues
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper functions
    
    // Remove whitespace from string (leading/trailing spaces, tabs, newlines)
    // Parameters: str - String to trim
    // Returns: Trimmed string
    static std::string trim(const std::string& str);
    
    // Convert string to lowercase
    // Parameters: str - String to convert
    // Returns: Lowercase version of input string
    static std::string toLower(const std::string& str);
};

} // namespace nlm

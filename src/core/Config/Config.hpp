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
// Supports various data types for neural simulation configuration
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
// Tracks the origin of configuration values (Default, File, CommandLine, Runtime)
enum class ConfigSource {
    Default,    // Default values hardcoded in the system
    File,       // Values loaded from configuration file
    CommandLine, // Values provided via command line arguments
    Runtime     // Values set at runtime via API
};

// Configuration entry
// Represents a single configuration key-value pair with metadata
struct ConfigEntry {
    std::string key;           // Configuration key identifier
    ConfigValue value;         // Configuration value
    ConfigSource source;       // Source of this configuration value
    std::string description;   // Human-readable description of the parameter
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
// Manages neural simulation parameters with support for multiple file formats,
// command-line arguments, and runtime configuration
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load configuration from file (JSON, YAML, or simple key=value format)
    // Automatically detects file format based on extension (.json, .yaml, .yml, .cfg)
    // Returns true on success, false on file not found or parse error
    bool loadFromFile(const std::string& filepath);
    
    // Load configuration from command line arguments
    // Supports --key=value and -key value formats
    // Logs all configuration values loaded from command line
    bool loadFromArgs(int argc, char** argv);
    
    // Save configuration to file with format support
    // Supports auto-detection or explicit format specification (simple, json, yaml)
    bool saveToFile(const std::string& filepath) const;
    bool saveToFile(const std::string& filepath, const std::string& format) const;
    
    // Get configuration values with type safety
    // Returns std::nullopt if key doesn't exist or type mismatch
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Get configuration value with default fallback
    // Returns default value if key doesn't exist or type mismatch
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration values
    // Overwrites existing key or creates new one with specified source
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
    
    // Clear all configuration
    void clear();
    
    // Get configuration summary as string
    // Useful for debugging and logging
    std::string summary() const;
    
    // Helper methods for common data types
    // Convenience methods with proper error handling
    bool getBool(const std::string& key, bool defaultValue = false) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    int64_t getInt64(const std::string& key, int64_t defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Configuration validation and utilities
    // Validates that all required keys are present
    bool validateRequired(const std::vector<std::string>& requiredKeys) const;
    
    // Get human-readable string for configuration source
    std::string getSourceString(ConfigSource source) const;
    
    // Convenience methods for NLM-specific parameters
    // Sets default values optimized for Phase 2 neural computation
    void setDefaultsForPhase2() const;
    
    // File format detection
    // Static helper methods for file format detection
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLFile(const std::string& filepath);
    static bool isSimpleFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers for file format detection and parsing
    bool loadFromSimpleFile(const std::string& filepath);
    bool loadFromJSONFile(const std::string& filepath);
    bool loadFromYAMLFile(const std::string& filepath);
    
    // Internal save methods for different formats
    bool saveToSimpleFile(const std::string& filepath) const;
    bool saveToJSONFile(const std::string& filepath) const;
    bool saveToYAMLFile(const std::string& filepath) const;
    
    // Internal string manipulation utilities
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Explicit template instantiations
// Required for static linking of template functions
namespace nlm {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;

    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

} // namespace nlm

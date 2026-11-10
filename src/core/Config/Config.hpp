#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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
    Default,    ///< Default values built into the code
    File,       ///< Loaded from configuration file
    CommandLine,///< Command line arguments
    Runtime     ///< Set at runtime during execution
};

// Configuration entry
struct ConfigEntry {
    std::string key;         ///< Configuration key
    ConfigValue value;        ///< Configuration value
    ConfigSource source;      ///< Source of this configuration value
    std::string description;  ///< Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM system
 * 
 * The Configuration class manages all settings for the Neural Learning Machine system.
 * It supports loading from files, command line arguments, and runtime settings.
 * Values can have different sources (Default, File, CommandLine, Runtime) with
 * File and CommandLine overriding Default values.
 * 
 * The class provides type-safe access to configuration values through get() and getOr() methods.
 * It also supports saving configurations to files for checkpointing.
 */
class Config {
public:
    /// Constructor - initializes with default configuration values
    Config();
    
    /// Destructor
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /// Load configuration from file (JSON or simple key=value format)
    /// @param filepath Path to configuration file
    /// @return true if file was successfully loaded, false otherwise
    bool loadFromFile(const std::string& filepath);
    
    /// Load configuration from command line arguments
    /// Supports --key=value and -key value formats
    /// @param argc Number of command line arguments
    /// @param argv Command line arguments array
    /// @return true if arguments were successfully parsed
    bool loadFromArgs(int argc, char** argv);
    
    /// Save configuration to file
    /// @param filepath Path where configuration should be saved
    /// @return true if file was successfully written, false otherwise
    bool saveToFile(const std::string& filepath) const;
    
    /// Get a configuration value with type safety
    /// @tparam T Expected type of value
    /// @param key Configuration key
    /// @return Optional value - contains value if key exists, empty otherwise
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /// Get a configuration value with default fallback
    /// @tparam T Expected type of value
    /// @param key Configuration key
    /// @param defaultValue Value to return if key doesn't exist
    /// @return Value from configuration or default if not found
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /// Set a configuration value
    /// @param key Configuration key
    /// @param value Configuration value
    /// @param source Source of this configuration value
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /// Set a string configuration value
    /// @param key Configuration key
    /// @param value String value
    /// @param source Source of this configuration value
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /// Set an integer configuration value
    /// @param key Configuration key
    /// @param value Integer value
    /// @param source Source of this configuration value
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /// Set a 64-bit integer configuration value
    /// @param key Configuration key
    /// @param value 64-bit integer value
    /// @param source Source of this configuration value
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /// Set a double-precision float configuration value
    /// @param key Configuration key
    /// @param value Double value
    /// @param source Source of this configuration value
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /// Set a boolean configuration value
    /// @param key Configuration key
    /// @param value Boolean value
    /// @param source Source of this configuration value
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /// Check if a configuration key exists
    /// @param key Configuration key
    /// @return true if key exists, false otherwise
    bool has(const std::string& key) const;
    
    /// Remove a configuration key and its value
    /// @param key Configuration key to remove
    void remove(const std::string& key);
    
    /// Get all configuration keys
    /// @return Vector of all configuration keys
    std::vector<std::string> getKeys() const;
    
    /// Clear all configuration values
    void clear();
    
    /// Get configuration summary as string
    /// @return String containing configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /// Internal helper to trim whitespace from strings
    /// @param str Input string
    /// @return String with leading/trailing whitespace removed
    static std::string trim(const std::string& str);
    
    /// Internal helper to convert string to lowercase
    /// @param str Input string
    /// @return Lowercase version of string
    static std::string toLower(const std::string& str);
};

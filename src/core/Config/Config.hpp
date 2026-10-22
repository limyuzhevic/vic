#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

/**
 * Forward declarations
 */
class Config;

/**
 * Configuration value types
 * 
 * Strong type system for configuration values using std::variant
 * to ensure type safety at compile time.
 */
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

/**
 * Configuration source enumeration
 * 
 * Tracks where a configuration value comes from for precedence resolution
 */
enum class ConfigSource {
    Default,      ///< Default value (internal default)
    File,         ///< Loaded from configuration file
    CommandLine,  ///< Set via command line arguments
    Runtime       ///< Set at runtime
};

/**
 * Configuration entry
 * 
 * Represents a single configuration key-value pair with metadata.
 * Used internally by the Config class to manage configuration state.
 */
struct ConfigEntry {
    std::string key;        ///< Configuration key
    ConfigValue value;       ///< Configuration value
    ConfigSource source;     ///< Source of this value
    std::string description; ///< Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * Main configuration class
 * 
 * Provides a type-safe configuration management system for NLM.
 * Supports loading from files, command line arguments, and runtime setting,
 * with proper serialization and type checking.
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * Disable copying, enable moving
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * Load configuration from file
     * 
     * Supports both JSON and YAML formats (implementation pending)
     * Currently uses simple key=value format for compatibility.
     * 
     * @param filepath Path to configuration file
     * @return True if file was loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Load configuration from command line arguments
     * 
     * Supports --key=value and -key value formats.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument values
     * @return True if arguments were processed successfully, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * Save configuration to file
     * 
     * Serializes configuration entries to a file with comments describing each entry.
     * Uses simple key=value format for compatibility.
     * 
     * @param filepath Path to output file
     * @return True if file was saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Get configuration value
     * 
     * Retrieves a configuration value with type safety using template parameter T.
     * 
     * @tparam T Expected type of configuration value
     * @param key Configuration key to retrieve
     * @return Optional containing the value if key exists and type matches, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * Get configuration value with default fallback
     * 
     * Retrieves a configuration value, returning the default if the key doesn't exist
     * or if the type doesn't match.
     * 
     * @tparam T Expected type of configuration value
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if key doesn't exist
     * @return Configuration value or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * Set configuration value
     * 
     * Sets a configuration value with the specified source.
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return True if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * Remove configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * Clear all configuration entries
     */
    void clear();
    
    /**
     * Get configuration summary
     * 
     * Generates a human-readable summary of all configuration entries.
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * Internal helper functions
     */
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm


#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types
 * 
 * NLM supports a flexible configuration system using type-safe variants.
 * Each configuration parameter can be one of several supported types,
 * allowing for rich configuration options while maintaining type safety.
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
 * @brief Configuration source enum
 * 
 * Indicates the source of a configuration value, which determines priority
 * during conflict resolution. Later sources override earlier ones.
 */
enum class ConfigSource {
    Default,      // Default values defined in code
    File,         // Values loaded from configuration file
    CommandLine,  // Values provided via command line
    Runtime       // Values set during runtime
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration parameter with its metadata.
 * Used internally by the configuration system to track value provenance
 * and enable advanced features like source tracking and conflict resolution.
 */
struct ConfigEntry {
    std::string key;          // Configuration parameter name
    ConfigValue value;        // Parameter value
    ConfigSource source;      // Source of the value
    std::string description;  // Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM
 * 
 * Provides a type-safe configuration system for NLM brain parameters,
 * simulation settings, and system options. Configuration values can be
 * loaded from files, command line arguments, or set programmatically at runtime.
 * 
 * The configuration system supports:
 * - Multiple value types (integers, floats, booleans, strings, arrays)
 * - Different configuration sources with priority rules
 * - File-based configuration (JSON format)
 * - Command line configuration
 * - Runtime configuration
 * - Type-safe value retrieval with defaults
 * 
 * This class is central to NLM's flexibility, allowing users to customize
 * brain parameters, simulation behavior, and system operation without
 * modifying source code.
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Loads configuration parameters from a file, typically JSON format.
     * Existing configuration values may be overridden, with file values
     * taking precedence over defaults.
     * 
     * @param filepath Path to configuration file
     * @return true if file was successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments and applies configuration values.
     * Arguments typically follow the format --key=value or --key value.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if arguments were successfully parsed, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Saves the current configuration to a file, typically JSON format.
     * Only non-default values are saved to minimize file size.
     * 
     * @param filepath Path to output configuration file
     * @return true if file was successfully written, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value
     * 
     * Retrieves a configuration value as a specific type.
     * Returns nullopt if the key doesn't exist.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration parameter name
     * @return Optional containing the value, or nullopt if not found
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default
     * 
     * Retrieves a configuration value with a default fallback.
     * If the key doesn't exist, returns the provided default value.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration parameter name
     * @param defaultValue Value to return if key doesn't exist
     * @return Configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value with the specified source. Later sources
     * override earlier sources during value resolution.
     * 
     * @param key Configuration parameter name
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string specialization)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer specialization)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double specialization)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean specialization)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration parameter name
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Removes a configuration parameter from the current configuration.
     * The key will be treated as if it doesn't exist for subsequent operations.
     * 
     * @param key Configuration parameter name to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration parameter names
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration parameters, leaving only default values.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * Returns a human-readable summary of the current configuration,
     * including all non-default values and their sources.
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

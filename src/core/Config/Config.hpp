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

/**
 * @file Config.hpp
 * @brief Configuration management system for NLM
 * @author NLM Development Team
 * @date 2026
 * @version 0.1.0
 *
 * This file provides a type-safe configuration system for the Neural Learning Machine.
 * It supports multiple configuration sources (default, file, command line, runtime)
 * and various data types including scalars and arrays.
 */

/**
 * @brief Configuration value types supporting various data types
 * 
 * This variant type supports basic types (int, int64_t, double, bool, string)
 * and collections of these types, enabling flexible configuration options.
 * 
 * @note Used internally by ConfigEntry to store arbitrary configuration values
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
 * @brief Configuration source enumeration indicating where a value originates
 * 
 * The source determines the precedence and mutability of configuration values:
 * - Default: Built-in defaults or hard-coded values
 * - File: Loaded from configuration file (lowest precedence)
 * - CommandLine: Command-line arguments (higher precedence)
 * - Runtime: Programatically set at runtime (highest precedence)
 */
enum class ConfigSource {
    Default,  ///< Built-in defaults
    File,     ///< Loaded from configuration file
    CommandLine,  ///< Command-line arguments
    Runtime   ///< Programatically set at runtime
};

/**
 * @brief Stores a single configuration value with metadata
 * 
 * A ConfigEntry combines a key string, its value, source, and description.
 * This enables type-safe configuration with provenance tracking.
 */
struct ConfigEntry {
    /**
     * @brief Configuration key string
     * 
     * The key is used to identify and retrieve configuration values.
     * Keys are case-insensitive and typically use dot notation (e.g., "brain.neuron_count")
     */
    std::string key;

    /**
     * @brief Configuration value
     * 
     * Can be any type supported by ConfigValue, including basic types and arrays.
     */
    ConfigValue value;

    /**
     * @brief Source of this configuration value
     * 
     * Determines precedence and whether the value can be overridden.
     */
    ConfigSource source;

    /**
     * @brief Human-readable description of the configuration
     * 
     * Used for documentation and debugging purposes.
     */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with full parameters
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the configuration
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for the NLM system
 * 
 * This class provides a unified interface for managing configuration values
 * from multiple sources with type safety and ease of use.
 * 
 * Features:
 * - Type-safe value access via get() and getOr() methods
 * - Multiple configuration sources with precedence
 * - File I/O support (JSON format)
 * - Command-line argument parsing
 * - Value validation and conversion
 * 
 * @note Uses the pimpl idiom for hiding implementation details
 * @see ConfigSource for source enumeration
 * @see ConfigValue for supported value types
 */
class Config {

/**
 * @brief Constructor
 * 
 * Creates an empty configuration with default values.
 */
Config();

/**
 * @brief Destructor
 * 
 * Cleans up configuration resources.
 */
~Config();
    
    /**
     * @brief Disable copying
     * 
     * Config objects cannot be copied to prevent inconsistent states.
     * Use move semantics instead.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment
     * 
     * Config objects cannot be assigned to prevent inconsistent states.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Enables efficient transfer of ownership between Config objects.
     * 
     * @param other Source Config object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Enables efficient transfer of ownership.
     * 
     * @param other Source Config object to move from
     * @return Reference to this Config object
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Loads configuration values from a JSON file. The file path can be
     * provided via command line arguments (--config=file) or hardcoded.
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if file was successfully loaded, false otherwise
     * 
     * @note File source values have the lowest precedence in the configuration
     * hierarchy (runtime > command line > file > default)
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments for configuration options.
     * Supported formats:
     * - --config=value (single value)
     * - --config key value (multiple arguments)
     * - --key=value syntax for various types
     * 
     * @param argc Argument count (from main())
     * @param argv Argument vector (from main())
     * @return true if arguments were successfully parsed, false otherwise
     * 
     * @note Command line source values have higher precedence than file values
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Saves all configuration values to a JSON file for persistence or sharing.
     * 
     * @param filepath Path where the configuration will be saved
     * @return true if file was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves a configuration value converted to the requested type.
     * Returns nullopt if the key doesn't exist or type conversion fails.
     * 
     * @tparam T Target type (must match ConfigValue variant types)
     * @param key Configuration key to retrieve
     * @return Optional containing the value if found and convertible, empty otherwise
     * 
     * @note Templates are defined in the header to ensure instantiation in all
     * translation units that use the configuration system
     */
template<typename T>
    std::optional<T> get(const std::string& key) const;

    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value or returns a default if not found.
     * Type-safe and never fails to return a value.
     * 
     * @tparam T Target type (must match ConfigValue variant types)
     * @param key Configuration key to retrieve
     * @param defaultValue Value returned if key doesn't exist
     * @return Configuration value if found, otherwise defaultValue
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    /**
     * @brief Set configuration value (generic version)
     * 
     * Sets a configuration value with specified source. The source determines
     * precedence and mutability (runtime > command line > file > default).
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (string version)
     * @param key Configuration key
     * @param value String value
     * @param source Source of the configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (int version)
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of the configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (int64_t version)
     * @param key Configuration key
     * @param value 64-bit integer value
     * @param source Source of the configuration
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (double version)
     * @param key Configuration key
     * @param value Floating-point value
     * @param source Source of the configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (bool version)
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of the configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists in the configuration
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Deletes the configuration entry for the specified key.
     * Only entries from runtime or command-line sources can be removed.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys (including default keys)
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all runtime and command-line configuration entries,
     * restoring to default values.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return String representation of all configuration entries
     * Useful for debugging and logging.
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     * 
     * Removes leading and trailing whitespace from a string.
     * 
     * @param str Input string
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Convert string to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of input
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

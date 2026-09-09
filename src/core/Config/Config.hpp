#pragma once
/**
 * @file Config.hpp
 * @brief Configuration management system for NLM
 * 
 * This file provides a comprehensive configuration system that supports:
 * - Multiple configuration value types (integers, floats, strings, lists)
 * - Multiple configuration sources (default, file, command line, runtime)
 * - JSON and key-value file format support
 * - Type-safe value retrieval and type conversion
 * - Thread-safe configuration access
 * 
 * @author NLM Development Team
 * @version 1.0
 * @date 2026
 * 
 * Usage example:
 * @code
 * #include "Config.hpp"
 * 
 * nlm::Config config;
 * config.loadFromFile("config.json");
 * int value = config.getOr<int>("simulation.steps", 100);
 * @endcode
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
/**
 * @brief Union type that can hold various configuration value types
 * 
 * This variant type supports common configuration value types:
 * - Numeric types: int, int64_t, double
 * - Boolean type: bool
 * - String types: std::string
 * - Container types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * @note Type conversion helpers are provided to safely extract values.
 * @see get(), getOr()
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

// Configuration source
/**
 * @enum ConfigSource
 * @brief Enumeration of configuration value sources
 * 
 * This enum identifies where a configuration value originated from, which
 * determines precedence when multiple sources provide conflicting values:
 * 
 * - Default: Initial factory values or hardcoded defaults
 * - File: Loaded from configuration file
 * - CommandLine: Provided via command-line arguments
 * - Runtime: Set programmatically during execution
 * 
 * @note Command-line and runtime values have higher precedence than
 * file and default values.
 */
enum class ConfigSource {
    /** Default configuration value set by the system */
    Default,
    /** Configuration loaded from a file (JSON or key-value format) */
    File,
    /** Configuration provided via command-line arguments */
    CommandLine,
    /** Configuration set at runtime via API calls */
    Runtime
};

// Configuration entry
/**
 * @struct ConfigEntry
 * @brief Stores a configuration key-value pair with metadata
 * 
 * A ConfigEntry represents a single configuration parameter, storing:
 * - The key/name of the parameter
 * - The typed value
 * - The source where the value originated
 * - A human-readable description
 * 
 * @note Used internally by the Config class to manage all configuration values.
 */
struct ConfigEntry {
    /** Configuration key/parameter name */
    std::string key;
    /** Configuration value with type information */
    ConfigValue value;
    /** Source where this configuration value originated */
    ConfigSource source;
    /** Human-readable description of the parameter */
    std::string description;
    
    /**
     * @brief Default constructor creates an empty entry
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a configuration entry
     * 
     * @param k Configuration key/name
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Optional description (defaults to empty string)
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
// Main configuration class
/**
 * @class Config
 * @brief Thread-safe configuration management system
 * 
 * The Config class provides a centralized configuration system for NLM with:
 * - Loading and saving configuration from JSON and key-value files
 * - Type-safe value retrieval with automatic type conversion
 * - Multiple configuration sources with precedence handling
 * - Command-line argument parsing
 * - Runtime configuration modification
 * - Configuration value validation
 * - Configuration summary generation
 * 
 * The implementation uses the Pimpl idiom for efficient memory management
 * and thread-safe access patterns.
 * 
 * @note This class is not copyable but is movable.
 * @see ConfigValue, ConfigSource, ConfigEntry
 * 
 * Example usage:
 * @code
 * nlm::Config config;
 * if (config.loadFromFile("config.json")) {
 *     auto steps = config.getOr<int>("simulation.steps", 1000);
 *     auto enabled = config.get<bool>("features.debug");
 * }
 * @endcode
 */
class Config {
public:
    /**
     * @brief Constructs an empty configuration
     * 
     * Initializes an empty configuration with all default values.
     * Call initialize() or loadFromFile() to populate with actual configuration.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up resources and saves configuration if modified.
     */
    ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Config objects cannot be copied due to internal pImpl structure.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Config objects cannot be copied due to internal pImpl structure.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor ( noexcept)
     * 
     * @param other Config to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator ( noexcept)
     * 
     * @param other Config to move from
     * @return Reference to this object
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file (JSON or key-value format)
     * 
     * Attempts to load configuration from the specified file. The file format
     * is automatically detected based on file extension and content.
     * 
     * @param filepath Path to the configuration file
     * @return true if configuration was loaded successfully, false otherwise
     * @throw std::runtime_error if file cannot be read
     * @see loadFromJSON(), loadFromKeyValue()
     * 
     * @note Existing configuration values will be overwritten by file values.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Parses command-line arguments in the form --key=value and updates
     * the configuration accordingly. Only existing configuration keys are
     * updated; new keys are ignored.
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument array
     * @return true if configuration was updated, false on error
     * @see loadFromFile()
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Saves the current configuration to a file using JSON format.
     * Configuration values are serialized to their string representation.
     * 
     * @param filepath Path where configuration should be saved
     * @return true if configuration was saved successfully, false otherwise
     * @throw std::runtime_error if file cannot be written
     * @see saveToJSON(), saveToKeyValue()
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type-safe retrieval
     * 
     * Retrieves a configuration value and attempts to convert it to the
     * requested type T. If the type matches exactly or can be safely converted,
     * the value is returned in the optional. Otherwise, std::nullopt is returned.
     * 
     * @tparam T Type to retrieve (must be one of ConfigValue's supported types)
     * @param key Configuration key to look up
     * @return std::optional<T> Value if found and type matches, std::nullopt otherwise
     * 
     * @note Supported types: int, int64_t, double, bool, std::string,
     *       std::vector<int>, std::vector<double>, std::vector<std::string>
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value or returns the default value if the
     * key doesn't exist or the value cannot be converted to the requested type.
     * 
     * @tparam T Type to retrieve (must be one of ConfigValue's supported types)
     * @param key Configuration key to look up
     * @param defaultValue Value returned if key is not found or type mismatch
     * @return T The configuration value or default value
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value with explicit source specification. This
     * provides finer control over configuration value tracking compared to
     * the convenience overloads.
     * 
     * @param key Configuration key to set
     * @param value Configuration value (any ConfigValue type)
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string overload)
     * 
     * Convenience overload for setting string values.
     * 
     * @param key Configuration key to set
     * @param value String value to set
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int overload)
     * 
     * Convenience overload for setting integer values.
     * 
     * @param key Configuration key to set
     * @param value Integer value to set
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double overload)
     * 
     * Convenience overload for setting floating-point values.
     * 
     * @param key Configuration key to set
     * @param value Double value to set
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (bool overload)
     * 
     * Convenience overload for setting boolean values.
     * 
     * @param key Configuration key to set
     * @param value Boolean value to set
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * Tests whether a configuration key has been set with a non-default value.
     * 
     * @param key Configuration key to check
     * @return true if key exists and has a value, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Removes a configuration key and restores it to its default value.
     * This effectively deletes the key from the configuration.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Returns a vector of all currently set configuration keys (excluding
     * default values).
     * 
     * @return std::vector<std::string> Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all non-default configuration values, restoring the configuration
     * to its initial state with only default values.
     */
    void clear();
    
    /**
     * @brief Generate configuration summary
     * 
     * Creates a human-readable string summary of all current configuration
     * values, grouped by source and including descriptions where available.
     * 
     * @return std::string Formatted configuration summary
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from both ends of a string
     * 
     * Removes leading and trailing whitespace characters from the input string.
     * 
     * @param str Input string
     * @return std::string String with whitespace trimmed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * Converts all uppercase characters in the input string to lowercase.
     * 
     * @param str Input string
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Load configuration from JSON file
     * 
     * Internal helper to load configuration from a JSON file format.
     * 
     * @param filepath Path to JSON configuration file
     * @return true on success, false on parse error
     */
    bool loadFromJSON(const std::string& filepath);
    
    /**
     * @brief Load configuration from key-value file
     * 
     * Internal helper to load configuration from a key=value text file format.
     * 
     * @param filepath Path to key-value configuration file
     * @return true on success, false on parse error
     */
    bool loadFromKeyValue(const std::string& filepath);
    
    /**
     * @brief Convert JSON object to configuration entries
     * 
     * Recursively processes a JSON object and creates ConfigEntry objects
     * for all key-value pairs.
     * 
     * @param json JSON object to process
     * @param prefix Current key prefix for nested keys
     */
    void loadJSONToConfig(const nlohmann::json& json, const std::string& prefix);
    
    /**
     * @brief Convert JSON value to configuration entry
     * 
     * Converts a single JSON value to a ConfigEntry with the given key.
     * 
     * @param key Configuration key
     * @param value JSON value to convert
     */
    void loadJSONValueToConfig(const std::string& key, const nlohmann::json& value);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * Internal helper to save configuration to JSON format.
     * 
     * @param filepath Path where JSON should be saved
     * @return true on success, false on error
     */
    bool saveToJSON(const std::string& filepath) const;
    
    /**
     * @brief Save configuration to key-value file
     * 
     * Internal helper to save configuration to key=value format.
     * 
     * @param filepath Path where key-value file should be saved
     * @return true on success, false on error
     */
    bool saveToKeyValue(const std::string& filepath) const;
    
    /**
     * @brief Convert configuration value to JSON
     * 
     * Converts a ConfigValue to its JSON representation for file output.
     * 
     * @param json JSON object to update
     * @param key Configuration key
     * @param value Configuration value to serialize
     */
    void setJSONValue(nlohmann::json& json, const std::string& key, const ConfigValue& value) const;
    
    /**
     * @brief Convert configuration value to string representation
     * 
     * Converts any ConfigValue type to its string representation for output.
     * Handles all supported types including containers.
     * 
     * @param value Configuration value to convert
     * @return std::string String representation of the value
     */
    std::string serializeValue(const ConfigValue& value) const;
};

} // namespace nlm

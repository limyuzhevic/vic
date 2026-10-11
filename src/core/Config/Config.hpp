#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;

// Include exception definitions
#include "Exception/NLMException.hpp"

/**
 * @file Config.hpp
 * @brief Configuration management system for NLM
 * 
 * This file defines the configuration management system for the NLM (Neural
 * Learning Machine) framework. The configuration system provides a type-safe,
 * flexible way to manage simulation parameters and settings.
 * 
 * The configuration system supports:
 * - Loading from files (JSON format)
 * - Loading from command line arguments
 * - Saving to files
 * - Type-safe value access
 * - Configuration validation
 * - Error handling with descriptive messages
 * 
 * Configuration values can be of various types including integers, floats,
 * booleans, strings, and arrays of these types.
 */

/**
 * @namespace nlm::config
 * @brief Configuration-related utilities and types
 */

/**
 * @namespace nlm::config::detail
 * @brief Implementation details for configuration management
 * 
 * This namespace contains internal implementation details that should not
 * be used directly by external code.
 */

/**
 * @brief Configuration value type enumeration
 * 
 * This variant type represents all supported configuration value types in
 * the NLM framework. The configuration system supports a variety of data
 * types to accommodate different simulation parameters.
 * 
 * @note The supported types are:
 * - Basic types: int, int64_t, double, bool
 * - String types: std::string
 * - Collection types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * @warning When accessing configuration values, ensure the type matches
 *          the requested template type to avoid undefined behavior.
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
 * @brief Configuration source enumeration
 * 
 * This enumeration defines the different sources from which configuration
 * values can be obtained. Each source has different precedence and
 * mutability characteristics.
 * 
 * @enum ConfigSource
 * 
 * @var ConfigSource::Default
 *   Default configuration values set during initialization.
 *   These are the fallback values when no other source is available.
 * 
 * @var ConfigSource::File
 *   Configuration values loaded from a file (typically JSON).
 *   These have higher priority than defaults but can be overridden
 *   by runtime settings.
 * 
 * @var ConfigSource::CommandLine
 *   Configuration values parsed from command line arguments.
 *   These have higher priority than file-based settings.
 * 
 * @var ConfigSource::Runtime
 *   Configuration values set programmatically at runtime.
 *   These have the highest priority and override all other sources.
 */
enum class ConfigSource {
    /** Default configuration values set during initialization */
    Default,
    /** Configuration values loaded from files */
    File,
    /** Configuration values from command line arguments */
    CommandLine,
    /** Runtime configuration values */
    Runtime
};

/**
 * @brief Configuration entry structure
 * 
 * This structure represents a single configuration key-value pair along
 * with metadata about its source and origin.
 * 
 * @struct ConfigEntry
 * 
 * @var key The configuration key/parameter name
 * @var value The configuration value
 * @var source The source from which this value was obtained
 * @var description Human-readable description of this configuration parameter
 * 
 * @note Configuration entries are used internally by the Config class to
 *       maintain the current configuration state and track the provenance
 *       of each value.
 */
struct ConfigEntry {
    /** Configuration key/parameter name */
    std::string key;
    /** Configuration value */
    ConfigValue value;
    /** Source from which this value was obtained */
    ConfigSource source;
    /** Human-readable description of this configuration parameter */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
class Config {
public:
    /**
     * @brief Construct a new Config object
     * 
     * Initializes the configuration system with default values. The default
     * constructor sets up the internal implementation structure and loads
     * any configuration files specified in the NLM configuration.
     */
    Config();
    
    /**
     * @brief Destroy the Config object
     * 
     * Cleans up the configuration system and frees any allocated resources.
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Loads configuration settings from a JSON file. The file is parsed and
     * the configuration values are added to the current configuration state.
     * 
     * @param filepath Path to the configuration file (JSON format)
     * 
     * @return true if the file was loaded successfully, false if the file
     *         was not found, could not be read, or contained invalid JSON.
     *         In case of error, the configuration system continues to use
     *         existing values or defaults.
     * 
     * @throws ConfigFileNotFoundException if the file cannot be opened
     * @throws ConfigParseException if the file contains invalid JSON
     * 
     * @note If the file is not found, the system logs an error but continues
     *       execution using existing configuration values.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments for configuration settings. Supports
     * both `--key=value` and `--key value` formats. Command line arguments
     * override values from files and defaults.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array (standard main() parameters)
     * 
     * @return true if arguments were parsed successfully, false on error.
     *         In case of error, invalid arguments are logged but parsing
     *         continues for valid arguments.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Saves the current configuration state to a JSON file. This includes
     * all runtime and file-based configuration values. The saved configuration
     * can be loaded later using loadFromFile().
     * 
     * @param filepath Path where the configuration file should be saved
     * 
     * @return true if the file was saved successfully, false on error
     *         (e.g., file permissions, disk full).
     * 
     * @throws ConfigFileWriteException if the file cannot be written
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value
     * 
     * Retrieves a configuration value by key with type-safe access. The
     * function searches for the key in the configuration (using case-insensitive
     * comparison) and returns the value if found and of the correct type.
     * 
     * @tparam T The type of value to retrieve (int, double, bool, std::string, etc.)
     * @param key Configuration key to look up
     * 
     * @return std::optional<T> The configuration value if found and of correct type,
     *         or std::nullopt if the key is not present or has the wrong type.
     * 
     * @note The function logs errors if there are issues accessing the configuration
     *       but returns an empty optional rather than throwing an exception to
     *       allow graceful handling of missing or invalid configuration.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value by key, returning a default value if the
     * key is not present or the value has the wrong type. This is useful for
     * providing fallback behavior for optional configuration parameters.
     * 
     * @tparam T The type of value to retrieve
     * @param key Configuration key to look up
     * @param defaultValue The value to return if key is not found or has wrong type
     * 
     * @return T The configuration value if found and of correct type, otherwise
     *         the default value.
     * 
     * @note Unlike get(), this function does not throw exceptions. It provides
     *       a simple way to access configuration values with sensible defaults.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Sets a configuration value with a generic ConfigValue type. This provides
     * flexibility for setting configuration values of different types. The
     * value is stored with the specified source.
     * 
     * @param key Configuration key to set
     * @param value Configuration value to store
     * @param source Source of this configuration value (default: Runtime)
     * 
     * @throws ConfigValidationException if the key-value pair is invalid
     * 
     * @note The source parameter determines the priority of this value relative
     *       to other sources. Runtime values have highest priority.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a string configuration value
     * 
     * Convenience method for setting string configuration values.
     * 
     * @param key Configuration key
     * @param value String value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set an integer configuration value
     * 
     * Convenience method for setting integer configuration values.
     * 
     * @param key Configuration key
     * @param value Integer value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a floating-point configuration value
     * 
     * Convenience method for setting double configuration values.
     * 
     * @param key Configuration key
     * @param value Double value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a boolean configuration value
     * 
     * Convenience method for setting boolean configuration values.
     * 
     * @param key Configuration key
     * @param value Boolean value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * Tests whether a configuration key is present in the current configuration.
     * The lookup is case-insensitive.
     * 
     * @param key Configuration key to check
     * 
     * @return true if the key exists, false otherwise.
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * Removes a configuration key from the current configuration state.
     * After removal, the key will be treated as if it were never set.
     * 
     * @param key Configuration key to remove
     * 
     * @throws ConfigException if the key cannot be removed
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Returns a list of all configuration keys currently set in the system.
     * This includes keys from all sources (defaults, files, command line, runtime).
     * 
     * @return std::vector<std::string> List of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration values, leaving the system in a state
     * equivalent to just after construction. This removes values from
     * all sources but does not remove the default values that will be
     * re-established on the next configuration access.
     */
    void clear();
    
    /**
     * @brief Get a summary of the current configuration
     * 
     * Generates a human-readable summary of the current configuration state,
     * showing all key-value pairs and their sources. This is useful for
     * debugging and logging configuration state.
     * 
     * @return std::string Human-readable configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation structure
     * 
     * This structure contains the internal implementation details of the
     * Config class. It uses the Pimpl idiom to hide implementation details
     * from the header file.
     * 
     * @struct Impl
     */
    struct Impl;
    /**
     * @brief Pointer to implementation
     * 
     * Stores the actual implementation details using the Pimpl idiom.
     * This provides better encapsulation and reduces compilation dependencies.
     */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper to trim whitespace
     * 
     * Removes leading and trailing whitespace from a string.
     * 
     * @param str Input string to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper to convert string to lowercase
     * 
     * Converts a string to lowercase for case-insensitive comparisons.
     * 
     * @param str Input string to convert
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Internal helper to validate key-value pairs
     * 
     * Validates that a configuration key-value pair is valid and consistent
     * with system constraints.
     * 
     * @param key Configuration key to validate
     * @param value Configuration value to validate
     * 
     * @throws ConfigValidationException if validation fails
     */
    void validateKeyValue(const std::string& key, const ConfigValue& value);
};

} // namespace nlm

#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management for NLM (熙然) neural simulation framework
 * 
 * Provides a flexible configuration system for storing and accessing simulation parameters,
 * including support for multiple value types, configuration sources, and file-based persistence.
 */

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
 * @brief Configuration value types supported by NLM
 * 
 * Configuration values can be scalar types (int, int64_t, double, bool), strings,
 * or arrays of these types. This provides flexibility for configuration parameters
 * of different data types.
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
 * Indicates the origin of a configuration value, which affects precedence and
 * documentation purposes.
 */
enum class ConfigSource {
public:
    /** @brief Default configuration values built into the code */
    Default = 0,
    /** @brief Values loaded from configuration files */
    File = 1,
    /** @brief Values provided via command line arguments */
    CommandLine = 2,
    /** @brief Values set at runtime via API calls */
    Runtime = 3
};

/**
 * @brief Single configuration entry
 * 
 * Represents a single key-value pair in the configuration system, along with
 * metadata about its source and description.
 */
struct ConfigEntry {
    std::string key;                    ///< Configuration key
    ConfigValue value;                  ///< Configuration value
    ConfigSource source;                ///< Source of this configuration value
    std::string description;            ///< Human-readable description of the parameter
    
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

/**
 * @brief Main configuration class for NLM
 * 
 * Provides a key-value configuration system for storing and accessing simulation
 * parameters. Supports multiple configuration sources with proper precedence and
 * serialization/deserialization to/from files.
 * 
 * Configuration values can be accessed using the get() or getOr() methods, and
 * set using the set() methods. The configuration system supports type-safe access
 * to values with automatic conversion.
 * 
 * @see ConfigSource for different configuration sources
 * @see ConfigValue for supported value types
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object.
     */
    Config();
    
    /**
     * @brief Destructor
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
     * Loads configuration values from a file (currently supports JSON format).
     * If the file cannot be read or parsed, the configuration remains unchanged.
     * 
     * @param filepath Path to the configuration file
     * @return true if configuration was successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format --key=value and adds them
     * to the configuration with CommandLine source precedence.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if configuration was successfully loaded, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Saves the current configuration to a file (currently supports JSON format).
     * Only configuration values from File and CommandLine sources are saved,
     * not Runtime or Default values.
     * 
     * @param filepath Path to the output configuration file
     * @return true if configuration was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value by key
     * 
     * Retrieves a configuration value of the specified type. If the key doesn't
     * exist or the value is of a different type, returns std::nullopt.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return std::optional<T> Value if found and of correct type, std::nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    /**
     * @brief Get configuration value with default
     * 
     * Retrieves a configuration value of the specified type, returning a default
     * value if the key doesn't exist or the value is of a different type.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist or is of wrong type
     * @return T Configuration value if found, otherwise defaultValue
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value with a generic ConfigValue type.
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string)
     * 
     * Convenience overload for setting string values.
     * 
     * @param key Configuration key
     * @param value Configuration string value
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int)
     * 
     * Convenience overload for setting integer values.
     * 
     * @param key Configuration key
     * @param value Configuration integer value
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int64_t)
     * 
     * Convenience overload for setting 64-bit integer values.
     * 
     * @param key Configuration key
     * @param value Configuration int64_t value
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double)
     * 
     * Convenience overload for setting floating-point values.
     * 
     * @param key Configuration key
     * @param value Configuration double value
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (bool)
     * 
     * Convenience overload for setting boolean values.
     * 
     * @param key Configuration key
     * @param value Configuration boolean value
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Removes a configuration key and its associated value.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> List of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration entries, leaving the configuration in an empty state.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * Creates a human-readable string summarizing all current configuration values.
     * Only values from File and CommandLine sources are included.
     * 
     * @return std::string String representation of the configuration
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation details
     * 
     * Pimpl idiom implementation for forward declaration of the internal Config implementation.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: trim whitespace from string
     * 
     * Removes leading and trailing whitespace from a string.
     * 
     * @param str Input string
     * @return std::string String with whitespace trimmed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: convert string to lowercase
     * 
     * Converts all characters in a string to lowercase.
     * 
     * @param str Input string
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

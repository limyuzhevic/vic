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

using ConfigValue = std::variant<
    int,                           ///< Integer value
    int64_t,                       ///< 64-bit integer value
    double,                        ///< Floating-point value
    bool,                          ///< Boolean flag
    std::string,                   ///< String value
    std::vector<int>,              ///< Integer array
    std::vector<double>,           ///< Floating-point array
    std::vector<std::string>       ///< String array
>;

/**
 * @brief Enumeration of configuration value sources
 * 
 * Tracks where configuration values originate to support proper
 * precedence handling and debugging.
 */
enum class ConfigSource {
public:
    Default,    ///< Built-in default value
    File,       ///< Loaded from configuration file
    CommandLine,///< Set via command line arguments
    Runtime     ///< Set programmatically at runtime
};

/**
 * @brief Structure representing a single configuration entry
 * 
 * Each configuration entry consists of a key, value, source, and
 * optional description for documentation and debugging purposes.
 */
struct ConfigEntry {
    /**
     * @brief Configuration key identifier
     * 
     * This string must be unique within the configuration and is used
     * to look up values. Keys are case-insensitive.
     */
    std::string key;

    /**
     * @brief Configuration value
     * 
     * Stored as a variant type to support multiple configuration
     * data types. The actual type can be determined at runtime
     * when retrieving values.
     */
    ConfigValue value;

    /**
     * @brief Source of this configuration value
     * 
     * Determines precedence when multiple sources provide values
     * for the same key. Later sources override earlier ones.
     */
    ConfigSource source;

    /**
     * @brief Human-readable description
     * 
     * Optional documentation string explaining what this configuration
     * value controls. Useful for debugging and configuration validation.
     */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with Default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}

    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Optional description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Centralized configuration management system
 * 
 * Provides a flexible, type-safe configuration system for NLM with
 * support for multiple sources (file, command line, runtime) and
 * automatic type conversion.
 * 
 * Features:
 * - Multiple configuration value types (int, double, bool, string, arrays)
 * - Source tracking with proper precedence (runtime > command line > file > default)
 * - Case-insensitive key lookup
 * - Persistent storage via file I/O
 * - Validation and error handling
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object with default values
     * for all known configuration keys.
     */
    Config();

    /**
     * @brief Destructor
     * 
     * Cleans up implementation details.
     */
    ~Config();

    /**
     * @brief Copy constructor (deleted)
     * 
     * Configuration objects are not copyable to ensure proper
     * management of configuration state.
     */
    Config(const Config&) = delete;

    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Configuration objects are not assignable to ensure proper
     * management of configuration state.
     */
    Config& operator=(const Config&) = delete;

    /**
     * @brief Move constructor (enabled)
     * 
     * Allows efficient transfer of configuration state.
     */
    Config(Config&&) noexcept;

    /**
     * @brief Move assignment operator (enabled)
     * 
     * Allows efficient transfer of configuration state.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Reads configuration from a file (currently supports simple key=value format).
     * Values from file override default values but are overridden by
     * command line and runtime values.
     * 
     * @param filepath Path to configuration file
     * @return true if file was successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format --key=value or --key value.
     * Command line values override file and default values.
     * 
     * @param argc Argument count
     * @param argv Argument array
     * @return true if arguments were successfully parsed, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Writes current configuration to a file in key=value format.
     * Only non-default values are typically saved.
     * 
     * @param filepath Path to output configuration file
     * @return true if file was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * Template function that retrieves a configuration value with the
     * specified type. Returns nullopt if the key doesn't exist or
     * if the value has the wrong type.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return Optional containing the value, or nullopt if not found
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    /**
     * @brief Get configuration value with default fallback
     * 
     * Template function that retrieves a configuration value with the
     * specified type. Returns the default value if the key doesn't exist
     * or if the value has the wrong type.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key is not found
     * @return The configuration value or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    /**
     * @brief Set configuration value (generic version)
     * 
     * @tparam T Type of value to store
     * @param key Configuration key
     * @param value Value to store
     * @param source Source of this configuration value
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (overloaded for string)
     * 
     * @param key Configuration key
     * @param value Value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (overloaded for int)
     * 
     * @param key Configuration key
     * @param value Value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (overloaded for int64_t)
     * 
     * @param key Configuration key
     * @param value Value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (overloaded for double)
     * 
     * @param key Configuration key
     * @param value Value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (overloaded for bool)
     * 
     * @param key Configuration key
     * @param value Value to store
     * @param source Source of this configuration value
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
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector containing all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration keys, restoring them to default values.
     */
    void clear();
    
    /**
     * @brief Generate human-readable configuration summary
     * 
     * @return String containing all configuration values formatted for display
     */
    std::string summary() const;
    
private:
    /**
     * @brief Pimpl implementation for configuration management
     * 
     * The actual implementation details are hidden in a private struct
     * to reduce compilation dependencies and improve encapsulation.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal configuration management implementation
     * 
     * @param str String to trim
     * @return Trimmed string with whitespace removed
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Convert string to lowercase for case-insensitive lookup
     * 
     * @param str String to convert
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

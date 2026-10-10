#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

namespace nlm {

// Configuration value type supporting different data types
using ConfigValue = std::variant<std::string, int64_t, double, bool, std::vector<float>>;

/**
 * @file Config.hpp
 * 
 * Configuration management system for NLM
 * 
 * This file provides a type-safe configuration management system that supports
 * multiple configuration sources (default, file, command line, runtime) and
 * various data types. It is used throughout the NLM system to configure
 * parameters, settings, and runtime behavior.
 * 
 * The configuration system supports:
 * - JSON file loading with schema validation
 * - Command line argument parsing
 * - Runtime configuration updates
 * - Configuration source tracking and conflict resolution
 * - Type-safe value retrieval with automatic conversion
 * 
 * @note Configuration values are stored in a variant type supporting
 *       basic C++ types, vectors of those types, and strings.
 * @note Configuration sources are prioritized: CommandLine > File > Default
 * @note Runtime settings can override file and command line settings
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @brief Configuration source enumeration
 * 
 * Indicates the origin of a configuration value, used to resolve conflicts
 * when the same key is set from multiple sources.
 */
enum class ConfigSource {
    /** Default configuration values built into the system */
    Default,
    /** Configuration loaded from file (typically JSON) */
    File,
    /** Configuration set via command line arguments */
    CommandLine,
    /** Configuration set at runtime via API calls */
    Runtime
};

/**
 * @struct ConfigEntry
 * 
 * Represents a single configuration value with metadata.
 * 
 * @note Used internally by the Config class to track configuration
 *       values along with their source and description for debugging
 *       and conflict resolution.
 */
struct ConfigEntry {
    /** The configuration key/parameter name */
    std::string key;
    
    /** The configuration value (type determined by key) */
    ConfigValue value;
    
    /** Source of this configuration value */
    ConfigSource source;
    
    /** Human-readable description of the configuration parameter */
    std::string description;
    
    /**
     * Default constructor creates an empty entry with default source
     * 
     * @post key is empty, value is uninitialized, source is Default
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a configuration entry with value
     * 
     * @param k Configuration key name
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * 
 * Central configuration manager for the NLM system.
 * 
 * This class provides a unified interface for managing configuration
 * values across multiple sources with type-safe access methods. It is
 * used extensively throughout the NLM system to configure neural
 * parameters, simulation settings, and runtime behavior.
 * 
 * The configuration system supports:
 * - Multiple value types with automatic conversion
 * - Configuration source priority and conflict resolution
 * - File I/O for persistent configuration
 * - Command line argument integration
 * - Real-time configuration updates
 * 
 * @note This class uses the PImpl idiom for efficient header-only
 *       interface with implementation details in the .cpp file.
 * @note Configuration access is thread-safe for read operations.
 * @note Configuration modification requires external synchronization.
 * 
 * @see ConfigSource - Enumeration of configuration sources
 * @see ConfigEntry - Structure for configuration values
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new configuration instance with default values
     * loaded from the default configuration source.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up configuration resources and saves if configured.
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Loads configuration from a JSON file. Configuration values from
     * this source take precedence over default configuration but are
     * lower priority than command line settings.
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if file was successfully loaded, false otherwise
     * 
     @retval true File loaded successfully
     * @retval false File not found or invalid JSON
     * 
     * @throw std::runtime_error if file cannot be read
     * 
     * @pre filepath must be a valid, readable file path
     * @post Configuration is updated with values from file
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format --key=value or
     * --key value. Configuration values from command line take highest
     * precedence in the configuration hierarchy.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if command line parsing was successful
     * 
     * @retval true Parsing successful
     * @retval false Command line parsing failed
     * 
     * @pre argv must be the standard command line argument array
     * @post Configuration is updated with values from command line
     * 
     * @see ConfigSource::CommandLine
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save current configuration to file
     * 
     * Saves all current configuration values to a JSON file. Only
     * configuration values from the File and Runtime sources are saved
     * to maintain the source priority hierarchy.
     * 
     * @param filepath Path where the configuration should be saved
     * @return true if file was successfully written, false otherwise
     * 
     * @retval true File saved successfully
     * @retval false File cannot be written
     * 
     * @throw std::runtime_error if file cannot be written
     * 
     * @pre filepath must be a valid, writable location
     * @post Configuration is persisted to file
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type conversion
     * 
     * Retrieves a configuration value with automatic type conversion
     * from the variant storage. If the key doesn't exist or the value
     * cannot be converted to the requested type, returns nullopt.
     * 
     * @tparam T Type to convert the configuration value to
     * @param key Configuration parameter name
     * @return Configuration value as type T, or nullopt if not found
     * 
     * @retval valid<T> Configuration value converted to type T
     * @retval nullopt Key not found or type conversion failed
     * 
     * @pre T must be a valid type for ConfigValue conversion
     * 
     * @see ConfigSource - For source priority information
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value with type conversion and returns
     * a default value if the key is not found or the value cannot be
     * converted. This is a convenience method for common use cases.
     * 
     * @tparam T Type to convert the configuration value to
     * @param key Configuration parameter name
     * @param defaultValue Value to return if key is not found
     * @return Configuration value or default value
     * 
     * @retval value Configured value converted to type T
     * @retval defaultValue Key not found or default value provided
     * 
     * @pre T must be a valid type for ConfigValue conversion
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value with type inference
     * 
     * Sets a configuration value using type inference for common types.
     * This is a convenience method that automatically determines the
     * appropriate ConfigValue variant type.
     * 
     * @param key Configuration parameter name
     * @param value Configuration value (string)
     * @param source Origin of the configuration value
     * @pre source is one of the ConfigSource values
     * 
     * @post Configuration updated with new value
     * @see ConfigSource - For source priority information
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value with type inference
     * 
     * Sets a configuration value using type inference for common types.
     * This is a convenience method that automatically determines the
     * appropriate ConfigValue variant type.
     * 
     * @param key Configuration parameter name
     * @param value Configuration value (integer)
     * @param source Origin of the configuration value
     * @pre source is one of the ConfigSource values
     * 
     * @post Configuration updated with new value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value with type inference
     * 
     * Sets a configuration value using type inference for common types.
     * This is a convenience method that automatically determines the
     * appropriate ConfigValue variant type.
     * 
     * @param key Configuration parameter name
     * @param value Configuration value (double)
     * @param source Origin of the configuration value
     * @pre source is one of the ConfigSource values
     * 
     * @post Configuration updated with new value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value with type inference
     * 
     * Sets a configuration value using type inference for common types.
     * This is a convenience method that automatically determines the
     * appropriate ConfigValue variant type.
     * 
     * @param key Configuration parameter name
     * @param value Configuration value (boolean)
     * @param source Origin of the configuration value
     * @pre source is one of the ConfigSource values
     * 
     * @post Configuration updated with new value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value with explicit type
     * 
     * Sets a configuration value using the explicit ConfigValue type.
     * Use this method for complex types or when full control over the
     * value type is needed.
     * 
     * @param key Configuration parameter name
     * @param value Configuration value (variant type)
     * @param source Origin of the configuration value
     * @pre source is one of the ConfigSource values
     * 
     * @post Configuration updated with new value
     * 
     * @see ConfigValue - For supported variant types
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * Checks whether a configuration key has been set in any source.
     * This does not check if the value is non-default.
     * 
     * @param key Configuration parameter name to check
     * @return true if key exists, false otherwise
     * 
     * @retval true Key has been set in at least one source
     * @retval false Key not found in any source
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Removes a configuration key from all sources, effectively reverting
     * to default values for that key. This is different from setting a
     * value to a default in a specific source.
     * 
     * @param key Configuration parameter name to remove
     * 
     * @post Key is removed from all sources
     * @post Key will use default configuration
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Returns a list of all configuration keys that have been set
     * (including defaults). The list includes keys from all sources
     * but duplicates from lower priority sources are filtered out.
     * 
     * @return Vector of all configuration key names
     * 
     * @retval non-empty Vector of key names
     * @retval empty No keys have been set (using all defaults)
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration values, reverting to factory defaults.
     * This affects all configuration sources equally.
     * 
     * @post All configuration values removed
     * @post System will use default configuration
     */
    void clear();
    
    /**
     * @brief Generate human-readable configuration summary
     * 
     * Generates a formatted string containing current configuration
     * values and their sources for debugging and logging purposes.
     * 
     * @return Formatted configuration summary string
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static bool endsWith(const std::string& str, const std::string& suffix);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool contains(const std::string& str, const std::string& substr);
    static void skipWhitespace(const std::string& str, std::string::size_type& pos);
    
    // Internal parsing helpers
    ConfigValue parseJSONValue(const std::string& json, std::string::size_type& pos);
    ConfigValue parseYAMLValue(const std::string& yaml, std::string::size_type& pos);
    void parseJSON(const std::string& json, const std::string& filepath);
    void parseYAML(const std::string& yaml, const std::string& filepath);
    void parseSimpleFormat(const std::string& content, const std::string& filepath);
};

} // namespace nlm

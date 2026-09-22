#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

/**
 * @brief NLM Configuration System
 * 
 * The configuration system provides a unified interface for accessing and managing
 * configuration parameters for the NLM (Neural Learning Machine) brain simulation.
 * It supports multiple configuration sources with priority: Runtime > CommandLine > File > Default.
 * 
 * The configuration system uses a type-safe variant system to store different types
 * of configuration values (integers, floats, strings, booleans, and arrays) and provides
 * compile-time type checking when accessing values.
 * 
 * @file Config.hpp
 * @author NLM Project
 * @date Phase 1 - Core Infrastructure
 * @version 0.1.0
 */

// Forward declarations
class Config;

/**
 * @brief Enumeration of configuration sources
 * 
 * Defines the priority order of configuration sources. Higher priority sources
 * override lower priority sources when a key exists in multiple sources.
 */
enum class ConfigSource {
    Default,   ///< Default configuration values (lowest priority)
    File,      ///< Values loaded from configuration files
    CommandLine, ///< Values from command-line arguments
    Runtime    ///< Values set at runtime (highest priority)
};

/**
 * @brief Represents a single configuration entry
 * 
 * A configuration entry contains a key-value pair along with metadata about the
 * source of the value and a human-readable description.
 */
struct ConfigEntry {
    std::string key;          ///< Configuration key (e.g., "neuron_count")
    ConfigValue value;         ///< Configuration value (type-safe variant)
    ConfigSource source;       ///< Source of this configuration value
    std::string description;   ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM brain simulation
 * 
 * The Config class provides a comprehensive configuration management system
 * with the following features:
 * - Loading configuration from files (key=value format)
 * - Command-line argument parsing
 * - Type-safe value retrieval with automatic type conversion
 * - Runtime configuration modification
 * - Configuration validation and error handling
 * - Summary and debugging output
 * 
 * The class uses the pImpl pattern for efficient compilation and future extensibility.
 * All configuration values are stored in a vector of ConfigEntry structures.
 * 
 * Usage example:
 * @code
 * auto config = std::make_shared<Config>();
 * config->loadFromFile("configs/default.cfg");
 * config->set("neuron_count", 1000);
 * int count = config->getOr<int>("neuron_count", 500);
 * @endcode
 * 
 * @note Thread safety: Configuration operations are not thread-safe. Use external
 * synchronization if multiple threads access the same configuration object.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new empty configuration object with default values.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Properly cleans up pImpl resources.
     */
    ~Config();
    
    /**
     * @brief Disable copying (non-copyable)
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * @param other Config object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * @param other Config object to move from
     * @return Reference to this object
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Loads configuration values from a text file using the key=value format.
     * Supports optional quoting for string values and ignores empty lines and comments.
     * 
     * @param filepath Path to the configuration file
     * @return true if file was successfully loaded, false otherwise
     * @note Current implementation uses simple key=value format. Future versions
     * will support JSON/YAML formats.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Parses command-line arguments in the format --key=value or -k value.
     * All parsed arguments are marked as CommandLine source, giving them highest
     * priority over file and default values.
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument array (standard C main() format)
     * @return true if arguments were successfully parsed, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Saves all configuration entries to a text file in key=value format.
     * Values are formatted appropriately based on their type (strings are quoted,
     * booleans use true/false, arrays use [value1, value2] notation).
     * 
     * @param filepath Path where the configuration file will be saved
     * @return true if file was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves a configuration value with compile-time type checking. If the
     * stored value is not of the requested type, the behavior is undefined
     * (program may crash).
     * 
     * @tparam T Type of value to retrieve (int, double, string, bool, etc.)
     * @param key Configuration key
     * @return std::optional<T> Value if found and type matches, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value, returning a default if the key doesn't exist
     * or the value type doesn't match. More convenient than get() for optional values.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist or type mismatch
     * @return T Retrieved value or defaultValue
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value with an optional source. If the key already exists,
     * the value and source are updated. Otherwise, a new configuration entry is created.
     * 
     * @tparam T Type of value to set (int, double, string, bool, etc.)
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of the configuration value
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value from ConfigValue variant
     * 
     * Overloads for setting configuration values with a ConfigValue variant.
     * This is used internally by other set() methods.
     * 
     * @param key Configuration key
     * @param value Value to set (as variant)
     * @param source Source of the configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set string configuration value
     * 
     * Convenience overload for setting string values.
     * 
     * @param key Configuration key
     * @param value String value to set
     * @param source Source of the configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set integer configuration value
     * 
     * Convenience overload for setting integer values.
     * 
     * @param key Configuration key
     * @param value Integer value to set
     * @param source Source of the configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set floating-point configuration value
     * 
     * Convenience overload for setting double values.
     * 
     * @param key Configuration key
     * @param value Double value to set
     * @param source Source of the configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set boolean configuration value
     * 
     * Convenience overload for setting boolean values.
     * 
     * @param key Configuration key
     * @param value Boolean value to set
     * @param source Source of the configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists in configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration entry
     * 
     * Removes the configuration entry with the specified key, if it exists.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * 
     * Removes all configuration entries, leaving the configuration empty.
     */
    void clear();
    
    /**
     * @brief Get configuration summary as string
     * 
     * @return String representation of all configuration entries including
     *         their values and sources
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: trim whitespace from string
     * 
     * @param str Input string
     * @return String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: convert string to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management system for NLM
 * 
 * This file defines the configuration system used throughout NLM (Neural Learning Machine).
 * The configuration system provides:
 * - Type-safe configuration value storage
 * - Multiple configuration sources (file, command line, runtime)
 * - JSON and simple key-value file format support
 * - Integration with all NLM components
 * 
 * Configuration values support:
 * - Basic types: int, int64_t, double, bool, std::string
 * - Complex types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * - Configuration sources: Default, File, CommandLine, Runtime
 * - Priority-based value resolution (CommandLine > File > Default)
 * 
 * Example usage:
 * @code
 * nlm::Config config;
 * config.set("brain.neuron_count", 1000);
 * int neurons = config.getOr<int>("brain.neuron_count", 1000);
 * if (auto val = config.get<int>("brain.synapse_density")) {
 *     // Value exists
 * }
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

/**
 * @enum ConfigSource
 * @brief Configuration value sources with priority ordering
 * 
 * Configuration sources determine the priority of configuration values:
 * - Default: Initial values set by the program
 * - File: Values loaded from configuration file (e.g., configs/default.cfg)
 * - CommandLine: Values from command line arguments (highest priority)
 * - Runtime: Values set at runtime during execution
 * 
 * Source priority: CommandLine > File > Default (Runtime can override)
 */
enum class ConfigSource {
public:
    /** Default configuration value */
    Default,
    /** Configuration value loaded from file */
    File,
    /** Configuration value from command line arguments */
    CommandLine,
    /** Runtime configuration value */
    Runtime
};

/**
 * @struct ConfigEntry
 * @brief Single configuration entry with metadata
 * 
 * Represents a configuration key-value pair with source information and description.
 * Used internally by the Config class for value management and tracking.
 * 
 * @tparam T Type of configuration value (int, double, std::string, etc.)
 */
struct ConfigEntry {
    std::string key;                    ///< Configuration key (e.g., "brain.neuron_count")
    ConfigValue value;                  ///< Configuration value (variant type)
    ConfigSource source;               ///< Source of this configuration value
    std::string description;           ///< Human-readable description
    
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
     * @param s Configuration source
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Main configuration management class for NLM
 * 
 * Provides comprehensive configuration management with type-safe value access,
 * multiple sources, file I/O, and command-line integration. This class is used
 * by all NLM components to access simulation parameters, neural properties,
 * and system settings.
 * 
 * The configuration system supports:
 * - File-based configuration (JSON format)
 * - Command-line parameter overrides
 * - Type-safe value retrieval with automatic type conversion
 * - Default value fallback
 * - Configuration validation
 * - State persistence and checkpointing
 * 
 * Configuration values are organized with dot notation (e.g., "brain.neuron_count",
 * "plasticity.stdp.learning_rate") for hierarchical organization.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object with default values for
     * all NLM system parameters.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up configuration resources and performs cleanup.
     */
    ~Config();
    
    // Rule of five: prevent copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Loads configuration from a file. Currently supports simple key-value format
     * (configs/default.cfg). Future versions may add JSON support.
     * 
     * @param filepath Path to configuration file
     * @return true if file was successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format:
     * --key=value or --key value
     * 
     * @param argc Argument count
     * @param argv Argument vector
     * @return true if command line was successfully parsed, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Saves current configuration state to a file.
     * 
     * @param filepath Path to save configuration file
     * @return true if file was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves a configuration value if it exists and is of the requested type.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return std::optional<T> Value if found and type matches, otherwise empty
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value, returning default if key doesn't exist.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return T Value from config or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value with specified source.
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     * @param source = ConfigSource::Runtime
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string)
     * 
     * Convenience method for string values.
     * 
     * @param key Configuration key
     * @param value String value
     * @param source = ConfigSource::Runtime
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int)
     * 
     * Convenience method for int values.
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source = ConfigSource::Runtime
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double)
     * 
     * Convenience method for double values.
     * 
     * @param key Configuration key
     * @param value Double value
     * @param source = ConfigSource::Runtime
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (bool)
     * 
     * Convenience method for bool values.
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source = ConfigSource::Runtime
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
     * Removes a configuration key and all its associated values.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> All configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration entries, resetting to default state.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return std::string Human-readable configuration summary
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

} // namespace nlm

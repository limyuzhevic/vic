// Configuration System for NLM (Neural Learning Machine)
//
// This file provides the configuration system for the NLM artificial brain.
// The configuration system allows for loading, storing, and accessing configuration
// values from various sources including files, command line arguments, and runtime.
//
// Configuration is used to control all aspects of the artificial brain including:
// - Neural network parameters (neuron count, region count, connection probability)
// - Plasticity parameters (STDP weights, rates, synaptic properties)
// - Simulation parameters (timestep, recording intervals, checkpointing)
// - Development parameters (plasticity rates, critical periods)
// - World and environment parameters
// - Experiment and logging parameters
//
// Configuration values are stored as a variant type supporting multiple data types:
// int, int64_t, double, bool, std::string, and vectors of these types.

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
 * @brief Configuration value types for storing various data types
 * 
 * The ConfigValue type is a variant that can store:
 * - Basic types: int, int64_t, double, bool, std::string
 * - Container types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * This flexibility allows the configuration system to handle diverse
 * configuration parameters without requiring complex wrapper classes.
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
 * Different sources can override each other with a priority order:
 * CommandLine > File > Runtime > Default
 * 
 * - Default: Internal defaults set by the program
 * - File: Loaded from configuration file
 * - CommandLine: Set via command line arguments
 * - Runtime: Set dynamically during program execution
 */
enum class ConfigSource {
public:
    /** @brief Default configuration value (internal defaults) */
    Default = 0,
    /** @brief Configuration loaded from file */
    File = 1,
    /** @brief Configuration set via command line */
    CommandLine = 2,
    /** @brief Configuration set at runtime */
    Runtime = 3
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration key-value pair with metadata.
 * 
 * @note The order of configuration entries matters for priority resolution.
 * Later entries (same source) can override earlier ones.
 */
struct ConfigEntry {
    /** @brief Configuration key (parameter name) */
    std::string key;
    /** @brief Configuration value (supports multiple types) */
    ConfigValue value;
    /** @brief Source of this configuration value */
    ConfigSource source;
    /** @brief Human-readable description of the configuration parameter */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with Default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with parameters
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for the NLM artificial brain
 * 
 * This class manages all configuration parameters for the brain simulation.
 * It supports loading from files (JSON format), command line arguments,
 * and runtime setting, with proper priority resolution.
 * 
 * The configuration system is used by all components of the brain to
 * control their behavior, parameters, and operation modes.
 * 
 * @note This class follows the PIMPL pattern for implementation hiding.
 * All public interface is defined here, implementation details are
 * in the private Impl structure.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object with default settings.
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    /**
     * @brief Disable copying (PIMPL pattern)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable assignment (PIMPL pattern)
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor (enables ownership transfer)
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator (enables ownership transfer)
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @param filepath Path to the configuration file
     * @return true if the file was successfully loaded, false otherwise
     * 
     * @note The configuration file should be in JSON format with key-value pairs.
     * Each key is a string, and values can be strings, numbers, booleans, or arrays.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if configuration was successfully loaded, false on error
     * 
     * @note Supported formats:
     * --key=value or --key value
     * Special handling for boolean values (true/false)
     * Array values using comma-separated lists
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * @param filepath Path where the configuration will be saved
     * @return true if the file was successfully saved, false otherwise
     * 
     * @note The output format is JSON for compatibility with other tools.
     * Only configuration values with non-default sources are saved.
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value by key
     * 
     * @tparam T Type of the configuration value to retrieve
     * @param key Configuration key
     * @return std::optional<T> The configuration value if found and of correct type, std::nullopt otherwise
     * 
     * @note This method performs type checking. If the stored value is not of type T,
     * it returns std::nullopt rather than attempting conversion.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Type of the configuration value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if the key is not found or of wrong type
     * @return T The configuration value or the default if not available
     * 
     * @note This is a convenience method that combines type checking and default handling.
     * It's safe to call this method for any configuration parameter.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration (default: Runtime)
     * 
     * @note Multiple overloaded methods exist for different value types.
     * The default source is Runtime, which means the value can be overridden
     * by File or CommandLine sources.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string)
     * 
     * @param key Configuration key
     * @param value String configuration value
     * @param source Source of this configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer)
     * 
     * @param key Configuration key
     * @param value Integer configuration value
     * @param source Source of this configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (floating point)
     * 
     * @param key Configuration key
     * @param value Double configuration value
     * @param source Source of this configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean)
     * 
     * @param key Configuration key
     * @param value Boolean configuration value
     * @param source Source of this configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists in the configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * @param key Configuration key to remove
     * 
     * @note This removes the configuration entry completely, regardless of source.
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
     * @note This removes all configuration entries, leaving the object empty.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return std::string Human-readable summary of all configuration values
     * 
     * @note The summary includes all keys, values, and their sources.
     * This is useful for debugging and configuration verification.
     */
    std::string summary() const;
    
private:
    /** @brief Forward declaration of implementation (PIMPL pattern) */
    struct Impl;
    /** @brief Pointer to implementation */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper methods
     * 
     * These methods are used internally by the public interface.
     * They handle the low-level details of configuration storage and retrieval.
     */
    
    // Additional private methods for internal use
    // (Implementation details are hidden in the PIMPL)
};

} // namespace nlm
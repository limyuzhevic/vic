#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management for NLM
 * @author NLM Project
 * @date Current phase
 * 
 * This file provides the configuration management system for NLM.
 * It allows loading, saving, and accessing configuration parameters from
 * files, command line arguments, and runtime settings.
 * 
 * The configuration system supports multiple data types and provides
 * thread-safe access to configuration values.
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <chrono>

namespace nlm {

/**
 * @defgroup config Configuration System
 * 
 * The configuration system provides a unified interface for managing
 * NLM parameters across different sources (defaults, files, command line,
 * and runtime). All parameters are typed and validated.
 * 
 * Key features:
 * - Multiple value types (int, float, string, bool, vectors)
 * - Configuration sources with precedence (Default < File < CommandLine < Runtime)
 * - File I/O with JSON-like format
 * - Command-line argument parsing
 * - Thread-safe access
 * @note Configuration values affect neural network parameters, simulation
 *       settings, and learning behavior.
 */

// Forward declarations
class Config;

/**
 * @brief Configuration value types
 * 
 * Supported value types for configuration parameters.
 * Each type can store different kinds of neural and simulation parameters.
 */
enum class ConfigType {
    Int,          ///< Integer value (e.g., neuron_count)
    Int64,        ///< 64-bit integer (e.g., random_seed)
    Double,       ///< Floating-point value (e.g., learning_rate)
    Bool,         ///< Boolean value (e.g., enable_plasticity)
    String,       ///< String value (e.g., config_file)
    VectorInt,    ///< Vector of integers (e.g., layer_sizes)
    VectorDouble, ///< Vector of floats (e.g., weights)
    VectorString  ///< Vector of strings (e.g., neuron_types)
};

/**
 * @brief Configuration source priority
 * 
 * Configuration sources have a precedence order. When accessing a value,
 * the highest priority source that contains the value is used.
 */
enum class ConfigSource {
    Default = 0,     ///< Default values from code
    File = 1,        ///< Values loaded from file
    CommandLine = 2, ///< Values from command line
    Runtime = 3      ///< Values set at runtime
};

/**
 * @brief Configuration entry
 * 
 * Represents a single configuration parameter with metadata.
 */
struct ConfigEntry {
    std::string key;       ///< Parameter name
    ConfigValue value;      ///< Parameter value
    ConfigSource source;    ///< Source of this value
    std::string description; ///< Human-readable description
    std::chrono::system_clock::time_point timestamp; ///< When value was set
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                    description(), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, 
                ConfigSource s, const std::string& desc = "",
                const std::chrono::system_clock::time_point& ts = 
                    std::chrono::system_clock::now())
        : key(k), value(v), source(s), description(desc), timestamp(ts) {}
};

/**
 * @brief Main configuration class
 * 
 * Manages all configuration parameters for the NLM system. Provides
 * access to neural network parameters, simulation settings, and learning
 * control parameters.
 * 
 * @note Thread-safe for read operations. Write operations should be done
 *       before initialization or when no simulation threads are active.
 * 
 * @see ConfigSource for value precedence
 * @see ConfigValue for supported types
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor (default)
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator (default)
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Loads configuration parameters from a file. The file format is
     * JSON-like with key = value pairs. Comments start with '#'.
     * 
     * @param filepath Path to configuration file
     * @return true if file was loaded successfully, false otherwise
     * 
     * @note File source has higher priority than default values
     * @see loadFromArgs()
     * 
     * @code
     * // Example usage
     * config.loadFromFile("configs/nlm.cfg");
     * @endcode
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format --key=value or --key value.
     * Supports common neural network and simulation parameters.
     * 
     * @param argc Argument count
     * @param argv Argument vector
     * @return true if arguments were parsed successfully
     * 
     * @note Command line source has higher priority than file values
     * @see loadFromFile()
     * 
     * @code
     * // Example usage
     * int main(int argc, char** argv) {
     *     Config config;
     *     config.loadFromArgs(argc, argv);
     * }
     * @endcode
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Saves all configuration parameters to a file in human-readable format.
     * Can be used to export configuration or save settings for later use.
     * 
     * @param filepath Path where to save the configuration
     * @return true if file was saved successfully, false otherwise
     * 
     * @note Only saves parameters that differ from default values
     * @see loadFromFile()
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves a configuration value with automatic type checking.
     * Returns nullopt if the key doesn't exist.
     * 
     * @tparam T Type of value to retrieve (int, double, string, bool, etc.)
     * @param key Configuration parameter name
     * @return Optional containing the value if key exists
     * 
     * @code
     * auto neurons = config.get<size_t>("neuron_count");
     * if (neurons) {
     *     std::cout << "Neuron count: " << *neurons << std::endl;
     * }
     * @endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value, returning a default if the key
     * doesn't exist. More efficient than get() + checking nullopt.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration parameter name
     * @param defaultValue Value to return if key doesn't exist
     * @return Configuration value or default
     * 
     * @code
     * size_t neurons = config.getOr("neuron_count", 1000);
     * std::cout << "Using " << neurons << " neurons" << std::endl;
     * @endcode
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration parameter to a new value. The parameter is stored
     * with the specified source priority (typically Runtime for runtime
     * settings).
     * 
     * @tparam T Type of value to set
     * @param key Configuration parameter name
     * @param value New value
     * @param source Source priority (default: Runtime)
     * 
     * @note Runtime settings override all other sources
     * @see ConfigSource
     */
    template<typename T>
    void set(const std::string& key, const T& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if key exists
     * 
     * @param key Configuration parameter name
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration parameter
     * 
     * Removes a configuration parameter, allowing a higher priority source
     * to provide the value instead.
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
     * @brief Clear all configuration parameters
     * 
     * Removes all configuration parameters, reverting to default values.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * Generates a human-readable summary of all configuration parameters.
     * Useful for debugging and logging.
     * 
     * @return Formatted string with configuration summary
     */
    std::string summary() const;
    
    /**
     * @brief Get configuration entry (internal use)
     * 
     * @param key Configuration parameter name
     * @return Reference to config entry
     * 
     * @note Internal function for implementation
     */
    const ConfigEntry& getEntry(const std::string& key) const;
    
    /**
     * @brief Get all configuration entries
     * 
     * @return Vector of all configuration entries
     * 
     * @note Internal function for implementation
     */
    std::vector<ConfigEntry> getAllEntries() const;
    
private:
    /**
     * @brief Internal implementation structure
     * 
     * PIMPL idiom for hiding implementation details
     */
    struct Impl;
    
    /**
     * @brief Pointer to implementation
     */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: Trim whitespace from string
     * 
     * @param str Input string
     * @return String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: Convert to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of input
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Internal helper: Format value for output
     * 
     * @param value Config value to format
     * @return String representation of value
     */
    static std::string formatValue(const ConfigValue& value);
    
    /**
     * @brief Internal helper: Parse value from string
     * 
     * @param key Configuration key
     * @param value String value to parse
     */
    void parseValue(const std::string& key, const std::string& value);
    
    /**
     * @brief Internal helper: Get current timestamp
     * 
     * @return Current system time as string
     */
    static std::string getCurrentTimestamp();
    
    /**
     * @brief Internal helper: Get value from variant
     * 
     * @tparam T Target type
     * @param variant Variant containing value
     * @return Value of type T if possible
     * @throw std::bad_variant_access if type mismatch
     */
    template<typename T>
    static T getFromVariant(const ConfigValue& variant) {
        return std::get<T>(variant);
    }
    
    /**
     * @brief Internal helper: Check if variant can be converted to type
     * 
     * @tparam T Target type
     * @param variant Variant to check
     * @return true if conversion is possible
     */
    template<typename T>
    static bool canConvertTo(const ConfigValue& variant) {
        return std::holds_alternative<T>(variant);
    }
};

} // namespace nlm

/**
 * @example config_example.cpp
 * 
 * Example of using the configuration system:
 * 
 * @code
 * #include "Config.hpp"
 * 
 * int main() {
 *     nlm::Config config;
 *     
 *     // Load from file
 *     config.loadFromFile("nlm.cfg");
 *     
 *     // Set runtime values
 *     config.set("neuron_count", 1000);
 *     config.set("learning_rate", 0.01);
 *     
 *     // Use values
 *     size_t neurons = config.getOr("neuron_count", 100);
 *     double rate = config.getOr("learning_rate", 0.001);
 *     
 *     std::cout << "Configuration: " << config.summary() << std::endl;
 * }
 * @endcode
 */

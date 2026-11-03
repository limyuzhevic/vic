#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Configuration value types
// Uses std::variant to support multiple value types including primitive types
// and collections (vectors of int, double, or string).
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
 * Indicates where a configuration value originated from:
 * - Default: Built-in default value
 * - File: Loaded from configuration file (JSON/YAML)
 * - CommandLine: Set via command line arguments
 * - Runtime: Set programmatically at runtime
 */
enum class ConfigSource {
    Default,      ///< Built-in default value
    File,         ///< Loaded from configuration file (JSON/YAML)
    CommandLine,  ///< Set via command line arguments
    Runtime       ///< Set programmatically at runtime
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration parameter with its value and source.
 */
struct ConfigEntry {
    std::string key;          ///< Configuration parameter name
    ConfigValue value;        ///< Configuration parameter value
    ConfigSource source;      ///< How the value was obtained
    std::string description;  ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor
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
 * @brief Main configuration class for the NLM brain system
 * 
 * This class manages all configuration parameters for the neural learning machine,
 * supporting multiple file formats (JSON, YAML, and legacy key=value format).
 * 
 * Features:
 * - Type-safe configuration values (int, double, bool, string, vectors)
 * - Multiple configuration sources (file, command line, runtime)
 * - JSON and YAML file format support
 * - Backward compatibility with simple key=value files
 * - Type conversion and validation
 * 
 * Configuration can be loaded from:
 * 1. JSON/YAML files with nested key-value pairs
 * 2. Command line arguments (--key=value or -key value)
 * 3. Programmatic setting at runtime
 * 
 * Example JSON configuration:
 * @code
 * {
 *   "brain": {
 *     "neuron_count": 1000,
 *     "region_count": 2,
 *     "connection_probability": 0.1
 *   },
 *   "plasticity": {
 *     "stdp_ltp_weight": 0.01,
 *     "stdp_ltd_weight": 0.012
 *   }
 * }
 * @endcode
 */
class Config {
public:
    /**
     * @brief Default constructor
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    /**
     * @brief Disabled copy constructor
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disabled copy assignment operator
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Automatically detects file format (JSON, YAML, or legacy key=value)
     * and parses accordingly.
     * 
     * @param filepath Path to configuration file
     * @return true if file loaded successfully, false on error
     * @throws std::runtime_error if file cannot be read or parsed
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports two formats:
     * - --key=value
     * - -key value
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if arguments parsed successfully, false on error
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Currently saves in legacy key=value format for compatibility.
     * JSON/YAML output can be added in the future.
     * 
     * @param filepath Path where configuration will be saved
     * @return true if file saved successfully, false on error
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type specification
     * 
     * Retrieves a configuration value and converts it to the requested type.
     * If the value doesn't exist or type conversion fails, returns std::nullopt.
     * 
     * @tparam T Type to convert the configuration value to
     * @param key Configuration parameter name
     * @return std::optional<T> The configuration value or std::nullopt if not found
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value or returns default if not found.
     * 
     * @tparam T Type to convert the configuration value to
     * @param key Configuration parameter name
     * @param defaultValue Default value to return if key not found
     * @return T The configuration value or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value with an optional source. If the key already
     * exists, its value and source are updated.
     * 
     * @param key Configuration parameter name
     * @param value Configuration value
     * @param source Source of the configuration value (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string overload)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int overload)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int64_t overload)
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double overload)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (bool overload)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration parameter name
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Removes a configuration key and all its values.
     * 
     * @param key Configuration parameter name to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> List of all configuration parameter names
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration entries, leaving the configuration object
     * in its default state.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * Returns a human-readable summary of all configuration values.
     * 
     * @return std::string Formatted configuration summary
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper to trim whitespace from strings
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper to convert string to lowercase
     * 
     * @param str String to convert
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

// Template method implementations need to be in the header file
// to avoid linker errors from multiple template instantiations.

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types for storing various data types
 * 
 * Supports basic types (int, int64_t, double, bool, string) and
 * collections (vectors of basic types). This enables flexible configuration
 * for neural network parameters, simulation settings, and experiment control.
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
 * @brief Type alias for float configuration values
 */
using FloatValue = float;

/**
 * @brief Type alias for string configuration values
 */
using StringValue = std::string;

/**
 * @brief Type alias for boolean configuration values
 */
using BoolValue = bool;

/**
 * @brief Exception thrown when configuration operations fail
 */
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Configuration source tracking for audit trail
 */
enum class ConfigSource {
public:
    /**
     * @brief Default configuration (hardcoded values)
     */
    Default = 0,
    
    /**
     * @brief Configuration loaded from file
     */
    File = 1,
    
    /**
     * @brief Configuration from command line arguments
     */
    CommandLine = 2,
    
    /**
     * @brief Configuration set at runtime via API
     */
    Runtime = 3
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

/**
 * @brief Configuration entry with metadata
 * 
 * Stores a configuration key-value pair along with its source and
 * descriptive information for documentation and debugging purposes.
 */
struct ConfigEntry {
    std::string key;                    ///< Configuration key
    ConfigValue value;                   ///< Configuration value (variant type)
    ConfigSource source;                 ///< Source of the configuration
    std::string description;             ///< Human-readable description of this parameter
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with all fields
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    /**
     * @brief Check if this entry is valid
     * 
     * @return true if key is not empty
     */
    bool isValid() const { return !key.empty(); }
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON/YAML file
     * 
     * Supports both JSON and YAML formats. If the file extension is .json,
     * JSON parsing is used. If .yaml or .yml, YAML parsing is used.
     * Falls back to simple key=value format for other extensions.
     * 
     * @param filepath Path to configuration file
     * @return true if configuration was loaded successfully
     * @throws ConfigException if file cannot be read or contains invalid syntax
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports both --key=value and -k value formats.
     * Command line arguments override file and default values.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if arguments were parsed successfully
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * Configuration is saved in human-readable JSON format with proper
     * indentation and value type preservation.
     * 
     * @param filepath Path to output configuration file
     * @return true if configuration was saved successfully
     * @throws ConfigException if file cannot be written
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves a configuration value by key. The return type must match
     * the stored value type exactly for safety.
     * 
     * @tparam T Type of the configuration value
     * @param key Configuration key
     * @return Optional containing the value if found, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value by key, returning a default value
     * if the key is not found. This is the preferred method for safe access
     * to configuration parameters.
     * 
     * @tparam T Type of the configuration value
     * @param key Configuration key
     * @param defaultValue Value to return if key is not found
     * @return Configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value with type safety
     * 
     * Sets a configuration value with the specified source. Different source
     * types allow for tracking the origin of configuration values.
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string version)
     * 
     * Convenience overload for string values. Automatically converts
     * to ConfigValue variant type.
     * 
     * @param key Configuration key
     * @param value String value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer version)
     * 
     * Convenience overload for integer values.
     * 
     * @param key Configuration key
     * @param value Integer value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int64 version)
     * 
     * Convenience overload for int64 values.
     * 
     * @param key Configuration key
     * @param value int64 value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double version)
     * 
     * Convenience overload for double values.
     * 
     * @param key Configuration key
     * @param value Double value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean version)
     * 
     * Convenience overload for boolean values.
     * 
     * @param key Configuration key
     * @param value Boolean value to store
     * @param source Source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration contains a key
     * 
     * @param key Configuration key to check
     * @return true if key exists in configuration
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration entry
     * 
     * Removes the configuration entry with the specified key if it exists.
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
     */
    void clear();
    
    /**
     * @brief Get configuration summary as string
     * 
     * @return Formatted string representation of all configuration entries
     */
    std::string summary() const;
    
    /**
     * @brief Validate configuration against required parameters
     * 
     * Checks that all required configuration parameters are present and
     * have valid values. Throws ConfigException if validation fails.
     * 
     * @param requiredKeys Vector of keys that must be present
     * @param valueValidators Map of key to validation function
     * @throws ConfigException if validation fails
     */
    void validate(const std::vector<std::string>& requiredKeys,
                  const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& valueValidators = {}) const;
    
    /**
     * @brief Merge another configuration into this one
     * 
     * Configuration from the other config will replace existing values
     * if the keys match. Source priority: File > CommandLine > Runtime > Default.
     * 
     * @param other Configuration to merge
     */
    void merge(const Config& other);
    
    /**
     * @brief Get configuration statistics
     * 
     * @return String containing configuration statistics (count, sources, etc.)
     */
    std::string getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

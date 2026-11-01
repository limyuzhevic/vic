#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

/**
 * @brief Configuration value types for NLM
 * 
 * This variant type supports all basic data types used in NLM configuration,
 * including scalars and collections. The type-safe approach allows storing
 * different kinds of configuration values while providing compile-time type
 * checking for retrieval operations.
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
 * Tracks the origin of each configuration value to manage precedence and
 * override behavior. Configuration values from different sources can be
 * combined, with command line arguments taking highest priority.
 */
enum class ConfigSource {
public:
    /** Default configuration values set by the system */
    Default = 0,
    /** Values loaded from configuration files */
    File = 1,
    /** Values provided via command line arguments */
    CommandLine = 2,
    /** Values set at runtime */
    Runtime = 3
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration key-value pair with metadata about
 * its source and description. This structure is used internally by the
 * Config class to maintain configuration state.
 */
struct ConfigEntry {
    /** Configuration key for identification */
    std::string key;
    /** Configuration value of flexible type */
    ConfigValue value;
    /** Source indicating how the value was obtained */
    ConfigSource source;
    /** Human-readable description of the configuration parameter */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Human-readable description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM
 * 
 * This class provides a comprehensive configuration management system
 * supporting multiple formats (JSON, simple key=value) with validation and
 * error handling. It serves as the central configuration hub for all
 * NLM components, enabling consistent parameter management across the
 * neural simulation framework.
 * 
 * The configuration system supports:
 * - JSON and YAML configuration files
 * - Command line argument parsing
 * - Value type conversion and validation
 * - Source-based precedence management
 * - Configuration validation and error reporting
 * 
 * @note Uses Pimpl pattern for implementation hiding and ABI stability
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object with default values
     * initialized for core simulation parameters.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up configuration resources and performs final cleanup.
     */
    ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Copying configuration objects is disabled to prevent unintended
     * sharing and maintain configuration integrity.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Assignment is disabled for the same reasons as the copy constructor.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Enables efficient transfer of configuration ownership between
     * objects while maintaining resource safety.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Enables efficient assignment of configuration objects by moving
     * ownership of internal resources.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Supports both JSON and simple key=value configuration formats.
     * Automatically detects file format and parses accordingly with
     * comprehensive error handling and validation.
     * 
     * @param filepath Path to the configuration file
     * @return true if configuration was successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in both --key=value and -k value
     * formats, supporting standard NLM command line options.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if configuration was successfully loaded, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Exports current configuration to a file in human-readable format.
     * Note: Currently saves in a simple key=value format with descriptions.
     * 
     * @param filepath Path where configuration should be saved
     * @return true if configuration was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type checking
     * 
     * Retrieves a configuration value with strong type safety. The returned
     * std::optional indicates whether the key exists in the configuration.
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key to retrieve
     * @return std::optional<T> Configuration value if key exists, std::nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value, returning the specified default
     * if the key does not exist. This is the preferred method for obtaining
     * configuration values in most code paths.
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if key doesn't exist
     * @return T Configuration value or default if key doesn't exist
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value (generic)
     * 
     * Sets a configuration value with automatic type conversion and validation.
     * The source parameter tracks where the value originated from.
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source indicating how the value was obtained
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string)
     * 
     * Convenience overload for setting string configuration values.
     * 
     * @param key Configuration key
     * @param value String configuration value
     * @param source Source indicating how the value was obtained
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer)
     * 
     * Convenience overload for setting integer configuration values.
     * 
     * @param key Configuration key
     * @param value Integer configuration value
     * @param source Source indicating how the value was obtained
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (64-bit integer)
     * 
     * Convenience overload for setting 64-bit integer configuration values.
     * 
     * @param key Configuration key
     * @param value 64-bit integer configuration value
     * @param source Source indicating how the value was obtained
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (floating-point)
     * 
     * Convenience overload for setting floating-point configuration values.
     * 
     * @param key Configuration key
     * @param value Floating-point configuration value
     * @param source Source indicating how the value was obtained
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean)
     * 
     * Convenience overload for setting boolean configuration values.
     * 
     * @param key Configuration key
     * @param value Boolean configuration value
     * @param source Source indicating how the value was obtained
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration contains a key
     * 
     * @param key Configuration key to check for
     * @return true if the key exists in the configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Removes the specified configuration key and its associated value,
     * restoring it to default status. This operation cannot remove
     * default configuration values.
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
     * Removes all non-default configuration values, restoring the
     * configuration to its initial state.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return std::string Human-readable summary of all configuration values
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation structure (Pimpl idiom)
     * 
     * The private implementation hides the concrete details of the
     * configuration storage and management from the public interface,
     * providing ABI stability and encapsulation.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     * 
     * @param str Input string
     * @return std::string String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str Input string
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm
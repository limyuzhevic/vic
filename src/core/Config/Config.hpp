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
 * @namespace nlm
 * @brief Neural Learning Machine (NLM) framework namespace
 * @details Contains all classes and types for the NLM neural simulation framework,
 * including neural computation, memory systems, cognition, and neuromodulation.
 */

/**
 * @typedef ConfigValue
 * @brief Type for configuration values (variant of supported types)
 * @details Configuration values can be integers, floating-point numbers, booleans,
 * strings, or arrays of these types. This provides flexibility for specifying
 * diverse configuration parameters for the neural system.
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
 * @enum ConfigSource
 * @brief Source of configuration values
 * @details Tracks where configuration values originate to resolve precedence
 * (File > CommandLine > Runtime > Default). This helps determine which value
 * should take precedence when multiple sources specify the same parameter.
 */
enum class ConfigSource {
    Default,   ///< Default built-in value
    File,      ///< Loaded from configuration file
    CommandLine, ///< From command-line arguments
    Runtime    ///< Set at runtime during execution
};

/**
 * @struct ConfigEntry
 * @brief Single configuration parameter with metadata
 * @details Represents one configuration parameter including its key, value,
 * source, and descriptive information. Used internally for the configuration
 * system and for generating configuration summaries.
 */
struct ConfigEntry {
    std::string key;           ///< Configuration parameter name
    ConfigValue value;         ///< Value of the parameter
    ConfigSource source;       ///< Source of the value
    std::string description;   ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor
     * @details Creates an empty configuration entry
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * @param k Configuration key/name
     * @param v Value to store
     * @param s Source of the value
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Central configuration management system
 * @details Provides a flexible, type-safe configuration system for the NLM
 * neural simulation framework. Supports loading from JSON/YAML files, command-line
 * arguments, and runtime modifications. Configuration values can be accessed
 * and modified using type-safe get() and set() methods.
 * 
 * The configuration system supports multiple value types and sources, with
 * built-in precedence resolution (File > CommandLine > Runtime > Default).
 * This enables complex configuration scenarios while maintaining simplicity
 * for common use cases.
 * 
 * @note JSON configuration format is now fully implemented in Phase 2
 * @see ConfigSource for configuration source precedence
 */
class Config {
public:
    /**
     * @brief Constructs an empty configuration object
     * @details Creates a configuration object with no predefined values.
     * @post Configuration is initialized, empty, and ready for loading or modification
     */
    Config();
    
    /**
     * @brief Destroys the configuration object
     * @details Cleans up all configuration entries and memory management structures
     */
    ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     * @details Configuration objects are not copyable to maintain proper
     * memory management and consistency
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * @details Configuration objects are not copyable to maintain proper
     * memory management and consistency
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor (defaulted)
     * @details Transfers ownership of the internal implementation
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator (defaulted)
     * @details Transfers ownership of the internal implementation
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Loads configuration from file
     * @details Reads configuration parameters from a file, supporting both JSON
     * and simple key=value formats. JSON format is preferred and supports
     * nested objects and arrays.
     * 
     * @param filepath Path to the configuration file
     * @return bool true if file was successfully loaded, false otherwise
     * @post Configuration contains values from the file
     * 
     * @note File format can be either JSON or simple key=value pairs
     * JSON format is tried first, falling back to simple format if parsing fails
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Loads configuration from command-line arguments
     * @details Parses command-line arguments in formats like --key=value or -key value,
     * allowing runtime configuration of the neural system without requiring files.
     * 
     * @param argc Number of command-line arguments
     * @param argv Array of command-line argument strings
     * @return bool true if arguments were successfully parsed, false otherwise
     * @post Configuration contains values from command-line arguments
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Saves configuration to file
     * @details Writes the current configuration to a file. Currently saves in
     * simple key=value format, though JSON format could be added in future phases.
     * 
     * @param filepath Path to the output file
     * @return bool true if file was successfully written, false otherwise
     * @post Configuration is persisted to file
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Gets a configuration value with type safety
     * @details Retrieves a configuration parameter by key, with automatic type
     * checking and conversion. Returns nullopt if the key doesn't exist or the
     * type doesn't match.
     * 
     * @tparam T Type of value to retrieve (int, int64_t, double, bool, or std::string)
     * @param key Configuration parameter name
     * @return std::optional<T> Value if found and type matches, nullopt otherwise
     * @post Configuration state is unchanged
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Gets a configuration value with type safety and default
     * @details Retrieves a configuration parameter, returning a default value
     * if the key doesn't exist or the type doesn't match.
     * 
     * @tparam T Type of value to retrieve (int, int64_t, double, bool, or std::string)
     * @param key Configuration parameter name
     * @param defaultValue Default value to return if key not found
     * @return T Value from configuration or default
     * @post Configuration state is unchanged
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Sets a configuration value
     * @details Assigns a configuration parameter with the specified value and source.
     * The source determines the precedence of this value (higher source = higher precedence).
     * 
     * @param key Configuration parameter name
     * @param value Value to set
     * @param source Origin of the value (default: Runtime)
     * @post Configuration contains the new value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a string configuration value
     * @details Convenience overload for setting string values.
     * 
     * @param key Configuration parameter name
     * @param value String value to set
     * @param source Origin of the value (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets an integer configuration value
     * @details Convenience overload for setting integer values.
     * 
     * @param key Configuration parameter name
     * @param value Integer value to set
     * @param source Origin of the value (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a floating-point configuration value
     * @details Convenience overload for setting double values.
     * 
     * @param key Configuration parameter name
     * @param value Floating-point value to set
     * @param source Origin of the value (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a boolean configuration value
     * @details Convenience overload for setting boolean values.
     * 
     * @param key Configuration parameter name
     * @param value Boolean value to set
     * @param source Origin of the value (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Checks if a configuration key exists
     * @details Determines whether a configuration parameter has been set.
     * 
     * @param key Configuration parameter name to check
     * @return bool true if key exists in configuration
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Removes a configuration parameter
     * @details Deletes a configuration parameter from the configuration object,
     * if present.
     * 
     * @param key Configuration parameter name to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Gets all configuration keys
     * @details Retrieves all configuration parameter names currently stored.
     * 
     * @return std::vector<std::string> Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clears all configuration parameters
     * @details Removes all configuration parameters, resetting the configuration
     * to an empty state.
     * 
     * @post Configuration is empty
     */
    void clear();
    
    /**
     * @brief Gets a summary of all configuration parameters
     * @details Generates a human-readable string representation of the current
     * configuration, including all keys, values, and sources.
     * 
     * @return std::string Formatted configuration summary
     * @post Configuration state is unchanged
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trims whitespace from a string
     * @details Removes leading and trailing whitespace characters (spaces, tabs,
     * newlines, carriage returns) from the input string.
     * 
     * @param str Input string
     * @return std::string String with whitespace trimmed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Converts a string to lowercase
     * @details Transforms all alphabetic characters in the input string to lowercase.
     * This is used internally for case-insensitive string comparisons.
     * 
     * @param str Input string
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
    // Internal JSON parser (declared for implementation in .cpp file)
    class JsonParser;
};

} // namespace nlm
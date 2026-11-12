#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management for NLM (Neural Learning Machine)
 * 
 * This file provides a comprehensive configuration system for the NLM brain simulation.
 * It supports loading/saving configurations from JSON files, command line arguments,
 * and runtime modifications. The configuration system uses type-safe access with
 * support for various data types including integers, doubles, strings, bomatics, and
 * complex vectors of these types.
 * 
 * @author NLM Development Team
 * @version 0.1.0
 * @date 2026
 * @warning This configuration system is optimized for the NLM brain simulation
 *          and may not be suitable for other applications without modification.
 * @see Config.cpp
 * @see main.cpp
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Union type that can hold any supported configuration value.
 * 
 * The ConfigValue type is a variant that supports the following types:
 * - Basic types: int, int64_t, double, bool, std::string
 * - Collection types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * @note This type provides type-safe access to configuration values and allows
 *       for flexible configuration options.
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
 * @brief Enumeration of configuration sources.
 * 
 * This enumeration tracks the source of configuration values to maintain
 * configuration precedence and for debugging purposes.
 */
enum class ConfigSource {
public:
    /** @brief Default configuration values set by the code */
    Default = 0,
    /** @brief Configuration loaded from file */
    File = 1,
    /** @brief Configuration from command line arguments */
    CommandLine = 2,
    /** @brief Configuration set at runtime */
    Runtime = 3
};

/**
 * @brief Structure representing a single configuration entry.
 * 
 * Each configuration entry stores a key-value pair along with metadata about
 * the source of the value and a description for documentation purposes.
 */
struct ConfigEntry {
    /** @brief The configuration key/name */
    std::string key;
    
    /** @brief The configuration value (type varies based on ConfigValue) */
    ConfigValue value;
    
    /** @brief The source of this configuration value */
    ConfigSource source;
    
    /** @brief Human-readable description of the configuration parameter */
    std::string description;
    
    /**
     * @brief Default constructor.
     * 
     * Creates an empty configuration entry with default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with parameters.
     * 
     * @param k The configuration key
     * @param v The configuration value
     * @param s The source of the configuration value
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM.
 * 
 * This class provides a comprehensive configuration management system for the
 * NLM brain simulation. It supports loading from JSON files, command line arguments,
 * runtime modifications, and saving configurations to files.
 * 
 * The class uses the Pimpl idiom (pointer to implementation) to hide the
 * internal implementation details and provide better encapsulation.
 * 
 * Configuration values can be accessed using type-specific getter methods
 * with automatic type conversion and default value support.
 * 
 * @warning Configuration values should be set before any NLM components are
 *          initialized, as many components may depend on specific configuration
 *          parameters.
 * 
 * @note Configuration keys are case-insensitive but the original case is
 *       preserved for serialization purposes.
 * 
 * @see ConfigSource for configuration source tracking
 * @see ConfigEntry for individual configuration entries
 * @see main.cpp for example usage
 */
class Config {
public:
    /**
     * @brief Default constructor.
     * 
     * Creates an empty configuration object. Typically, configuration values
     * should be loaded from file or set manually before use.
     */
    Config();
    
    /**
     * @brief Destructor.
     * 
     * Properly cleans up internal implementation resources.
     */
    ~Config();
    
    /**
     * @brief Copy constructor (deleted).
     * 
     * Configuration objects are not copyable to prevent shallow copies
     * and maintain the Pimpl idiom. Use move constructor instead.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted).
     * 
     * Configuration objects are not copyable to prevent shallow copies
     * and maintain the Pimpl idiom. Use move assignment instead.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor.
     * 
     * Enables efficient transfer of ownership of the internal implementation
     * from one Config object to another.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator.
     * 
     * Enables efficient transfer of ownership of the internal implementation
     * from one Config object to another.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a JSON file.
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if the file was successfully loaded and parsed, false otherwise
     * @note If the file cannot be found or parsed, the operation fails silently
     *       and the configuration continues with existing values or defaults.
     * @see saveToFile for saving configurations
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments.
     * 
     * Parses command line arguments and applies them to the current configuration.
     * Expected argument format: --key value or --key=value
     * 
     * @param argc Argument count (typically from main())
     * @param argv Argument vector (typically from main())
     * @return true if arguments were successfully parsed, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a JSON file.
     * 
     * @param filepath Path where the configuration should be saved
     * @return true if the file was successfully written, false otherwise
     * @see loadFromFile for loading configurations
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type checking.
     * 
     * Retrieves a configuration value of the specified type. If the key doesn't
     * exist or the value has a different type, returns std::nullopt.
     * 
     * @tparam T The expected type of the configuration value
     * @param key The configuration key to retrieve
     * @return std::optional containing the value if found and type matches, 
     *         std::nullopt otherwise
     * @note This method is defined in the header file to allow template instantiation
     *       in every translation unit that uses it.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with a default fallback.
     * 
     * Retrieves a configuration value of the specified type. If the key doesn't
     * exist or the value has a different type, returns the default value.
     * 
     * @tparam T The expected type of the configuration value
     * @param key The configuration key to retrieve
     * @param defaultValue The value to return if the key is not found or has wrong type
     * @return The configuration value or defaultValue if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value with type safety.
     * 
     * This is the generic version that accepts a ConfigValue variant.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to set
     * @param source The source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from a string.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to set
     * @param source The source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from an integer.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to set
     * @param source The source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from a 64-bit integer.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to set
     * @param source The source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from a double.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to set
     * @param source The source of this configuration value (default: Runtime)
     */
>\
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from a boolean.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to set
     * @param source The source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**\n     * @brief Check if a configuration key exists.
     * 
     * @param key The configuration key to check
     * @return true if the key exists in the configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key from the configuration.
     * 
     * @param key The configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys.
     * 
     * @return Vector containing all configuration keys currently in use
     * @note The keys are not guaranteed to be in any particular order
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values.
     * 
     * Removes all configuration entries from the configuration object.
     * After this operation, the configuration object will be empty.
     feral:// consider if we need to protect against reentrant calls
     */
    void clear();
    
    /**
     * @brief Generate a human-readable summary of the configuration.
     * 
     * Creates a formatted string containing all configuration values with their
     * keys, types, and values for debugging and documentation purposes.
     * 
     * @return String containing the configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation structure (Pimpl idiom).
     * 
     * This structure contains the actual implementation details of the Config class.
     * It's used to hide implementation complexity and reduce compilation coupling.
     * feral:// document the memory management requirements
     */
    struct Impl;
    
    /**
     * @brief Pointer to the internal implementation.
     * 
     * This follows the Pimpl idiom pattern where the implementation details are
     * stored in a separate structure. This allows the header file to remain clean
     * and minimizes compilation dependencies.
     */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: trim whitespace from a string.
     * 
     * @param str The string to trim
     * @return String with leading and trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: convert string to lowercase.
     * 
     * @param str The string to convert
     * @return Lowercase version of the input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm
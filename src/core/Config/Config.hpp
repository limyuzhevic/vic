#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types - supports multiple data types for flexible configuration
// Used for type-safe configuration value storage and retrieval
using ConfigValue = std::variant<
    int,                    ///< Integer value (typically for simulation steps, counts, flags)
    int64_t,                ///< 64-bit integer (large counts, IDs)
    double,                 ///< Floating-point value (rates, probabilities, durations)
    bool,                   ///< Boolean value (flags, enabled/disabled states)
    std::string,            ///< String value (names, paths, identifiers)
    std::vector<int>,       ///< Integer array (lists of integers)
    std::vector<double>,    ///< Double array (lists of floating-point values)
    std::vector<std::string>///< String array (lists of strings)
>;

// Configuration source - indicates where a configuration value originated
// Used for precedence resolution and debugging configuration sources
enum class ConfigSource {
    Default,      ///< Default value built into the system
    File,         ///< Loaded from configuration file (JSON)
    CommandLine,  ///< Set via command line arguments
    Runtime       ///< Set programmatically during runtime
};

// Configuration entry - represents a single key-value pair with metadata
// Used for configuration inspection, validation, and debugging
struct ConfigEntry {
    std::string key;      ///< Configuration key/name
    ConfigValue value;     ///< Stored configuration value
    ConfigSource source;   ///< Origin of this configuration value
    std::string description; ///< Human-readable description of the configuration option
    
    /**
     * @brief Default constructor creates an empty configuration entry
     * 
     * @note Use explicit constructors for meaningful configuration entries
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Create a configuration entry with key, value, and source
     * 
     * @param k Configuration key/name
     * @param v Configuration value
     * @param s Origin of the configuration value
     * @param desc Human-readable description (optional, defaults to empty string)
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for the NLM brain system
 * 
 * Manages all configuration parameters including default values, file loading,
 * command-line parsing, and runtime modifications. Provides type-safe value access
 * through templated getter methods.
 * 
 * Configuration follows a precedence hierarchy:
 * 1. Command line arguments (highest precedence)
 * 2. Configuration file (JSON format)
 * 3. Runtime set() calls
 * 4. Default values (lowest precedence)
 * 
 * Thread safety: Not thread-safe for concurrent modification.
 */
class Config {
public:
    /**
     * @brief Default constructor creates an empty configuration
     * 
     * @note Configuration will have access to default values only
     */
    Config();
    
    /**
     * @brief Destructor releases configuration resources
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @param filepath Path to JSON configuration file
     * @return true if file was successfully loaded and parsed, false otherwise
     * 
     * @note File loading respects configuration hierarchy and merges with existing values
     * @throws May throw exceptions for file I/O or JSON parsing errors
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array (typically from main())
     * @return true if command line arguments were successfully parsed, false otherwise
     * 
     * @note Command line arguments override file and default values
     * @note Expected format: --key value [--key value ...]
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save current configuration to JSON file
     * 
     * @param filepath Path where configuration will be saved
     * @return true if configuration was successfully saved, false otherwise
     * 
     * @note Only non-default values may be saved, depending on implementation
     * @note File format is JSON for interoperability
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Retrieve a configuration value with type safety
     * 
     * @tparam T Type to retrieve (must be one of ConfigValue supported types)
     * @param key Configuration key to look up
     * @return std::optional<T> Value if key exists and matches type, nullopt otherwise
     * 
     * @note Automatically handles type conversion and validation
     * @note Returns nullopt for missing keys or type mismatches
     * @note Const method - does not modify configuration
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Retrieve a configuration value with type safety and default fallback
     * 
     * @tparam T Type to retrieve (must be one of ConfigValue supported types)
     * @param key Configuration key to look up
     * @param defaultValue Value to return if key doesn't exist or type mismatch
     * @return T Requested value or default if key not found
     * 
     * @note Template specialization enables compile-time type checking
     * @note More efficient than get() + optional checking for default cases
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value with explicit source
     * 
     * @param key Configuration key to set
     * @param value Configuration value
     * @param source Origin of this configuration (defaults to Runtime)
     * 
     * @note Runtime sources override file and command-line values
     * @note Source tracking helps with debugging and precedence resolution
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for string values
     * 
     * @param key Configuration key to set
     * @param value String value
     * @param source Origin of this configuration (defaults to Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for integer values
     * 
     * @param key Configuration key to set
     * @param value Integer value
     * @param source Origin of this configuration (defaults to Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for floating-point values
     * 
     * @param key Configuration key to set
     * @param value Double value
     * @param source Origin of this configuration (defaults to Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for boolean values
     * 
     * @param key Configuration key to set
     * @param value Boolean value
     * @param source Origin of this configuration (defaults to Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists and has a value, false otherwise
     * 
     * @note Only checks for non-default values
     * @note Const method - does not modify configuration
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key and its value
     * 
     * @param key Configuration key to remove
     * 
     * @note After removal, key will revert to default value
     * @note Source information is cleared for removed entries
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys currently set
     * 
     * @return std::vector<std::string> List of all configuration keys with non-default values
     * 
     * @note Only includes keys that override default values
     * @note Order is not guaranteed
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * @note Resets configuration to default values only
     * @note Source information is cleared for all entries
     */
    void clear();
    
    /**
     * @brief Get a human-readable summary of current configuration
     * 
     * @return std::string Formatted configuration summary including sources and values
     * 
     * @note Useful for debugging and logging configuration state
     * @note Includes both key values and their sources for precedence analysis
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;  ///< Private implementation for PIMPL pattern
    
    /**
     * @brief Internal helper - trim whitespace from both ends of a string
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     * 
     * @note Used for cleaning configuration keys and values
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper - convert string to lowercase
     * 
     * @param str String to convert
     * @return std::string Lowercase version of input string
     * 
     * @note Used for case-insensitive configuration key handling
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm
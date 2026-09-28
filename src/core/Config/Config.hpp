#pragma once
/**
 * @file Config.hpp
 * @brief NLM Configuration System
 * 
 * This file provides a flexible, type-safe configuration system for NLM.
 * It supports configuration values from multiple sources:
 * - Default values (hard-coded)
 * - Configuration files (JSON format)
 * - Command line arguments
 * - Runtime modifications
 * 
 * @author NLM Research Team
 * @date Phase 6 Integration
 * @version 0.1.0
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <sstream>
#include <iomanip>

namespace nlm {

/**
 * @enum class ConfigSource
 * @brief Source of configuration values
 * 
 * Each configuration value can come from a specific source, used to determine
 * precedence when multiple sources define the same value.
 */
enum class ConfigSource {
public:
    /** Default/hard-coded values */
    Default = 0,
    /** Values loaded from configuration files */
    File,
    /** Values from command line arguments */
    CommandLine,
    /** Values set at runtime (highest precedence) */
    Runtime
};

/**
 * @struct ConfigEntry
 * @brief A single configuration key-value pair with metadata
 * 
 * Each configuration entry contains the actual value, its source, and a
 * description for documentation purposes.
 */
struct ConfigEntry {
    std::string key;              ///< Configuration key/name
    ConfigValue value;            ///< Configuration value
    ConfigSource source;          ///< Where this value came from
    std::string description;      ///< Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a configuration entry
     * 
     * @param k Key name
     * @param v Value
     * @param s Source
     * @param desc Description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    /**
     * @brief Check if this entry is valid (has a key)
     */
    bool isValid() const { return !key.empty(); }
    
    /**
     * @brief Get source as string for logging
     */
    static std::string sourceToString(ConfigSource source) {
        switch (source) {
            case ConfigSource::Default: return "DEFAULT";
            case ConfigSource::File: return "FILE";
            case ConfigSource::CommandLine: return "COMMAND_LINE";
            case ConfigSource::Runtime: return "RUNTIME";
            default: return "UNKNOWN";
        }
    }
};

/**
 * @class Config
 * @brief Central configuration manager for NLM
 * 
 * The Config class manages configuration values for the entire NLM system.
 * It provides a unified interface for:
 * - Setting and getting configuration values
 * - Loading from files and command line
 * - Saving configuration state
 * - Validating configuration
 * - Accessing typed values with type safety
 * 
 * The configuration system uses a simple key-value model where keys are
 * strings and values can be integers, floats, booleans, strings, or arrays
 * of these types.
 * 
 * Example usage:
 * @code
 * Config config;
 * config.set("neuron_count", 1000);
 * config.set("simulation_timestep", 0.001);
 * auto neurons = config.getOr<int64_t>("neuron_count", 1000);
 * @endcode
 */
class Config {
public:
    /**
     * @brief Construct a new empty configuration
     */
    Config();
    
    /**
     * @brief Destroy the configuration
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Currently supports JSON format configuration files.
     * Unknown file formats result in a warning but don't fail.
     * 
     * @param filepath Path to configuration file
     * @return true if file was loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports the following command line formats:
     * --key=value
     * --key value
     * 
     * Command line arguments have higher precedence than file values.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if command line arguments were processed successfully
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Currently supports JSON format for output.
     * 
     * @param filepath Path where configuration should be saved
     * @return true if configuration was saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    // =====================================================================
    // Value Access Methods
    // =====================================================================
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * This method attempts to get a configuration value as type T.
     * If the value doesn't exist or isn't of type T, returns std::nullopt.
     * 
     * @tparam T Type to retrieve (int, double, bool, std::string, etc.)
     * @param key Configuration key
     * @return std::optional<T> The value if found and of correct type, otherwise nullopt
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * This method retrieves a configuration value, returning a default value
     * if the key doesn't exist or isn't of the specified type.
     * 
     * @tparam T Type to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist or has wrong type
     * @return T The configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // =====================================================================
    // Value Setting Methods
    // =====================================================================
    
    /**
     * @brief Set a configuration value (generic)
     * 
     * This method sets a configuration value from a generic ConfigValue.
     * For convenience, type-specific versions are provided below.
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (string)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (integer)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (64-bit integer)
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (floating point)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (boolean)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // =====================================================================
    // Query Methods
    // =====================================================================
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
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
     * This removes all values except for default values (those that were
     * originally hard-coded or loaded from configuration files).
     */
    void clear();
    
    /**
     * @brief Get a summary of current configuration
     * 
     * @return std::string Human-readable configuration summary
     */
    std::string summary() const;
    
    // =====================================================================
    // Validation and Helper Methods
    // =====================================================================
    
    /**
     * @brief Validate configuration values
     * 
     * Checks for:
     * - Valid ranges for numeric values
     * - Consistent combinations of values
     * - File paths that exist
     * 
     * @return true if configuration is valid, false otherwise
     */
    bool validate() const;
    
    /**
     * @brief Get all configuration entries
     * 
     * @return std::vector<ConfigEntry> All configuration entries
     */
    std::vector<ConfigEntry> getAllEntries() const;
    
    /**
     * @brief Get entries from a specific source
     * 
     * @param source Configuration source to filter by
     * @return std::vector<ConfigEntry> Entries from the specified source
     */
    std::vector<ConfigEntry> getEntriesFromSource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumericString(const std::string& str);
    static ConfigSource parseConfigSource(const std::string& sourceStr);
    
    // Private constructor for internal use
    explicit Config(std::unique_ptr<Impl> impl);
    
    // Friendship declarations for internal classes
    friend class ConfigImpl;
};

} // namespace nlm

// =====================================================================
// Convenience Template Implementations
// =====================================================================

// These are the actual template implementations that were forward declared
template<typename T>
std::optional<T> nlm::Config::get(const std::string& key) const {
    // Implementation would go here
    return std::nullopt;
}

template<typename T>
T nlm::Config::getOr(const std::string& key, const T& defaultValue) const {
    // Implementation would go here
    return defaultValue;
}

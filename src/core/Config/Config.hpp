#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <limits>

namespace nlm {

// Forward declarations
class Config;

/**
 * @enum ConfigSource
 * @brief Enumeration of configuration sources with precedence order
 *
 * Configuration sources are ordered by precedence (highest to lowest):
 * 1. Runtime - Set programmatically at runtime
 * 2. CommandLine - Set via command line arguments
 * 3. File - Set from configuration file
 * 4. Default - Built-in default values
 */
enum class ConfigSource {
public:
    /** @brief Default configuration values (lowest precedence) */
    Default = 0,
    
    /** @brief Configuration loaded from file (higher precedence than Default) */
    File = 1,
    
    /** @brief Configuration from command line (higher precedence than File) */
    CommandLine = 2,
    
    /** @brief Configuration set at runtime (highest precedence) */
    Runtime = 3
};

/**
 * @struct ConfigEntry
 * @brief Single configuration entry with metadata
 *
 * Represents a single configuration parameter with its value, source,
 * and descriptive documentation.
 */
struct ConfigEntry {
    std::string key;                ///< Configuration parameter key
    ConfigValue value;              ///< Configuration parameter value
    ConfigSource source;            ///< Source of this configuration value
    std::string description;        ///< Human-readable description of the parameter
    
    /** @brief Default constructor */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with all parameters
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Centralized configuration management system
 *
 * The Config class provides a unified interface for managing neural simulation
 * parameters with support for multiple sources, validation, and type safety.
 *
 * Key features:
 * - Multi-source configuration (file, command line, runtime)
 * - Type-safe parameter access with automatic conversion
 * - Parameter validation with custom rules
 * - Configuration persistence (save/load)
 * - Error handling and logging for invalid configurations
 *
 * @note Uses the pImpl pattern for opaque implementation
 * @see ConfigSource for source precedence
 */
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
     * @brief Load configuration from a file
     * @param filepath Path to configuration file (key=value format)
     * @return true if file was successfully loaded, false otherwise
     * 
     * @note File format supports simple key=value pairs
     * @warning Currently supports only basic text format
     * @todo Implement JSON/YAML parser for Phase 2
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * @param argc Argument count
     * @param argv Argument vector
     * @return true if arguments were successfully parsed, false on validation errors
     *
     * Supported formats:
     * - --key=value
     * - --key value
     * - -k value
     *
     * @note All parameters are validated against type and range constraints
     * @warning Invalid parameters are logged as warnings but don't terminate execution
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * @param filepath Path where configuration should be saved
     * @return true if file was successfully saved, false otherwise
     *
     * @note Currently only writes keys and descriptions
     * @todo Implement proper value serialization
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * @tparam T Target type (int, int64_t, double, bool, std::string)
     * @param key Configuration key
     * @return Optional containing value if key exists and type matches, nullopt otherwise
     *
     * @note Type matching is strict - wrong types return nullopt
     * @see getOr() for type-safe access with default values
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * @tparam T Target type
     * @param key Configuration key
     * @param defaultValue Value returned if key doesn't exist or type mismatch
     * @return Value or default if not found
     *
     * @note Type conversion is attempted, but may fail silently
     * @example config->getOr("neuron_count", 1000)
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration value
     *
     * @note Source determines precedence for duplicate keys
     * @see ConfigSource for source precedence
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string version)
     * @param key Configuration key
     * @param value Configuration value as string
     * @param source Source of the configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer version)
     * @param key Configuration key
     * @param value Configuration value as integer
     * @param source Source of the configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (floating-point version)
     * @param key Configuration key
     * @param value Configuration value as floating-point number
     * @param source Source of the configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean version)
     * @param key Configuration key
     * @param value Configuration value as boolean
     * @param source Source of the configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Get configuration summary as string
     * @return String containing all configuration entries with source information
     */
    std::string summary() const;
    
    /**
     * @brief Get human-readable name of configuration source
     * @param source Configuration source
     * @return String representation of the source
     */
    static std::string getConfigSourceName(ConfigSource source);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Remove whitespace from string
     * @param str Input string
     * @return String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
    friend class ConfigValidator;
};

} // namespace nlm

// Template instantiations (implementation in .cpp file)
template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;

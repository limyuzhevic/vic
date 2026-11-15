#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
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

// Configuration source
    enum class ConfigSource {
        Default,
        File,
        CommandLine,
        Runtime
    };

    // Configuration error types
    enum class ConfigError {
        None,
        FileNotFound,
        ParseError,
        InvalidKey,
        InvalidValue,
        PermissionDenied,
        InvalidArgument,
        ConversionError
    };

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @file Config.hpp
 * @brief Centralized configuration management for NLM.
 * @details Provides hierarchical configuration system with:
 * - File-based configuration (JSON format)
 * - Command-line argument parsing
 * - Runtime configuration
 * - Type-safe value access with conversions
 * - Change tracking and source identification
 * - Validation and default value handling
 * 
 * Configuration hierarchy (higher priority overrides lower):
 * 1. Command-line arguments
 * 2. File-based configuration
 * 3. Runtime settings
 * 4. Built-in defaults
 * 
 * @author NLM Development Team
 * @version 0.1.0
 * @ingroup core
 */
class Config {
public:
/**
     * @brief Constructor with default configuration.
     * @details Initializes configuration with built-in defaults:
     * - Neural network parameters (neuron count, connectivity)
     * - Simulation settings (timestep, duration)
     * - Logging configuration
     * - Debug and development options
     */
    Config();
/**
     * @brief Virtual destructor.
     * @details Cleans up implementation resources.
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file.
     * @param[in] filepath Path to configuration file.
     * @return bool True on successful load, false on error.
     * @details Supports JSON format with:
     * - Key-value pairs for settings
     * - Nested objects for grouped settings
     * - Array values for lists
     * - Type conversion and validation
     * @note File must be readable and valid JSON.
     * @warning Invalid file paths or malformed JSON will cause load failure.
     */
    bool loadFromFile(const std::string& filepath);
    
/**
     * @brief Load configuration from command line arguments.
     * @param[in] argc Number of command line arguments.
     * @param[in] argv Array of command line argument strings.
     * @return bool True on successful parsing, false on error.
     * @details Supports standard command line options:
     * - --key=value or --key value
     * - --help and --version options
     * - Environment variable substitution
     * - File inclusion and inheritance
     * @note Command line arguments override file settings.
     */
    bool loadFromArgs(int argc, char** argv);

    /**
     * @brief Save configuration to file.
     * @param[in] filepath Path to save configuration file.
     * @return bool True on successful save, false on error.
     * @details Saves current configuration in JSON format.
     * Creates file if it doesn't exist, overwrites if it does.
     * @note File permissions and directory must be writable.
     */
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    /**
     * @brief Remove leading and trailing whitespace from a string.
     * @param[in] str Input string.
     * @return std::string String with whitespace removed.
     * @details Removes spaces, tabs, carriage returns, and newlines.
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Convert string to lowercase.
     * @param[in] str Input string.
     * @return std::string Lowercase version of input string.
     * @details Uses standard C++ tolower function for ASCII conversion.
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

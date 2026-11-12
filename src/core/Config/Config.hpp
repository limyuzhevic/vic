#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

/**
 * @file Config.hpp
 * @brief Configuration management system for NLM
 * 
 * This header defines the configuration system for the Neural Learning Machine (NLM).
 * It provides a flexible, type-safe configuration management system that supports:
 * - Multiple configuration sources (default, file, command-line, runtime)
 * - Type-safe value storage and retrieval
 * - Configuration validation
 * - Help system for users
 * - Support for common data types (integers, floats, booleans, strings, arrays)
 * 
 * @version 2.0 - Enhanced with validation, help system, and better error handling
 */

// Forward declarations
class Config;

/**
 * @brief Configuration value types
 * 
 * The configuration system supports various value types using std::variant.
 * Each type provides type-safe access through template methods.
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
 * Tracks where a configuration value originated from to handle conflicts.
 * Sources are ordered by priority (highest to lowest):
 * 1. CommandLine - Explicitly set via command line
 * 2. File - Loaded from configuration file
 * 3. Default - Built into the application
 * 4. Runtime - Set dynamically during execution
 */
enum class ConfigSource {
public:
    Default = 0,  ///< Default configuration value
    File = 1,     ///< Loaded from configuration file
    CommandLine = 2,  ///< Set via command line arguments
    Runtime = 3   ///< Set dynamically during runtime
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration key-value pair with metadata.
 * Used internally by the Config class to track configuration values.
 */
struct ConfigEntry {
    std::string key;          ///< Configuration key
    ConfigValue value;        ///< Configuration value
    ConfigSource source;      ///< Source of this configuration value
    std::string description;  ///< Human-readable description
    
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
     * @param desc Human-readable description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class
 * 
 * Provides a comprehensive configuration management system for NLM.
 * Supports loading from files, command-line arguments, and runtime setting.
 * Includes validation, help system, and type-safe value access.
 * 
 * @note This is Phase 2.0 of the configuration system, enhanced from Phase 1.0
 *       with validation, help system, and better error handling.
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
     * @brief Disable copying (non-copyable)
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable moving (specialized constructor)
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file (JSON/YAML format)
     * 
     * @param filepath Path to configuration file
     * @return true if file loaded successfully, false otherwise
     * 
     * @note Phase 2.0: This is a placeholder for Phase 3.0 which will implement
     *       proper JSON/YAML parsing support.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line arguments
     * @return true if parsing succeeded, false if help was displayed
     * 
     * @note Supports both --key=value and -key value formats
     *       Includes --help and --list-configs flags for user assistance.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * @param filepath Path where configuration should be saved
     * @return true if save succeeded, false otherwise
     * 
     * @note Currently writes placeholder values; Phase 3.0 will implement
     *       proper value serialization.
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value by key
     * 
     * @tparam T Target type (must match stored type)
     * @param key Configuration key
     * @return Optional containing value if key exists, nullopt otherwise
     * 
     * @note Type conversion is supported between numeric types (int, int64, double, float)
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default
     * 
     * @tparam T Target type
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return Configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string version)
     * 
     * @param key Configuration key
     * @param value Configuration value as string
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer version)
     * 
     * @param key Configuration key
     * @param value Configuration value as int
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int64 version)
     * 
     * @param key Configuration key
     * @param value Configuration value as int64_t
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double version)
     * 
     * @param key Configuration key
     * @param value Configuration value as double
     * @param source Source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean version)
     * 
     * @param key Configuration key
     * @param value Configuration value as bool
     * @param source Source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration contains a key
     * 
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
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
     * @brief Clear all configuration values
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
    /**
     * @brief Display configuration help
     * 
     * Shows available configuration options and usage instructions.
     */
    void printHelp() const;
    
    /**
     * @brief Display all configuration options
     * 
     * Shows all currently available configuration keys and their types.
     */
    void printAvailableConfigs() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Convert ConfigValue to human-readable type name
     */
    static std::string getTypeName(const ConfigValue& value);
    
    /**
     * @brief Set configuration value from string
     * 
     * @param key Configuration key
     * @param valueStr String representation of value
     * @param source Configuration source
     * @return true if successful, false otherwise
     */
    bool setFromString(const std::string& key, const std::string& valueStr, ConfigSource source);
    
    /**
     * @brief Check if string represents a number
     */
    static bool isNumeric(const std::string& str);
    
    /**
     * @brief Validate configuration values
     * 
     * @param config Configuration to validate
     * @return true if valid, false otherwise
     */
    friend bool validateConfiguration(std::shared_ptr<Config> config);
};

} // namespace nlm

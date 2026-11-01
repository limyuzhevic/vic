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

// Main configuration class
/**
 * @class Config
 * @brief Central configuration management system for NLM
 * 
 * The Config class provides a unified interface for loading, saving, and managing
 * configuration settings from multiple sources including files, command line arguments,
 * and runtime settings. It supports multiple file formats (JSON, YAML, key=value)
 * and provides type-safe access to configuration values.
 * 
 * @note This class follows the Pimpl pattern for implementation hiding
 * @note Thread-safe for read operations, but not for concurrent modifications
 * @note Configuration values are stored with source tracking (Default, File, CommandLine, Runtime)
 */
class Config {
public:
    /**
     * @brief Constructor
     * @details Creates an empty configuration with default values
     */
    Config();
    
    /**
     * @brief Destructor
     * @details Cleans up configuration data
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * @details Loads configuration from a file, supporting JSON, YAML, and simple key=value formats.
     * JSON and YAML formats are tried first, with key=value format as a fallback for backward compatibility.
     * @param filepath Path to the configuration file
     * @return true if configuration was loaded successfully, false otherwise
     * @note Errors are logged to stderr for debugging
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * @details Parses command line arguments in --key=value or -key value format
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if configuration was loaded successfully, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * @details Saves the current configuration to a file in key=value format.
     * All configuration values including their source information are preserved.
     * @param filepath Path to the configuration file to save to
     * @return true if configuration was saved successfully, false otherwise
     * @note Errors are logged to stderr for debugging
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value by key
     * @details Retrieves a configuration value with type-safe access. The actual type
     * must match the stored type for successful retrieval.
     * @tparam T Type of the configuration value to retrieve
     * @param key Configuration key to look up
     * @return Optional configuration value. Returns nullopt if key not found or type mismatch
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    /**
     * @brief Get a configuration value with default fallback
     * @details Retrieves a configuration value, returning a default value if the
     * key is not found or the type doesn't match.
     * @tparam T Type of the configuration value to retrieve
     * @param key Configuration key to look up
     * @param defaultValue Default value to return if key not found
     * @return Configuration value or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * @details Sets a configuration value with specified source. Creates new entry
     * if key doesn't exist, updates existing entry otherwise.
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration (Default, File, CommandLine, Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (string specialization)
     * @param key Configuration key
     * @param value String configuration value
     * @param source Source of the configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (integer specialization)
     * @param key Configuration key
     * @param value Integer configuration value
     * @param source Source of the configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (int64_t specialization)
     * @param key Configuration key
     * @param value int64_t configuration value
     * @param source Source of the configuration
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (double specialization)
     * @param key Configuration key
     * @param value Double configuration value
     * @param source Source of the configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (boolean specialization)
     * @param key Configuration key
     * @param value Boolean configuration value
     * @param source Source of the configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * @param key Configuration key to check
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
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
     * @details Removes all configuration entries, leaving an empty configuration
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * @return String representation of all configuration entries with source information
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    /**
     * @brief Trim whitespace from a string
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
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

/**
 * @class Config
 * @brief Central configuration management for the NLM brain system.
 * 
 * The Config class provides a key-value store for system parameters,
 * supporting multiple value types (integers, doubles, strings, booleans,
 * and arrays). Configuration values can be sourced from default settings,
 * configuration files, command-line arguments, or runtime settings.
 * 
 * @note Thread-safe for concurrent read access. Write operations require
 *       synchronization if accessed from multiple threads.
 * 
 * @since Phase 1: Core Types and Configuration
 * @author NLM Research Team
 */

// Forward declarations
class Config;

/**
 * @brief Configuration value types using std::variant for type safety.
 * 
 * Supported types: int, int64_t, double, bool, std::string,
 * std::vector<int>, std::vector<double>, std::vector<std::string>
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
 */
enum class ConfigSource {
public:
    /** Default configuration values */
    Default,
    /** Loaded from configuration file */
    File,
    /** Parsed from command-line arguments */
    CommandLine,
    /** Set at runtime */
    Runtime
};

/**
 * @struct ConfigEntry
 * @brief Single configuration entry with metadata
 */
struct ConfigEntry {
    std::string key;          ///< Configuration key
    ConfigValue value;         ///< Configuration value
    ConfigSource source;       ///< Source of this configuration
    std::string description;   ///< Human-readable description
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of what this configuration controls
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Central configuration management for the NLM brain system.
 * 
 * The Config class provides a key-value store for system parameters,
 * supporting multiple value types (integers, doubles, strings, booleans,
 * and arrays). Configuration values can be sourced from default settings,
 * configuration files, command-line arguments, or runtime settings.
 * 
 * @note Thread-safe for concurrent read access. Write operations require
 *       synchronization if accessed from multiple threads.
 * 
 * @since Phase 1: Core Types and Configuration
 * @author NLM Research Team
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * @brief Disable copying to prevent shallow copy issues
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable moving for efficient resource transfer
     * 
     * @param rhs Source configuration to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * @param rhs Source configuration to move from
     * @return Reference to this configuration
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file (JSON format)
     * 
     * @param filepath Path to configuration file
     * @return true if file was successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Supports --key=value format and --key value format
     * 
     * @param argc Argument count (from main)
     * @param argv Argument values (from main)
     * @return true if arguments were successfully parsed
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file (JSON format)
     * 
     * @param filepath Path where configuration will be saved
     * @return true if file was successfully saved
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @tparam T Expected type of configuration value
     * @param key Configuration key to retrieve
     * @return std::optional containing the value if key exists, nullopt otherwise
     * 
     * @note The type T must match one of the supported ConfigValue types
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Expected type of configuration value
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key doesn't exist
     * @return Value if key exists, defaultValue otherwise
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value with specific type
     * 
     * @tparam T Type of value to set (must be supported by ConfigValue)
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this configuration (Runtime, File, etc.)
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set string configuration value
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set integer configuration value
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set double configuration value
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set boolean configuration value
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
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
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Get human-readable configuration summary
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: Trim whitespace from string
     * 
     * @param str String to trim
     * @return String without leading/trailing whitespace
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: Convert string to lowercase
     * 
     * @param str String to convert
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

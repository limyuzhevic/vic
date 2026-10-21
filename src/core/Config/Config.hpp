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
 * @brief Configuration value types for flexible NLM configuration
 * 
 * This variant type supports all primitive types and common collections
 * used in NLM configuration, allowing for flexible and type-safe value storage.
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
 * Tracks the origin of configuration values to support hierarchical
 * configuration with precedence: Default < File < CommandLine < Runtime
 */
enum class ConfigSource {
public:
    /**
     * @brief Default configuration values (hardcoded defaults)
     */
    Default = 0,
    
    /**
     * @brief File-based configuration (JSON or key=value format)
     */
    File = 1,
    
    /**
     * @brief Command-line configuration arguments
     */
    CommandLine = 2,
    
    /**
     * @brief Runtime configuration (programmatically set)
     */
    Runtime = 3
};

/**
 * @brief Configuration entry with metadata
 * 
 * Represents a single configuration parameter with its value,
 * source, and descriptive documentation.
 */
struct ConfigEntry {
    std::string key;           ///< Configuration key/parameter name
    ConfigValue value;          ///< Configuration value
    ConfigSource source;        ///< Origin of this configuration value
    std::string description;    ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Central configuration management system for NLM
 * 
 * The Config class provides a unified interface for managing all NLM
 * configuration parameters. It supports hierarchical configuration
 * with multiple sources, type-safe value access, and serialization.
 * 
 * Configuration values can be loaded from JSON files, command-line
 * arguments, or set programmatically at runtime. The system provides
 * type-safe access to all configuration parameters.
 * 
 * Usage example:
 * @code
 * Config config;
 * config.set("neuron_count", 1000);
 * config.set("plasticity_learning_rate", 0.01);
 * auto count = config.get<int>("neuron_count");
 * @endcode
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
     * @brief Disable copying (use move semantics instead)
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Supports both JSON format and simple key=value format.
     * JSON format is preferred when available.
     * 
     * @param filepath Path to configuration file
     * @return true if configuration was loaded successfully
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Supports two formats:
     * --key=value (for string values)
     * -key value (for all value types)
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument array
     * @return true if configuration was loaded successfully
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Saves all current configuration values in key=value format
     * with descriptions as comments.
     * 
     * @param filepath Path where configuration will be saved
     * @return true if configuration was saved successfully
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * @tparam T Type of configuration value (int, double, string, bool, etc.)
     * @param key Configuration key
     * @return Optional containing the value if found, std::nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * @tparam T Type of configuration value
     * @param key Configuration key
     * @param defaultValue Default value to return if key not found
     * @return Value from configuration or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * @tparam T Type of configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for string values
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for integer values
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for double values
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for boolean values
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists in configuration
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
     * @return Vector of all configuration key names
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Get a formatted summary of all configuration entries
     * 
     * @return String containing configuration summary with values and sources
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Parse JSON configuration file
     * 
     * Enhanced JSON parser supporting common NLM configuration formats.
     * Handles string, numeric, and boolean JSON values.
     * 
     * @param filepath Path to JSON configuration file
     * @return true if JSON parsing succeeded
     */
    bool parseJSONFile(const std::string& filepath);
    
    /**
     * @brief Trim whitespace from both ends of a string
     * 
     * @param str Input string
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

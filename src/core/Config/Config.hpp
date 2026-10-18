#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
 * @brief Configuration value types supporting multiple data types for the NLM framework
 * 
 * The ConfigValue type system allows storing various configuration parameters
 * including primitive types (int, double, bool), strings, and collections.
 * This flexibility enables NLM to handle diverse configuration needs from
 * simulation parameters to runtime settings.
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
 * @brief Configuration source tracking for determining parameter precedence
 * 
 * Different configuration sources are prioritized in this order:
 * 1. CommandLine (highest priority - overrides all others)
 * 2. Runtime (user code directly sets parameters)
 * 3. File (loaded from configuration files)
 * 4. Default (built-in defaults)
 * 
 * This hierarchy ensures that command-line overrides file settings,
 * and runtime settings override everything except command-line arguments.
 */
enum class ConfigSource {
public:
    Default,    /**< Built-in default configuration values */
    File,       /**< Values loaded from configuration files */
    CommandLine, /**< Values set via command-line arguments */
    Runtime     /**< Values set programmatically at runtime */
};

/**
 * @brief Single configuration entry with metadata
 * 
 * Represents a key-value pair in the configuration system, along with
 * source tracking and descriptive information for better debugging and
 * configuration management.
 */
struct ConfigEntry {
    std::string key;               /**< Configuration key/name */
    ConfigValue value;             /**< Configuration value */
    ConfigSource source;           /**< How this value was set */
    std::string description;       /**< Human-readable description */
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct configuration entry with all metadata
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of this configuration value
     * @param desc Description of this configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM simulation parameters
 * 
 * The Config class manages simulation-wide settings including neural network
 * parameters, simulation timing, learning rules, and runtime behavior.
 * It provides a flexible API for setting, getting, and saving configuration
 * values with proper type safety and source tracking.
 * 
 * Key features:
 * - Type-safe value retrieval with get<T>() and getOr<T>() methods
 * - Multiple configuration sources with priority hierarchy
 * - File-based configuration support (JSON format)
 * - Command-line argument parsing
 * - Runtime configuration modification
 * - Configuration validation and error handling
 * - Summary generation for debugging
 * 
 * Usage example:
 * @code
 * auto config = std::make_shared<Config>();
 * config->set("neuron_count", 1000);
 * config->set("simulation_timestep", 0.001);
 * config->set("random_seed", 42);
 * 
 * int neurons = config->getOr<int>("neuron_count", 500);
 * double timestep = config->getOr<double>("simulation_timestep", 0.001);
 * @endcode
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * @brief Disable copying (use move semantics instead)
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable move semantics for efficient copying
     */
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @param filepath Path to JSON configuration file
     * @return true if file loaded successfully, false otherwise
     * 
     * The file should contain key-value pairs in JSON format. Supported types
     * include strings, numbers, booleans, and arrays. File values have lower
     * priority than command-line and runtime settings.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if parsing succeeded, false otherwise
     * 
     * Supports patterns like --key=value or --key value
     * Command-line values have highest priority and override file and default values.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * @param filepath Path where configuration should be saved
     * @return true if save succeeded, false otherwise
     * 
     * Only runtime and command-line values are saved, not default values.
     * This allows users to save their custom configuration for later use.
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return std::optional<T> Value if key exists, std::nullopt otherwise
     * 
     * Type-safe retrieval prevents type errors and provides clear API.
     * Returns std::nullopt for missing keys to distinguish from default values.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return T Retrieved value or default if key missing
     * 
     * Provides a convenient way to get configuration values with sensible defaults.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value with explicit type
     * 
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this configuration value
     * @return Reference to this config for method chaining
     * 
     * Overloads are provided for common types (string, int, double, bool).
     * Values set with ConfigSource::Runtime override file and default values.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
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
     * 
     * After removal, the key will no longer be accessible and will fall back
     * to the next priority configuration source (file or default).
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
     * Removes all runtime and command-line values, leaving only defaults
     * and file-based configuration (if any).
     */
    void clear();
    
    /**
     * @brief Generate configuration summary for debugging
     * 
     * @return std::string Formatted configuration summary
     * 
     * Creates a human-readable summary showing all active configuration
     * values with their sources, useful for debugging and logging.
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

} // namespace nlm

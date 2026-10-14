#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

namespace nlm {

/**
 * @enum ConfigSource
 * @brief Enumeration of configuration data sources
 * 
 * Identifies where configuration values originate from, used for determining
 * precedence when multiple sources provide the same configuration key.
 * 
 * The source hierarchy (highest to lowest priority):
 * 1. Runtime - Set during program execution
 * 2. CommandLine - Parsed from command line arguments
 * 3. File - Loaded from configuration files
 * 4. Default - Built-in default values
 */
enum class ConfigSource {
    Default,    /**< Built-in default values */
    File,       /**< Loaded from configuration files */
    CommandLine,/** Parsed from command line arguments */
    Runtime     /**< Set during program execution */
};

/**
 * @struct ConfigEntry
 * @brief Single configuration key-value pair with metadata
 * 
 * Represents a configuration setting with its source, description, and value.
 * Used internally by the Config class for managing configuration data.
 */
struct ConfigEntry {
    std::string key;                    /**< Configuration key/path */
    std::variant<int, int64_t, double, bool, std::string,
                 std::vector<int>, std::vector<double>, std::vector<std::string>> value;
                                                                         /**< Configuration value */
    ConfigSource source;                /**< Source of this configuration value */
    std::string description;            /**< Human-readable description */
    
    /**
     * @brief Default constructor creates empty entry
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of configuration parameter
     */
    ConfigEntry(const std::string& k, 
                const std::variant<int, int64_t, double, bool, std::string,
                           std::vector<int>, std::vector<double>, std::vector<std::string>>& v,
                ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
} __attribute__((aligned(32)));

/**
 * @class Config
 * @brief Configuration management system for NLM brain parameters
 * 
 * The Config class provides a unified interface for managing all configuration
 * parameters needed by the NLM brain system. It supports multiple data sources,
 * type-safe value access, file persistence, and command-line parsing.
 * 
 * @details Key features:
 * - Hierarchical configuration using dot notation (e.g., "brain.neuron_count")
 * - Support for multiple value types (int, double, bool, strings, arrays)
 * - Four-level source priority system (Runtime > CommandLine > File > Default)
 * - JSON file I/O for configuration persistence
 * - Type-safe get/set operations with automatic conversion
 * - Comprehensive validation and error handling
 * - Configuration summary and debugging support
 */
class Config {
public:
    /**
     * @brief Construct empty configuration with default values
     */
    Config();
    
    /**
     * @brief Destructor releases configuration resources
     */
    ~Config();
    
    /**
     * @brief Disable copying (configuration has internal state)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor enables configuration transfer
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator enables configuration transfer
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @param filepath Path to JSON configuration file
     * @return bool true if file loaded successfully, false otherwise
     * @throws std::runtime_error if file cannot be read or parsed
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format --key=value or --key value.
     * Supported keys include all configuration parameters.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return bool true if parsing successful, false otherwise
     * @note Command line values take precedence over file and default values
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * @param filepath Path where configuration should be saved
     * @return bool true if save successful, false otherwise
     * @throws std::runtime_error if file cannot be written
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @tparam T Type of value to retrieve (must match variant type)
     * @param key Configuration key/path (dot notation)
     * @return std::optional<T> Value if key exists, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key/path
     * @param defaultValue Default value if key doesn't exist
     * @return T Value or default if key not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value with type safety
     * 
     * @tparam T Type of value to set (must be convertible to variant)
     * @param key Configuration key/path
     * @param value Value to set
     * @param source Source of configuration (default: Runtime)
     */
    template<typename T>
    void set(const std::string& key, const T& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (specialized for string type)
     * 
     * @param key Configuration key/path
     * @param value String value to set
     * @param source Source of configuration
     */
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (specialized for int type)
     * 
     * @param key Configuration key/path
     * @param value Integer value to set
     * @param source Source of configuration
     */
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (specialized for double type)
     * 
     * @param key Configuration key/path
     * @param value Double value to set
     * @param source Source of configuration
     */
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (specialized for bool type)
     * 
     * @param key Configuration key/path
     * @param value Boolean value to set
     * @param source Source of configuration
     */
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key/path to check
     * @return bool true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * @param key Configuration key/path to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Resets configuration to default state, removing all runtime and file settings.
     */
    void clear();
    
    /**
     * @brief Get configuration summary string
     * 
     * @return std::string Human-readable summary of all configuration values
     */
    std::string summary() const;
    
    /**
     * @brief Validate configuration parameters
     * 
     * Checks configuration for consistency and biological plausibility.
     * 
     * @return std::vector<std::string> List of validation errors (empty if valid)
     */
    std::vector<std::string> validate() const;
    
    /**
     * @brief Get configuration value for specific section
     * 
     * @param section Configuration section (e.g., "brain", "plasticity")
     * @return std::map<std::string, ConfigEntry> Configuration entries for section
     */
    std::map<std::string, ConfigEntry> getSection(const std::string& section) const;
    
    /**
     * @brief Apply configuration to target object
     * 
     * Uses configuration settings to initialize and configure target objects
     * such as Brain instances or other NLM components.
     * 
     * @tparam T Target object type
     * @param target Object to configure
     * @param prefix Configuration prefix for targeting specific parameters
     */
    template<typename T>
    void applyTo(T& target, const std::string& prefix = "") const;
    
    /**
     * @brief Merge configuration with another configuration
     * 
     * Combines this configuration with another, with other configuration
     * taking precedence for overlapping keys.
     * 
     * @param other Other configuration to merge
     * @param source Source for merged configuration
     */
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
private:
    /**
     * @brief Pimpl idiom implementation details
     * 
     * Forward declaration to Config implementation for reduced compile times
     * and encapsulation of complex configuration management logic.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     * 
     * @param str Input string
     * @return std::string String without leading/trailing whitespace
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str Input string
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Parse dot notation configuration key
     * 
     * @param key Configuration key (e.g., "brain.neuron_count")
     * @return std::pair<std::string, std::string> Section and parameter
     */
    static std::pair<std::string, std::string> parseKey(const std::string& key);
    
    /**
     * @brief Validate configuration key format
     * 
     * @param key Configuration key to validate
     * @return bool true if key format is valid, false otherwise
     */
    static bool validateKey(const std::string& key);
};

} // namespace nlm

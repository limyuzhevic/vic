#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

/**
 * @brief Configuration management for the NLM system
 * 
 * The Config class provides a flexible key-value configuration system for
 * the Neural Learning Machine framework. It supports multiple value types,
 * source tracking, file loading, and command-line argument parsing.
 * 
 * Configuration values are stored with their source (Default, File, CommandLine, Runtime)
 * to enable proper precedence and visibility tracking.
 */

// Forward declarations
class Config;

/**
 * @brief Configuration value types
 * 
 * The ConfigValue variant supports all primitive types and collections used
 * in the NLM configuration system. This enables flexible configuration
 * of neural parameters, simulation settings, and system behavior.
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
 * Tracks the origin of configuration values to determine precedence and
 * visibility. File and CommandLine sources override Default and Runtime values.
 */
enum class ConfigSource {
    /** @brief Default configuration values built into the system */
    Default,
    /** @brief Values loaded from configuration files */
    File,
    /** @brief Values from command-line arguments */
    CommandLine,
    /** @brief Values set at runtime through API */
    Runtime
};

/**
 * @brief Configuration entry structure
 * 
 * Stores a single configuration value with metadata about its source and
 * description. Each entry is associated with a unique key for lookup and
 * modification operations.
 */
struct ConfigEntry {
    std::string key;           /**< @brief Configuration key for lookup */
    ConfigValue value;         /**< @brief Configuration value */
    ConfigSource source;       /**< @brief Source of this configuration value */
    std::string description;   /**< @brief Human-readable description of the configuration */
    
    /**
     * @brief Default constructor
     * Creates an empty configuration entry.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the configuration
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class
 * 
 * Provides comprehensive configuration management for the NLM system,
 * supporting loading from files and command-line arguments, value retrieval
 * with type safety, and serialization to files. The class uses a private
 * implementation (pImpl pattern) for efficient memory management.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * Creates an empty configuration object.
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    /**
     * @brief Disable copying
     * Config objects should not be copied for safety.
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable move construction
     * Allows efficient transfer of configuration ownership.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Enable move assignment
     * Allows efficient transfer of configuration ownership.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file (JSON/YAML format)
     * 
     * Currently implements a simple key=value parser for Phase 2.
     * @param filepath Path to the configuration file
     * @return true if file loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses arguments in --key=value or -key value format.
     * Command-line arguments override file settings.
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if parsing succeeded
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Exports configuration entries in key=value format with comments.
     * @param filepath Path to save the configuration file
     * @return true if save succeeded, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves a configuration value by key, returning optional value.
     * If the key doesn't exist or type doesn't match, returns std::nullopt.
     * @tparam T Target type for conversion
     * @param key Configuration key to retrieve
     * @return Optional containing the value if found and type matches
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value, returning default if key doesn't exist
     * or type doesn't match.
     * @tparam T Target type for conversion
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key not found or type mismatch
     * @return Value if key found and type matches, otherwise default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * Sets a configuration value with specified source and description.
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration value
     * @param desc Description of the configuration (optional)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string overload)
     * @see void set(const std::string& key, const ConfigValue& value, ConfigSource source)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer overload)
     * @see void set(const std::string& key, const ConfigValue& value, ConfigSource source)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int64_t overload)
     * @see void set(const std::string& key, const ConfigValue& value, ConfigSource source)
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double overload)
     * @see void set(const std::string& key, const ConfigValue& value, ConfigSource source)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean overload)
     * @see void set(const std::string& key, const ConfigValue& value, ConfigSource source)
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
     * Removes all entries regardless of source.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * Returns a formatted string describing all configuration entries.
     * @return String containing configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @brief Private implementation (pImpl pattern)
     * 
     * Forward declaration to the implementation class for efficient memory management
     * and better encapsulation.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     * Helper function for parsing configuration files.
     * @param str Input string
     * @return String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * Helper function for case-insensitive key comparison.
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

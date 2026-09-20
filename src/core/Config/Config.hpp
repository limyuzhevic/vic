#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management for NLM (Neural Learning Machine)
 * @author NLM Project
 * @date Phase 1 - Complete
 * 
 * This header defines the Config class for managing configuration parameters
 * throughout the NLM system. Supports loading from JSON files, command line,
 * and runtime configuration with validation.
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>

namespace nlm {

/**
 * @enum ConfigSource
 * @brief Source of configuration value
 * 
 * Defines where a configuration parameter originates from, used for tracking
 * and overriding configuration sources.
 */
enum class ConfigSource {
    Default,    ///< Built-in default value
    File,       ///< Loaded from configuration file
    CommandLine,///< Set via command line arguments
    Runtime     ///< Set at runtime via API
};

/**
 * @typedef ConfigValue
 * @brief Union type for all possible configuration values
 * 
 * Stores any of the supported configuration types using std::variant.
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
 * @struct ConfigEntry
 * @brief Single configuration entry with metadata
 * 
 * Represents a single configuration parameter with its value and origin.
 */
struct ConfigEntry {
    std::string key;              ///< Configuration key (e.g., "brain.neuron_count")
    ConfigValue value;            ///< Configuration value
    ConfigSource source;          ///< Source of this configuration
    std::string description;      ///< Human-readable description
    
    /**
     * @brief Default constructor
     * Creates a default ConfigEntry with empty values
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of the configuration
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Main configuration management class for NLM
 * 
 * Manages configuration parameters for the entire NLM system. Provides
 * functionality to load, access, and modify configuration from various sources,
 * with proper validation and type safety.
 * 
 * Features:
 * - Load from JSON files
 * - Load from command line arguments
 * - Type-safe access with templates
 * - Source tracking for overrides
 * - Configuration validation
 * 
 * Usage example:
 * @code
 * nlm::Config config;
 * config.loadFromFile("config.json");
 * auto neurons = config.getOr<int>("brain.neuron_count", 1000);
 * @endcode
 */
class Config {
public:
    /**
     * @brief Default constructor
     * Creates a configuration with empty initial state
     */
    Config();
    
    /**
     * @brief Destructor
     * Clean up implementation resources
     */
    ~Config();
    
    /**
     * @brief Disable copying
     * Config objects are not copyable to prevent shared state issues
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable move constructor
     * @param other Configuration to move from
     * Move constructor for efficient transfer of ownership
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Enable move assignment
     * @param other Configuration to move from
     * @return Reference to this configuration
     * Move assignment for efficient transfer of ownership
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * @param filepath Path to JSON configuration file
     * @return true if successfully loaded, false otherwise
     * @throw std::runtime_error if file cannot be read or parsed
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * @param argc Argument count from main()
     * @param argv Argument values from main()
     * @return true if successfully loaded, false otherwise
     * Parses standard --key=value format arguments
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to JSON file
     * @param filepath Path where to save configuration
     * @return true if successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * @tparam T Target type (int, double, bool, string, or vector)
     * @param key Configuration key
     * @return Optional containing value if exists, empty if not found
     * @throw std::runtime_error if type mismatch occurs
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * @tparam T Target type
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return Value from config or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration change
     * @details Automatically handles type conversion and validation
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value - string specialization
     * @param key Configuration key
     * @param value String value
     * @param source Source of configuration change
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value - integer specialization
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of configuration change
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value - double specialization
     * @param key Configuration key
     * @param value Double value
     * @param source Source of configuration change
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value - boolean specialization
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of configuration change
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
     * @param key Key to remove from configuration
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * Removes all configuration parameters, reverting to defaults
     */
    void clear();
    
    /**
     * @brief Get configuration summary string
     * @return String representation of current configuration
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation structure (pImpl idiom)
     * 
     * All implementation details are hidden in this private struct to
     * reduce compilation dependencies and improve encapsulation.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: trim whitespace from string
     * @param str Input string
     * @return String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: convert string to lowercase
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm
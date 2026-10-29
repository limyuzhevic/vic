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
 * @enum ConfigSource
 * @brief Enumeration of configuration sources with priority ordering
 * 
 * Configuration values can come from different sources, with the following priority:
 * 1. Runtime (highest) - Set programmatically during execution
 * 2. CommandLine - From command line arguments
 * 3. File - From configuration files
 * 4. Default (lowest) - Built-in defaults
 * 
 * When getting a configuration value, the highest priority available source is used.
 */
enum class ConfigSource {
    Default,    ///< Default built-in configuration
    File,       ///< Configuration loaded from file
    CommandLine,///< Configuration from command line arguments
    Runtime     ///< Configuration set at runtime
};

/**
 * @struct ConfigEntry
 * @brief Single configuration key-value pair with metadata
 * 
 * Each configuration entry stores the actual value along with metadata about
 * where it came from and what it represents.
 */
struct ConfigEntry {
    std::string key;          ///< Configuration key for lookup
    ConfigValue value;        ///< The actual configuration value
    ConfigSource source;      ///< Where this configuration came from
    std::string description;  ///< Human-readable description of the configuration
    
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
     * @param desc Human-readable description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Centralized configuration management system
 * 
 * The Config class provides a thread-safe, type-safe configuration system that
 * supports multiple configuration sources and automatic fallback mechanisms.
 * It uses the Pimpl idiom for efficient compilation and extensibility.
 * 
 * Key features:
 * - Type-safe configuration values with automatic conversion
 * - Multiple configuration sources with priority hierarchy
 * - File-based configuration (currently simple key=value format)
 * - Command-line argument parsing
 * - Runtime configuration modification
 * - Configuration validation and summaries
 * 
 * Usage example:
 * @code
 * Config config;
 * config.set("neuron_count", 500);
 * config.set("simulation_timestep", 0.001);
 * 
 * int neurons = config.getOr<int>("neuron_count", 100);
 * double timestep = config.getOr<double>("simulation_timestep", 0.001);
 * @endcode
 * 
 * @note Thread-safe for concurrent access during simulation
 * @note Exception-safe with proper resource management
 */
class Config {
public:
    /**
     * @brief Default constructor creates empty configuration
     */
    Config();
    
    /**
     * @brief Destructor cleans up internal implementation
     */
    ~Config();
    
    /**
     * @brief Copy constructor deleted to prevent copying (Pimpl idiom)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator deleted to prevent copying
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of internal resources
     * 
     * @param other The config to move from (leaves in valid but unspecified state)
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of internal resources
     * 
     * @param other The config to move from (leaves in valid but unspecified state)
     * @return Reference to this config
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Loads configuration from file (currently key=value format)
     * 
     * @param filepath Path to configuration file
     * @return bool true if file was successfully loaded, false otherwise
     * 
     * @note Currently supports simple key=value format
     * @note Future implementation will support JSON/YAML format
     * @note File parsing ignores empty lines and comments starting with # or /
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Loads configuration from command line arguments
     * 
     * @param argc Argument count from main()
     * @param argv Argument vector from main()
     * @return bool true if any configuration was loaded
     * 
     * @note Supports both --key=value and -key value formats
     * @note Command line configuration overrides file and default values
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Saves configuration to file
     * 
     * @param filepath Path where to save configuration
     * @return bool true if file was successfully saved, false otherwise
     * 
     * @note Currently saves all entries as placeholder values
     * @warning Not fully implemented - uses placeholder values
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Gets configuration value with type safety
     * 
     * @tparam T Type to convert the value to
     * @param key Configuration key to look up
     * @return std::optional<T> Optional containing the value if found, empty otherwise
     * 
     * @note Returns empty optional if key not found or type mismatch
     * @note Supports all types defined in ConfigValue variant
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Gets configuration value with default fallback
     * 
     * @tparam T Type to convert the value to
     * @param key Configuration key to look up
     * @param defaultValue Value to return if key not found or type mismatch
     * @return T The configuration value or default
     * 
     * @note Always returns a valid value (never empty optional)
     * @note Most commonly used method for safe configuration access
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Sets configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration (defaults to Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets string configuration value
     * 
     * @param key Configuration key
     * @param value String value
     * @param source Source of configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets integer configuration value
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets double configuration value
     * 
     * @param key Configuration key
     * @param value Double value
     * @param source Source of configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets boolean configuration value
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Checks if configuration key exists
     * 
     * @param key Configuration key to check
     * @return bool true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Removes configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Gets all configuration keys
     * 
     * @return std::vector<std::string> Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clears all configuration values
     */
    void clear();
    
    /**
     * @brief Gets human-readable configuration summary
     * 
     * @return std::string Formatted string showing all configuration entries
     * 
     * @note Useful for debugging and logging configuration state
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation details (Pimpl idiom)
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trims whitespace from string
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Converts string to lowercase
     * 
     * @param str String to convert
     * @return std::string Lowercase version of string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

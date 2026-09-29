#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

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
 * @brief Configuration value types supported by the NLM system
 * 
 * The configuration system supports a comprehensive set of value types
 * to handle the diverse configuration needs of the neural brain simulation.
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
 * Tracks the origin of configuration values to ensure proper precedence
 * and to enable rollback capabilities.
 */
enum class ConfigSource {
public:
    /** Default configuration values */
    Default,
    
    /** Values loaded from configuration files */
    File,
    
    /** Values from command line arguments */
    CommandLine,
    
    /** Runtime-set values */
    Runtime
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration key-value pair with metadata about
 * its source and description.
 */
struct ConfigEntry {
    /** Configuration key */
    std::string key;
    
    /** Configuration value (type-erased) */
    ConfigValue value;
    
    /** Source of this configuration value */
    ConfigSource source;
    
    /** Human-readable description of the configuration parameter */
    std::string description;
    
    /** Default constructor */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /** Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for the NLM system
 * 
 * The Config class provides a comprehensive configuration management system
 * for the Neural Learning Machine (NLM). It supports loading and saving
 * configuration from multiple sources (files, command line, runtime),
 * type-safe value access, and provides a unified interface for all
 * NLM simulation parameters.
 * 
 * The implementation uses the Pimpl idiom for efficient compilation and
 * maintains separation of concerns between the interface and implementation.
 * 
 * Configuration is essential for:
 * - Neural network parameters (neuron count, connectivity, etc.)
 * - Simulation settings (timestep, duration, etc.)
 * - Plasticity rules (STDP, Hebbian, structural plasticity)
 * - Development stages and parameters
 * - Neuromodulation settings and thresholds
 * - Memory system configurations
 */
class Config {
public:
    /** Default constructor */
    Config();
    
    /** Virtual destructor */
    ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Configuration objects are not copyable to prevent accidental
     * sharing of configuration state.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Configuration objects are not assignable to prevent accidental
     * sharing of configuration state.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of the internal configuration
     * implementation from one object to another.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of the internal configuration
     * implementation.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Supports multiple file formats with fallback to simple key=value format.
     * Files are searched in order of precedence, and values from later
     * files override earlier ones.
     * 
     * @param filepath Path to the configuration file
     * @return true if configuration was loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports multiple command line argument formats:
     * --key=value, -key value, and --key value
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if configuration was parsed successfully, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Writes the current configuration state to a file in a human-readable
     * format that can be parsed back by loadFromFile().
     * 
     * @param filepath Path where the configuration should be saved
     * @return true if configuration was saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * Retrieves a configuration value of the specified type. If the key
     * doesn't exist or the type doesn't match, returns std::nullopt.
     * 
     * @tparam T Type of the configuration value
     * @param key Configuration key to retrieve
     * @return Optional containing the value if found and type matches, std::nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value of the specified type, returning a
     * default value if the key doesn't exist or the type doesn't match.
     * 
     * @tparam T Type of the configuration value
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if key not found or type mismatch
     * @return The configuration value or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Sets a configuration value with the specified key and type.
     * The source parameter tracks where the value originated from.
     * 
     * @tparam T Type of the configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a string configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set an integer configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a double configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a boolean configuration value
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists, false otherwise
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
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * 
     * Removes all configuration entries, returning the Config object to
     * its default empty state.
     */
    void clear();
    
    /**
     * @brief Get a configuration summary
     * 
     * Returns a human-readable string describing all current configuration
     * entries, including their values and sources.
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation structure (Pimpl idiom)
     * 
     * The Pimpl idiom separates the interface from the implementation,
     * reducing compilation dependencies and allowing for efficient
     * changes to the implementation without affecting users.
     */
    struct Impl;
    
    /** Pointer to internal implementation */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper function to trim whitespace
     * 
     * Removes leading and trailing whitespace from a string.
     * 
     * @param str Input string
     * @return String with whitespace trimmed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper function to convert string to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of the string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

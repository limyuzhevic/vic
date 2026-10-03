#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management for NLM system
 * @author NLM Project
 * @date Phase 6 Implementation
 * @version 0.1.0
 * 
 * The Config class provides a flexible key-value configuration system for NLM.
 * Supports multiple data types including int, int64_t, double, bool, string,
 * and vector types. Configuration values can be loaded from files, command line
 * arguments, or set at runtime. Each configuration entry tracks its source
 * (Default, File, CommandLine, Runtime) for proper precedence handling.
 * 
 * Configuration is used to set parameters for brain simulation, memory systems,
 * neuromodulation, plasticity rules, and development stages.
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Enumeration of configuration sources
 * 
 * Configuration values can come from different sources with different priorities:
 * - Default: Built-in defaults
 * - File: Configuration file
 * - CommandLine: Command line arguments
 * - Runtime: Set programmatically during execution
 */
enum class ConfigSource {
    Default,     /**< Built-in default values */
    File,        /**< Loaded from configuration file */
    CommandLine, /**< Passed via command line arguments */
    Runtime      /**< Set programmatically at runtime */
};

/**
 * @brief Configuration entry with key-value pair
 * 
 * Each configuration entry stores a key, value, source, and optional description.
 * The value is stored as a variant type to support multiple data types.
 */
struct ConfigEntry {
    std::string key;           /**< Configuration key/name */
    ConfigValue value;          /**< Configuration value (variant type) */
    ConfigSource source;       /**< Source of this configuration */
    std::string description;   /**< Human-readable description */
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Union type for configuration values
 * 
 * ConfigValue can store different types of configuration data:
 * - Integer types (int, int64_t)
 * - Floating-point types (double)
 * - Boolean values (bool)
 * - String values (std::string)
 * - Vector types (std::vector<int>, std::vector<double>, std::vector<std::string>)
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
 * @brief Main configuration class for NLM system
 * 
 * The Config class manages key-value pairs for configuring neural simulation parameters.
 * It supports loading from files (JSON format), command line arguments, and programmatic
 * access. The class provides type-safe getters and setters for all supported types.
 * 
 * Configuration is used to set parameters for:
 * - Brain simulation (neuron count, regions, connections)
 * - Plasticity rules (STDP, Hebbian, structural)
 * - Neuromodulation (dopamine, curiosity, novelty)
 * - Development system parameters
 * - Memory system settings
 * - Simulation parameters (timestep, etc.)
 * 
 * Example usage:
 * @code
 * auto config = pynlm.createDefaultConfig()
 * config.set("brain.neuron_count", 1000)
 * config.set("plasticity.stdp.enable", true)
 * config.set("neuromod.dopamine.scale", 1.0)
 * 
 * auto brain = pynlm.createBrain(config)
 * brain.initialize()
 * @endcode
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * @brief Disable copying, enable moving
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a JSON file
     * 
     * @param filepath Path to the configuration file
     * @return True if file was successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports both --key=value and -k value formats.
     * 
     * @param argc Command line argument count
     * @param argv Command line argument values
     * @return True if arguments were successfully parsed
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a JSON file
     * 
     * @param filepath Path where configuration will be saved
     * @return True if file was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type checking
     * 
     * @tparam T Type of value to retrieve (must match stored type)
     * @param key Configuration key to look up
     * @return Optional containing the value if found, nullopt if not found
     *         or if type mismatch occurs
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * @tparam T Type of value to retrieve (must match stored type)
     * @param key Configuration key to look up
     * @param defaultValue Value to return if key is not found
     * @return Value from configuration or default if key not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * @param key Configuration key to set
     * @param value Value to set
     * @param source Source of this configuration (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (string version)
     * 
     * @param key Configuration key to set
     * @param value String value to set
     * @param source Source of this configuration (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (integer version)
     * 
     * @param key Configuration key to set
     * @param value Integer value to set
     * @param source Source of this configuration (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (floating-point version)
     * 
     * @param key Configuration key to set
     * @param value Floating-point value to set
     * @param source Source of this configuration (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (boolean version)
     * 
     * @param key Configuration key to set
     * @param value Boolean value to set
     * @param source Source of this configuration (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return True if key exists, false otherwise
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
     */
    void clear();
    
    /**
     * @brief Get a summary of the configuration
     * 
     * @return String containing configuration summary for debugging
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from both ends of a string
     * 
     * @param str Input string
     * @return String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Explicit template instantiations for get() method
     */
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    /**
     * @brief Explicit template instantiations for getOr() method
     */
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
    
} // namespace nlm

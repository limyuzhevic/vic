#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types for the NLM system
 * 
 * This variant type supports all configuration values used throughout the
 * NLM project. The types are chosen to balance flexibility with type safety.
 * 
 * @note Configuration values are stored as strings in files but typed as
 *       specific variants in memory for efficient access.
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
 * Tracks the origin of configuration values to resolve conflicts and
 * support the configuration precedence hierarchy:
 * CommandLine > File > Default > Runtime
 */
enum class ConfigSource {
public:
    Default,      ///< Default values from code
    File,         ///< Values from configuration file
    CommandLine,  ///< Values from command line arguments
    Runtime       ///< Values set at runtime
};

/**
 * @brief Single configuration entry
 * 
 * Represents a single configuration parameter with its value, source,
 * and description. Used internally by the Config class to maintain
 * the complete configuration state.
 */
struct ConfigEntry {
    std::string key;         ///< Configuration key
    ConfigValue value;       ///< Configuration value
    ConfigSource source;     ///< Source of this value
    std::string description; ///< Human-readable description
    
    /// Default constructor
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /// Parameterized constructor
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for the NLM system
 * 
 * This class provides a type-safe configuration system that supports:
 * 
 * ## Configuration Sources
 * 1. **Command-line arguments**: Highest priority (e.g., --neuron_count=1000)
 * 2. **Configuration files**: Medium priority (JSON/YAML format)
 * 3. **Default values**: Code-defined defaults (lowest priority)
 * 4. **Runtime settings**: Set programmatically during execution
 * 
 * ## Configuration Access
 * 
 * Configuration values are accessed through typed getter methods:
 * - `get<T>(key)` - Returns std::optional<T> or std::nullopt if not found
 * - `getOr<T>(key, default)` - Returns value or default if not found
 * 
 * Configuration values are set through typed setter methods:
 * - `set(key, value, source)` - All types supported
 * - `set(key, string, source)` - String values
 * - `set(key, int, source)` - Integer values
 * - `set(key, double, source)` - Floating-point values
 * - `set(key, bool, source)` - Boolean values
 * 
 * ## Configuration File Format
 * 
 * The configuration file uses a simple key=value format:
 * 
 * @code
 * # Example config file
 * # Neural network configuration
 * neuron_count = 1000
 * region_count = 1
 * simulation_timestep = 0.001
 * connection_probability = 0.1
 * stdp_ltp_weight = 0.02
 * stdp_ltd_weight = 0.015
 * stdp_tau = 20.0
 * synaptogenesis_rate = 0.0001
 * pruning_rate = 0.00001
 * @endcode
 * 
 * ## Configuration Usage Examples
 * 
 * @code
 * // Get an integer value
 * auto neuronCount = config->get<int>("neuron_count");
 * if (neuronCount) {
 *     std::cout << "Neuron count: " << *neuronCount << std::endl;
 * }
 * 
 * // Get a value with default
 * int regionCount = config->getOr<int>("region_count", 1);
 * 
 * // Set a value at runtime
 * config->set("random_seed", 123, ConfigSource::Runtime);
 * 
 * // Check if key exists
 * if (config->has("custom_setting")) {
 *     auto value = config->get<std::string>("custom_setting");
 * }
 * @endcode
 * 
 * @note The configuration system is used by all NLM components including
 *       brain initialization, simulation parameters, and experimental settings.
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * @brief Disable copying (PIMPL pattern)
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable move operations (PIMPL pattern)
     */
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * @param filepath Path to configuration file (JSON/YAML planned for Phase 8)
     * @return bool True if file loaded successfully, false if file not found or error
     * @note Currently implements simple key=value parsing (Phase 2).
     *       Future phases will support full JSON/YAML parsing.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports multiple formats:
     * - `--key=value` (long options)
     * - `-k value` (short options)
     * 
     * @param argc Argument count
     * @param argv Argument vector
     * @return bool True if parsing succeeded
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * @param filepath Path to output configuration file
     * @return bool True if save succeeded, false on error
     * @note Currently writes placeholder values. Future phases will implement
     *       full serialization of current configuration.
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value by key with type safety
     * 
     * @tparam T Expected type (int, int64_t, double, bool, or std::string)
     * @param key Configuration key
     * @return std::optional<T> Value if found and type matches, std::nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Expected type
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return T Value if found, otherwise defaultValue
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @tparam T Type of value (all supported ConfigValue types)
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of this value (default: Runtime)
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set string configuration value
     * 
     * Convenience overload for string values.
     * 
     * @param key Configuration key
     * @param value String value
     * @param source Source of this value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set integer configuration value
     * 
     * Convenience overload for integer values.
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of this value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set double configuration value
     * 
     * Convenience overload for floating-point values.
     * 
     * @param key Configuration key
     * @param value Double value
     * @param source Source of this value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set boolean configuration value
     * 
     * Convenience overload for boolean values.
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of this value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return bool True if key exists in configuration
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
     * @return std::vector<std::string> Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Get configuration summary string
     * 
     * @return std::string Formatted summary of all configuration entries
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from string
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str String to convert
     * @return std::string Lowercase version
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

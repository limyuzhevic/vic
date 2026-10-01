#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management for the NLM system
 * 
 * This file defines the configuration system used throughout NLM to manage
 * simulation parameters, neuron properties, plasticity rules, and system settings.
 * The configuration system supports:
 * - Loading from JSON files
 * - Command-line argument parsing
 * - Runtime configuration modification
 * - Type-safe value access with automatic conversion
 * - Configuration validation and logging
 * 
 * The configuration system is hierarchical and supports nested key-value pairs
 * through dot notation (e.g., "brain.neuron_count", "plasticity.stdp.learning_rate").
 * 
 * @author NLM Project
 * @date Phase 1: Core Configuration System
 * @version 0.1.0
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
 * Different sources have different precedence for resolving key values:
 * - CommandLine has highest priority (overrides all)
 * - File overrides Default
 * - Runtime overrides File and Default
 * 
 * This allows for flexible configuration management where defaults can be
 * overridden by file configuration, which can be overridden by command-line
 * arguments, and runtime modifications can still take precedence.
 * 
 * @enum ConfigSource
 * @ingroup config_system
 */
enum class ConfigSource {
    Default,    ///< Default configuration values (built into code)
    File,       ///< Configuration loaded from file (JSON format)
    CommandLine,///< Configuration from command-line arguments
    Runtime     ///< Configuration set at runtime via API
};

/**
 * @brief Configuration value type system
 * 
 * The NLM configuration system uses a variant type system to support multiple
 * data types while maintaining type safety. This allows configuration values
 * to be stored flexibly but accessed with explicit type conversion.
 * 
 * @note When accessing values, always use the get<T>() method with proper type
 * checking or the getOr<T>() method with default values for safety.
 * 
 * @typedef ConfigValue
 * @ingroup config_system
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
 * @brief Single configuration entry
 * 
 * Represents a key-value pair with metadata about its source and origin.
 * This allows the configuration system to track where values come from
 * and resolve conflicts when multiple sources provide different values.
 * 
 * @struct ConfigEntry
 * @ingroup config_system
 */
struct ConfigEntry {
    std::string key;              ///< Configuration key (dot notation for nested values)
    ConfigValue value;            ///< Configuration value
    ConfigSource source;          ///< Source of this configuration value
    std::string description;      ///< Human-readable description of the configuration
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a configuration entry
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of what this configuration controls
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for the NLM system
 * 
 * This is the central configuration management class that provides access to
 * all NLM configuration parameters. It supports:
 * 
 * - **Value Storage**: Hierarchical key-value storage with dot notation
 * - **Type Safety**: Automatic type conversion with error checking
 * - **Multiple Sources**: Default, file, command-line, and runtime values
 * - **File I/O**: JSON format support for configuration files
 * - **Command-line**: Automatic parsing of command-line arguments
 * - **Validation**: Basic validation of configuration values
 * - **Logging**: Configuration changes and access logging
 * 
 * The configuration system uses a pImpl idiom for efficient compilation
 * and the ability to change the implementation without recompiling headers.
 * 
 * Key usage patterns:
 * ```cpp
 * // Create and initialize configuration
 * auto config = std::make_shared<Config>();
 * 
 * // Get values with type safety
 * int neuronCount = config->getOr<int>("brain.neuron_count", 1000);
 * 
 * // Check if key exists
 * if (config->has("plasticity.stdp.enabled")) {
 *     bool stdpEnabled = config->get<bool>("plasticity.stdp.enabled").value();
 * }
 * 
 * // Set values
 * config->set("brain.sim_timestep", 0.001);
 * 
 * // Load from file
 * config->loadFromFile("config.json");
 * 
 * // Load from command line
 * config->loadFromArgs(argc, argv);
 * ```
 * 
 * @class Config
 * @ingroup config_system
 * @ingroup core
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration with all default values.
     * Default values are defined in the implementation file (Config.cpp).
     */
    Config();
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Config();
    
    /**
     * @brief Disable copy construction
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable move construction
     * 
     * This allows configuration objects to be efficiently moved
     * between variables without copying their internal storage.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Enable move assignment
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a JSON file
     * 
     * This method loads configuration values from a JSON file and merges
     * them with existing configuration. File source values have lower priority
     * than default and command-line values.
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if file was successfully loaded and parsed
     * @return false if file could not be opened or contains invalid JSON
     * 
     * @note File format follows JSON standard. Common patterns:
     * @code
     * {
     *   "brain": {
     *     "neuron_count": 1000,
     *     "region_count": 1,
     *     "sim_timestep": 0.001
     *   },
     *   "plasticity": {
     *     "stdp": {
     *       "enabled": true,
     *       "learning_rate": 0.01
     *     }
     *   }
     * }
     * @endcode
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * This method parses command-line arguments in the format:
     * --key=value or --key value
     * 
     * Common command-line arguments:
     * --brain.neuron_count=1000
     * --plasticity.stdp.learning_rate=0.01
     * --output=file.bin
     * --verbose
     * 
     * @param argc Number of command-line arguments
     * @param argv Array of command-line argument strings
     * @return true if arguments were successfully parsed
     * 
     * @note Command-line arguments have the highest priority and will override
     * file and default values.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a JSON file
     * 
     * This method serializes the current configuration to a JSON file.
     * Only values from the File and Runtime sources are saved.
     * Default values are not included in the output.
     * 
     * @param filepath Path where to save the JSON configuration
     * @return true if file was successfully written
     * @return false if file could not be opened or written
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * This method retrieves a configuration value by key and attempts to
     * convert it to the requested type. If the conversion fails, it returns
     * an empty optional.
     * 
     * @tparam T Type to convert the value to
     * @param key Configuration key (dot notation for nested values)
     * @return std::optional<T> The value if successfully converted, nullopt otherwise
     * 
     * @note Supported types: int, int64_t, double, bool, std::string,
     * std::vector<int>, std::vector<double>, std::vector<std::string>
     * 
     * @warning Always check if the returned value is valid using .has_value()
     * before using it, especially when dealing with optional configuration.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * This method retrieves a configuration value and returns the default
     * if the key doesn't exist or the value can't be converted to the
     * requested type.
     * 
     * @tparam T Type to convert the value to
     * @param key Configuration key
     * @param defaultValue Value to return if key is not found or invalid
     * @return T The configuration value or defaultValue
     * 
     * @note This is the preferred method for accessing configuration values
     * as it provides safe access without needing to check for optional values.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * This method sets a configuration value with the specified source.
     * If the key already exists from a higher-priority source, the new value
     * will be added as a lower-priority entry.
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a string
     * 
     * @param key Configuration key
     * @param value String value
     * @param source Source of the configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as an integer
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of the configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a double
     * 
     * @param key Configuration key
     * @param value Double value
     * @param source Source of the configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a boolean
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of the configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * A key exists if it has been set from any source (Default, File, CommandLine, or Runtime).
     * If the key exists but the value can't be converted to the requested type, has() will still return true.
     * 
     * @param key Configuration key
     * @return true if the key has a value from any source
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * This removes the key from all configuration sources and forces
     * the system to use default values.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> List of all configuration keys (unique)
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * This removes all user-set configuration (File, CommandLine, Runtime)
     * and reverts to default values only.
     */
    void clear();
    
    /**
     * @brief Get a configuration summary
     * 
     * Returns a human-readable string describing all current configuration
     * values, organized by source and key.
     * 
     * @return std::string Configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation structure (pImpl idiom)
     * 
     * This uses the pImpl idiom to hide implementation details and allow
     * for efficient compilation. The actual implementation is in Config.cpp.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from a string
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str String to convert
     * @return std::string Lowercase version of input
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

#endif // NLM_CONFIG_HPP

#pragma once

/**
 * @file Config.hpp
 * @brief Core configuration management for the NLM simulation framework.
 * 
 * This header provides a thread-safe, key-value configuration system for managing
 * simulation parameters and runtime settings. Configuration values are stored as
 * variants to support multiple data types, and can be sourced from multiple origins
 * (default, file, command line, runtime) with proper precedence rules.
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * @see src/core/Types/Types.hpp
 * @see src/agent/AgentBrain.hpp
 * @see src/experiments/Experiment.hpp
 * @note Configuration follows the pattern: command line > runtime > file > default
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

/**
 * Forward declarations to break circular dependencies.
 * 
 * These classes are only declared here to allow Config to be included without
 * pulling in their full definitions.
 */
class Config;

/**
 * Generic configuration value type supporting multiple data types.
 * 
 * The configuration system supports a variety of value types to accommodate
 * different parameter needs. Each type can be used with type-safe getters and setters.
 * 
 * @see Config::get()
 * @see Config::set()
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
 * Enumeration of configuration value sources.
 * 
 * Defines the origin of a configuration value, which determines precedence
 * when multiple sources define the same key.
 */
enum class ConfigSource {
    /** Default configuration (built into code) - lowest precedence */
    Default,
    
    /** File-based configuration - loaded from disk */
    File,
    
    /** Command-line configuration - parsed from arguments */
    CommandLine,
    
    /** Runtime configuration - set programmatically */
    Runtime
};

/**
 * Represents a single configuration entry with metadata.
 * 
 * Each configuration value includes its key, the actual value, the source,
 * and a description for documentation purposes. This structure enables
 * configuration introspection and help systems.
 */
struct ConfigEntry {
    std::string key;                    /**< Configuration key for lookup */
    ConfigValue value;                  /**< The configuration value */
    ConfigSource source;                /**< Origin of this configuration value */
    std::string description;            /**< Human-readable description */
    
    /**
     * Default constructor creates an empty configuration entry.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct configuration entry with specified parameters
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Human-readable description (default: empty)
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * Main configuration class providing thread-safe key-value storage.
 * 
 * The Config class implements a hierarchical configuration system with support
 * for multiple value types, different sources, and persistence. It uses the
 * PImpl idiom for efficient implementation changes without recompilation.
 * 
 * Configuration precedence: Command Line > Runtime > File > Default
 * 
 * @see ConfigSource
 * @see ConfigEntry
 * @see src/core/Types/Types.hpp
 * @warning Thread safety: Individual methods are thread-safe, but concurrent
 * modifications to the same keys from multiple threads should be coordinated.
 */
class Config {
public:
    /**
     * Default constructor creates an empty configuration with default values.
     */
    Config();
    
    /**
     * Destructor ensures proper cleanup of PImpl implementation.
     */
    ~Config();
    
    /**
     * @name Object Lifetime Management
     * @warning Copying configuration is disabled (Config&) to prevent
     * unintended sharing of configuration state.
     */
    /// @{
    
    /**
     * @brief Copy constructor is deleted to prevent copying.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted to prevent copying.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor enables efficient transfer of configuration state.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator enables efficient transfer of configuration state.
     */
    Config& operator=(Config&&) noexcept;
    /// @}
    
    /**
     * @name File I/O Operations
     * 
     * These methods provide JSON-based configuration file support for
     * sharing configuration across different runs and environments.
     */
    /// @{
    
    /**
     * @brief Load configuration from a JSON file
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if file was successfully loaded and parsed, false otherwise
     * @see saveToFile()
     * @warning File loading may overwrite existing runtime configuration
     * values due to source precedence rules.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument array
     * @return true if arguments were successfully parsed, false otherwise
     * @note Command-line arguments take the highest precedence in the
     * configuration hierarchy (overriding file and runtime values).
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save current configuration to a JSON file
     * 
     * @param filepath Path where to save the configuration file
     * @return true if file was successfully saved, false otherwise
     * @see loadFromFile()
     */
    bool saveToFile(const std::string& filepath) const;
    /// @}
    
    /**
     * @name Value Access Methods
     * 
     * These methods provide type-safe access to configuration values.
     * Each type has both checked (returning optional) and unchecked (default)
     * variants for different use cases.
     */
    /// @{
    
    /**
     * @brief Get configuration value with type safety and null checking
     * 
     * @tparam T Type of value to retrieve (must match stored type)
     * @param key Configuration key to look up
     * @return Optional containing the value if key exists and type matches,
     *         empty optional otherwise
     * @see getOr()
     * @see set()
     * 
     * @code
     * auto maxNeurons = config.get<size_t>("max_neurons");
     * if (maxNeurons) {
     *     // Use maxNeurons.value()
     * }
     * @endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Type of value to retrieve (must match stored type)
     * @param key Configuration key to look up
     * @param defaultValue Value to return if key doesn't exist or type doesn't match
     * @return The configuration value if found and type matches, otherwise
     *         defaultValue
     * @see get()
     * 
     * @code
     * auto learningRate = config.getOr("learning_rate", 0.01f);
     * @endcode
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    /// @}
    
    /**
     * @name Value Modification Methods
     * 
     * These methods set configuration values with type safety and
     * source tracking. The source parameter allows tracking the origin
     * of runtime modifications.
     */
    /// @{
    
    /**
     * @brief Set configuration value with explicit source
     * 
     * @param key Configuration key to set
     * @param value Configuration value to store
     * @param source Origin of the configuration value (default: Runtime)
     * @see ConfigSource
     * @see get()
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set string configuration value
     * 
     * @param key Configuration key to set
     * @param value String value to store
     * @param source Origin of the configuration value (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set integer configuration value
     * 
     * @param key Configuration key to set
     * @param value Integer value to store
     * @param source Origin of the configuration value (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set double configuration value
     * 
     * @param key Configuration key to set
     * @param value Double value to store
     * @param source Origin of the configuration value (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set boolean configuration value
     * 
     * @param key Configuration key to set
     * @param value Boolean value to store
     * @param source Origin of the configuration value (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    /// @}
    
    /**
     * @name Configuration Query Methods
     * 
     * These methods provide information about the current configuration state.
     */
    /// @{
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists (regardless of source), false otherwise
     * @see getKeys()
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys (including defaults)
     * @note Does not distinguish between keys from different sources,
     *       only shows the currently effective keys.
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Remove a configuration key
     * 
     * @param key Configuration key to remove
     * @note Removing a key restores it to its default value or
     *       removes it entirely if no default exists.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Clear all configuration values
     * 
     * @note This removes all non-default values, restoring the
     *       configuration to its default state.
     */
    void clear();
    
    /**
     * @brief Get a human-readable configuration summary
     * 
     * @return String containing formatted configuration information
     * @see loadFromFile()
     * @see saveToFile()
     */
    std::string summary() const;
    /// @}
    
    /**
     * @name Related Documentation
     * 
     * Configuration is used extensively throughout the NLM system.
     * Key files that depend on or use the Config class:
     */
    /// @{
    
    /**
     * @see src/agent/AgentBrain.hpp for configuration usage
     * @see src/experiments/Experiment.hpp for experiment configuration
     * @see TODO Configuration validation and schema support (Phase 3)
     */
    /// @}
    
private:
    /**
     * @name Internal Implementation Details
     * 
     * PImpl idiom implementation for efficient updates without recompilation.
     */
    /// @{
    
    /**
     * Forward declaration of private implementation.
     * 
     * The actual implementation is hidden in a separate compilation unit
     * to reduce compilation dependencies and allow implementation changes.
     */
    struct Impl;
    
    std::unique_ptr<Impl> pImpl;  /**< Private implementation pointer */
    
    /**
     * @name Internal Helper Methods
     * 
     * Utility methods for configuration processing.
     */
    /// @{
    
    /**
     * @brief Trim whitespace from string
     * 
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str String to convert
     * @return Lowercase version of the input string
     */
    static std::string toLower(const std::string& str);
    /// @}

}; // class Config

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types supported by the Config class
 * 
 * This variant type supports the basic data types used throughout NLM:
 * - Primitive types: int, int64_t, double, bool
 * - String type: std::string
 * - Array types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * The choice of types is based on the needs of the neural simulation system:
 * - Numeric values for neuron parameters and plasticity weights
 * - String values for configuration keys and descriptions
 * - Array types for configuration lists and parameter sets
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
 * @brief Source of configuration data
 * 
 * Different sources are prioritized when loading configuration:
 * - Default: Built-in defaults
 * - File: Configuration saved/loaded from files
 * - CommandLine: Configuration from command line arguments
 * - Runtime: Configuration set during program execution
 * 
 * When a key exists in multiple sources, the CommandLine source takes precedence
 * over File, which takes precedence over Default.
 */
enum class ConfigSource {
public:
    Default,    ///< Built-in default values
    File,       ///< Configuration loaded from file
    CommandLine,///< Configuration from command line arguments
    Runtime     ///< Configuration set during program execution
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration key-value pair with metadata.
 * Used internally by the Config class to maintain configuration state.
 */
struct ConfigEntry {
    std::string key;      ///< Configuration key (e.g., "neuron_count", "stdp_ltp_weight")
    ConfigValue value;    ///< Configuration value
    ConfigSource source;  ///< Source of this configuration value
    std::string description; ///< Human-readable description of the configuration
    
    /**
     * @brief Default constructor
     * 
     * Initializes an empty configuration entry with default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with key, value, and source
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the configuration
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration management class for NLM
 * 
 * The Config class provides a unified interface for managing configuration
 * data with support for multiple sources, serialization, and type-safe access.
 * It serves as the central configuration manager for the neural simulation system.
 * 
 * Key features:
 * - Support for JSON, YAML, and simple key=value file formats
 * - Command line argument parsing with --key=value syntax
 * - Type-safe value access with automatic conversion
 * - Source prioritization (CommandLine > File > Default)
 * - Configuration persistence to file
 * - Fallback mechanisms for robust operation
 * 
 * The class uses the Pimpl idiom for efficient updates and compilation.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration with default values.
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Configuration objects should not be copied to prevent shallow copies
     * of the internal implementation.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Configuration objects should not be assigned to prevent shallow copies
     * of the internal implementation.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Allows efficient transfer of ownership of the internal configuration data.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Allows efficient transfer of ownership of the internal configuration data.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Attempts to parse the configuration file as JSON first. If JSON parsing
     * fails, falls back to the simple key=value format used in Phase 1.
     * 
     * @param filepath Path to the configuration file
     * @return true if configuration was loaded successfully, false otherwise
     * 
     * @note Supported formats: JSON (preferred), simple key=value (fallback)
     * @note JSON format example: {"neuron_count": 1000, "stdp_ltp_weight": 0.01}
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format --key=value or -key value.
     * Command line values take highest precedence over file and default values.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument strings
     * @return true if arguments were parsed successfully, false otherwise
     * 
     * @note Supported formats: --key=value, -key value
     * @warning Command line values override file and default settings
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Exports the current configuration as JSON to the specified file.
     * This can be used to save configuration changes or create configuration
     * templates.
     * 
     * @param filepath Path where the configuration should be saved
     * @return true if configuration was saved successfully, false otherwise
     * 
     * @note Always saves in JSON format, regardless of input format
     * @note File will be created if it doesn't exist, overwritten if it does
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves a configuration value with automatic type conversion.
     * The template parameter T specifies the expected type of the value.
     * 
     * @tparam T Type of the expected configuration value
     * @param key Configuration key to retrieve
     * @return Optional containing the value if key exists, std::nullopt otherwise
     * 
     * @note Supports automatic numeric type conversion (int -> double, etc.)
     * @note Returns std::nullopt if key doesn't exist or type conversion fails
     * 
     * @example Get an integer value
     *   auto neuronCount = config->get<int>("neuron_count");
     *   if (neuronCount) {
     *       std::cout << "Neurons: " << *neuronCount << std::endl;
     *   }
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value or returns the provided default if
     * the key doesn't exist or the value cannot be retrieved.
     * 
     * @tparam T Type of the expected configuration value
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if key doesn't exist
     * @return Value from configuration or default value
     * 
     * @note More efficient than checking for std::nullopt before using get()
     * @example Get with default
     *   auto maxSteps = config->getOr<int>("max_steps", 1000);
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value with source tracking
     * 
     * Sets a configuration value and tracks its source. The source determines
     * the precedence of this value relative to other sources.
     * 
     * @param key Configuration key to set
     * @param value Configuration value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string overload)
     * 
     * @param key Configuration key to set
     * @param value String configuration value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer overload)
     * 
     * @param key Configuration key to set
     * @param value Integer configuration value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (64-bit integer overload)
     * 
     * @param key Configuration key to set
     * @param value 64-bit integer configuration value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (floating-point overload)
     * 
     * @param key Configuration key to set
     * @param value Floating-point configuration value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean overload)
     * 
     * @param key Configuration key to set
     * @param value Boolean configuration value to set
     * @param source Source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists in the configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * Deletes the specified configuration key from the configuration.
     * This only affects the configuration if the key wasn't set via a
     * higher-precedence source.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys currently set
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration entries, leaving only default values.
     * This is useful for resetting the configuration to its initial state.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * Returns a human-readable summary of all configuration entries,
     * including values and their sources.
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @brief Pimpl implementation detail
     * 
     * The actual implementation details are hidden in this private struct
     * to reduce compilation dependencies and allow for efficient updates.
     */
    struct Impl;
    
    std::unique_ptr<Impl> pImpl;  ///< Pointer to implementation
    
    /**
     * @brief Internal helper: Trim whitespace from string
     * 
     * Removes leading and trailing whitespace from the input string.
     * 
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: Convert string to lowercase
     * 
     * Converts all characters in the input string to lowercase.
     * 
     * @param str String to convert
     * @return Lowercase version of the input string
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Convert JSON node to configuration entry (private helper)
     * 
     * @param node JSON node to convert
     * @param source Configuration source for the converted entry
     */
    void convertJsonToConfig(const json::JsonNode& node, ConfigSource source);
};

} // namespace nlm

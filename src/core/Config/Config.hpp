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
 * @brief Configuration value types for NLM system
 * 
 * Supports all major data types needed for neural simulation configuration,
 * including arrays for multi-dimensional parameters.
 * 
 * @note Uses std::variant for flexible configuration storage
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
 * Tracks the origin of configuration values for precedence resolution.
 * Later sources override earlier ones when conflicts occur.
 */
enum class ConfigSource {
public:
    /// Default/initial value
    Default,
    /// Loaded from configuration file
    File,
    /// Set from command line arguments
    CommandLine,
    /// Set at runtime programmatically
    Runtime
};

/**
 * @brief Single configuration entry
 * 
 * Contains a key-value pair with metadata about its source and description.
 * Used internally by the Config class to maintain configuration history.
 */
struct ConfigEntry {
    std::string key;              ///< Configuration key (identifier)
    ConfigValue value;            ///< Configuration value
    ConfigSource source;          ///< Origin of this configuration value
    std::string description;      ///< Human-readable description
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with all parameters
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of configuration
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM system
 * 
 * The Config class provides a flexible, type-safe configuration system
 * for the neural simulation framework. It supports:
 * - Loading from JSON configuration files
 * - Command-line argument overrides
 * - Runtime configuration changes
 * - Type-safe value retrieval with defaults
 * - Value validation and clamping
 * 
 * Configuration is hierarchical but flat in storage, allowing easy
 * access to any parameter by its string key.
 * 
 * @note Thread-safe for read operations
 * @note Supports both compile-time and runtime configuration
 */
class Config {
public:
    /**
     * @brief Default constructor creates empty configuration
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    /**
     * @brief Disable copying (configurations are not copyable)
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable moving (configurations can be moved efficiently)
     */
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @param filepath Path to JSON configuration file
     * @return bool True if file was successfully loaded
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format --key=value or --key value
     * and applies them to the current configuration, overriding previous values.
     * 
     * @param argc Argument count
     * @param argv Argument vector
     * @return bool True if any valid configuration arguments were found
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * @param filepath Path where configuration should be saved
     * @return bool True if configuration was successfully saved
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type checking
     * 
     * @tparam T Desired type of value
     * @param key Configuration key
     * @return std::optional<T> Value if key exists and matches type, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Desired type of value
     * @param key Configuration key
     * @param defaultValue Value returned if key doesn't exist or has wrong type
     * @return T Configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string version)
     * 
     * @param key Configuration key
     * @param value String value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer version)
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (floating-point version)
     * 
     * @param key Configuration key
     * @param value Floating-point value
     * @param source Origin of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean version)
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Origin of this configuration value
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
     * @return std::vector<std::string> List of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Get human-readable configuration summary
     * 
     * @return std::string Formatted summary of all configuration values
     */
    std::string summary() const;
    
private:
    /**
     * @brief PIMPL idiom implementation details
     * 
     * Pointer-to-implementation pattern for efficient compilation
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: Trim whitespace from string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: Convert string to lowercase
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

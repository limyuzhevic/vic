#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <limits>

// nlohmann/json for modern configuration file support
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
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
 * @enum ParameterType
 * @brief Type definitions for configuration parameters
 */
enum class ParameterType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntArray,
    DoubleArray,
    StringArray,
    Unknown
};

/**
 * @struct ConfigParameter
 * @brief Defines a configuration parameter with type and validation constraints
 */
struct ConfigParameter {
    std::string key;                ///< Parameter name
    ParameterType type;             ///< Expected data type
    ConfigValue defaultValue;       ///< Default value
    std::string description;        ///< Human-readable description
    bool required;                  ///< Whether this parameter is required
    std::string validationRegex;    ///< Regex pattern for string validation
    double minValue;                ///< Minimum value for numeric types
    double maxValue;                ///< Maximum value for numeric types
    std::vector<ConfigValue> allowedValues;  ///< List of allowed values
    
    ConfigParameter() : type(ParameterType::Unknown), required(false), 
                       minValue(std::numeric_limits<double>::lowest()),
                       maxValue(std::numeric_limits<double>::max()) {}
};

/**
 * @class ConfigSchema
 * @brief Schema validation for configuration parameters
 */
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Disable copying, enable moving
    ConfigSchema(const ConfigSchema&) = delete;
    ConfigSchema& operator=(const ConfigSchema&) = delete;
    ConfigSchema(ConfigSchema&&) noexcept;
    ConfigSchema& operator=(ConfigSchema&&) noexcept;
    
    /**
     * @brief Add a parameter definition to the schema
     */
    void addParameter(const ConfigParameter& param);
    
    /**
     * @brief Get parameter by key
     */
    const ConfigParameter* getParameter(const std::string& key) const;
    
    /**
     * @brief Get all parameter keys
     */
    std::vector<std::string> getParameterKeys() const;
    
    /**
     * @brief Validate configuration against schema
     */
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    
    /**
     * @brief Get all validation errors for configuration
     */
    std::vector<std::string> getValidationErrors(const Config& config) const;
    
    /**
     * @brief Check if schema has validation enabled
     */
    bool isValidationEnabled() const { return enableValidation; }
    
    /**
     * @brief Enable/disable schema validation
     */
    void setValidationEnabled(bool enabled) { enableValidation = enabled; }
    
    /**
     * @brief Clear all parameters from schema
     */
    void clear() { parameters.clear(); }
    
private:
    std::map<std::string, ConfigParameter> parameters;
    bool enableValidation = true;
};

/**
 * @enum ConfigSource
 * @brief Enumeration of possible configuration sources
 * 
 * Configuration values can originate from different sources, with a clear precedence order:
 * Default < File < CommandLine < Runtime (runtime overrides are highest priority)
 */
enum class ConfigSource {
    Default,      ///< Default configuration values
    File,         ///< Loaded from configuration file
    CommandLine,  ///< Override from command line arguments
    Runtime       ///< Set at runtime during execution
};

/**
 * @struct ConfigEntry
 * @brief Single configuration parameter with metadata
 * 
 * Represents a single configuration parameter including its value, source, and description.
 * Used internally by the Config class to track configuration state and precedence.
 */
struct ConfigEntry {
    std::string key;          ///< Configuration key name
    ConfigValue value;         ///< Configuration value
    ConfigSource source;       ///< Source of this configuration value
    std::string description;   ///< Human-readable description of the parameter
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Hierarchical configuration management system
 * 
 * The Config class provides a robust configuration system supporting multiple file formats
 * with automatic detection. It replaces the simple key=value format from Phase 1 with a
 * modern JSON-based configuration system while maintaining full backward compatibility.
 * 
 * Key features:
 * - Dual-format support: JSON (preferred) and simple key=value (legacy)
 * - Type-safe value retrieval with automatic type conversion
 * - Source tracking and precedence management
 * - Comprehensive validation and error handling
 * - Serialization/deserialization capabilities
 * 
 * Configuration sources have clear precedence: Default < File < CommandLine < Runtime
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file
     * 
     * Supports multiple file formats with automatic detection:
     * - JSON format (preferred): Modern key-value pairs with rich data types
     * - Simple key=value format (backward compatibility): Phase 1 format
     * 
     * @param filepath Path to configuration file
     * @return true if configuration loaded successfully, false otherwise
     * 
     * @note JSON format supports: integers, floats, booleans, strings, arrays
     *       Simple format supports: key=value pairs (with optional quoting)
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Supports both --key=value and -key value formats for flexible command-line usage.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line arguments array
     * @return true if arguments processed successfully
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Automatically uses JSON format for modern configuration storage.
     * Maintains backward compatibility with simple key=value format for legacy systems.
     * 
     * @param filepath Path where configuration should be saved
     * @return true if configuration saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @tparam T Desired value type (int, double, bool, std::string, or std::vector)
     * @param key Configuration key to retrieve
     * @return Optional containing the value if key exists, std::nullopt otherwise
     * 
     * @note Supported types: int, int64_t, double, bool, std::string,
     *       std::vector<int>, std::vector<double>, std::vector<std::string>
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Desired value type
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key doesn't exist
     * @return Configuration value or default if key missing
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @tparam T Value type (inferred from value)
     * @param key Configuration key to set
     * @param value New configuration value
     * @param source Source of this configuration value
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience overloads for different value types
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key exists in configuration
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
     * @return Vector of all configuration key names
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return Human-readable string containing all configuration entries
     */
    std::string summary() const;
    
    /**
     * @brief Batch set multiple configuration values
     * 
     * @param values Map of key-value pairs to set
     * @param source Configuration source for all values
     */
    void batchSet(const std::map<std::string, ConfigValue>& values, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Batch get multiple configuration values
     * 
     * @param keys Vector of keys to retrieve
     * @return Map of key-value pairs for requested keys
     */
    std::map<std::string, ConfigValue> batchGet(const std::vector<std::string>& keys) const;
    
    /**
     * @brief Merge configuration from another Config object
     * 
     * @param other Source configuration to merge from
     * @param override If true, always override current values
     * @return true if any values were changed
     */
    bool mergeFrom(const Config& other, bool override = false);
    
    /**
     * @brief Set parent configuration for inheritance
     * 
     * @param parent Parent configuration object
     */
    void setParentConfig(const std::shared_ptr<Config>& parent);
    
    /**
     * @brief Validate configuration against schema
     * 
     * @param schema Schema to validate against
     * @throws std::runtime_error if validation fails
     */
    void validateAgainstSchema(const ConfigSchema& schema);
    
    // Internal helper methods for validation and utilities
    bool isArrayType(const ConfigValue& value) const;
    size_t getArraySize(const ConfigValue& value) const;
    void log(const std::string& message, const std::string& level) const;
    
    // Internal helper methods
    bool loadFromJSON(const std::string& filepath);
    bool loadFromSimpleFormat(const std::string& filepath);
    ConfigValue jsonToConfigValue(const json& j);
    json configToJson(const ConfigValue& value);
    
    // Legacy compatibility helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Set for generic get operations
    std::optional<ConfigValue> getGeneric(const std::string& key) const;
    
    // Performance optimization methods
    void setupPerformanceOptimizations();
    void invalidateCache() const;
    ConfigValue getCachedValue(const std::string& key) const;
    
    // Configuration parsing methods
    bool parseValue(const std::string& str, ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

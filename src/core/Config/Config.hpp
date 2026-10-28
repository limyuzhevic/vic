#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <nlohmann/json.hpp>

namespace nlm {

/**
 * @brief Exception class for NLM configuration errors
 */
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Exception class for file I/O errors during configuration operations
 */
class ConfigIOError : public ConfigException {
public:
    explicit ConfigIOError(const std::string& message) : ConfigException(message) {}
};

/**
 * @brief Exception class for validation errors during configuration operations
 */
class ConfigValidationError : public ConfigException {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigException(message) {}
};

/**
 * @brief Configuration value types
 * 
 * Supports basic types and collections for flexible configuration storage
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
 * Tracks where configuration values originate from for priority resolution
 */
enum class ConfigSource {
public:
    Default = 0,      ///< Default values built into the system
    File = 1,         ///< Loaded from configuration file
    CommandLine = 2,  ///< Set via command line arguments
    Runtime = 3       ///< Set at runtime during execution
};

/**
 * @brief Configuration entry with metadata
 * 
 * Represents a single configuration parameter with its value, source, and description
 */
struct ConfigEntry {
    std::string key;              ///< Configuration key/parameter name
    ConfigValue value;             ///< Configuration value
    ConfigSource source;           ///< Source of the configuration value
    std::string description;       ///< Human-readable description of the parameter
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    /**
     * @brief Get string representation of configuration entry
     * 
     * @return Formatted string for debugging
     */
    std::string toString() const;
};

/**
 * @brief Configuration schema definition for validation
 * 
 * Provides type information, default values, and validation rules for configuration parameters
 */
struct ConfigSchemaEntry {
    std::string key;              ///< Configuration key
    std::string type;              ///< Expected type ("int", "double", "string", "bool", "vector<int>", etc.)
    ConfigValue defaultValue;      ///< Default value if not specified
    std::string description;       ///< Description of the parameter
    std::string validationRegex;   ///< Regex pattern for string validation
    double minValue;               ///< Minimum allowed value for numeric types
    double maxValue;               ///< Maximum allowed value for numeric types
    bool required;                 ///< Whether this parameter is required
    
    ConfigSchemaEntry()
        : key(), type(), defaultValue(), description(), validationRegex(), 
          minValue(std::numeric_limits<double>::lowest()), 
          maxValue(std::numeric_limits<double>::max()),
          required(false) {}
};

/**
 * @brief Configuration validation result
 */
struct ConfigValidationResult {
    bool valid;                    ///< Whether configuration is valid
    std::vector<std::string> errors; ///< List of validation error messages
    std::vector<std::string> warnings; ///< List of validation warning messages
    
    ConfigValidationResult() : valid(true) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

/**
 * @brief Advanced configuration system with JSON/YAML support and validation
 * 
 * The main configuration class provides comprehensive configuration management including:
 * - Loading from JSON/YAML files and command line arguments
 * - Type-safe value access with validation
 * - Configuration schema validation
 * - Default value management
 * - Configuration serialization
 * 
 * This replaces the simple key=value format with a more robust and feature-rich
 * configuration system that supports structured configuration files.
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
     * @brief Load configuration from JSON file
     * 
     * @param filepath Path to JSON configuration file
     * @param merge Whether to merge with existing configuration
     * @return true if configuration loaded successfully
     * @throws ConfigIOError if file cannot be read or parsed
     */
    bool loadFromJSON(const std::string& filepath, bool merge = false);
    
    /**
     * @brief Load configuration from YAML file
     * 
     * @param filepath Path to YAML configuration file
     * @param merge Whether to merge with existing configuration
     * @return true if configuration loaded successfully
     * @throws ConfigIOError if file cannot be read or parsed
     */
    bool loadFromYAML(const std::string& filepath, bool merge = false);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line arguments
     * @param prefix Prefix for configuration keys (e.g., "--" for "--brain.neuron_count")
     * @return true if configuration loaded successfully
     */
    bool loadFromArgs(int argc, char** argv, const std::string& prefix = "--");
    
    /**
     * @brief Save configuration to JSON file
     * 
     * @param filepath Path to output JSON file
     * @param pretty Whether to format output with indentation
     * @return true if configuration saved successfully
     * @throws ConfigIOError if file cannot be written
     */
    bool saveToJSON(const std::string& filepath, bool pretty = true) const;
    
    /**
     * @brief Save configuration to YAML file
     * 
     * @param filepath Path to output YAML file
     * @param pretty Whether to format output with indentation
     * @return true if configuration saved successfully
     * @throws ConfigIOError if file cannot be written
     */
    bool saveToYAML(const std::string& filepath, bool pretty = true) const;
    
    /**
     * @brief Set configuration values from JSON string
     * 
     * @param jsonString JSON string containing configuration
     * @param merge Whether to merge with existing configuration
     * @return true if configuration parsed successfully
     */
    bool loadFromJSONString(const std::string& jsonString, bool merge = false);
    
    /**
     * @brief Set configuration values from YAML string
     * 
     * @param yamlString YAML string containing configuration
     * @param merge Whether to merge with existing configuration
     * @return true if configuration parsed successfully
     */
    bool loadFromYAMLString(const std::string& yamlString, bool merge = false);
    
    /**
     * @brief Get configuration value by key with type safety
     * 
     * @tparam T Type to retrieve (int, double, bool, string, etc.)
     * @param key Configuration key
     * @return Optional value if key exists, otherwise std::nullopt
     * @throws ConfigValidationError if value exists but type doesn't match
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value by key with default fallback
     * 
     * @tparam T Type to retrieve
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist or type doesn't match
     * @return Value or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value with type checking
     * 
     * @tparam T Type to set
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of configuration (File, CommandLine, Runtime)
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience methods for specific types
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key
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
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Get all configuration entries
     * 
     * @return Vector of configuration entries
     */
    std::vector<ConfigEntry> getEntries() const;
    
    /**
     * @brief Clear all configuration values
     */
    void clear();
    
    /**
     * @brief Get configuration summary for debugging
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
    /**
     * @brief Validate configuration against schema
     * 
     * @param schema Vector of schema entries
     * @return Validation result with errors and warnings
     */
    ConfigValidationResult validate(const std::vector<ConfigSchemaEntry>& schema) const;
    
    /**
     * @brief Add schema entry for configuration validation
     * 
     * @param entry Schema entry to add
     */
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    
    /**
     * @brief Get schema entries
     * 
     * @return Vector of schema entries
     */
    std::vector<ConfigSchemaEntry> getSchemaEntries() const;
    
    /**
     * @brief Merge another configuration into this one
     * 
     * @param other Configuration to merge
     * @param override Whether to override existing keys
     */
    void merge(const Config& other, bool override = true);
    
    /**
     * @brief Get configuration version
     * 
     * @return Version string
     */
    std::string getVersion() const { return "2.0.0"; }
    
    /**
     * @brief Get configuration description
     * 
     * @return Description string
     */
    static std::string getDescription() { return "Advanced configuration system with JSON/YAML support"; }
    
    /**
     * @brief Set configuration schema for validation
     * 
     * @param schema Vector of schema entries
     */
    void setSchema(const std::vector<ConfigSchemaEntry>& schema) {
        schema_ = schema;
    }
    
    /**
     * @brief Get configuration schema
     * 
     * @return Reference to schema vector
     */
    const std::vector<ConfigSchemaEntry>& getSchema() const { return schema_; }
    
    /**
     * @brief Load default configuration values
     */
    void loadDefaults();
    
    /**
     * @brief Get configuration statistics
     * 
     * @return String containing configuration statistics
     */
    std::string getStatistics() const;
    
    /**
     * @brief Check if configuration is valid against current schema
     * 
     * @return true if configuration is valid
     */
    bool isValid() const;
    
    /**
     * @brief Get all configuration errors
     * 
     * @return Vector of error messages
     */
    std::vector<std::string> getErrors() const;
    
    /**
     * @brief Get all configuration warnings
     * 
     * @return Vector of warning messages
     */
    std::vector<std::string> getWarnings() const;
    
    /**
     * @brief Create configuration from JSON string
     * 
     * @param jsonString JSON string to parse
     * @return Config instance
     */
    static Config fromJSONString(const std::string& jsonString);
    
    /**
     * @brief Create configuration from YAML string
     * 
     * @param yamlString YAML string to parse
     * @return Config instance
     */
    static Config fromYAMLString(const std::string& yamlString);
    
    /**
     * @brief Create configuration from file
     * 
     * @param filepath Path to configuration file (JSON or YAML)
     * @return Config instance
     */
    static Config fromFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal schema
    std::vector<ConfigSchemaEntry> schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    
    // Type checking utilities
    template<typename T>
    static bool isTypeMatch(const ConfigValue& value);
    
    // JSON/YAML parsing utilities
    nlohmann::json toJSON(const ConfigEntry& entry) const;
    static ConfigEntry fromJSON(const std::string& key, const nlohmann::json& j);
    
    // Validation utilities
    static ConfigValidationResult validateEntry(const ConfigEntry& entry, const ConfigSchemaEntry& schema) const;
    
    // Statistics
    mutable size_t loadCount_;
    mutable size_t saveCount_;
    mutable time_t lastModified_;
};

// Type checking utility functions
namespace ConfigUtils {
    /**
     * @brief Get type name as string
     * 
     * @tparam T Type to get name for
     * @return Type name string
     */
    template<typename T>
    std::string getTypeName();
    
    /**
     * @brief Convert value to string representation
     * 
     * @param value Configuration value
     * @return String representation
     */
    std::string valueToString(const ConfigValue& value);
    
    /**
     * @brief Convert string to configuration value
     * 
     * @param typeName Expected type name
     * @param valueString String value to convert
     * @return Configuration value
     */
    ConfigValue stringToValue(const std::string& typeName, const std::string& valueString);
}

} // namespace nlm

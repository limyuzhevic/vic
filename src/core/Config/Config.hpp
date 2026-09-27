#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <limits>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

/**
 * @file Config.hpp
 * @brief Header file for the Config class implementing configuration management
 * @author NLM Framework
 * @date 2024
 * @version 1.0.0
 *
 * This file contains the Config class definition, which provides a comprehensive
 * configuration management system for the neural framework. The Config class
 * supports multiple configuration sources, validation, type checking, and
 * serialization capabilities essential for autonomous agent configuration
 * and runtime adaptation.
 */

/**
 * @enum ConfigErrorType
 * @brief Enumeration of configuration error types
 * @since Version 1.0 (2024)
 *
 * Defines the types of errors that can occur during configuration operations,
 * enabling specific error handling and recovery strategies.
 */
enum class ConfigErrorType {
    InvalidValue,       ///< Value does not match expected type or constraints
    MissingRequired,    ///< Required configuration key is missing
    InvalidType,        ///< Configuration value has incorrect type
    OutOfRange,         ///< Configuration value is outside allowed range
    InvalidPath         ///< Configuration file path is invalid
};

/**
 * @struct ConfigError
 * @brief Represents a configuration error
 * @since Version 1.0 (2024)
 *
 * Contains detailed information about configuration errors, including the
 * error type, descriptive message, and optional path to the problematic configuration.
 */
struct ConfigError {
    ConfigErrorType type;       ///< Type of configuration error
    std::string message;        ///< Human-readable error message
    std::string path;           ///< Path to the problematic configuration (optional)
    
    /**
     * @brief Constructor for ConfigError
     * @param t Error type
     * @param msg Error message
     * @param path Configuration path (optional)
     */
    ConfigError(ConfigErrorType t, const std::string& msg, const std::string& path = "")
        : type(t), message(msg), path(path) {}
    
    /**
     * @brief Format error as string
     * @return std::string Formatted error message
     */
    std::string format() const {
        std::ostringstream oss;
        oss << "ConfigError[" << static_cast<int>(type) << "]: " << message;
        if (!path.empty()) {
            oss << " (at " << path << ")";
        }
        return oss.str();
    }
};

/**
 * @struct ValidationResult
 * @brief Result of configuration validation operations
 * @since Version 1.0 (2024)
 *
 * Contains the outcome of configuration validation, including validity status,
 * list of errors, and list of warnings for configuration issues.
 */
struct ValidationResult {
    bool valid;                    ///< Whether configuration is valid
    std::vector<ConfigError> errors;     ///< List of validation errors
    std::vector<ConfigError> warnings;   ///< List of validation warnings
    
    /**
     * @brief Default constructor
     * @post Valid is set to true
     */
    ValidationResult() : valid(true) {}
    
    /**
     * @brief Constructor with parameters
     * @param v Initial validity status
     * @param errs List of errors
     * @param warns List of warnings
     */
    ValidationResult(bool v, const std::vector<ConfigError>& errs = {}, const std::vector<ConfigError>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
    
    /**
     * @brief Add error to validation result
     * @param error Error to add
     */
    void addError(const ConfigError& error) {
        errors.push_back(error);
        valid = false;
    }
    
    /**
     * @brief Add warning to validation result
     * @param warning Warning to add
     */
    void addWarning(const ConfigError& warning) {
        warnings.push_back(warning);
    }
    
    /**
     * @brief Get formatted error list
     * @return std::string Concatenated error messages
     */
    std::string getErrors() const {
        std::ostringstream oss;
        for (size_t i = 0; i < errors.size(); ++i) {
            oss << errors[i].format();
            if (i < errors.size() - 1) oss << "; ";
        }
        return oss.str();
    }
    
    /**
     * @brief Get formatted warning list
     * @return std::string Concatenated warning messages
     */
    std::string getWarnings() const {
        std::ostringstream oss;
        for (size_t i = 0; i < warnings.size(); ++i) {
            oss << warnings[i].format();
            if (i < warnings.size() - 1) oss << "; ";
        }
        return oss.str();
    }
};

/**
 * @typedef ConfigValue
 * @brief Union type for all supported configuration values
 * @since Version 1.0 (2024)
 *
 * Type-safe union supporting common configuration data types including
 * primitive types, strings, and collections of various types.
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
 * @enum ConfigSource
 * @brief Enumeration of configuration sources
 * @since Version 1.0 (2024)
 *
 * Defines the possible sources from which configuration values can originate,
 * enabling priority handling and source tracking.
 */
enum class ConfigSource {
    Default,      ///< Default configuration values
    File,         ///< Configuration loaded from file
    CommandLine,  ///< Configuration from command-line arguments
    Runtime       ///< Configuration set at runtime
};

/**
 * @enum ConfigType
 * @brief Enumeration of configuration value types
 * @since Version 1.0 (2024)
 *
 * Maps configuration values to specific types for validation and type checking.
 */
enum class ConfigType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntVector,
    DoubleVector,
    StringVector
};

/**
 * @struct ConfigRange
 * @brief Defines constraints and validation rules for configuration values
 * @since Version 1.0 (2024)
 *
 * Contains range limits, inclusive/exclusive flags, descriptions, and
 * allowed values for validating configuration parameters.
 */
struct ConfigRange {
    ConfigType type;              ///< Type of value being constrained
    double min = std::numeric_limits<double>::lowest();   ///< Minimum value (inclusive by default)
    double max = std::numeric_limits<double>::max();       ///< Maximum value (inclusive by default)
    bool minInclusive = true;     ///< Whether minimum value is inclusive
    bool maxInclusive = true;     ///< Whether maximum value is inclusive
    std::string description;      ///< Human-readable description of constraints
    std::vector<std::string> allowedValues;   ///< List of allowed string values
    
    /**
     * @brief Default constructor
     * @post Type set to Double
     */
    ConfigRange() : type(ConfigType::Double) {}
    
    /**
     * @brief Constructor with numeric range
     * @param t Value type
     * @param minVal Minimum allowed value
     * @param maxVal Maximum allowed value
     * @param desc Description of constraints
     */
    ConfigRange(ConfigType t, double minVal, double maxVal, const std::string& desc = "")
        : type(t), min(minVal), max(maxVal), description(desc) {}
    
    /**
     * @brief Constructor with allowed values
     * @param t Value type
     * @param values List of allowed string values
     * @param desc Description of constraints
     */
    ConfigRange(ConfigType t, const std::vector<std::string>& values, const std::string& desc = "")
        : type(t), description(desc), allowedValues(values) {}
    
    /**
     * @brief Check if a value satisfies the constraints
     * @param value Value to validate
     * @return bool true if value is valid, false otherwise
     */
    bool isValid(double value) const {
        if (!allowedValues.empty()) {
            // Check if value matches one of the allowed string values
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << value;
            std::string strValue = oss.str();
            // Use std::any_of for linear search since allowedValues may not be sorted
            return std::any_of(allowedValues.begin(), allowedValues.end(),
                [&strValue](const std::string& allowed) { return allowed == strValue; });
        }
        
        if (minInclusive ? value < min : value <= min) return false;
        if (maxInclusive ? value > max : value >= max) return false;
        return true;
    }
};

/**
 * @struct ConfigEntry
 * @brief Represents a single configuration key-value pair with metadata
 * @since Version 1.0 (2024)
 *
 * Contains the configuration value, its source, type, range constraints,
 * and descriptive information for validation and documentation purposes.
 */
struct ConfigEntry {
    std::string key;          ///< Configuration key
    ConfigValue value;        ///< Configuration value
    ConfigSource source;      ///< Source of this configuration
    std::string description;  ///< Human-readable description
    ConfigType type;          ///< Type of value for validation
    ConfigRange range;        ///< Constraints and validation rules
    
    /**
     * @brief Default constructor
     * @post Type set to Double
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(ConfigType::Double) {}
    
    /**
     * @brief Constructor with basic parameters
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type(ConfigType::Double) {}
    
    /**
     * @brief Constructor with full parameters
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description
     * @param t Type of value
     * @param r Range constraints
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, ConfigType t, const ConfigRange& r)
        : key(k), value(v), source(s), description(desc), type(t), range(r) {}
};

/**
 * @class Config
 * @brief Main configuration class for the neural system
 * @since Version 1.0 (2024)
 * @version 1.0.0
 *
 * The Config class provides comprehensive configuration management for the
 * neural framework, supporting multiple sources, validation, type checking,
 * serialization, and schema management. It serves as the central configuration
 * hub for all neural system components.
 *
 * Key features:
 * - Multiple configuration sources with priority handling
 * - Comprehensive type validation and range checking
 * - JSON file loading and saving
 * - Command-line argument parsing
 * - Schema-based default configuration
 * - Error reporting and recovery
 *
 * @note This class uses the Pimpl idiom for implementation hiding and efficient
 * compilation. Configuration is stored in a private implementation detail.
 *
 * @warning Configuration errors may cause system startup failures or
 * unexpected behavior if critical parameters are missing or invalid.
 *
 * @see ConfigError, ConfigEntry, ConfigRange, ValidationResult
 */
class Config {
public:
    /**
     * @brief Constructor for the Config class
     * @post Creates empty configuration with default schema
     * @note Default schema is automatically registered
     */
    Config();
    
    ~Config();
    
    // Disable copying, enable moving
    /**
     * @brief Copy constructor (deleted)
     * @warning Configurations cannot be copied due to internal pointer representation
     * @note Memory management handled through pImpl pattern
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Config object to move from
     * @post This config owns other's resources; other is in valid but unspecified state
     * @note Enables efficient transfer of configuration ownership
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Config object to move from
     * @return Reference to this config
     * @post Current config's resources are transferred from other
     * @note Enables efficient transfer of configuration ownership
     */
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    /**
     * @brief Load configuration from JSON file
     * @param filepath Path to JSON configuration file
     * @return bool true if file is loaded successfully, false otherwise
     * @post Configuration is populated from file contents
     * @note File must be valid JSON format with proper structure
     * @warning File operations may fail due to permissions, disk errors, or invalid format
     */
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    /**
     * @brief Load configuration from command line arguments
     * @param argc Number of command line arguments
     * @param argv Command line argument strings
     * @return bool true if arguments are parsed successfully, false otherwise
     * @post Configuration is populated from command line arguments
     * @note Arguments should follow standard key=value format
     */
    bool loadFromArgs(int argc, char** argv);
    
    // Load from default configuration
    /**
     * @brief Load configuration from default schema
     * @return bool true if defaults are loaded successfully, false otherwise
     * @post Configuration is populated with default values from schema
     * @note Used when no other configuration sources are available
     */
    bool loadFromDefaults();
    
    // Save to file
    /**
     * @brief Save configuration to JSON file
     * @param filepath Path to output file
     * @return bool true if save succeeds, false otherwise
     * @post Complete configuration is serialized to file
     * @note File is written in JSON format for easy parsing
     * @warning May fail due to disk space, permissions, or write errors
     */
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    /**
     * @brief Get configuration value with type conversion
     * @tparam T Target type for conversion
     * @param key Configuration key
     * @return std::optional<T> Value if key exists and conversion succeeds, std::nullopt otherwise
     * @post Value is retrieved from configuration
     * @note Automatically handles type conversion with error checking
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * @tparam T Target type for conversion
     * @param key Configuration key
     * @param defaultValue Default value to return if key doesn't exist
     * @return T Configuration value or default if key not found
     * @post Returns configuration value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    /**
     * @brief Set configuration value with source tracking
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of configuration (default: Runtime)
     * @post Value is set and source is recorded
     * @note Runtime sources override file and command-line sources
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration string value
     * @param key Configuration key
     * @param value String value
     * @param source Source of configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration integer value
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration floating-point value
     * @param key Configuration key
     * @param value Floating-point value
     * @param source Source of configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration boolean value
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    /**
     * @brief Check if configuration key exists
     * @param key Configuration key to check
     * @return bool true if key exists, false otherwise
     * @post Key existence is checked in configuration
     */
    bool has(const std::string& key) const;
    
    // Remove key
    /**
     * @brief Remove configuration key
     * @param key Configuration key to remove
     * @post Key and value are removed from configuration
     * @note Does nothing if key doesn't exist
     */
    void remove(const std::string& key);
    
    // Get all keys
    /**
     * @brief Get all configuration keys
     * @return std::vector<std::string> Vector of all configuration keys
     * @post Returns complete list of configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    // Clear all
    /**
     * @brief Clear entire configuration
     * @post All configuration values are removed
     * @warning This action cannot be undone
     */
    void clear();
    
    // Get configuration summary
    /**
     * @brief Get human-readable configuration summary
     * @return std::string Formatted summary of configuration
     * @post Summary includes all configuration keys and values
     */
    std::string summary() const;
    
    // Validation methods
    /**
     * @brief Validate entire configuration
     * @return ValidationResult Result containing validity status and any errors/warnings
     * @post Configuration is validated against schema and constraints
     */
    ValidationResult validate() const;
    
    /**
     * @brief Validate specific configuration key
     * @param key Configuration key to validate
     * @return bool true if key is valid, false otherwise
     */
    bool validateKey(const std::string& key) const;
    
    /**
     * @brief Validate configuration value against constraints
     * @param key Configuration key
     * @param value Configuration value to validate
     * @return bool true if value is valid, false otherwise
     */
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    /**
     * @brief Get formatted validation errors
     * @return std::string String containing all validation errors
     */
    std::string getValidationErrors() const;
    
    // Schema management
    /**
     * @brief Register default configuration schema
     * @post Default configuration schema is registered in the system
     * @note Called automatically by constructor
     */
    static void registerDefaultSchema();
    
    /**
     * @brief Create configuration entry for schema
     * @param key Configuration key
     * @param defaultValue Default value for this entry
     * @param source Source of configuration
     * @param description Human-readable description
     * @param type Type of value for validation
     * @param range Range constraints (optional)
     * @return ConfigEntry Created configuration entry
     * @post Entry is created with specified parameters
     */
    static ConfigEntry createEntry(const std::string& key, const ConfigValue& defaultValue, 
                                   ConfigSource source, const std::string& description,
                                   ConfigType type, const ConfigRange& range = ConfigRange());
    
    /**
     * @brief Get default configuration schema
     * @return std::unordered_map<std::string, ConfigEntry> Map of configuration keys to entries
     * @post Returns complete default schema
     */
    static std::unordered_map<std::string, ConfigEntry> getDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    /**
     * @brief Remove leading and trailing whitespace
     * @param str String to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * @param str String to convert
     * @return std::string Lowercase version of input
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Convert ConfigValue to string
     * @param value ConfigValue to convert
     * @return std::string String representation of value
     */
    static std::string toString(const ConfigValue& value);
    
    /**
     * @brief Convert ConfigValue to double
     * @param value ConfigValue to convert
     * @return double Double representation of value
     */
    static double toDouble(const ConfigValue& value);
    
    // Schema storage
    static std::unordered_map<std::string, ConfigEntry> defaultSchema;
    
    // Parse string to ConfigValue
    static ConfigValue parseValue(const std::string& str, ConfigType expectedType);
    
    // Convert ConfigValue to specific type
    template<typename T>
    static T variantTo(const ConfigValue& value);
};

} // namespace nlm
#pragma once

/**
 * @file Config.hpp
 * @brief NLM Configuration Management System
 * 
 * This file provides a comprehensive configuration management system for the NLM
 * (Neural Learning Machine) framework. It supports loading, saving, and managing
 * configuration values with robust error handling and validation.
 * 
 * @author NLM Development Team
 * @version 0.1.0
 * @date 2026
 * 
 * Configuration features:
 * - Support for multiple configuration sources (file, command line, runtime)
 * - Type-safe value storage using std::variant
 * - Comprehensive error handling with meaningful error codes
 * - Configuration validation with detailed error messages
 * - Backward compatibility with existing key=value format
 * - Extensible architecture for future JSON/YAML parser implementation
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

/**
 * @enum ConfigSource
 * @brief Source enumeration for configuration values
 * 
 * Indicates the origin of a configuration value, which affects precedence
 * and how the value should be interpreted.
 */
enum class ConfigSource {
    Default,              ///< Default configuration value
    File,                 ///< Loaded from configuration file
    CommandLine,          ///< Set via command line arguments
    Runtime               ///< Set programmatically at runtime
};

/**
 * @typedef ConfigValue
 * @brief Variant type for configuration values
 * 
 * Configuration values can be of multiple types to support flexible
 * configuration options. The variant provides type safety while allowing
 * for heterogeneous storage.
 * 
 * Supported types:
 * - Primitive types: int, int64_t, double, bool
 * - String types: std::string
 * - Collection types: std::vector<int>, std::vector<double>, std::vector<std::string>
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
 * @struct ConfigEntry
 * @brief Represents a single configuration entry with metadata
 * 
 * Stores a configuration key-value pair along with information about the
 * source and description of the configuration value.
 */
struct ConfigEntry {
    std::string key;                      ///< Configuration key identifier
    ConfigValue value;                     ///< Configuration value
    ConfigSource source;                   ///< Source of this configuration value
    std::string description;               ///< Human-readable description of the setting
    
    /**
     * @brief Default constructor
     * Creates an empty configuration entry.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the configuration setting
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @enum ConfigError
 * @brief Configuration operation error codes
 * 
 * Provides detailed error information for configuration operations to enable
 * robust error handling and recovery. Each error code has an associated
 * human-readable message via getErrorString().
 */
enum class ConfigError {
    Success = 0,              ///< Operation completed successfully
    FileNotFound,             ///< Configuration file does not exist
    FileCannotOpen,           ///< Cannot open configuration file for writing
    FileParseError,           ///< Error parsing configuration file
    InvalidFormat,            ///< Invalid configuration file format
    KeyNotFound,              ///< Requested configuration key does not exist
    TypeMismatch,             ///< Configuration value type does not match requested type
    ConversionError,          ///< Error converting configuration value
    MemoryError,              ///< Memory allocation error
    ValidationError           ///< Configuration validation failed
};

/**
 * @class Config
 * @brief Main configuration management class
 * 
 * The central configuration manager for the NLM system. Provides comprehensive
 * methods for loading, saving, and managing configuration values with
 * robust error handling and validation.
 * 
 * Configuration supports multiple sources with precedence:
 * 1. Command line arguments (highest priority)
 * 2. Configuration files
 * 3. Runtime settings (programmatic)
 * 4. Default values
 * 
 * The configuration system uses a variant-based value storage to support
 * multiple types while maintaining type safety through std::variant.
 * 
 * Error handling is integrated throughout the class, with each operation
 * returning error codes that can be queried via getLastError().
 * 
 * Configuration validation is available to ensure all values meet
 * predefined constraints and requirements.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * Creates a new empty configuration object.
     */
    Config();
    
    /**
     * @brief Destructor
     * Cleans up configuration resources.
     */
    ~Config();
    
    /**
     * @brief Disable copy construction
     * Configuration objects cannot be copied to prevent resource issues.
     * 
     * The configuration system uses internal pointer-based storage that
     * would be dangerous to copy.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment
     * Configuration objects cannot be assigned to prevent resource issues.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor (noexcept)
     * Efficiently moves resources from another configuration object.
     * 
     * This allows for efficient transfer of configuration data without
     * copying the internal implementation.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator (noexcept)
     * Efficiently moves resources from another configuration object.
     */
    Config& operator=(Config&&) noexcept;
    
    // ============================================================================
    // Core Configuration Operations
    // ============================================================================
    
    /**
     * @brief Load configuration from file (JSON format planned)
     * 
     * Loads configuration values from a file. Currently supports the simple
     * key=value format for backward compatibility. Planned to support JSON/YAML
     * format in Phase 2.
     * 
     * @param filepath Path to the configuration file
     * @return true if configuration was loaded successfully, false otherwise
     * 
     * @note Currently only supports key=value format. JSON/YAML parsing planned.
     * @warning File format will change in Phase 2 to support JSON/YAML.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from file with error handling
     * 
     * Enhanced version of loadFromFile that provides detailed error information
     * and validation support.
     * 
     * @param filepath Path to the configuration file
     * @param validate Whether to validate the configuration after loading
     * @return ConfigError indicating success or failure with detailed error info
     * 
     * @see loadFromFile() for basic file loading
     */
    ConfigError loadFromFileWithError(const std::string& filepath, bool validate = true);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format:
     * --key=value or -key value
     * 
     * Command line values take precedence over file and default values.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return true if arguments were parsed successfully, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file
     * 
     * Saves all configuration values to a file in the key=value format.
     * This implementation saves actual values (not placeholders) for all types.
     * 
     * @param filepath Path where configuration should be saved
     * @return true if configuration was saved successfully, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Save configuration to file with error handling
     * 
     * Enhanced version of saveToFile that provides detailed error information.
     * 
     * @param filepath Path where configuration should be saved
     * @return ConfigError indicating success or failure with detailed error info
     */
    ConfigError saveToFileWithError(const std::string& filepath) const;
    
    // ============================================================================
    // Error Handling
    // ============================================================================
    
    /**
     * @brief Get the last error that occurred
     * 
     * @return ConfigError indicating the last error that occurred
     * @note Returns ConfigError::Success if no errors have occurred
     */
    ConfigError getLastError() const;
    
    /**
     * @brief Get human-readable error message for an error code
     * 
     * Provides detailed descriptions of configuration errors to aid in
     * debugging and error recovery.
     * 
     * @param error ConfigError code to get message for
     * @return Human-readable string describing the error
     */
    static const char* getErrorString(ConfigError error);
    
    // ============================================================================
    // Configuration Validation
    // ============================================================================
    
    /**
     * @brief Validate configuration values
     * 
     * Validates all configuration values against predefined rules and constraints.
     * If validation fails, getValidationErrors() will contain details.
     * 
     * @return true if configuration is valid, false otherwise
     * @see getValidationErrors()
     */
    bool validate() const;
    
    /**
     * @brief Get validation error messages
     * 
     * Returns detailed error messages for any validation failures that occurred
     * during validation.
     * 
     * @return Vector of error messages describing validation failures
     * @see validate()
     */
    std::vector<std::string> getValidationErrors() const;
    
    // ============================================================================
    // Get Value Operations
    // ============================================================================
    
    /**
     * @brief Get configuration value with error handling
     * 
     * Retrieves a configuration value with detailed error reporting. This version
     * returns both the value and an error code indicating success or failure.
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key to retrieve
     * @return Pair containing the configuration value and error code
     * 
     * @see get() for simpler retrieval without error details
     */
    template<typename T>
    std::pair<std::optional<T>, ConfigError> getWithError(const std::string& key) const;
    
    /**
     * @brief Get configuration value
     * 
     * Retrieves a configuration value and returns std::nullopt if the key
     * does not exist or the type does not match.
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key to retrieve
     * @return Optional containing the configuration value, or std::nullopt if not found
     * 
     * @note Returns std::nullopt for missing keys or type mismatches
     * @see getWithError() for detailed error information
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default
     * 
     * Retrieves a configuration value, returning a default value if the key
     * does not exist or the type does not match.
     * 
     * @tparam T Expected type of the configuration value
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if key is not found
     * @return Configuration value or default value if key is not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // ============================================================================
    // Set Value Operations
    // ============================================================================
    
    /**
     * @brief Set configuration value with validation
     * 
     * Sets a configuration value with comprehensive validation. This method
     * validates the key and value before setting, and reports any validation
     * errors through getLastError().
     * 
     * @param key Configuration key to set
     * @param value Configuration value to set
     * @param source Source of this configuration value
     * @return ConfigError indicating success or failure
     */
    ConfigError setWithValidation(const std::string& key, const ConfigValue& value, 
                                  ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as string
     * @copydetails setWithValidation
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as integer
     * @copydetails setWithValidation
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as double
     * @copydetails setWithValidation
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as boolean
     * @copydetails setWithValidation
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // ============================================================================
    // Configuration Query Operations
    // ============================================================================
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if the key exists in the configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * Removes a configuration key and its value from the configuration.
     * If the key does not exist, no action is taken.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys currently stored
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration entries and resets validation errors.
     * The last error is set to ConfigError::Success.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * Returns a formatted string containing a summary of all configuration
     * values, including their source and validation status.
     * 
     * @return String containing configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @struct Impl
     * @brief Internal implementation details for the Config class
     * 
     * Encapsulates the internal storage and state management for the Config class.
     * This pattern allows for easier future modifications to the implementation
     * without changing the public interface.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from both ends of a string
     * 
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str String to convert
     * @return Lowercase version of the string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include <filesystem>

namespace nlm {

/**
 * @brief Custom exception class for configuration-related errors.
 * 
 * This exception is the base class for all configuration errors, providing
 * a standardized way to handle configuration failures in NLM.
 */
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown when configuration file operations fail.
 * 
 * This exception is raised when there are issues accessing, reading, or writing
 * configuration files (e.g., file not found, permission denied, corrupted file).
 */
class ConfigFileError : public ConfigError {
public:
    explicit ConfigFileError(const std::string& message) : ConfigError("Configuration file error: " + message) {}
};

/**
 * @brief Exception thrown when configuration file parsing fails.
 * 
 * This exception is raised when the configuration file contains syntax errors
 * or invalid data that cannot be parsed into the expected format.
 */
class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& message) : ConfigError("Configuration parse error: " + message) {}
};

/**
 * @brief Exception thrown when configuration validation fails.
 * 
 * This exception is raised when configuration values violate validation rules
 * (e.g., wrong data type, value outside allowed range, length constraints).
 */
class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigError("Configuration validation error: " + message) {}
};

/**
 * @brief Configuration value types supported by NLM.
 * 
 * NLM supports multiple configuration value types:
 * - Basic numeric types (int, int64, double, bool)
 * - String values
 * - Collection types (vectors of basic types)
 * 
 * The variant type allows storing different value types in a single ConfigEntry,
 * enabling flexible configuration schema definitions.
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
 * @brief Configuration source enumeration.
 * 
 * Tracks the origin of configuration values to determine precedence during
 * configuration merging and override resolution.
 */
enum class ConfigSource {
    Default,      ///< Default values set by the application
    File,         ///< Values loaded from configuration files
    CommandLine,  ///< Values parsed from command line arguments
    Runtime       ///< Values set at runtime during program execution
};

/**
 * @brief Complete configuration entry with metadata.
 * 
 * A ConfigEntry stores a single configuration value along with its metadata,
 * including the source, description, and validation rules. This information
 * is essential for proper configuration management and validation.
 */
struct ConfigEntry {
    std::string key;        ///< Configuration key (identifier)
    ConfigValue value;      ///< Configuration value (stored as variant)
    ConfigSource source;    ///< Origin of this configuration value
    std::string description;///< Human-readable description of the configuration
    std::string validationRule;///< Validation rule for this value (e.g., "int(1,100)")
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validationRule() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& rule = "")
        : key(k), value(v), source(s), description(desc), validationRule(rule) {}
};

// Forward declarations
class ConfigValidator;

/**
 * @brief Core configuration management class for NLM.
 * 
 * The Config class provides a unified interface for managing NLM configuration
 * values across multiple sources. It supports loading from files (JSON, YAML, INI),
 * command line arguments, and runtime settings, with proper validation and
 * error handling.
 * 
 * Key features:
 * - Multi-format file support (JSON, YAML, INI-style)
 * - Command line argument parsing
 * - Configuration validation with user-defined rules
 * - Source tracking for precedence resolution
 * - Thread-safe operations
 * - Comprehensive error handling
 * 
 * The class uses the pImpl pattern to hide implementation details and provide
 * efficient storage management.
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
     * @brief Load configuration from a file.
     * 
     * Automatically detects file format (JSON, YAML, or INI) based on file extension
     * and content, then loads configuration values into the config object.
     * 
     * @param filepath Path to the configuration file
     * @return true if configuration was successfully loaded, false otherwise
     * @throws ConfigFileError if file cannot be accessed
     * @throws ConfigParseError if file contains parsing errors
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments.
     * 
     * Parses command line arguments in both --key=value and -k value formats,
     * allowing runtime configuration overrides. Handles type conversion and
     * validation for all supported configuration value types.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if arguments were successfully parsed, false on error
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Load configuration from a string.
     * 
     * Parses configuration from a string in the specified format (JSON, YAML, or INI).
     * If format is "auto", the format is detected from the content.
     * 
     * @param content String containing configuration data
     * @param format Format of the configuration content ("json", "yaml", "ini", or "auto")
     * @return true if configuration was successfully parsed, false otherwise
     */
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    /**
     * @brief Save configuration to a file.
     * 
     * Exports all current configuration values to a file in JSON format with
     * comments and metadata for each configuration entry.
     * 
     * @param filepath Path where the configuration file should be saved
     * @return true if configuration was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value by key.
     * 
     * Retrieves a configuration value with the specified key. If the key doesn't
     * exist, returns std::nullopt. Values are automatically converted to the
     * requested type with appropriate type checking and conversion.
     * 
     * @tparam T Type to convert the configuration value to
     * @param key Configuration key to retrieve
     * @return std::optional<T> Configuration value if key exists, std::nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value or return default if key doesn't exist.
     * 
     * Retrieves a configuration value by key, returning a default value if the
     * key is not present. This is a convenience method for code that always needs
     * a value without checking for existence first.
     * 
     * @tparam T Type of the configuration value and default value
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key doesn't exist
     * @return T Configuration value or default value
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value with default parameters.
     * 
     * Sets a configuration value using default source (Runtime) without validation
     * rules. This is the basic setter for most configuration operations.
     * 
     * @param key Configuration key to set
     * @param value Configuration value to store
     * @param source Configuration source (defaults to Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a string.
     * 
     * Convenience method for setting string configuration values.
     * 
     * @param key Configuration key to set
     * @param value String configuration value
     * @param source Configuration source (defaults to Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as an integer.
     * 
     * Convenience method for setting integer configuration values.
     * 
     * @param key Configuration key to set
     * @param value Integer configuration value
     * @param source Configuration source (defaults to Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a 64-bit integer.
     * 
     * Convenience method for setting 64-bit integer configuration values.
     * 
     * @param key Configuration key to set
     * @param value 64-bit integer configuration value
     * @param source Configuration source (defaults to Runtime)
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a floating-point number.
     * 
     * Convenience method for setting double configuration values.
     * 
     * @param key Configuration key to set
     * @param value Double configuration value
     * @param source Configuration source (defaults to Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value as a boolean.
     * 
     * Convenience method for setting boolean configuration values.
     * 
     * @param key Configuration key to set
     * @param value Boolean configuration value
     * @param source Configuration source (defaults to Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value with description and validation.
     * 
     * Advanced setter that allows specifying a human-readable description and
     * validation rules for the configuration value. The validation rule should
     * follow the format "type(params)" (e.g., "int(1,100)" for range validation).
     * 
     * @param key Configuration key to set
     * @param value Configuration value to store
     * @param description Human-readable description of the configuration
     * @param validationRule Validation rule for this value
     * @param source Configuration source (defaults to Runtime)
     * @throws ConfigValidationError if value violates validation rules
     */
    void set(const std::string& key, const ConfigValue& value, 
             const std::string& description, const std::string& validationRule = "",
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists.
     * 
     * @param key Configuration key to check
     * @return true if the key exists in the configuration, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key.
     * 
     * Completely removes the configuration key and its associated value.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys.
     * 
     * @return Vector of all configuration keys currently in the config
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries.
     * 
     * Removes all configuration values, leaving the config in a clean state.
     * This is useful for resetting configuration or preparing for a new load.
     */
    void clear();
    
    /**
     * @brief Get configuration summary string.
     * 
     * Generates a human-readable summary of all configuration values including
     * their keys, values, and sources. Useful for debugging and logging.
     * 
     * @return String containing the configuration summary
     */
    std::string summary() const;
    
    /**
     * @brief Get the number of configuration entries.
     * 
     * @return Number of configuration entries in the config
     */
    size_t getEntryCount() const;
    
    /**
     * @brief Get the source of a configuration value.
     * 
     * Determines where a configuration value originated from (Default, File, 
     * CommandLine, or Runtime). This is important for configuration precedence.
     * 
     * @param key Configuration key to query
     * @return ConfigSource indicating where the value came from
     */
    ConfigSource getSource(const std::string& key) const;
    
    /**
     * @brief Validate all configuration values.
     * 
     * Checks all configuration values against their validation rules and logs
     * any validation errors. Returns true if all values are valid, false otherwise.
     * 
     * @return true if all configuration values are valid, false otherwise
     */
    bool validate() const;
    
    /**
     * @brief Get validation error messages.
     * 
     * @return Vector of error messages for invalid configuration values
     */
    std::vector<std::string> getValidationErrors() const;
    
    /**
     * @brief Detect configuration file format.
     * 
     * Automatically detects the file format based on file extension and content.
     * Supported formats: JSON, YAML, and INI-style.
     * 
     * @param filepath Path to the configuration file
     * @return Detected file format as a string ("json", "yaml", "ini", or "unknown")
     */
    static std::string detectFileFormat(const std::string& filepath);
    
    /**
     * @brief Detect configuration format from content.
     * 
     * Analyzes configuration content to determine its format (JSON, YAML, or INI).
     * 
     * @param content Configuration content to analyze
     * @return Detected format as a string ("json", "yaml", "ini", or "unknown")
     */
    static std::string detectFormatFromContent(const std::string& content);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<ConfigValidator> validator;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    static std::string trimQuotes(const std::string& str);
    
    // Format-specific parsers
    bool parseJSON(const std::string& content);
    bool parseYAML(const std::string& content);
    bool parseINI(const std::string& content);
    
    // Validation helpers
    bool validateValue(const ConfigEntry& entry) const;
    static bool validateIntRange(const ConfigEntry& entry);
    static bool validateDoubleRange(const ConfigEntry& entry);
    static bool validateStringLength(const ConfigEntry& entry);
    static bool validateVectorSize(const ConfigEntry& entry);
};

// Configuration validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    
    /**
     * @brief Validate a configuration entry against rules.
     * 
     * Subclasses should implement specific validation logic based on the validation
     * rules defined in the ConfigEntry. This method should return true if the value
     * is valid, false otherwise.
     * 
     * @param entry Configuration entry to validate
     * @return true if the entry is valid, false otherwise
     */
    virtual bool validate(const ConfigEntry& entry) const = 0;
    
    /**
     * @brief Get error message for validation failure.
     * 
     * Provides a human-readable error message explaining why validation failed.
     * 
     * @param entry Configuration entry that failed validation
     * @return Error message string
     */
    virtual std::string getErrorMessage(const ConfigEntry& entry) const = 0;
};

} // namespace nlm

// Remove duplicate namespace closing
// The namespace closing at line 178 should be removed since it doesn't exist in this version

#pragma once

/**
 * @file Config.hpp
 * @brief Enhanced configuration system for the NLM brain simulation
 * 
 * This file defines the enhanced Config class for the NLM brain simulation.
 * It provides comprehensive configuration management with robust error handling,
 * input validation, and security features. The system supports multiple
 * configuration sources (files, command line, runtime) and provides
 * type-safe value access with comprehensive error reporting.
 * 
 * @author NLM Research Team
 * @date Phase 6 Enhancement
 * @version 2.0
 * 
 * @features
 * - Comprehensive error handling with detailed error messages
 * - Input validation and type checking
 * - Security threat detection and prevention
 * - Multiple configuration sources with priority ordering
 * - Type-safe value access with automatic type conversion
 * - Configuration validation and range checking
 * - Comprehensive documentation with Doxygen-style comments
 * - Exception safety and resource management
 * 
 * @error_handling All configuration operations may throw exceptions from
 * the error hierarchy defined in ConfigError.hpp. Use try-catch blocks
 * to handle configuration errors appropriately.
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <chrono>
#include <functional>
#include <map>
#include <unordered_set>

// Error handling
#include "ConfigError.hpp"

namespace nlm {

/**
 * @enum ConfigSource
 * @brief Enumeration of configuration sources
 * 
 * @enumclass ConfigSource
 * 
 * @var ConfigSource::Default Default configuration values
 * @var ConfigSource::File Configuration loaded from file
 * @var ConfigSource::CommandLine Configuration loaded from command line
 * @var ConfigSource::Runtime Configuration set at runtime
 * 
 * @details Configuration sources have a priority order for resolving
 * conflicting values:
 * 1. Runtime (highest priority - overrides all)
 * 2. CommandLine (overrides File and Default)
 * 3. File (overrides Default)
 * 4. Default (base values)
 */
enum class ConfigSource {
    Default,    ///< Default configuration values
    File,       ///< Configuration loaded from file
    CommandLine,///< Configuration loaded from command line
    Runtime     ///< Configuration set at runtime
};

/**
 * @struct ConfigEntry
 * @brief Structure for storing configuration entries
 * 
 * This structure holds a single configuration entry with its key, value,
 * source, and description. It provides comprehensive information about
 * each configuration parameter.
 * 
 * @details The ConfigEntry structure includes:
 * - Key: Unique identifier for the configuration parameter
 * - Value: The actual configuration value
 * - Source: How the configuration was obtained
 * - Description: Human-readable description of the parameter
 * - ValidationRules: Optional validation rules for the value
 * - Timestamp: When the configuration was set
 * - ErrorCount: Number of validation errors encountered
 * 
 * @note This structure is designed for extensibility and maintainability,
 *       allowing for additional metadata and validation information
 *       to be added without breaking existing code.
 */
struct ConfigEntry {
    std::string key;                     ///< Configuration key
    ConfigValue value;                   ///< Configuration value
    ConfigSource source;                 ///< How this configuration was obtained
    std::string description;             ///< Human-readable description
    std::string validationRules;         ///< Validation rules string
    std::chrono::system_clock::time_point timestamp; ///< When configuration was set
    size_t errorCount;                   ///< Number of validation errors
    
    /**
     * @brief Default constructor creates an empty entry
     */
    ConfigEntry() 
        : source(ConfigSource::Default), errorCount(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    /**
     * @brief Construct a configuration entry with basic information
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Configuration source
     * @param description Human-readable description
     */
    ConfigEntry(const std::string& key, const ConfigValue& value, 
                ConfigSource source, const std::string& description = "")
        : key(key), value(value), source(source), description(description), 
          errorCount(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    /**
     * @brief Check if this entry has any validation errors
     * 
     * @return true if entry has validation errors
     */
    bool hasErrors() const { return errorCount > 0; }
    
    /**
     * @brief Get age of this configuration entry in seconds
     * 
     * @return Age in seconds since entry was created/modified
     */
    double getAge() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - timestamp);
        return static_cast<double>(duration.count());
    }
    
    /**
     * @brief Reset error count for this entry
     */
    void resetErrors() { errorCount = 0; }
    
    /**
     * @brief Increment error count for this entry
     */
    void incrementError() { errorCount++; }
};

/**
 * @class Config
 * @brief Enhanced configuration manager for the NLM brain simulation
 * 
 * This class provides comprehensive configuration management with robust
 * error handling, input validation, and security features. It supports
 * multiple configuration sources, type-safe value access, and provides
 * comprehensive error reporting for configuration operations.
 * 
 * @details The Config class uses a sophisticated approach to configuration
 * management:
 * 
 * 1. **Multi-source Configuration**: Supports configuration from files,
 *    command line arguments, and runtime API calls.
 * 
 * 2. **Error Handling**: Comprehensive error handling with detailed
 *    error messages and error codes for programmatic error handling.
 * 
 * 3. **Input Validation**: Validates all configuration keys and values
 *    against security threats and business rules.
 * 
 * 4. **Type Safety**: Type-safe access to configuration values with
 *    automatic type conversion and validation.
 * 
 * 5. **Performance**: Efficient data structures and algorithms for
 *    fast configuration access and modification.
 * 
 * 6. **Extensibility**: Extensible design that allows for additional
 *    configuration sources, validation rules, and error types.
 * 
 * @thread_safety This class is thread-safe for read operations.
 * Write operations should be protected by appropriate locking.
 * 
 * @exception_safety All operations provide strong exception safety.
 * If an exception is thrown, the configuration object remains in a
 * valid state.
 */
class Config {
public:
    /**
     * @brief Default constructor creates an empty configuration
     * 
     * Creates a Config object with no entries. Use the public API to
     * load configuration from files, command line, or set values directly.
     */
    Config();
    
    /**
     * @brief Destructor cleans up configuration resources
     */
    ~Config();
    
    /**
     * @brief Disable copying to prevent resource conflicts
     * 
     * @param other Config object to copy from
     * @throws ConfigError if copying is attempted
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable assignment to prevent resource conflicts
     * 
     * @param other Config object to assign from
     * @return Reference to this Config object
     * @throws ConfigError if assignment is attempted
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable move construction for efficient transfer
     * 
     * @param other Config object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Enable move assignment for efficient transfer
     * 
     * @param other Config object to move from
     * @return Reference to this Config object
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file (JSON/YAML format)
     * 
     * @param filepath Path to the configuration file
     * @throws ConfigLoadError if file cannot be read or parsed
     * @throws ConfigValidationError if configuration values are invalid
     * @throws ConfigSecurityError if configuration contains security threats
     * 
     * @details This method loads configuration from a file using a
     * sophisticated parser that supports multiple formats (JSON, YAML,
     * and simple key=value format). The file must be well-formed and
     * contain valid configuration values.
     * 
     * @note The file format should follow JSON/YAML syntax:
     * ```json
     * {
     *   "brain.neuron_count": 1000,
     *   "plasticity.learning_rate": 0.01,
     *   "environment.width": 20
     * }
     * ```
     */
    void loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @throws ConfigArgError if arguments are malformed or conflicting
     * @throws ConfigValidationError if configuration values are invalid
     * 
     * @details This method parses command line arguments using standard
     * command line syntax:
     * - `--key=value` for key-value pairs
     * - `-key value` for short arguments
     * 
     * Supported argument formats:
     * ```bash
     * --brain.neuron-count=1000
     * --plasticity.learning-rate 0.01
     * -e --environment.width 20
     * ```
     */
    void loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * @param filepath Path to save the configuration file
     * @param format File format (json, yaml, or simple)
     * @throws ConfigIOException if file cannot be written
     * 
     * @details This method saves the current configuration to a file
     * in the specified format. Supported formats include:
     * - JSON: Standard JSON format, human-readable and machine-parseable
     * - YAML: YAML format, similar to JSON but with better support for
     *         complex data structures
     * - Simple: Simple key=value format, compatible with existing
     *           configuration files
     */
    void saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    /**
     * @brief Get a configuration value with type conversion
     * 
     * @tparam T Type to convert to
     * @param key Configuration key
     * @return Configuration value or nullopt if not found
     * @throws ConfigValidationError if value cannot be converted
     * @throws ConfigValueError if value is invalid
     * 
     * @details This method retrieves a configuration value and converts
     * it to the requested type. The conversion follows these rules:
     * 
     * 1. Exact type match: If the stored value is already of type T,
     *    return it directly.
     * 
     * 2. Arithmetic conversion: If both the stored value and the
     *    requested type are arithmetic, perform the conversion.
     * 
     * 3. Type error: Otherwise, throw a ConfigValueError.
     * 
     * @note This method uses `std::optional` to indicate the absence of
     *       a value. If the key is not found, it returns `std::nullopt`.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default value
     * 
     * @tparam T Type to convert to
     * @param key Configuration key
     * @param defaultValue Default value if key is not found
     * @return Configuration value or default
     * @throws ConfigValidationError if value cannot be converted
     * @throws ConfigValueError if value is invalid
     * 
     * @details This method is similar to `get()`, but returns a default
     * value if the key is not found. It is useful for providing fallback
     * values for optional configuration parameters.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration
     * @param validate Whether to validate the value
     * @throws ConfigValidationError if value is invalid
     * @throws ConfigSecurityError if value contains security threats
     * 
     * @details This method sets a configuration value. The validation
     * parameter determines whether the value should be validated before
     * being set. Validation includes:
     * 
     * 1. Type validation: Ensure the value is of the correct type
     * 2. Range validation: Ensure the value is within allowed ranges
     * 3. Security validation: Check for security threats
     * 4. Business validation: Ensure the value meets business rules
     * 
     * @note The source parameter determines the priority of the
     *       configuration value. Runtime values (ConfigSource::Runtime)
     *       have the highest priority and override values from other
     *       sources.
     */
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    
    /**
     * @brief Set a configuration value from a string
     * 
     * @param key Configuration key
     * @param value String representation of the value
     * @param source Source of the configuration
     * @throws ConfigValidationError if value is invalid
     * @throws ConfigValueError if value cannot be parsed
     */
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from an integer
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of the configuration
     * @throws ConfigValidationError if value is invalid
     */
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from an int64
     * 
     * @param key Configuration key
     * @param value int64 value
     * @param source Source of the configuration
     * @throws ConfigValidationError if value is invalid
     */
    void set(const std::string& key, int64_t value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from a double
     * 
     * @param key Configuration key
     * @param value Double value
     * @param source Source of the configuration
     * @throws ConfigValidationError if value is invalid
     */
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value from a boolean
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of the configuration
     * @throws ConfigValidationError if value is invalid
     */
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key
     * @return true if key exists
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
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
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return Human-readable configuration summary
     */
    std::string summary() const;
    
    /**
     * @brief Get configuration entries with a specific source
     * 
     * @param source Configuration source to filter by
     * @return Vector of configuration entries with the specified source
     */
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    /**
     * @brief Validate all configuration entries
     * 
     * @return Vector of validation errors (empty if no errors)
     */
    std::vector<ConfigErrorPtr> validateAll() const;
    
    /**
     * @brief Set validation callback
     * 
     * @param callback Function to call for validation errors
     */
    void setValidationCallback(std::function<void(const ConfigError&)> callback);
    
    /**
     * @brief Set error callback
     * 
     * @param callback Function to call for errors
     */
    void setErrorCallback(std::function<void(const ConfigError&)> callback);
    
    /**
     * @brief Get number of configuration entries
     * 
     * @return Number of configuration entries
     */
    size_t size() const;
    
    /**
     * @brief Check if configuration is empty
     * 
     * @return true if configuration has no entries
     */
    bool empty() const;
    
    /**
     * @brief Get configuration age in seconds
     * 
     * @return Age in seconds since last modification
     */
    double getAge() const;
    
    /**
     * @brief Reset configuration to default values
     */
    void resetToDefaults();
    
    /**
     * @brief Merge configuration from another config object
     * 
     * @param other Config object to merge from
     * @param override Whether to override existing values
     */
    void merge(const Config& other, bool override = true);
    
    /**
     * @brief Get configuration statistics
     * 
     * @return Configuration statistics including entry counts by source
     */
    std::string getStatistics() const;
    
    /**
     * @brief Export configuration to JSON string
     * 
     * @return JSON representation of configuration
     */
    std::string exportToJSON() const;
    
    /**
     * @brief Import configuration from JSON string
     * 
     * @param jsonString JSON string to import
     * @throws ConfigLoadError if JSON cannot be parsed
     */
    void importFromJSON(const std::string& jsonString);
    
    /**
     * @brief Get configuration entry for a key
     * 
     * @param key Configuration key
     * @return Configuration entry or nullptr if not found
     */
    const ConfigEntry* getEntry(const std::string& key) const;
    
    /**
     * @brief Set entry description for a key
     * 
     * @param key Configuration key
     * @param description New description
     */
    void setEntryDescription(const std::string& key, const std::string& description);
    
    /**
     * @brief Add validation rule for a key
     * 
     * @param key Configuration key
     * @param rules Validation rules string
     */
    void addValidationRule(const std::string& key, const std::string& rules);
    
    /**
     * @brief Remove validation rules for a key
     * 
     * @param key Configuration key
     */
    void removeValidationRule(const std::string& key);
    
    /**
     * @brief Get validation rules for a key
     * 
     * @param key Configuration key
     * @return Validation rules string
     */
    std::string getValidationRules(const std::string& key) const;
    
    /**
     * @brief Set callback for configuration changes
     * 
     * @param callback Function to call when configuration changes
     */
    void setChangeCallback(std::function<void(const std::string& key, const ConfigValue& value)> callback);
    
    /**
     * @brief Get configuration version
     * 
     * @return Configuration version string
     */
    static std::string getVersion();
    
    /**
     * @brief Get configuration default file path
     * 
     * @return Default configuration file path
     */
    static std::string getDefaultFilepath();
    
    /**
     * @brief Check if configuration file exists
     * 
     * @param filepath Path to check
     * @return true if file exists
     */
    static bool fileExists(const std::string& filepath);
    
    /**
     * @brief Get supported configuration file formats
     * 
     * @return Vector of supported file formats
     */
    static std::vector<std::string> getSupportedFormats();
    
    /**
     * @brief Get supported configuration value types
     * 
     * @return Vector of supported type names
     */
    static std::vector<std::string> getSupportedTypes();
    
    /**
     * @brief Get configuration documentation
     * 
     * @return Configuration documentation string
     */
    static std::string getDocumentation();
    
    /**
     * @brief Validate configuration key name
     * 
     * @param key Key name to validate
     * @throws ConfigSecurityError if key name is invalid
     */
    static void validateKeyName(const std::string& key);
    
    /**
     * @brief Validate configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of the configuration
     * @throws ConfigValidationError if value is invalid
     * @throws ConfigSecurityError if value contains security threats
     */
    static void validateValue(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check for security threats in configuration
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @throws ConfigSecurityError if value contains security threats
     */
    static void checkSecurityThreats(const std::string& key, const ConfigValue& value);
    
    /**
     * @brief Convert configuration value to string
     * 
     * @param value Configuration value
     * @return String representation of value
     */
    static std::string valueToString(const ConfigValue& value);
    
    /**
     * @brief Convert string to configuration value
     * 
     * @param value String to convert
     * @return Configuration value
     * @throws ConfigValueError if string cannot be converted
     */
    static ConfigValue stringToValue(const std::string& value);
    
    /**
     * @brief Get configuration hash
     * 
     * @return Hash of configuration contents
     */
    size_t getHash() const;
    
    /**
     * @brief Compare configuration with another
     * 
     * @param other Configuration to compare with
     * @return true if configurations are equal
     */
    bool operator==(const Config& other) const;
    
    /**
     * @brief Compare configuration with another
     * 
     * @param other Configuration to compare with
     * @return true if configurations are not equal
     */
    bool operator!=(const Config& other) const;
    
private:
    /**
     * @struct Impl
     * @brief Private implementation details for Config
     */
    struct Impl;
    
    std::unique_ptr<Impl> pImpl; ///< Private implementation pointer
    
    // Internal helper functions
    void initializeDefaultEntries();
    void validateEntry(const ConfigEntry& entry);
    bool isValidKeyName(const std::string& key) const;
    bool isValidValue(const ConfigValue& value) const;
    std::string getValueTypeName(const ConfigValue& value) const;
    std::vector<std::string> splitString(const std::string& str, char delimiter) const;
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    bool stringContainsThreat(const std::string& str) const;
    double getDefaultMin(const std::string& key) const;
    double getDefaultMax(const std::string& key) const;
    std::string getDefaultDescription(const std::string& key) const;
    ConfigSource getDefaultSource(const std::string& key) const;
    void updateStatistics();
    
    // Validation rules
    struct ValidationRule {
        std::string key;
        std::string rules;
        std::chrono::system_clock::time_point timestamp;
    };
    
    std::vector<ValidationRule> validationRules_;
    
    // Callbacks
    std::function<void(const ConfigError&)> validationCallback_;
    std::function<void(const ConfigError&)> errorCallback_;
    std::function<void(const std::string& key, const ConfigValue& value)> changeCallback_;
    
    // Statistics
    struct ConfigStats {
        size_t totalEntries = 0;
        size_t entriesBySource[4] = {0}; // Default, File, CommandLine, Runtime
        size_t validationErrors = 0;
        double ageSeconds = 0.0;
        std::chrono::system_clock::time_point lastModified;
    } stats_;
    
    // Constants
    static constexpr double SECURITY_MAX_VALUE = 1e6;
    static constexpr size_t SECURITY_MAX_STRING_LENGTH = 1024;
    static constexpr const char* SECURITY_THREATS = ";&|`$(){}[]<>\"\\'\n\r\t\0";
    static constexpr const char* DEFAULT_FORMAT = "json";
    static constexpr size_t MAX_CONFIG_SIZE = 1024 * 1024; // 1MB
    
    // Forbidden key patterns
    static const std::unordered_set<std::string> FORBIDDEN_KEY_PATTERNS;
    
    // Helper functions for error handling
    void throwValidationError(const std::string& key, const std::string& message) const;
    void throwSecurityError(const std::string& key, const std::string& message) const;
    void throwLoadError(const std::string& filepath, const std::string& message) const;
    void throwArgError(const std::string& message) const;
    void throwIOError(const std::string& filepath, const std::string& message, bool isWrite = true) const;
    void throwValueError(const std::string& key, const std::string& message) const;
    
    // File I/O helpers
    bool readFile(const std::string& filepath, std::string& content) const;
    bool writeFile(const std::string& filepath, const std::string& content) const;
    bool isFileReadable(const std::string& filepath) const;
    bool isFileWritable(const std::string& filepath) const;
    
    // JSON/YAML parsing helpers
    bool parseJSON(const std::string& content);
    bool parseYAML(const std::string& content);
    bool parseSimple(const std::string& content);
    
    // Command line argument parsing helpers
    void parseCommandLineArgs(int argc, char** argv);
    void parseKeyValuePair(const std::string& arg);
    bool isValidArgumentFormat(const std::string& arg) const;
    
    // Validation helpers
    bool validateKeyName(const std::string& key) const;
    bool validateValue(const ConfigValue& value) const;
    bool validateRange(const std::string& key, double value) const;
    bool checkSecurityThreats(const std::string& key, const std::string& value) const;
    bool matchesValidationRule(const std::string& key, const ConfigValue& value) const;
    
    // Statistics helpers
    void incrementEntryCount(ConfigSource source);
    void incrementValidationError();
    void updateLastModified();
}; // namespace nlm

// Template implementations
#include "Config.tpp"

} // namespace nlm
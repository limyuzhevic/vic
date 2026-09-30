#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <map>
#include <functional>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <any>
#include <regex>
#include <set>
#include <ctime>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Validation error
struct ValidationError {
    std::string key;
    std::string message;
    std::string type;
    
    ValidationError(const std::string& k, const std::string& msg, const std::string& t = "validation")
        : key(k), message(msg), type(t) {}
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<std::string> tags;
    std::string validationRule;
    std::any defaultValue;
    bool required;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), tags(), 
          validationRule(), defaultValue(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), tags(), 
          validationRule(), defaultValue(), required(req) {}
};

// Configuration validation schema
class ConfigSchema {
private:
    std::vector<ValidationError> errors;
    std::vector<std::pair<std::string, std::function<bool(const ConfigValue&)>>> customValidators;
    
public:
    ConfigSchema() = default;
    
    // Custom validation callback
    ConfigSchema& addCustomValidation(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
        customValidators.emplace_back(key, validator);
        return *this;
    }
    
    // Validate a config entry
    ValidationError validate(const ConfigEntry& entry) const {
        // Run custom validators
        for (const auto& pair : customValidators) {
            if (pair.first == entry.key && !pair.second(entry.value)) {
                return ValidationError(entry.key, "Custom validation failed", "custom");
            }
        }
        return ValidationError("", "", ""); // No error
    }
    
    const std::vector<ValidationError>& getErrors() const { return errors; }
    
    bool hasErrors() const { return !errors.empty(); }
};

/**
 * @class Config
 * @brief Enhanced configuration management class for neural simulation parameters.
 * 
 * The Config class provides a comprehensive, type-safe configuration system for managing
 * neural simulation parameters. It supports multiple configuration sources (default,
 * file, command line, runtime), allows runtime configuration changes, and provides
 * advanced type-safe value access with automatic type conversion, validation,
 * serialization, and schema management.
 * 
 * Configuration values are stored as variant types (ConfigValue) supporting integers,
 * floating-point numbers, booleans, strings, and vector types. This allows the
 * configuration system to handle diverse simulation parameters in a unified manner.
 * 
 * The Config class is used by the Brain class to initialize and control neural simulation
 * behavior, including neural architecture, plasticity rules, memory systems, and
 * neuromodulation parameters.
 * 
 * @note Configuration values set at runtime (ConfigSource::Runtime) take precedence
 *       over configuration values from files or command line arguments.
 * 
 * @see Brain for primary usage of configuration
 * @see ConfigEntry for individual configuration entries
 * @see ConfigSource for configuration source types
 * @see ConfigSchema for configuration validation
 */

class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object with default schema and validation enabled.
     */
    Config();
    
    /**
     * @brief Enhanced constructor with schema
     * 
     * Creates a configuration object with the specified validation schema.
     * 
     * @param schema Optional validation schema for this configuration
     */
    explicit Config(const ConfigSchema& schema);
    
    /**
     * @brief Copy constructor (deleted for safety)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Move constructor
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Copy assignment operator (deleted for safety)
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move assignment operator
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    // ==================== Enhanced Type Safety ====================
    
    /**
     * @brief Get configuration value with compile-time type checking
     * 
     * Retrieves a configuration value for the specified key with runtime type validation
     * and safe conversion. Provides comprehensive error handling and type checking.
     * 
     * @tparam T The type of value to retrieve
     * @param key Configuration key to retrieve
     * @return std::optional<T> Optional containing the configuration value if found and type matches
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback and type inference
     * 
     * Retrieves a configuration value with type-safe default handling. The return type
     * matches the default value type, ensuring type safety at compile time.
     * 
     * @tparam T The type of value to retrieve (inferred from defaultValue)
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if key not found or type conversion fails
     * @return T The configuration value if found and type matches, otherwise defaultValue
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Get configuration value with type validation and conversion
     * 
     * Advanced get method with explicit type validation and conversion options.
     * Supports string-to-number and number-to-string conversions with validation.
     * 
     * @tparam T Target type
     * @param key Configuration key to retrieve
     * @param allowConversion Whether to allow automatic type conversion (default: true)
     * @return std::optional<T> Validated and converted value, or std::nullopt if conversion fails
     */
    template<typename T>
    std::optional<T> get(const std::string& key, bool allowConversion) const;
    
    /**
     * @brief Batch get multiple configuration values
     * 
     * Retrieves multiple configuration values in a single operation for efficiency.
     * Returns a map of successfully retrieved values with their keys.
     * 
     * @tparam T The type of values to retrieve
     * @param keys List of configuration keys to retrieve
     * @return std::unordered_map<std::string, T> Map of successfully retrieved values
     */
    template<typename T>
    std::unordered_map<std::string, T> batchGet(const std::vector<std::string>& keys) const;
    
    // ==================== Convenience Accessors ====================
    
    /**
     * @brief Get integer value with type safety
     * 
     * Convenience method for retrieving integer configuration values.
     * 
     * @param key Configuration key to retrieve
     * @return std::optional<int> Optional containing the integer value
     */
    std::optional<int> getInt(const std::string& key) const;
    
    /**
     * @brief Get double value with type safety
     * 
     * Convenience method for retrieving floating-point configuration values.
     * 
     * @param key Configuration key to retrieve
     * @return std::optional<double> Optional containing the double value
     */
    std::optional<double> getDouble(const std::string& key) const;
    
    /**
     * @brief Get string value with type safety
     * 
     * Convenience method for retrieving string configuration values.
     * 
     * @param key Configuration key to retrieve
     * @return std::optional<std::string> Optional containing the string value
     */
    std::optional<std::string> getString(const std::string& key) const;
    
    /**
     * @brief Get boolean value with type safety
     * 
     * Convenience method for retrieving boolean configuration values.
     * 
     * @param key Configuration key to retrieve
     * @return std::optional<bool> Optional containing the boolean value
     */
    std::optional<bool> getBool(const std::string& key) const;
    
    /**
     * @brief Get integer vector with type safety
     * 
     * Convenience method for retrieving integer vector configuration values.
     * 
     * @param key Configuration key to retrieve
     * @return std::optional<std::vector<int>> Optional containing the vector value
     */
    std::optional<std::vector<int>> getIntVector(const std::string& key) const;
    
    // ==================== Enhanced Setters ====================
    
    /**
     * @brief Set configuration value with full type safety
     * 
     * Template method for setting any ConfigValue type with explicit source specification.
     * Supports all variant types including vectors.
     * 
     * @tparam T The type of value to set (must be convertible to ConfigValue)
     * @param key Configuration key to set
     * @param value Value to set
     * @param source Configuration source (defaults to Runtime)
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as string
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as integer
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as double
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as boolean
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value as integer vector
     */
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    
    // ==================== Enhanced Validation ====================
    
    /**
     * @brief Validate configuration against schema
     * 
     * Validates all configuration entries against the configured schema.
     * Collects all validation errors rather than failing fast.
     * 
     * @return std::vector<ValidationError> Vector of validation errors (empty if valid)
     */
    std::vector<ValidationError> validate() const;
    
    /**
     * @brief Check if configuration value is valid according to schema
     * 
     * Performs validation for a specific key without validating all entries.
     * 
     * @param key Configuration key to validate
     * @return bool true if key is valid or has no schema rules, false if invalid
     */
    bool isValid(const std::string& key) const;
    
    /**
     * @brief Get validation errors for a specific key
     * 
     * @param key Configuration key to get errors for
     * @return std::vector<ValidationError> Vector of validation errors for the key
     */
    std::vector<ValidationError> getValidationErrors(const std::string& key) const;
    
    // ==================== Configuration Management ====================
    
    /**
     * @brief Create configuration from template
     * 
     * Creates a new configuration with default values based on a predefined template.
     * 
     * @param templateName Name of the template to use (e.g., "brain_default", "simulation_basic")
     * @return Config New configuration instance
     */
    static Config fromTemplate(const std::string& templateName);
    
    /**
     * @brief Clone configuration
     * 
     * Creates a deep copy of the configuration with the same entries and sources.
     * 
     * @return Config Cloned configuration
     */
    Config clone() const;
    
    /**
     * @brief Merge another configuration
     * 
     * Merges another configuration into this one, with the other config's values
     * taking precedence for overlapping keys.
     * 
     * @param other Configuration to merge into this one
     * @param strategy Merge strategy (override, mergeNested, keepExisting)
     */
    void merge(const Config& other, const std::string& strategy = "override");
    
    /**
     * @brief Diff configuration against another
     * 
     * Compares this configuration with another and returns differences.
     * 
     * @param other Configuration to compare against
     * @return std::vector<ConfigEntry> Vector of differing entries
     */
    std::vector<ConfigEntry> diff(const Config& other) const;
    
    // ==================== Enhanced Serialization ====================
    
    /**
     * @brief Save configuration to file (enhanced JSON format)
     * 
     * Saves configuration to JSON file with human-readable formatting and metadata.
     * 
     * @param filepath Path to the file where configuration will be saved
     * @param pretty Whether to use pretty formatting (default: true)
     * @return bool true if save operation succeeded, false otherwise
     */
    bool saveToJSON(const std::string& filepath, bool pretty = true) const;
    
    /**
     * @brief Get configuration as JSON string
     * 
     * Returns configuration as JSON string without writing to file.
     * 
     * @param pretty Whether to use pretty formatting (default: true)
     * @return std::string JSON representation of configuration
     */
    std::string toJSON(bool pretty = true) const;
    
    /**
     * @brief Load configuration from JSON
     * 
     * Loads configuration from JSON file with enhanced parsing.
     * 
     * @param filepath Path to JSON file
     * @return bool true if load succeeded, false otherwise
     */
    bool loadFromJSON(const std::string& filepath);
    
    // ==================== Backup and Restore ====================
    
    /**
     * @brief Create configuration backup
     * 
     * Creates a timestamped backup of the current configuration.
     * 
     * @param directory Directory to save backup (default: current directory)
     * @return std::string Path to created backup file
     */
    std::string createBackup(const std::string& directory = ".");
    
    /**
     * @brief Restore configuration from backup
     * 
     * Restores configuration from a backup file.
     * 
     * @param backupPath Path to backup file
     * @return bool true if restore succeeded, false otherwise
     */
    bool restoreFromBackup(const std::string& backupPath);
    
    // ==================== Configuration Comparison ====================
    
    /**
     * @brief Get configuration diff summary
     * 
     * Generates a human-readable diff between this configuration and another.
     * 
     * @param other Configuration to compare against
     * @return std::string Formatted diff summary
     */
    std::string getDiffSummary(const Config& other) const;
    
    /**
     * @brief Check if configuration changed
     * 
     * Checks if the configuration has changed from a previous state.
     * 
     * @param other Previous configuration to compare against
     * @return bool true if configuration has changed
     */
    bool hasChanged(const Config& other) const;
    
    // ==================== Information and Analysis ====================
    
    /**
     * @brief Get configuration summary
     * 
     * Returns a comprehensive summary including statistics and metadata.
     * 
     * @return std::string Formatted configuration summary
     */
    std::string summary() const;
    
    /**
     * @brief Get configuration statistics
     * 
     * Returns statistical information about the configuration.
     * 
     * @return std::string Statistical summary
     */
    std::string getStatistics() const;
    
    /**
     * @brief Get all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Check if configuration key exists
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Get configuration entry metadata
     * 
     * Returns detailed information about a configuration entry including source,
     * description, and metadata.
     * 
     * @param key Configuration key
     * @return std::optional<ConfigEntry> Entry information or std::nullopt if not found
     */
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // ==================== Modification Operations ====================
    
    /**
     * @brief Remove configuration key
     */
    void remove(const std::string& key);
    
    /**
     * @brief Clear all configuration entries
     */
    void clear();
    
    /**
     * @brief Set configuration entry with description
     */
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source, const std::string& description);
    
    // ==================== Change Notifications ====================
    
    /**
     * @brief Set configuration change callback
     * 
     * Registers a callback function that will be called when configuration
     * values are modified.
     * 
     * @param callback Function to call on configuration changes
     */
    void setChangeCallback(std::function<void(const std::string&, const ConfigValue&)> callback);
    
    /**
     * @brief Remove all change callbacks
     */
    void clearChangeCallbacks();
    
    // ==================== Debug and Inspection ====================
    
    /**
     * @brief Get configuration as debug dump
     * 
     * Returns a detailed debug representation including all metadata.
     * 
     * @return std::string Debug dump of configuration
     */
    std::string debugDump() const;
    
    /**
     * @brief Get configuration schema
     */
    const ConfigSchema& getSchema() const { return schema; }
    
    /**
     * @brief Set configuration schema
     */
    void setSchema(const ConfigSchema& newSchema) { schema = newSchema; }
    
    // ==================== Legacy Methods ====================
    
    /**
     * @brief Load configuration from file (legacy method)
     * 
     * Original file loading method for backward compatibility.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments (legacy method)
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file (legacy method)
     */
    bool saveToFile(const std::string& filepath) const;
    
    // ==================== Internal Helpers ====================
    
    /**
     * @brief Trim whitespace from string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     */
    static std::string toLower(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema;
    std::vector<std::function<void(const std::string&, const ConfigValue&)>> changeCallbacks;
    
    // Internal helper methods
    ConfigEntry* findEntry(const std::string& key);
    const ConfigEntry* findEntry(const std::string& key) const;
    
    // Type conversion utilities
    template<typename T>
    std::optional<T> convertValue(const ConfigValue& value) const;
    
    std::string nestedKeyToPath(const std::string& nestedKey) const;
    std::string pathToNestedKey(const std::vector<std::string>& path) const;
    
    void notifyChange(const std::string& key, const ConfigValue& value) const;
    
    // JSON serialization helpers
    static void toJSONInternal(std::ostream& os, const ConfigEntry& entry, bool pretty) const;
    static ConfigValue jsonToConfigValue(const std::string& jsonValue);
    static std::string configValueToJSON(const ConfigValue& value) const;
    
    // File operation helpers
    static std::string generateBackupFilename(const std::string& prefix = "config_backup");
};

} // namespace nlm

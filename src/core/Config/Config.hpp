#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <filesystem>
#include <system_error>
#include "Error.h"
#include "Validation/ValidationException.hpp"
#include "Validation/ConfigValidator.hpp"
#include "Validation/SchemaValidator.hpp"

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Type alias for configuration values, supporting multiple primitive types
 * 
 * ConfigValue is a type-safe variant that can hold:
 * - Integer types (int, int64_t)
 * - Floating-point numbers (double)
 * - Boolean flags (bool)
 * - String values (std::string)
 * - Integer arrays (std::vector<int>)
 * - Floating-point arrays (std::vector<double>)
 * - String arrays (std::vector<std::string>)
 * 
 * This allows a single configuration system to handle diverse data types
 * while maintaining type safety through the getter templated methods.
 * 
 * @see Config::get() for type-safe retrieval of values
 * @see Config::set() for setting values of any supported type
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
 * @brief Enumeration indicating the origin/source of a configuration value
 * 
 * This helps track the precedence and lifecycle of configuration values,
 * allowing the system to understand which values came from where and how they
 * should be managed.
 * 
 * @note Values from lower precedence sources can be overridden by higher
 *       precedence sources during runtime.
 * 
 * @see Config::set() for parameter source specification
 * @see Config::summary() for displaying source information
 */
enum class ConfigSource {
public:
    /** @brief Default value used during initialization */
    Default,
    /** @brief Configuration loaded from a file (JSON, YAML, or simple format) */
    File,
    /** @brief Configuration loaded from command-line arguments */
    CommandLine,
    /** @brief Configuration set programmatically at runtime */
    Runtime
};

/**
 * @brief Internal structure storing a single configuration key-value pair
 * 
 * ConfigEntry represents one configuration parameter along with metadata
 * about its origin and purpose. This structure is used internally by the
 * Config class to maintain the configuration state.
 * 
 * @note This is an internal class. Users typically interact with the Config class
 *       directly rather than using ConfigEntry directly.
 * 
 * @see Config for the main public interface
 * @see ConfigSource for the possible value sources
 */
struct ConfigEntry {
    /** @brief Configuration parameter key/identifier */
    std::string key;
    /** @brief Configuration parameter value (can be any supported type) */
    ConfigValue value;
    /** @brief Origin/source of this configuration value */
    ConfigSource source;
    /** @brief Human-readable description of what this configuration does */
    std::string description;
    
    /**
     * @brief Default constructor creating an empty configuration entry
     * 
     * Creates a ConfigEntry with empty key and value, Default source,
     * and empty description. This is primarily used internally.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor for creating a configuration entry
     * 
     * @param k Unique identifier/key for the configuration parameter
     * @param v Configuration value (supports all types in ConfigValue)
     * @param s Origin/source of this configuration value
     * @param desc Human-readable description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Central configuration management class for application settings
 * 
 * The Config class provides a unified interface for managing application
 * configuration from multiple sources (files, command-line arguments, runtime).
 * It supports multiple formats (JSON, YAML, simple key=value) and type-safe
 * access to configuration values.
 * 
 * Key features:
 * - Multi-source configuration management with precedence resolution
 * - Support for JSON, YAML, and simple text formats
 * - Type-safe value retrieval using templated get() methods
 * - Default value fallback with getOr() method
 * - Thread-safe internal implementation (pImpl pattern)
 * - Rich metadata including source tracking and descriptions
 * 
 * Usage example:
 * @code
 * nlm::Config config;
 * config.loadFromFile("config.json");
 * config.loadFromArgs(argc, argv);
 * 
 * int port = config.getOr("port", 8080);
 * std::string host = config.getOr("host", "localhost");
 * config.set("custom.setting", "value");
 * @endcode
 * 
 * @note This class uses the pImpl pattern for opaque pointer implementation
 *       to allow binary compatibility and reduce compile dependencies.
 * 
 * @see ConfigSource for tracking configuration value origins
 * @see ConfigEntry for internal storage format
 * 
 * @warning Configuration values have precedence: CommandLine > File > Runtime > Default
 * 
 * @since Version 1.0.0
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object with all default settings.
     * The configuration starts empty and can be populated via loadFromFile(),
     * loadFromArgs(), or direct set() calls.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up configuration resources. The destructor is virtual to ensure
     * proper cleanup when using Config through pointers/references.
     */
    ~Config();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Copy operations are disabled to prevent slicing and ensure proper
     * resource management through the pImpl pattern.
     * 
     * @warning Attempting to copy a Config object will result in a compilation error
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Copy operations are disabled to prevent slicing and ensure proper
     * resource management through the pImpl pattern.
     * 
     * @warning Attempting to assign a Config object will result in a compilation error
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor ( noexcept)
     * 
     * Efficient move constructor that transfers ownership of the internal
     * implementation without deep copying. Provides exception safety.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator ( noexcept)
     * 
     * Efficient move assignment that transfers ownership of the internal
     * implementation without deep copying. Provides exception safety.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Automatically detects file format based on file extension (.json, .yaml, .yml)
     * or falls back to simple key=value format for backward compatibility.
     * 
     * @param filepath Path to the configuration file to load
     * 
     * @return true if the file was successfully loaded, false otherwise
     * 
     * @throws May throw std::filesystem::filesystem_error if file operations fail
     * @throws May throw nlohmann::json::exception for JSON parsing errors
     * @throws May throw YAML::Exception for YAML parsing errors
     * 
     * @note File loading precedence (highest to lowest):
     *       1. JSON files (.json) - Full feature support
     *       2. YAML files (.yaml, .yml) - Extensive type support
     *       3. Simple format - Basic key=value pairs (backward compatibility)
     * 
     * @see loadFromJSON() for JSON-specific loading
     * @see loadFromYAML() for YAML-specific loading
     * @see loadFromSimpleFormat() for basic key=value format
     * @see ConfigSource::File for setting the source of loaded values
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Parses command-line arguments in two supported formats:
     * --key=value or -key value
     * 
     * @param argc Number of command-line arguments (argc from main())
     * @param argv Array of command-line argument strings (argv from main())
     * 
     * @return true if parsing completed (even with invalid entries), false on critical errors
     * 
     * @note Invalid arguments are silently ignored; only valid --key=value or -key value pairs
     *       are processed. This allows applications to use other command-line options.
     * 
     * @see set() for the method that actually sets the values
     * @see ConfigSource::CommandLine for marking values from command line
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save current configuration to a file
     * 
     * Saves configuration in simple key=value format for maximum compatibility.
     * Note: Only the most recent value for each key is saved; previous values
     * from different sources may be lost.
     * 
     * @param filepath Path where the configuration file should be saved
     * 
     * @return true if the file was successfully saved, false otherwise
     * 
     * @throws std::filesystem::filesystem_error if file operations fail
     * 
     * @note This method saves in the simple key=value format, not JSON or YAML.
     *       For complex types (arrays, nested structures), only the string
     *       representation is saved.
     * 
     * @see ConfigSource for information about value origins
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Load configuration from a JSON file
     * 
     * @param filepath Path to the JSON configuration file
     * 
     * @return true if the file was successfully parsed, false on parse errors or file issues
     * 
     * @throws nlohmann::json::exception for JSON parsing errors
     * 
     * @see loadFromFile() for automatic format detection
     * @see processJSONValue() for per-key value processing
     */
    bool loadFromJSON(const std::string& filepath);
    
    /**
     * @brief Load configuration from a YAML file
     * 
     * @param filepath Path to the YAML configuration file
     * 
     * @return true if the file was successfully parsed, false on parse errors or file issues
     * 
     * @throws YAML::Exception for YAML parsing errors
     * 
     * @see loadFromFile() for automatic format detection
     * @see processYAMLValue() for per-key value processing
     */
    bool loadFromYAML(const std::string& filepath);
    
    /**
     * @brief Load configuration from a simple key=value format file
     * 
     * @param filepath Path to the configuration file
     * 
     * @return true if the file was successfully parsed, false on file errors
     * 
     * @note Supports:
     *       - Empty lines (ignored)
     *       - Comments starting with # or // (ignored)
     *       - Quoted string values (both " and ' quotes)
     *       - Unquoted values with optional type conversion
     * 
     * @see loadFromFile() for format auto-detection
     * @see ConfigSource::File for setting the source of loaded values
     */
    bool loadFromSimpleFormat(const std::string& filepath);
    
    /**
     * @brief Process a single JSON value and store it in the configuration
     * 
     * Recursively processes JSON values of various types (scalars, arrays) and
     * converts them to appropriate ConfigValue types. Handles type conversion
     * errors gracefully by skipping problematic entries.
     * 
     * @param key Configuration key for the value
     * @param value JSON value to process
     * @param source Origin/source of this configuration value
     * 
     * @note Arrays are processed by detecting element types and creating
     *       corresponding std::vector<ConfigValue> types.
     *       Nested objects are not supported (will be treated as strings).
     * 
     * @see ConfigValue for supported value types
     * @see set() for the underlying storage operation
     */
    void processJSONValue(const std::string& key, const nlohmann::json& value, ConfigSource source);
    
    /**
     * @brief Process a single YAML value and store it in the configuration
     * 
     * Recursively processes YAML nodes of various types and converts them to
     * appropriate ConfigValue types. Implements sophisticated type detection
     * and conversion for scalars and arrays.
     * 
     * @param key Configuration key for the value
     * @param value YAML node to process
     * @param source Origin/source of this configuration value
     * 
     * @note Handles complex type detection including:
     *       - Integer vs float detection based on decimal point presence
     *       - Boolean detection for "true"/"false" strings
     *       - Array type detection (string, int, float)
     *       - Fallback string conversion for complex types
     * 
     * @see ConfigValue for supported value types
     * @see set() for the underlying storage operation
     */
    void processYAMLValue(const std::string& key, const YAML::Node& value, ConfigSource source);
    
    /**
     * @brief Template method to retrieve a configuration value with type safety
     * 
     * Extracts a configuration value by key and returns it as the requested type.
     * If the value doesn't exist or can't be converted to the requested type,
     * returns std::nullopt. This method provides compile-time type checking.
     * 
     * @tparam T Type to retrieve (must match one of ConfigValue supported types)
     * @param key Configuration key to look up
     * 
     * @return std::optional<T> The value if found and of correct type, std::nullopt otherwise
     * 
     * @note The supported types T are exactly those supported by ConfigValue:
     *       - int, int64_t
     *       - double
     *       - bool
     *       - std::string
     *       - std::vector<int>, std::vector<double>, std::vector<std::string>
     * 
     * @see getOr() for a version that returns a default value
     * @see set() for storing values of the same types
     * @see ConfigValue for the complete list of supported types
     * 
     * @warning Calling get with a type that doesn't match the stored type will return
     *          std::nullopt, not throw an exception.
     * 
     * @since Version 1.0.0
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Template method to retrieve a configuration value with default fallback
     * 
     * Retrieves a configuration value by key, returning a default value if the
     * key doesn't exist or can't be converted to the requested type. This provides
     * a convenient way to specify default values directly in the call.
     * 
     * @tparam T Type to retrieve (must match one of ConfigValue supported types)
     * @param key Configuration key to look up
     * @param defaultValue Value to return if the key doesn't exist or conversion fails
     * 
     * @return T The retrieved value, or defaultValue if not found
     * 
     * @note This is a convenience wrapper around get() that provides a default
     *       fallback. The default value is returned as-is without any type checking.
     * 
     * @see get() for the version that returns std::nullopt on failure
     * @see set() for storing values of the same types
     * 
     * @since Version 1.0.0
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Store a configuration value (internal version)
     * 
     * Stores a configuration value using the generic ConfigValue type. This is the
     * core implementation method used by all other set() overloads.
     * 
     * @param key Configuration parameter key
     * @param value Configuration value (any type supported by ConfigValue)
     * @param source Origin/source of this configuration value
     * 
     * @note If a key already exists, its value and source will be updated.
     *       If it doesn't exist, a new entry will be created.
     * 
     * @see ConfigValue for supported value types
     * @see ConfigSource for source specification
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Store a string configuration value
     * 
     * @param key Configuration parameter key
     * @param value String value to store
     * @param source Origin/source of this configuration value (default: Runtime)
     * 
     * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Store an integer configuration value
     * 
     * @param key Configuration parameter key
     * @param value Integer value to store
     * @param source Origin/source of this configuration value (default: Runtime)
     * 
     * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Store a floating-point configuration value
     * 
     * @param key Configuration parameter key
     * @param value Double value to store
     * @param source Origin/source of this configuration value (default: Runtime)
     * 
     * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Store a boolean configuration value
     * 
     * @param key Configuration parameter key
     * @param value Boolean value to store
     * @param source Origin/source of this configuration value (default: Runtime)
     * 
     * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Store an integer array configuration value
     * 
     * @param key Configuration parameter key
     * @param value Integer array to store
     * @param source Origin/source of this configuration value (default: Runtime)
     * 
     * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
     */
    // Vector types for JSON/YAML support
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Store a floating-point array configuration value
     * 
     * @param key Configuration parameter key
     * @param value Double array to store
     * @param source Origin/source of this configuration value (default: Runtime)
     * 
     * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
     */
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Store a string array configuration value
     * 
     * @param key Configuration parameter key
     * @param value String array to store
     * @param source Origin/source of this configuration value (default: Runtime)
     * 
     * @see set(const std::string&, const ConfigValue&, ConfigSource) for the generic version
     */
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key to check
     * 
     * @return true if the key exists in the configuration, false otherwise
     * 
     * @complexity O(n) where n is the number of configuration entries
     * 
     * @see get() for retrieving the value associated with a key
     * @see remove() for removing a configuration entry
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration entry
     * 
     * Removes all entries matching the specified key. This operation cannot
     * be undone.
     * 
     * @param key Configuration key to remove
     * 
     * @note This permanently removes the configuration entry from memory.
     *       It does not modify the original file (if loaded from file).
     * 
     * @see has() for checking key existence
     * @see clear() for removing all entries at once
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> List of all configuration keys currently stored
     * 
     * @note The order of returned keys is not guaranteed (depends on internal storage)
     * 
     * @complexity O(n) where n is the number of configuration entries
     * 
     * @see has() for checking individual key existence
     * @see get() for retrieving values for specific keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * 
     * Removes all configuration entries from memory. This operation cannot
     * be undone. After calling clear(), the configuration is empty.
     * 
     * @note This does not modify the original file (if loaded from file).
     *       To reload from file, call loadFromFile() again.
     * 
     * @see remove() for removing individual entries
     * @see loadFromFile() for repopulating the configuration
     */
    void clear();
    
    /**
     * @brief Generate a human-readable summary of the current configuration
     * 
     * Creates a formatted string showing all configuration entries with their
     * values, types, and source information. Useful for debugging and logging.
     * 
     * @return std::string Formatted configuration summary
     * 
     * @note The output format is designed for human readability:
     *       - Shows key = value [source] for each entry
     *       - Converts values to human-readable strings
     *       - Displays source as integer code (ConfigSource enum value)
     * 
     * @see ConfigSource for source code meanings
     * @see getKeys() for getting just the key names
     */
    std::string summary() const;

    /**
     * @brief Validate the configuration against a schema
     * 
     * Validates the current configuration against a JSON schema-like definition.
     * Provides detailed error messages for validation failures.
     * 
     * @param schema JSON schema object to validate against
     * @return bool true if configuration is valid, false otherwise
     * 
     * @throws ValidationException if schema is invalid
     * 
     * @note Schema format supports type, required, min, max, enum, pattern, and format constraints
     * @see SchemaValidator for schema validation details
     */
    bool validate(const nlohmann::json& schema);

    /**
     * @brief Validate the configuration using the built-in validator
     * 
     * Performs comprehensive validation including type checking, range validation,
     * dependency validation, and consistency checking.
     * 
     * @return bool true if configuration is valid, false otherwise
     * 
     * @see ConfigValidator for validation details
     */
    bool validate() const;

    /**
     * @brief Get validation errors for the configuration
     * 
     * @return ValidationResult structure containing validation errors and warnings
     */
    ValidationResult getValidationResult() const;

    /**
     * @brief Validate configuration before saving to file
     * 
     * Ensures configuration integrity before writing to file. Can be called
     * as a pre-validation step or integrated into saveToFile().
     * 
     * @throws ValidationException if configuration is invalid
     * 
     * @note This method is called automatically when saveToFile() is invoked
     *       if autoValidation is enabled
     */
    void validateBeforeSave() const;

    /**
     * @brief Check if a configuration key is valid
     * 
     * @param key Configuration key to check
     * @return bool true if key is valid, false otherwise
     * 
     * @note This method performs basic validation of configuration keys
     */
    bool isValidKey(const std::string& key) const;

    /**
     * @brief Get a list of valid configuration keys
     * 
     * @return std::vector<std::string> List of valid configuration keys
     */
    static std::vector<std::string> getValidKeys();
    
private:
    /**
     * @brief Pimpl implementation structure for opaque pointer idiom
     * 
     * This structure contains the actual implementation details of the Config class.
     * Using the pImpl pattern allows us to change the implementation without
     * recompiling client code, and reduces compile-time dependencies.
     * 
     * @note This is an internal structure. Users should not access this directly.
     */
    struct Impl;
    
    /**
     * @brief Opaque pointer to the implementation details
     * 
     * This unique_ptr provides access to the private implementation (pImpl)
     * while maintaining the Config class's opaque interface.
     */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Escape special characters in a string for file output
     * 
     * @param str String to escape
     * @return std::string Escaped string with proper quoting
     * 
     * @note Escapes quotes, newlines, tabs, and other control characters
     * 
     * @see saveToFile() for usage in file writing
     */
    static std::string escapeString(const std::string& str);
    
    /**
     * @brief Convert a character to hexadecimal string
     * 
     * @param c Character to convert
     * @return std::string Hexadecimal representation
     */
    static std::string stringToHex(char c);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types
 * 
 * The ConfigValue type represents all possible configuration values that can
 * be stored in the configuration system. This includes primitive types, strings,
 * and complex types like vectors of different data types. The use of std::variant
 * allows for type-safe storage of different value types while maintaining the
 * flexibility to handle various configuration needs.
 * 
 * Supported types:
 * - Integer types: int, int64_t
 * - Floating point types: double  
 * - Boolean types: bool
 * - String types: std::string
 * - Vector types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * This type system enables the configuration system to handle everything from
 * simple boolean flags to complex vector parameters required by neural network
 * architectures and simulation settings.
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
 * @brief Configuration source tracking
 * 
 * The ConfigSource enum tracks the origin of configuration values, which is
 * crucial for understanding the precedence and reliability of settings in the
 * configuration system. Different sources have different priorities and behaviors:
 * 
 * - **Default**: Hard-coded default values provided by the system
 * - **File**: Values loaded from configuration files
 * - **CommandLine**: Values provided via command-line arguments  
 * - **Runtime**: Values set programmatically at runtime
 * 
 * This tracking allows the configuration system to:
 * - Show users where values came from
 * - Handle conflicts when multiple sources specify the same key
 * - Provide transparency about configuration precedence
 * - Support debugging by identifying configuration sources
 */
enum class ConfigSource {
public:
    /** @brief Hard-coded default values from the system */
    Default = 0,
    /** @brief Values loaded from configuration files */
    File = 1,
    /** @brief Values from command-line arguments */
    CommandLine = 2,
    /** @brief Values set programmatically at runtime */
    Runtime = 3
};

/**
 * @brief Individual configuration entry
 * 
 * A ConfigEntry represents a single configuration key-value pair along with
 * metadata about its origin and purpose. This structure provides the foundation
 * for the configuration system's type-safe key-value storage.
 * 
 * Each entry includes:
 * - **key**: The configuration parameter name
 * - **value**: The parameter value (stored as ConfigValue variant)
 * - **source**: Where the value originated
 * - **description**: Human-readable explanation of the parameter's purpose
 * 
 * The description field is particularly important for user-facing tools and
 * documentation, helping users understand what each configuration parameter
 * does and how to use it effectively.
 */
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * Creates a configuration entry with specified values.
     * 
     * @param k Configuration key/name
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for the NLM system
 * 
 * The Config class provides a comprehensive configuration management system
 * for the Neural Learning Machine (NLM) project. It supports loading, saving,
 * and managing configuration values with type safety, source tracking, and
 * validation capabilities.
 * 
 * Key features:
 * 
 * 1. **Type Safety**: Template-based access to configuration values with
 *    automatic type checking and conversion
 * 
 * 2. **Source Tracking**: Each configuration value remembers where it came from
 *    (defaults, files, command-line, or runtime)
 * 
 * 3. **Multiple Input Sources**: Configuration can be loaded from:
 *    - Configuration files (JSON, INI, or custom format)
 *    - Command-line arguments (--key=value or -k value)
 *    - Programmatic runtime settings
 * 
 * 4. **Flexible Value Storage**: Supports primitive types, strings, and vectors
 *    of various types to accommodate different configuration needs
 * 
 * 5. **Convenience Methods**: High-level methods for common operations like
 *    getting values with defaults, checking existence, and removing keys
 * 
 * 6. **Global Configuration**: Supports a global configuration instance for
 *    applications that want centralized configuration management
 * 
 * The configuration system is designed to be both powerful and user-friendly,
 * supporting everything from simple boolean flags to complex neural network
 * parameters while providing transparency about where each value came from.
 */
class Config {
public:
    /**
     * @brief Construct a new Config instance
     * 
     * Creates an empty configuration object with default initialization.
     * The configuration starts with no entries and can be populated through
     * file loading, command-line parsing, or programmatic settings.
     */
    Config();
    
    /**
     * @brief Destroy the Config instance
     * 
     * Cleans up the configuration object and releases any allocated resources.
     * The implementation uses a pImpl pattern to hide implementation details
     * and provide better encapsulation.
     */
    ~Config();
    
    /**
     * @brief Disable copying (Rule of Five)
     * 
     * Configuration objects should not be copied to prevent unexpected behavior
     * and ensure clear ownership semantics. Use move semantics instead.
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable moving (Rule of Five)
     * 
     * Configuration objects can be moved efficiently, allowing for transfer of
     * ownership of the underlying configuration data between objects.
     */
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Parses configuration data from a file and populates the configuration
     * object. The file format is configurable but typically supports simple
     * key=value pairs with support for comments and string quoting.
     * 
     * Supported file formats:
     * - Simple text format with key=value pairs
     * - Comments starting with # or /
     * - Quoted string values
     * - Multiple values for the same key (later values override earlier ones)
     * 
     * @param filepath Path to the configuration file to load
     * @return true if the file was successfully loaded, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Parses command-line arguments to extract configuration parameters.
     * Supports multiple formats:
     * - --key=value (long option)
     * - -k value (short option)
     * 
     * Command-line arguments override values from files and defaults.
     * This allows users to easily customize configuration without modifying
     * configuration files.
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument array
     * @return true if command-line arguments were successfully processed
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Writes the current configuration to a file in a human-readable format.
     * The saved configuration includes all key-value pairs along with their
     * descriptions, making it easy for users to understand and modify settings.
     * 
     * @param filepath Path where the configuration should be saved
     * @return true if the configuration was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type checking
     * 
     * Retrieves a configuration value of the specified type. If the value exists
     * and is of the correct type, it is returned. If the value doesn't exist or
     * is of the wrong type, std::nullopt is returned.
     * 
     * This method provides type safety by ensuring that configuration values are
     * accessed in a controlled manner. Attempting to retrieve a string value
     * when an integer is stored will return nullopt.
     * 
     * @tparam T The type of value to retrieve
     * @param key The configuration key to look up
     * @return Optional containing the value if found and correct type, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value of the specified type, returning a default
     * value if the key doesn't exist or the value is of the wrong type.
     * 
     * This is the most commonly used method for accessing configuration values
     * as it provides both type safety and convenient default behavior.
     * 
     * @tparam T The type of value to retrieve
     * @param key The configuration key to look up
     * @param defaultValue The value to return if the key is not found
     * @return The configuration value or the default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Sets a configuration value with the specified source. The source indicates
     * where the value originated (e.g., File, CommandLine, Runtime), which is
     * useful for tracking configuration precedence and debugging.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a string configuration value
     * 
     * Convenience method for setting string configuration values.
     * 
     * @param key The configuration key to set
     * @param value The string value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set an integer configuration value
     * 
     * Convenience method for setting integer configuration values.
     * 
     * @param key The configuration key to set
     * @param value The integer value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a double configuration value
     * 
     * Convenience method for setting double configuration values.
     * 
     * @param key The configuration key to set
     * @param value The double value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a boolean configuration value
     * 
     * Convenience method for setting boolean configuration values.
     * 
     * @param key The configuration key to set
     * @param value The boolean value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * Returns true if the configuration contains the specified key, false otherwise.
     * This method does not check the type of the value, only its presence.
     * 
     * @param key The configuration key to check for
     * @return true if the key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * Removes the specified configuration key and its associated value.
     * If the key doesn't exist, this method does nothing.
     * 
     * @param key The configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Returns a vector containing all configuration keys currently stored
     * in the configuration object. The order is not guaranteed.
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * 
     * Removes all configuration entries from the object, returning it to an
     * empty state. This is useful for reinitializing the configuration or
     * cleaning up before a new configuration load.
     */
    void clear();
    
    /**
     * @brief Get a human-readable configuration summary
     * 
     * Returns a string containing a summary of all configuration entries,
     * including keys, values, and their sources. This is useful for:
     * - Debugging configuration issues
     * - Showing users their current configuration
     * - Logging configuration state
     * 
     * @return String containing the configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @brief Pimpl implementation detail
     * 
     * The implementation details of the Config class are hidden behind this
     * pointer-to-implementation pattern. This allows for better encapsulation
     * and easier maintenance of the configuration system.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Trim whitespace from a string
     * 
     * Removes leading and trailing whitespace characters from a string.
     * Used internally by the configuration file parser.
     * 
     * @param str The string to trim
     * @return The trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * Converts all characters in a string to lowercase. Used internally
     * for case-insensitive configuration key matching.
     * 
     * @param str The string to convert
     * @return The lowercase version of the string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

#pragma once

/**
 * @brief Configuration Management System for NLM
 * 
 * The Config class provides a flexible configuration management system for the
 * Neural Learning Machine (NLM) project. It supports multiple configuration sources
 * (file, command line, default, runtime) with automatic type inference and validation.
 * 
 * This configuration system is designed to:
 * - Support multiple value types (int, double, bool, string, and complex types)
 * - Handle configuration from different sources with proper precedence
 * - Provide easy access to configuration values with type safety
 * - Support configuration validation and error handling
 * - Enable advanced features like type conversion and default values
 * 
 * Configuration sources follow this priority (highest to lowest):
 * 1. Command line arguments
 * 2. Configuration file
 * 3. Default configuration
 * 4. Runtime configuration
 * 
 * The system uses a key=value format for files and command line arguments,
 * with intelligent type parsing (int, double, bool, string).
 * 
 * @note This is a Phase 1 implementation. Phase 2 will include:
 * - Full JSON/YAML parser with schema validation
 * - Advanced configuration features (lists, maps, validation schemas)
 * - Configuration inheritance and merging
 * - Runtime configuration updates
 * 
 * @author NLM Development Team
 * @version 0.1.0
 * @date 2026
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <string>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types for type-safe configuration storage
 * 
 * ConfigValue is a variant type that can store multiple different data types
 * to support flexible configuration. The use of std::variant provides compile-time
 * type safety while maintaining flexibility.
 * 
 * Supported types:
 * - Basic types: int, int64_t, double, bool, std::string
 * - Complex types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * @note Complex types like vectors are useful for:
 * - Lists of values (e.g., sensor IDs, neuron populations)
 * - Multi-dimensional parameters
 * - Flexible configuration that can grow with requirements
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
 * Each configuration entry can have a source that indicates where the value
 * came from. This is important for:
 * - Determining precedence (e.g., command line overrides file)
 * - Configuration validation (checking if all required values are set)
 * - Documentation (showing where each value originated)
 * - Debugging (understanding configuration changes)
 * 
 * The priority order (highest to lowest) is:
 * 1. Default - Built-in defaults from the code
 * 2. File - Loaded from configuration file
 * 3. CommandLine - Set via command line arguments
 * 4. Runtime - Set during program execution
 */
enum class ConfigSource {
public:
    /**
     * @brief Configuration source enumeration values
     */
    enum Value {
        Default,      ///< Default values built into the code
        File,         ///< Values loaded from configuration file
        CommandLine,  ///< Values set via command line arguments
        Runtime       ///< Values set during program execution
    };
    
    /**
     * @brief Convert ConfigSource to integer for serialization
     * 
     * @return Integer representation of the configuration source
     */
    int value() const { return static_cast<int>(source); }
    
    /**
     * @brief Convert integer to ConfigSource
     * 
     * @param val Integer value to convert
     * @return Corresponding ConfigSource value
     */
    static ConfigSource fromInt(int val) {
        switch (val) {
            case 0: return ConfigSource::Default;
            case 1: return ConfigSource::File;
            case 2: return ConfigSource::CommandLine;
            case 3: return ConfigSource::Runtime;
            default: return ConfigSource::Default;
        }
    }
    
private:
    ConfigSource(Value v) : source(v) {}
    Value source;
    
    friend class Config;
};

/**
 * @brief Configuration entry structure
 * 
 * A single configuration entry contains a key-value pair along with metadata
 * about the source of the value and an optional description. This structure
 * is used internally by the Config class to maintain the configuration state.
 * 
 * @note ConfigEntry is designed to be self-contained and provides all
 * necessary information about a configuration parameter in one place.
 */
struct ConfigEntry {
    /**
     * @brief Configuration parameter key/name
     */
    std::string key;
    
    /**
     * @brief Configuration parameter value
     * 
     * The value can be of multiple types (see ConfigValue for supported types).
     * The specific type is determined at runtime by the variant type system.
     */
    ConfigValue value;
    
    /**
     * @brief Source of the configuration value
     * 
     * Indicates where this configuration value came from (file, command line,
     * default, or runtime). This is crucial for understanding precedence and
     * debugging configuration issues.
     */
    ConfigSource source;
    
    /**
     * @brief Human-readable description of the configuration parameter
     * 
     * This is used in documentation, configuration validation, and error messages.
     * It should provide context about what the configuration parameter controls
     * and what values are valid.
     */
    std::string description;
    
    /**
     * @brief Default constructor for ConfigEntry
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor for ConfigEntry
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration management class
 * 
 * The Config class provides a comprehensive configuration management system
 * for the NLM project. It allows loading configuration from multiple sources,
 * setting default values, and accessing configuration parameters with type
 * safety.
 * 
 * Key features:
 * - Support for multiple configuration sources with proper precedence
 * - Automatic type inference and conversion
 * - Configuration validation and error handling
 * - Easy access to configuration values via template methods
 * - Configuration persistence (save to file)
 * - Detailed configuration summaries and debugging information
 * 
 * @note The class uses the Pimpl idiom to hide implementation details and
 * reduce compilation dependencies. This makes the Config class more flexible
 * and easier to maintain.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration object that can be populated
     * by loading from files, command line arguments, or direct calls to set().
     */
    Config();
    
    /**
     * @brief Default destructor
     * 
     * The destructor is defaulted to allow the unique_ptr implementation
     * to automatically clean up resources.
     */
    ~Config();
    
    /**
     * @brief Disable copying (Pimpl pattern)
     * 
     * Copying is disabled because the Config class uses the Pimpl idiom
     * and has a complex internal implementation. Use move operations instead.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment (Pimpl pattern)
     * 
     * See copy constructor for details on why copying is disabled.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Allows efficient transfer of configuration data between Config objects
     * by moving the unique_ptr implementation rather than copying it.
     * 
     * @param other Config object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Allows efficient assignment of configuration data by moving the
     * unique_ptr implementation rather than copying it.
     * 
     * @param other Config object to move from
     * @return Reference to this object for chaining
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Loads configuration from a file using a simple key=value format with
     * intelligent type inference. Values are parsed as integers, doubles,
     * booleans, or strings based on their format.
     * 
     * @param filepath Path to the configuration file
     * @return bool True if the file was successfully loaded, false otherwise
     * 
     * @note This is a Phase 1 implementation. Phase 2 will include:
     * - Full JSON/YAML parser support
     * - Schema validation
     * - Configuration inheritance
     * - Advanced file formats
     * 
     * @note File format supports:
     * - Comments starting with # or /
     * - Key=value pairs
     * - Quoted string values
     * - Automatic type inference
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments to load configuration values. Supports
     * both --key=value and -key value formats for command line arguments.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument strings
     * @return bool True if arguments were successfully parsed, false otherwise
     * 
     * @note Command line arguments override file configuration values.
     * Type inference works the same way as file parsing.
     * 
     * @note Supported formats:
     * - --key=value (with =)
     * - -key value (without =)
     * - --key "string value" (quoted strings)
     * - --key=123 (integer)
     * - --key=3.14 (double)
     * - --key=true (boolean)
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Saves the current configuration to a file using the key=value format.
     * Note that this creates a minimal configuration file with placeholder
     * values since it doesn't capture all type information.
     * 
     * @param filepath Path to the file to save configuration to
     * @return bool True if the file was successfully saved, false otherwise
     * 
     * @note This is a basic save functionality. A full save/load cycle
     * with all type information would require a more sophisticated format
     * like JSON or YAML.
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * Retrieves a configuration value with the specified type. If the value
     * doesn't exist or is of the wrong type, returns std::nullopt.
     * 
     * @tparam T Type of the configuration value to retrieve
     * @param key Configuration key to look up
     * @return std::optional<T> The configuration value if found and of correct type,
     *         std::nullopt otherwise
     * 
     * @note Template specialization allows access to all supported types:
     * - int
     * - int64_t
     * - double
     * - bool
     * - std::string
     * 
     * @note Error handling:
     * - If the key doesn't exist, returns std::nullopt
     * - If the key exists but is of wrong type, returns std::nullopt
     * - If the key exists and is of correct type, returns the value
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value, returning a default value if the key
     * doesn't exist or is of the wrong type. This is useful for optional
     * configuration parameters.
     * 
     * @tparam T Type of the configuration value to retrieve
     * @param key Configuration key to look up
     * @param defaultValue Default value to return if key doesn't exist
     * @return T The configuration value if found and of correct type,
     *         otherwise the default value
     * 
     * @note This method is more convenient than get() for optional configuration
     * parameters because it always returns a valid value.
     * 
     * @note Example usage:
     * @code
     * int timeout = config.getOr("timeout", 30); // Returns 30 if "timeout" not found
     * bool debug = config.getOr("debug", false); // Returns false if "debug" not found
     * std::string filename = config.getOr("filename", "default.txt");
     * @endcode
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Sets a configuration value with the specified source. This can be used
     * to set configuration values at runtime, override configuration values
     * loaded from files, or provide default values.
     * 
     * @param key Configuration key to set
     * @param value Configuration value to set
     * @param source Source of this configuration value
     * 
     * @note The set() method has multiple overloads for different value types:
     * - ConfigValue (general variant type)
     * - std::string
     * - int
     * - double
     * - bool
     * 
     * @note Example usage:
     * @code
     * config->set("neuron_count", 1000, ConfigSource::Default);
     * config->set("debug_mode", true, ConfigSource::CommandLine);
     * config->set("learning_rate", 0.01, ConfigSource::File);
     * @endcode
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * Determines whether a configuration key exists in the configuration.
     * This can be used to check if a configuration parameter has been set
     * by any of the configuration sources.
     * 
     * @param key Configuration key to check
     * @return bool True if the key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * Removes a configuration key from the configuration. This can be used
     * to clear a configuration parameter or override a configuration file
     * value by setting it to a different value.
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Returns a list of all configuration keys currently set in the configuration.
     * This can be useful for documentation, debugging, or programmatically
     * exploring the configuration state.
     * 
     * @return std::vector<std::string> List of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration values from the configuration, effectively
     * resetting it to an empty state. This can be useful for:
     * - Resetting configuration for a new run
     * - Clearing configuration for testing
     * - Starting fresh with new configuration values
     */
    void clear();
    
    /**
     * @brief Get a configuration summary
     * 
     * Returns a human-readable summary of the configuration state, including
     * all configuration keys, their values, and their sources. This is useful
     * for debugging, logging, and configuration validation.
     * 
     * @return std::string Human-readable configuration summary
     * 
     * @note The summary includes:
     * - Total number of configuration entries
     * - Each configuration key with its value
     * - The source of each configuration value
     * - Type information for each value
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation details
     * 
     * The Pimpl idiom hides the implementation details of the Config class
     * behind a pointer to an internal implementation struct. This reduces
     * compilation dependencies and makes the Config class more flexible.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper method to trim whitespace from strings
     * 
     * Removes leading and trailing whitespace from a string. This is used
     * when parsing configuration files to handle formatting variations.
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper method to convert string to lowercase
     * 
     * Converts all characters in a string to lowercase. This is useful for
     * case-insensitive string comparisons, such as when checking boolean
     * values or configuration keys.
     * 
     * @param str String to convert
     * @return std::string Lowercase version of the input string
     */
    static std::string toLower(const std::string& str);
    
    // Friend declaration for template method instantiation
    template<typename T>
    friend std::optional<T> Config::get(const std::string& key) const;
    
    template<typename T>
    friend T Config::getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    friend void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source);
}; // namespace nlm

// Explicit template instantiations for better compilation performance
/**
 * @brief Explicit template instantiations for the get() method
 * 
 * These explicit instantiations ensure that the compiler generates code
 * for these specific template types, improving compilation performance
 * and reducing binary size.
 */
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

/**
 * @brief Explicit template instantiations for the getOr() method
 * 
 * These explicit instantiations ensure that the compiler generates code
 * for these specific template types for the getOr() method.
 */
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

/**
 * @brief Explicit template instantiations for the set() method
 * 
 * These explicit instantiations ensure that the compiler generates code
 * for these specific template types for the set() method with ConfigValue.
 */
template void Config::set(const std::string&, const ConfigValue&, ConfigSource);

} // namespace nlm

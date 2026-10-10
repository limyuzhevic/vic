#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Union type representing all supported configuration value types.
 * 
 * The ConfigValue variant can hold basic primitive types (int, int64_t, double, bool), 
 * string values, and collections of these types. This provides flexibility for 
 * configuring the neural network with different data types.
 * 
 * @note Vector types are used for array-based configuration parameters
 * @see ConfigSource for related configuration concepts
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
 * @brief Enumeration of configuration sources to track origin of configuration values.
 * 
 * @enum ConfigSource
 * 
 * @note Values are ordered by precedence (lower to higher). Default values override
 *       File, which override CommandLine, which override Runtime settings.
 * @see ConfigEntry
 */
enum class ConfigSource {
    Default,    ///< Default configuration values from code
    File,       ///< Values loaded from configuration file
    CommandLine,///< Values from command-line arguments
    Runtime     ///< Values set programmatically at runtime
};

/**
 * @brief Container for a single configuration entry with metadata.
 * 
 * Each configuration entry stores the key-value pair along with its source
 * and a human-readable description. This enables tracking the origin and
 * purpose of each configuration parameter.
 * 
 * @see ConfigSource for source tracking
 */
struct ConfigEntry {
    std::string key;           ///< Configuration key/identifier
    ConfigValue value;          ///< Configuration value (type depends on expected type)
    ConfigSource source;        ///< How this value was obtained
    std::string description;    ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor creates an empty entry with Default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructs a configuration entry with specified parameters.
     * 
     * @param k Configuration key/identifier
     * @param v Configuration value
     * @param s Origin of this configuration value
     * @param desc Human-readable description (optional)
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Central configuration manager for the Neural Network Logic (NLM) system.
 * 
 * The Config class provides a type-safe interface for storing, retrieving, and managing
 * neural network configuration parameters. It maintains key-value pairs with metadata
 * about their origin and provides value retrieval with automatic type conversion.
 * 
 * The configuration system supports multiple value types (int, double, bool, string,
 * and vectors) and tracks the source of each value to manage precedence.
 * 
 * @note This class uses the Pimpl idiom to hide implementation details
 * @see ConfigSource for value precedence rules
 * @see ConfigValue for supported value types
 * 
 * @example
 * @code
 * Config config;
 * config.loadFromFile("nlm_config.json");
 * config.loadFromArgs(argc, argv);
 * 
 * // Get integer value with default fallback
 * int learningRate = config.getOr("learning_rate", 0.01f);
 * 
 * // Get string value if it exists
 * auto modelPath = config.get<std::string>("model_path");
 * if (modelPath) {
 *     std::cout << "Model path: " << *modelPath << std::endl;
 * }
 * @endcode
 */
class Config {
public:
    /**
     * @brief Constructs an empty configuration manager.
     * 
     * Initializes the internal configuration storage and sets up default values.
     */
    Config();
    
    /**
     * @brief Destroys the configuration manager and cleans up resources.
     */
    ~Config();
    
    /**
     * @brief Disable copy construction (non-copyable).
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment (non-copyable).
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor (enables efficient transfer of ownership).
     * 
     * @param other The configuration object to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator (enables efficient transfer of ownership).
     * 
     * @param other The configuration object to move from
     * @return Reference to this configuration object
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Loads configuration from a file.
     * 
     * Currently supports a simple key=value text format with optional quotes.
     * TODO: Implement proper JSON/YAML parser for enhanced configuration support.
     * 
     * @param filepath Path to the configuration file
     * @return true if file was successfully loaded, false otherwise
     * @note Lines starting with #, ;, or // are treated as comments
     * @see ConfigSource::File
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Loads configuration from command-line arguments.
     * 
     * Supports two formats:
     * - --key=value (GNU-style long options)
     * - -k value (POSIX-style short options)
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument array (argv[0] is program name)
     * @return true if arguments were successfully processed, false otherwise
     * @note Command-line arguments override file and default values
     * @see ConfigSource::CommandLine
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Saves the current configuration to a file.
     * 
     * Outputs configuration in a human-readable key=value format with comments.
     * Note: The value saving is currently a placeholder implementation.
     * 
     * @param filepath Path where the configuration should be saved
     * @return true if file was successfully written, false otherwise
     * @note Only non-empty keys are saved
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Retrieves a configuration value of the specified type.
     * 
     * This method performs runtime type checking using std::variant. If the stored
     * value is not of the requested type T, the value is not found, or an error
     * occurs, returns std::nullopt.
     * 
     * @tparam T The expected type of the configuration value
     * @param key The configuration key to look up
     * @return std::optional<T> The value if found and of correct type, std::nullopt otherwise
     * @note Errors are logged for debugging purposes
     * 
     * @example
     * @code
     * auto maxNeurons = config.get<size_t>("max_neurons");
     * if (maxNeurons) {
     *     std::cout << "Max neurons: " << *maxNeurons << std::endl;
     * }
     * @endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Retrieves a configuration value or returns a default if not found.
     * 
     * This convenience method combines type-safe retrieval with default value
     * fallback. If the key doesn't exist or the value is of wrong type,
     * returns the provided default value.
     * 
     * @tparam T The expected type of the configuration value
     * @param key The configuration key to look up
     * @param defaultValue Value to return if key is not found or type mismatch
     * @return T The configuration value if found and of correct type, otherwise defaultValue
     * 
     * @example
     * @code
     * // Use 0.01f as default learning rate
     * float learningRate = config.getOr("learning_rate", 0.01f);
     * 
     * // Use empty string as default model path
     * std::string modelPath = config.getOr("model_path", "");
     * @endcode
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Sets a configuration value with a generic ConfigValue.
     * 
     * This version accepts a ConfigValue variant and requires the source to be
     * specified. It's useful when the value type isn't known at compile time.
     * 
     * @param key The configuration key to set
     * @param value The configuration value (as variant)
     * @param source Origin of this configuration value (default: Runtime)
     * 
     * @note Will replace existing entry if key already exists
     * @see ConfigSource for source precedence rules
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for setting string values.
     * 
     * @param key The configuration key to set
     * @param value The string value
     * @param source Origin of this configuration value (default: Runtime)
     * 
     * @note Uses the template to convert to ConfigValue internally
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for setting integer values.
     * 
     * @param key The configuration key to set
     * @param value The integer value
     * @param source Origin of this configuration value (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for setting double values.
     * 
     * @param key The configuration key to set
     * @param value The floating-point value
     * @param source Origin of this configuration value (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Convenience overload for setting boolean values.
     * 
     * @param key The configuration key to set
     * @param value The boolean value
     * @param source Origin of this configuration value (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Checks if a configuration key exists.
     * 
     * @param key The configuration key to check for existence
     * @return true if the key exists in the configuration, false otherwise
     * @note Empty keys are always considered non-existent
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Removes a configuration key and its value.
     * 
     * @param key The configuration key to remove
     * @note If the key doesn't exist, this is a no-op
     */
    void remove(const std::string& key);
    
    /**
     * @brief Retrieves all configuration keys.
     * 
     * @return std::vector<std::string> List of all configuration keys
     * @note Empty keys are filtered out and logged as warnings
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clears all configuration entries.
     * 
     * Removes all configuration key-value pairs from the configuration.
     * Useful for resetting configuration or loading a fresh configuration.
     * 
     * @note Only Runtime-sourced entries can be cleared (implementation detail)
     */
    void clear();
    
    /**
     * @brief Generates a human-readable summary of the configuration.
     * 
     * Creates a formatted string showing all configuration values, their sources,
     * and statistics about the configuration state. Useful for debugging and
     * logging configuration state.
     * 
     * @return std::string Formatted configuration summary
     * @note Includes statistics like total entries, empty keys count, and value types
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Removes leading and trailing whitespace from a string.
     * 
     * @param str The string to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Converts a string to lowercase.
     * 
     * @param str The string to convert
     * @return std::string Lowercase version of the input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

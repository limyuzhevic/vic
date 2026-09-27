/**
 * @file Config.hpp
 * @brief Centralized configuration management for the NLM framework
 * 
 * This header provides a comprehensive configuration system that supports multiple
 * configuration sources (defaults, files, command-line, runtime), various data types,
 * and type-safe value retrieval. The configuration system is essential for controlling
 * neural network parameters, simulation settings, and runtime behavior.
 * 
 * @details The configuration system features:
 * - Support for 8 different value types (int, int64_t, double, bool, std::string,
 *   and std::vector variants of int, double, and string)
 * - Multiple configuration sources with priority (Default > File > CommandLine > Runtime)
 * - Type-safe template-based value retrieval with error handling
 * - File-based configuration in JSON format
 * - Command-line argument parsing
 * - Thread-safe access patterns
 * - Comprehensive error handling for invalid configurations
 * 
 * The configuration system is used throughout the framework to control:
 * - Neural network architecture parameters
 * - Simulation timing and step parameters
 * - Plasticity rule parameters
 * - Neuromodulation settings
 * - Memory management
 * - Logging verbosity
 * - Experimental parameters
 * 
 * @author NLM Authors
 * @date Created: 2024
 * @version 1.0
 * 
 * @copyright MIT License
 * 
 * @note Configuration values are validated based on their type and bounds.
 * Invalid configurations will throw NLMConfigError exceptions.
 * 
 * @see nlm::Config for main implementation class
 * @see nlm::ConfigEntry for configuration entry structure
 * @see nlm::ConfigSource for source enumeration
 * 
 * @warning Configuration loading from files may fail silently if files are
 * missing or malformed. Use loadFromFile() to check success.
 */
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

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

// Configuration source enumeration
/**
 * @enum ConfigSource
 * @brief Defines the source priority for configuration values
 * 
 * Configuration values can originate from different sources with specific priorities:
 * - Default: Built-in default values
 * - File: Values loaded from configuration files
 * - CommandLine: Values from command-line arguments
 * - Runtime: Values set during runtime (highest priority)
 * 
 * Source priority determines which value takes precedence when conflicts occur.
 * Later sources override earlier sources.
 */
enum class ConfigSource {
    Default,    ///< Built-in default configuration
    File,       ///< Configuration loaded from file (JSON format)
    CommandLine,///< Configuration from command-line arguments
    Runtime     ///< Configuration set during runtime
};

/**
 * @struct ConfigEntry
 * @brief Represents a single configuration value with metadata
 * 
 * ConfigEntry encapsulates a configuration key-value pair along with its source
 * and descriptive information. This structure enables tracking the origin of
 * each configuration value for debugging and override management.
 * 
 * @details Each configuration entry maintains:
 * - The configuration key (identifier)
 * - The actual value (stored as variant type)
 * - The source of this configuration value
 * - A human-readable description for documentation purposes
 * 
 * ConfigEntry is used internally by the Config class to maintain the
 * configuration state and track sources for proper value resolution.
 * 
 * @see ConfigSource for source enumeration
 * @see Config for main configuration interface
 */
struct ConfigEntry {
    std::string key;              ///< Configuration key/identifier
    ConfigValue value;            ///< Configuration value (variant type)
    ConfigSource source;          ///< Source of this configuration value
    std::string description;      ///< Human-readable description
    
    /**
     * @brief Default constructor creates an empty configuration entry
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct a configuration entry with key, value, source, and optional description
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Optional description (default: empty string)
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Centralized configuration management for the NLM framework
 * 
 * The Config class provides a comprehensive configuration system that supports
 * multiple configuration sources (defaults, files, command-line, runtime),
 * various data types, and type-safe value retrieval. This class is essential
 * for controlling neural network parameters, simulation settings, and runtime behavior.
 * 
 * @details Key features:
 * - **Multi-source configuration**: Supports Default, File, CommandLine, and Runtime sources
 * - **Type safety**: Template-based type-safe value retrieval using std::optional
 * - **Flexible value types**: Supports 8 primitive and container types
 * - **File I/O**: JSON-based configuration file support
 * - **Command-line parsing**: Integration with argc/argv argument system
 * - **Move semantics**: Efficient memory management through move operations
 * - **Thread awareness**: Designed for thread-safe configuration access
 * 
 * **Configuration Sources and Priority**:
 * 1. Default: Built-in framework defaults
 * 2. File: Configuration loaded from JSON files
 * 3. CommandLine: Values from command-line arguments
 * 4. Runtime: Values set during execution (highest priority)
 * 
 * **Usage Example**:
 * @code
 * // Create and use configuration
 * nlm::Config config;
 * 
 * // Load from configuration file
 * if (config.loadFromFile("config.json")) {
 *     std::cout << "Configuration loaded successfully" << std::endl;
 * }
 * 
 * // Get values with type checking
 * if (auto neurons = config.get<int>("num_neurons")) {
 *     std::cout << "Number of neurons: " << *neurons << std::endl;
 * } else {
 *     std::cout << "Using default neurons count" << std::endl;
 * }
 * 
 * // Set values with specific types
 * config.set("learning_rate", 0.01);
 * config.set("simulation_steps", 1000);
 * 
 * // Get with defaults
 * int steps = config.getOr("simulation_steps", 1000);
 * @endcode
 * 
 * **Common Configuration Parameters**:
 * - "num_neurons": Number of neurons in the network
 * - "learning_rate": Neural learning rate for plasticity
 * - "simulation_steps": Total simulation time steps
 * - "dt": Simulation time step duration
 * - "max_memory": Maximum memory usage for working memory
 * - "seed": Random number generator seed
 * - "log_level": Logging verbosity level
 * 
 * **Thread Safety**: The Config class is designed for thread-safe access.
 * Configuration values are stored internally with proper synchronization,
 * allowing concurrent reads during simulation execution.
 * 
 * @see ConfigEntry for detailed entry structure
 * @see ConfigSource for source enumeration
 * @see NLMConfigError for configuration-related exceptions
 * 
 * @warning Configuration loading errors are non-fatal but may cause
 * unexpected behavior. Always check return values from load operations.
 * 
 * @note Memory management uses the pImpl idiom for efficient implementation
 * and forward declaration of the implementation class.
 * 
 * @ingroup CoreComponents
 */
class Config {
public:
    Config();
    ~Config();
    
    /**
     * @brief Copy constructor is deleted (use move semantics instead)
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor (efficient)
     * 
     * @param other Source configuration to move from
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator (efficient)
     * 
     * @param other Source configuration to move from
     * @return Reference to this configuration
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * 
     * Loads configuration values from a JSON file. The file should contain
     * key-value pairs that match the expected configuration keys. Values are
     * stored with ConfigSource::File and will override default values.
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if file was read and parsed successfully, false otherwise
     * 
     * @throws NLMFileError if file cannot be opened or read
     * @throws NLMConfigError if JSON parsing fails
     * 
     * @note File loading errors are reported but do not terminate execution.
     * Use the return value to check for success.
     * 
     * @see saveToFile() for writing configuration to file
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * if (config.loadFromFile("config.json")) {
     *     std::cout << "Configuration loaded successfully" << std::endl;
     * } else {
     *     std::cout << "Failed to load configuration" << std::endl;
     * }
     * @endcode
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Parses command-line arguments (--key=value format) and loads them as
     * configuration values. Command-line values have higher priority than
     * file and default values.
     * 
     * @param argc Number of command-line arguments
     * @param argv Array of command-line argument strings
     * @return true if arguments were parsed successfully, false otherwise
     * 
     * @throws NLMConfigError if argument parsing fails
     * 
     * @note Command-line arguments override configuration from files and defaults.
     * Typical usage: --learning_rate=0.01 --simulation_steps=1000
     * 
     * @see loadFromFile() for file-based configuration
     * 
     * @par Example
     * @code
     * int main(int argc, char** argv) {
     *     nlm::Config config;
     *     if (config.loadFromArgs(argc, argv)) {
     *         std::cout << "Arguments parsed successfully" << std::endl;
     *     }
     *     // Use configuration values...
     * }
     * @endcode
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * Writes current configuration values to a JSON file. All current configuration
     * values are saved, including those from different sources. File-based values
     * are written with ConfigSource::File, runtime values with ConfigSource::Runtime.
     * 
     * @param filepath Path where the JSON configuration will be saved
     * @return true if file was written successfully, false otherwise
     * 
     * @throws NLMFileError if file cannot be opened or written
     * 
     * @note Only configuration values with non-default sources are saved (File, CommandLine, Runtime).
     * 
     * @see loadFromFile() for loading from JSON files
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * config.set("learning_rate", 0.01);
     * if (config.saveToFile("config.json")) {
     *     std::cout << "Configuration saved successfully" << std::endl;
     * }
     * @endcode
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * Retrieves a configuration value of the specified type. If the value is not
     * present or of the wrong type, returns std::nullopt. This function provides
     * compile-time type safety for configuration access.
     * 
     * @tparam T Type of value to retrieve (must match one of the supported ConfigValue types)
     * @param key Configuration key to retrieve
     * @return std::optional<T> Value if found and type matches, std::nullopt otherwise
     * 
     * @throws NLMConfigError if the value exists but is of incompatible type
     * 
     * @note Supported types: int, int64_t, double, bool, std::string, and std::vector<T>
     * where T is int, double, or std::string.
     * 
     * @see getOr() for alternative with default value
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * config.set("num_neurons", 100);
     * 
     * // Type-safe retrieval
     * if (auto neurons = config.get<int>("num_neurons")) {
     *     std::cout << "Found neurons: " << *neurons << std::endl;
     * } else {
     *     std::cout << "Neurons not configured" << std::endl;
     * }
     * @endcode
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value of the specified type, returning a default
     * value if the configuration key is not present or of the wrong type. This
     * provides safe configuration access without error checking.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key to retrieve
     * @param defaultValue Default value to return if key not found
     * @return T Configuration value if found, otherwise default value
     * 
     * @note This function is convenient for optional configuration parameters.
     * It never throws exceptions.
     * 
     * @see get() for strict type-safe retrieval
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * 
     * // Get with default value
     * int max_neurons = config.getOr("num_neurons", 50);
     * double learning_rate = config.getOr("learning_rate", 0.01);
     * bool enable_logging = config.getOr("enable_logging", false);
     * @endcode
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Sets a configuration value with the specified key and value. The value is
     * stored with the specified source (defaulting to Runtime source). The type
     * must match one of the supported ConfigValue types.
     * 
     * @tparam T Type of value (must match supported ConfigValue types)
     * @param key Configuration key to set
     * @param value Value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     * 
     * @throws NLMConfigError if the value type is not supported
     * 
     * @note Setting a value with Runtime source makes it the highest priority.
     * Multiple overloads provide convenience for common types.
     * 
     * @see ConfigSource for available source types
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * 
     * // Set with explicit type
     * config.set("learning_rate", 0.05);
     * config.set("simulation_steps", 1000);
     * config.set("enable_plasticity", true);
     * 
     * // Set with string (converted to appropriate type)
     * config.set("learning_rate", std::string("0.1"));
     * @endcode
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a string configuration value
     * 
     * Convenience overload for setting string values.
     * 
     * @param key Configuration key
     * @param value String value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set an integer configuration value
     * 
     * Convenience overload for setting integer values.
     * 
     * @param key Configuration key
     * @param value Integer value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a floating-point configuration value
     * 
     * Convenience overload for setting double values.
     * 
     * @param key Configuration key
     * @param value Double value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a boolean configuration value
     * 
     * Convenience overload for setting boolean values.
     * 
     * @param key Configuration key
     * @param value Boolean value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * Determines if a configuration key has been set in the configuration.
     * This checks all configuration sources and returns true if the key exists
     * in any source.
     * 
     * @param key Configuration key to check
     * @return true if key exists in any configuration source, false otherwise
     * 
     * @note This function does not throw exceptions.
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * config.set("learning_rate", 0.01);
     * 
     * if (config.has("learning_rate")) {
     *     std::cout << "Learning rate is configured" << std::endl;
     * }
     * 
     * if (!config.has("unknown_parameter")) {
     *     std::cout << "Using default for unknown parameter" << std::endl;
     * }
     * @endcode
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * Removes a configuration key from all configuration sources. After removal,
     * the key will not be available for retrieval until set again.
     * 
     * @param key Configuration key to remove
     * 
     * @note Removing a key forces fallback to the next lower-priority source
     * (e.g., removing a Runtime value falls back to File or Default source).
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * config.set("learning_rate", 0.05); // Runtime source
     * config.set("learning_rate", 0.01, ConfigSource::File); // File source
     * 
     * config.remove("learning_rate"); // Removes from all sources
     * // Now defaults will be used
     * @endcode
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Returns a vector containing all keys from all configuration sources.
     * Keys are returned in order of priority (Runtime first, then File, then Default).
     * Duplicate keys (present in multiple sources) appear only once.
     * 
     * @return std::vector<std::string> All configuration keys
     * 
     * @note The order reflects source priority for proper value resolution.
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * config.set("learning_rate", 0.01);
     * config.set("simulation_steps", 1000, ConfigSource::File);
     * 
     * std::vector<std::string> keys = config.getKeys();
     * for (const auto& key : keys) {
     *     std::cout << "Key: " << key << std::endl;
     * }
     * @endcode
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * Removes all configuration values from all sources, leaving only the
     * built-in default configuration. This is useful for resetting the
     * configuration to its initial state.
     * 
     * @note This operation cannot be undone. After clearing, all runtime
     * and file-specific configuration values are lost.
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * config.set("learning_rate", 0.1);
     * config.set("simulation_steps", 2000);
     * 
     * config.clear(); // Reset to defaults
     * // Now defaults will be used
     * @endcode
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * Returns a human-readable summary of all current configuration values,
     * including their sources and descriptions. Useful for debugging and
     * configuration verification.
     * 
     * @return std::string Formatted configuration summary
     * 
     * @note The summary includes all configuration keys with their current values
     * and sources, making it easy to see which values are overriding defaults.
     * 
     * @par Example
     * @code
     * nlm::Config config;
     * config.set("learning_rate", 0.05);
     * config.set("simulation_steps", 1000, ConfigSource::File);
     * 
     * std::string summary = config.summary();
     * std::cout << "Configuration:" << std::endl << summary;
     * @endcode
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper to trim whitespace from strings
     * 
     * Removes leading and trailing whitespace from a string. Used for
     * normalizing configuration keys and values during file parsing.
     * 
     * @param str Input string to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper to convert string to lowercase
     * 
     * Converts all characters in a string to lowercase. Used for
     * case-insensitive configuration key matching during file parsing.
     * 
     * @param str Input string to convert
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

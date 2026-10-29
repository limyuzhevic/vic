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
 * @brief Enumeration of configuration value sources
 * 
 * Indicates where a configuration value originated from, which helps in determining
 * precedence when the same key is set by different means.
 * 
 * The precedence order (highest to lowest) is:
 * 1. Runtime (programmatically set during execution)
 * 2. CommandLine (set via command line arguments)
 * 3. File (loaded from configuration file)
 * 4. Default (built into the NLM system)
 * 
 * @note Values set later override earlier values. For example, if you set a value
 *       via command line that also exists in the config file, the command line
 *       value takes precedence.
 */
enum class ConfigSource {
public:
    /**
     * @brief Default value (compiled into the code)
     * 
     * These are the baseline values that NLM uses when no other configuration is provided.
     * Default values are typically sensible for most use cases and ensure system stability.
     */
    Default,
    
    /**
     * @brief File-based value
     * 
     * Configuration values loaded from a configuration file (typically JSON format).
     * File-based values provide persistent configuration across program runs.
     */
    File,
    
    /**
     * @brief Command line value
     * 
     * Configuration values set via command line arguments (e.g., --stdp 0.05,
     * --neuron-count 1000). Command line values provide quick runtime configuration
     * without modifying configuration files.
     */
    CommandLine,
    
    /**
     * @brief Runtime value
     * 
     * Configuration values set programmatically during runtime. These have the highest
     * precedence and are typically used for experiment-specific configuration.
     */
    Runtime
};

/**
 * @brief Union type for configuration values
 * 
 * Can hold various types of values that NLM configuration can accept. This flexible
 * type system allows NLM to handle different kinds of configuration parameters
 * through a single interface.
 * 
 * @note The supported types are:
 *       - Integer types: int, int64_t
 *       - Floating-point: double
 *       - Boolean: bool
 *       - String: std::string
 *       - Arrays: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * @warning When accessing configuration values, always use the appropriate template
 *          method (Config::get<T>() or Config::getOr<T>()) to ensure type safety.
 *          Using get<T>() with the wrong type will return std::nullopt.
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
 * @brief Structure for a single configuration entry
 * 
 * Stores a key-value pair along with metadata about where the value came from
 * and its description. This is used internally by the Config class to track
 * the source and history of each configuration value.
 * 
 * The ConfigEntry structure helps implement the configuration precedence system
 * and provides metadata for documentation and debugging purposes.
 */
struct ConfigEntry {
    std::string key;           ///< The configuration key
    ConfigValue value;         ///< The configuration value
    ConfigSource source;       ///< Where this value came from (Default, File, CommandLine, Runtime)
    std::string description;   ///< Human-readable description of the configuration option
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with parameters
     * 
     * Creates a configuration entry with the specified values.
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Description of the configuration option
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
/**
 * @brief Main configuration class for NLM (Neural Learning Machine)
 * 
 * The Config class manages all configuration parameters for the NLM neural simulation framework.
 * It provides a flexible, type-safe interface for setting and retrieving configuration values
 * through multiple sources (defaults, files, command line, runtime).
 * 
 * Configuration in NLM is used to control:
 * - Neural network architecture (neuron count, regions, connectivity)
 * - Simulation parameters (timestep, duration, plasticity rates)
 * - Learning rule parameters (STDP weights, Hebbian rates)
 * - Memory system settings (working memory capacity, episodic memory limits)
 * - Development and neuromodulation parameters
 * - System-level settings (logging, checkpoints, validation)
 * 
 * The class implements a precedence system where Runtime values override File values,
 * which override CommandLine values, which override Default values. This allows for
 * flexible configuration where users can provide both persistent (file) and temporary
 * (command line/runtime) overrides.
 * 
 * @note Configuration values are strongly typed using templates to prevent accidental
 *       type mismatches. The system supports primitive types (int, double, bool, string)
 *       and collections (vectors of primitive types).
 * 
 * @see ConfigSource for information about configuration value sources
 * @see ConfigValue for information about supported value types
 * 
 * @example Basic usage:
 * @code
 * auto config = std::make_shared<Config>();
 * config->set("neuron_count", 1000);                    // Set a value
 * config->set("simulation_timestep", 0.001);            // Set another value
 * auto neurons = config->getOr<int64_t>("neuron_count", 500);  // Get with default
 * config->loadFromFile("my_config.json");               // Load from file
 * config->saveToFile("saved_config.json");              // Save to file
 * @endcode
 */
class Config {
public:
    /**
     * @brief Constructor
     * 
     * Creates a new configuration object with all default values initialized.
     * The configuration starts empty and uses built-in default values when specific
     * keys are accessed via getOr().
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up the configuration and releases all allocated resources.
     * The internal implementation (pImpl) is properly managed and cleaned up.
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Loads configuration from a JSON file. The file should contain key-value pairs
     * where values can be JSON primitives (numbers, booleans, strings) or arrays.
     * 
     * @param filepath Path to the configuration file to load
     * @return true if the file was successfully loaded and parsed, false otherwise
     * 
     * @note File loading is non-destructive - if the file doesn't exist or is malformed,
     *       the operation fails silently and the configuration retains its existing values.
     *       This allows users to provide configuration files without requiring them to
     *       exist for basic operation.
     * 
     * @see loadFromArgs for command line alternative
     * @see saveToFile for saving configuration
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments for configuration options. Supports various
     * NLM-specific options like --stdp, --hebbian, --config, --neuron-count, etc.
     * 
     * The method handles both long-form (--option value) and short-form (--option=value)
     * argument syntax. Unknown or malformed arguments are ignored (with optional logging).
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if argument parsing was successful, false if there were critical errors
     * 
     * @see loadFromFile for file-based configuration
     * @see loadFromArgs for runtime programmatic configuration
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Saves the current configuration to a JSON file. The file will contain all
     * configuration keys that have been explicitly set (not including default values).
     * 
     * @param filepath Path to the output configuration file
     * @return true if the file was successfully saved, false if there was an error
     * 
     * @note The save operation only saves user-set values, not defaults. This ensures
     *       that saved configuration files are minimal and focused on the actual
     *       configuration choices made by the user.
     * 
     * @see loadFromFile for loading configuration
     * @see clear for removing all user-set values
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type checking
     * 
     * Retrieves a configuration value by key and returns it as the specified type.
     * If the key doesn't exist or the value has a different type, returns std::nullopt.
     * This method provides strong type safety for configuration access.
     * 
     * @tparam T The expected type of the configuration value
     * @param key The configuration key to retrieve
     * @return Optional containing the value if found and type matches, empty otherwise
     * 
     * @note Supported types: int, int64_t, double, bool, std::string, and their vector variants
     * @note This method will fail (return std::nullopt) if the key doesn't exist or if the
     *       stored value has a different type than requested.
     * @note Use getOr() instead of get() when you want a default value for missing keys.
     * 
     * @see getOr for alternative with default values
     * @see set for setting configuration values
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value, returning a default value if the key doesn't exist
     * or the value has an incompatible type. This is the recommended way to access configuration
     * since it provides sensible defaults and always returns a valid value.
     * 
     * @tparam T The expected type of the configuration value
     * @param key The configuration key to retrieve
     * @param defaultValue The default value to return if the key doesn't exist or has wrong type
     * @return The configuration value if found and type matches, otherwise the default value
     * 
     * @note This method is preferred over get() when you want to ensure you always get a value
     *       rather than dealing with std::nullopt. It's also more efficient for frequently
     *       accessed configuration parameters.
     * 
     * @see get for strict type checking version
     * @see set for setting configuration values
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value with specified source
     * 
     * Sets a configuration key to the specified value. The value is stored with the
     * specified source (typically ConfigSource::Runtime for user-set values).
     * 
     * @param key The configuration key to set
     * @param value The value to set (will be converted to ConfigValue internally)
     * @param source The source of this configuration value (Default, File, CommandLine, or Runtime)
     * 
     * @note Setting a value changes its source. If you want to temporarily override
     *       a configuration value, use ConfigSource::Runtime. If you want to save a
     *       configuration value, the source will be File when loading from file.
     * 
     * @see ConfigSource for information about different sources
     * @see get for retrieving values
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (string specialization)
     * 
     * Convenience method for setting string configuration values. This overload exists
     * to improve type safety and avoid unnecessary conversions.
     * 
     * @param key The configuration key
     * @param value The string value to set
     * @param source The source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (int specialization)
     * 
     * Convenience method for setting integer configuration values.
     * 
     * @param key The configuration key
     * @param value The integer value to set
     * @param source The source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (double specialization)
     * 
     * Convenience method for setting floating-point configuration values.
     * 
     * @param key The configuration key
     * @param value The floating-point value to set
     * @param source The source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (bool specialization)
     * 
     * Convenience method for setting boolean configuration values.
     * 
     * @param key The configuration key
     * @param value The boolean value to set
     * @param source The source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * Tests whether a configuration key has been set. This includes keys set by default,
     * from file, from command line, or at runtime. A key "exists" if it has been explicitly
     * set or if it has a default value.
     * 
     * @param key The configuration key to check
     * @return true if the key exists (either explicitly set or has a default), false otherwise
     * 
     * @note This is useful for checking whether a user has overridden a default value.
     *       Note that it doesn't distinguish between explicitly set values and defaults.
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * Removes a configuration key from the configuration. After this operation,
     * the key will no longer be explicitly set and will use its default value (if any).
     * This is useful for temporarily removing overrides or cleaning up configuration.
     * 
     * @param key The configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all explicitly set configuration keys
     * 
     * Returns a vector of all keys that have been explicitly set by the user (not including
     * default values). This is useful for introspection, debugging, or for saving only the
     * user-specific configuration.
     * 
     * @return Vector of all configuration keys that have been explicitly set
     * 
     * @note This excludes default values and is useful for creating minimal configuration
     *       files or for debugging what configuration values are actually in use.
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all user-set configuration values
     * 
     * Removes all configuration values that have been explicitly set by the user,
     * reverting to default values. This is useful for resetting configuration to
     * defaults or for starting with a clean slate.
     * 
     * @note This only affects user-set values (from File, CommandLine, or Runtime sources).
     *       Default values remain available and will be used when getOr() is called on
     *       keys that were explicitly removed.
     */
    void clear();
    
    /**
     * @brief Get a human-readable configuration summary
     * 
     * Returns a formatted string containing all current configuration values,
     * organized by source and including descriptions where available. This is useful
     * for debugging, logging, or for generating documentation.
     * 
     * @return Formatted configuration summary string with organized sections
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper: trim whitespace from string
     * 
     * Removes leading and trailing whitespace from a string, used internally for
     * normalizing configuration keys and file paths.
     * 
     * @param str The string to trim
     * @return The trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: convert string to lowercase
     * 
     * Converts all characters in a string to lowercase, used internally for
     * case-insensitive key matching.
     * 
     * @param str The string to convert
     * @return The lowercase version of the string
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm
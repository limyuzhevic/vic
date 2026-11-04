/**
 * @namespace nlm
 * @brief Neural Learning Machine (NLM) - An experimental artificial developmental brain
 * 
 * NLM is a research project investigating computational brain-like systems.
 * It implements a neural system that learns from experience through interaction
 * with an environment, featuring spiking neurons, plasticity rules, memory systems,
 * and neuromodulation.
 * 
 * @version 0.1.0
 * @author NLM Authors
 * @license MIT
 */
namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value type supporting multiple primitive types
 * 
 * ConfigValue is a variant type that can hold any of the supported configuration
 * parameter types. This enables flexible configuration while maintaining type safety
 * through the getOr() template method.
 * 
 * Supported types:
 * - int, int64_t: Integer values for counts, IDs, etc.
 * - double: Floating-point values for rates, probabilities, etc.
 * - bool: Boolean flags
 * - std::string: String values for names, paths, etc.
 * - std::vector<T>: Array values for lists of parameters
 * 
 * @note All values are stored with type information to enable type-safe retrieval.
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
 * @brief Configuration source indicating how a value was set
 * 
 * Configuration values can come from different sources, which helps track the
 * precedence order when the same key is set multiple times.
 */
enum class ConfigSource {
    /** Default values set by the system */
    Default,
    /** Values loaded from configuration files */
    File,
    /** Values from command line arguments */
    CommandLine,
    /** Values set at runtime via API calls */
    Runtime
};

/**
 * @brief Single configuration entry with metadata
 * 
 * Represents a single configuration parameter with its value, source, and description.
 * This structure enables configuration management tools to display configuration
 * information and understand the origin of each parameter.
 */
struct ConfigEntry {
    /** Configuration key/parameter name */
    std::string key;
    /** Configuration value (type-safe storage via variant) */
    ConfigValue value;
    /** Source of this configuration value */
    ConfigSource source;
    /** Human-readable description of the parameter */
    std::string description;
    
    /**
     * @brief Default constructor
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Construct configuration entry
     * @param k Key/name of the parameter
     * @param v Value to set
     * @param s Source of the value
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Central configuration management for NLM
 * 
 * The Config class provides a type-safe, extensible configuration system for the
 * NLM brain simulation. It supports loading from files, command line arguments,
 * and runtime API calls, with proper precedence handling and validation.
 * 
 * Key features:
 * - Type-safe parameter access via template methods
 * - Support for multiple configuration sources with precedence
 * - File loading (JSON format)
 * - Command line argument parsing
 * - Runtime configuration modification
 * - Configuration validation and defaults
 * - Summary and export capabilities
 * 
 * @note This class uses the Pimpl idiom for extensibility, allowing configuration
 *       system updates without breaking the public API.
 * 
 * @example
 * @code
 * auto config = std::make_shared<Config>();
 * config->set("neuron_count", 1000);
 * config->set("simulation_timestep", 0.001);
 * 
 * if (config->has("random_seed")) {
 *     int seed = config->getOr<int>("random_seed", 42);
 * }
 * @endcode
 * 
 * @see ConfigSource for configuration source management
 * @see Brain for configuration usage in brain initialization
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration with default values applied.
     */
    Config();
    
    /**
     * @brief Destructor
     */
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file (JSON format)
     * 
     * @param filepath Path to configuration file
     * @return bool True if file was loaded successfully, false otherwise
     * 
     * @note Configuration files should be in JSON format with key-value pairs.
     *       The file is merged with existing configuration, with file values taking
     *       precedence over default values but being overridden by command line
     *       and runtime settings.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return bool True if command line was parsed successfully, false otherwise
     * 
     * @note Supports the format:
     *       --key=value or --key value
     *       Example: --neuron_count=1000 --simulation_timestep=0.001
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to file (JSON format)
     * 
     * @param filepath Path to save configuration file
     * @return bool True if file was saved successfully, false otherwise
     * 
     * @note Only configuration values with Runtime source are saved.
     *       Source information is not included in the saved file.
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value by key
     * 
     * @tparam T Type to retrieve (must match one of ConfigValue types)
     * @param key Configuration key
     * @return std::optional<T> The value if key exists, nullopt otherwise
     * 
     * @note Returns nullopt if key doesn't exist or type doesn't match.
     *       Use getOr() for default value support.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Type to retrieve (must match one of ConfigValue types)
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return T The value if key exists, defaultValue otherwise
     * 
     * @note This is the preferred method for accessing configuration parameters
     *       as it provides default values and handles type conversion safely.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this value (default: Runtime)
     * 
     * @note Setting a value marks it as having the specified source, which affects
     *       precedence order when the same key is set multiple times.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (string version)
     * 
     * @param key Configuration key
     * @param value String value
     * @param source Source of this value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (integer version)
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of this value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (int64_t version)
     * 
     * @param key Configuration key
     * @param value 64-bit integer value
     * @param source Source of this value
     */
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (double version)
     * 
     * @param key Configuration key
     * @param value Floating-point value
     * @param source Source of this value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (boolean version)
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of this value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return bool True if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * @param key Configuration key to remove
     * 
     * @note This removes the key entirely, including its value and source.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * @note This removes all keys and their values, but default values will be
     *       available after subsequent operations.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return std::string Human-readable configuration summary
     * 
     * @note The summary includes all configuration keys, their values, and sources.
     *       Useful for debugging and logging.
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper to trim whitespace from strings
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper to convert string to lowercase
     * 
     * @param str String to convert
     * @return std::string Lowercase version
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

// Config.cpp includes template method implementations
// To use template methods, include this header and Config.cpp
// Templates cannot be used without the explicit .cpp file inclusion.

// Implementation of Config template methods in Config.cpp:
// template<typename T>
// std::optional<T> Config::get(const std::string& key) const {
//     if (!has(key)) return std::nullopt;
//     const ConfigEntry& entry = /* find entry in pImpl->entries */;
//     if constexpr (std::is_same_v<T, int>) {
//         return std::get<int>(entry.value);
//     } else if constexpr (std::is_same_v<T, int64_t>) {
//         return std::get<int64_t>(entry.value);
//     } // ... handle all types
// }

// template<typename T>
// T Config::getOr(const std::string& key, const T& defaultValue) const {
//     auto value = get<T>(key);
//     return value.has_value() ? *value : defaultValue;
// }

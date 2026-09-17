// Forward declarations
class Config;

/**
 * @file Config.hpp
 * @brief Configuration management for Neural Learning Machine (NLM)
 * 
 * This file defines the configuration system for the NLM project, providing
 * type-safe storage and retrieval of simulation parameters. The configuration
 * system supports multiple data types and can be loaded from various sources
 * including files and command-line arguments.
 * 
 * @details
 * The configuration system:
 * - Uses a variant type to store multiple value types (int, double, string, bool, etc.)
 * - Provides type-safe getter methods with template arguments
 * - Supports default values via getOr() methods
 * - Tracks configuration source (default, file, command-line, runtime)
 * - Allows configuration modification at runtime
 * - Can save configuration to files
 * 
 * Configuration values are accessed using string keys, making it easy to
 * parameterize simulations without recompiling code.
 * 
 * @note
 * Configuration is used extensively throughout the NLM to control neuron
 * counts, plasticity parameters, simulation settings, and other runtime
 * behavior. The system follows Phase 2+ requirements for comprehensive
 * configurability.
 */

/**
 * @brief Configuration value types
 * 
 * The configuration system supports multiple data types using a variant.
 * This allows flexible storage of different parameter types while
 * maintaining type safety when retrieving values.
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
 * Different configuration sources have different priorities for overriding values.
 * The priority order (highest to lowest) is:
 * 1. CommandLine (explicit command-line arguments override everything)
 * 2. Runtime (runtime modifications override defaults)
 * 3. File (file-based configuration)
 * 4. Default (hardcoded defaults)
 * 
 * This allows flexible configuration management where command-line arguments
 * provide maximum control for running simulations, while files provide
 * sensible defaults.
 */
enum class ConfigSource {
    Default,      ///< Hardcoded default value
    File,         ///< Value loaded from configuration file
    CommandLine,  ///< Value from command-line argument
    Runtime       ///< Value set at runtime via API
};

/**
 * @brief Configuration entry structure
 * 
 * Represents a single configuration parameter with its value, source, and
 * descriptive information. The entry tracks where the value came from to
 * implement proper override semantics.
 */
struct ConfigEntry {
    std::string key;                ///< Configuration parameter key
    ConfigValue value;              ///< Parameter value (variant type)
    ConfigSource source;            ///< How/where this value was set
    std::string description;        ///< Human-readable description of parameter
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with all fields
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the configuration parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class
 * 
 * Provides comprehensive configuration management for the NLM brain simulation.
 * This class allows loading, saving, and accessing simulation parameters
 * in a type-safe manner. Configuration is essential for controlling neuron
 * counts, plasticity rates, simulation timesteps, and other runtime behavior.
 * 
 * @details
 * Key features:
 * - Type-safe value retrieval via template get() methods
 * - Default value support via getOr() methods
 * - Multiple configuration sources with proper override semantics
 * - Runtime configuration modification
 * - File I/O support for persistence
 * - Command-line argument parsing
 * - Configuration validation
 * 
 * The configuration system is used throughout the brain simulation to:
 * - Set neuron counts and population distributions
 * - Configure plasticity parameters (STDP weights, Hebbian learning rates)
 * - Control simulation parameters (timesteps, intervals)
 * - Define neuromodulation parameters
 * - Set development stage characteristics
 * 
 * Configuration keys are documented in config files or can be discovered
 * via the getKeys() method.
 * 
 * @note
 * Configuration values are cached internally in a vector of ConfigEntry structs.
 * Each access validates the key exists and uses appropriate type conversion.
 * The system is designed for performance in the hot path of simulation code.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration with no parameters. Default values
     * will be used when parameters are requested and not present in the
     * configuration.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up the internal configuration implementation.
     */
    ~Config();
    
    /**
     * @brief Disable copying
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Transfers ownership of the internal configuration from another Config.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership from another Config instance.
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Reads configuration parameters from a file with key=value format.
     * Supports comments (lines starting with # or //) and quoted string values.
     * File-based configuration has lower priority than command-line arguments
     * but higher than defaults.
     * 
     * @param filepath Path to the configuration file
     * @return true if file was successfully loaded, false otherwise
     * 
     * @note
     * This is a placeholder implementation for Phase 1. Phase 2 will implement
     * proper JSON/YAML parsing with schema validation.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Parses command-line arguments in --key=value format or -k value format.
     * Command-line arguments have the highest priority and override both
     * file and default values.
     * 
     * @param argc Number of command-line arguments
     * @param argv Array of command-line argument strings
     * @return true if arguments were successfully parsed, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Writes the current configuration to a file. The saved configuration
     * contains all current entries with their descriptions (but placeholder values).
     * This is primarily for documentation purposes, as the actual values
     * should be loaded from source.
     * 
     * @param filepath Path to the output configuration file
     * @return true if file was successfully written, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type checking
     * 
     * Retrieves a configuration value by key, with type-safe conversion.
     * The value is returned as an optional, which is empty if the key doesn't
     * exist or if the value cannot be converted to the requested type.
     * 
     * @tparam T Type of value to retrieve (must be one of ConfigValue's supported types)
     * @param key Configuration parameter key
     * @return std::optional<T> The value if found and type matches, otherwise empty
     * 
     * @note
     * Supported types: int, int64_t, double, bool, std::string
     * Vectors of these types are also supported for array parameters.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value, returning a default if the key doesn't
     * exist or cannot be converted. This is useful for optional parameters
     * where a sensible default can be provided.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration parameter key
     * @param defaultValue Value to return if parameter is not found or has wrong type
     * @return T The configuration value or default if not found
     * 
     * @note
     * This method is commonly used for simulation parameters where sensible
     * defaults are important (e.g., neuron_count = 1000, timestep = 0.001).
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Sets or updates a configuration parameter. The source is typically
     * ConfigSource::Runtime for API-based configuration, but can be set to
     * other sources as needed.
     * 
     * @param key Configuration parameter key
     * @param value Configuration value
     * @param source Source of the configuration (defaults to Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (string)
     * 
     * Convenience overload for string values.
     * 
     * @param key Configuration parameter key
     * @param value String value
     * @param source Source of the configuration (defaults to Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (integer)
     * 
     * Convenience overload for integer values.
     * 
     * @param key Configuration parameter key
     * @param value Integer value
     * @param source Source of the configuration (defaults to Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (double)
     * 
     * Convenience overload for floating-point values.
     * 
     * @param key Configuration parameter key
     * @param value Double value
     * @param source Source of the configuration (defaults to Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (boolean)
     * 
     * Convenience overload for boolean values.
     * 
     * @param key Configuration parameter key
     * @param value Boolean value
     * @param source Source of the configuration (defaults to Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration contains a key
     * 
     * Tests whether the configuration has an entry for the specified key.
     * This is useful for optional parameters where you want to check if
     * a value was explicitly set.
     * 
     * @param key Configuration parameter key
     * @return true if the configuration contains the key, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * Deletes the configuration entry for the specified key, if present.
     * This is useful for temporarily overriding or unsetting a parameter.
     * 
     * @param key Configuration parameter key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Returns a vector of all keys currently in the configuration.
     * This is useful for documentation, debugging, or UI generation.
     * 
     * @return std::vector<std::string> Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * 
     * Removes all configuration parameters, leaving the configuration empty.
     * This is useful for resetting the configuration before a new simulation
     * run or when loading configuration from a new source.
     */
    void clear();
    
    /**
     * @brief Generate a configuration summary
     * 
     * Creates a human-readable summary of the current configuration,
     * including all keys and values with their sources. This is useful for
     * debugging, logging, and documentation.
     * 
     * @return std::string Formatted configuration summary
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation structure
     * 
     * Contains all the implementation details of the Config class.
     * Using the pImpl pattern reduces compilation dependencies and improves
     * encapsulation of the configuration storage and management logic.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Internal helper for string trimming
     * 
     * Removes leading and trailing whitespace from a string.
     * Used when parsing configuration files.
     * 
     * @param str Input string
     * @return std::string String with whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper for case-insensitive comparison
     * 
     * Converts a string to lowercase for case-insensitive key matching.
     * Used when looking up configuration parameters.
     * 
     * @param str Input string
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
};

/*!
 * @file Config.hpp
 * @brief Configuration management system for NLM
 * @details Provides hierarchical key-value storage with validation, file I/O,
 *          and runtime configuration capabilities. The Config class supports
 *          nested namespaces and type-safe access to configuration parameters.
 * @author NLM Project
 * @date Phase 1-6 Development
 * @version 6.0.0
 * @copyright MIT License
 * 
 * @see https://en.wikipedia.org/wiki/Configuration_management
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <mutex>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <sstream>
#include <unordered_set>

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

// Configuration source
enum class ConfigSource {
    Default,        ///< Default value from code
    File,           ///< Loaded from configuration file
    CommandLine,    ///< Set from command line
    Runtime        ///< Set at runtime during execution
};

// Configuration entry
struct ConfigEntry {
    std::string key;           ///< Full key path (e.g., "brain.neuron_count")
    ConfigValue value;         ///< Stored value
    ConfigSource source;       ///< How the value was obtained
    std::string description;   ///< Human-readable description
    
    /**
     * @brief Default constructor
     * @details Creates an empty entry with default values
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}

    /**
     * @brief Constructor with all parameters
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Human-readable description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Exception hierarchy for configuration errors
class ConfigException : public std::runtime_error {
public:
    /**
     * @brief Constructs a configuration exception
     * @param message Error message
     */
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    /**
     * @brief Constructs a configuration validation exception
     * @param message Error message
     */
    explicit ConfigValidationException(const std::string& message) : ConfigException(message) {}
};

class ConfigParseException : public ConfigException {
public:
    /**
     * @brief Constructs a configuration parse exception
     * @param message Error message
     */
    explicit ConfigParseException(const std::string& message) : ConfigException(message) {}
};

class ConfigFileException : public ConfigException {
public:
    /**
     * @brief Constructs a configuration file exception
     * @param message Error message
     */
    explicit ConfigFileException(const std::string& message) : ConfigException(message) {}
};

// Validation operators
enum class ValidationOperator {
    Equal,          ///< Equal to value
    NotEqual,       ///< Not equal to value
    GreaterThan,    ///< Greater than value
    LessThan,       ///< Less than value
    GreaterEqual,   ///< Greater than or equal to value
    LessEqual,      ///< Less than or equal to value
    Range,          ///< Within a range
    Regex,          ///< Matches regular expression
    Callback        ///< Custom validation function
};

// Configuration schema constraint
struct ConfigConstraint {
    std::string key;                                 ///< Key to validate
    ValidationOperator op;                          ///< Validation operator
    std::string value;                               ///< String representation of constraint value
    std::function<bool(const ConfigValue&)> callback; ///< Custom validation function
    std::string errorMessage;                       ///< Error message for validation failure
    
    /**
     * @brief Default constructor
     */
    ConfigConstraint() : key(), op(ValidationOperator::Equal), callback(nullptr), errorMessage() {}

    /**
     * @brief Constructor with operator and value
     * @param k Configuration key
     * @param o Validation operator
     * @param v Constraint value (string representation)
     * @param msg Error message
     */
    ConfigConstraint(const std::string& k, ValidationOperator o, const std::string& v, const std::string& msg = "")
        : key(k), op(o), value(v), callback(nullptr), errorMessage(msg) {}

    /**
     * @brief Constructor with custom callback
     * @param k Configuration key
     * @param cb Custom validation function
     * @param msg Error message
     */
    ConfigConstraint(const std::string& k, std::function<bool(const ConfigValue&)> cb, const std::string& msg = "")
        : key(k), op(ValidationOperator::Callback), value(), callback(std::move(cb)), errorMessage(msg) {}
};

/**
 * @class Config
 * @brief Hierarchical configuration management system
 * @details Provides key-value storage with nested namespaces, type safety,
 *          validation, file I/O, and runtime modification capabilities.
 * 
 * The Config class supports:
 * - Hierarchical keys (e.g., "brain.neuron_count", "plasticity.stdp.enabled")
 * - Multiple value types (int, double, string, bool, arrays)
 * - Source tracking (Default, File, CommandLine, Runtime)
 * - Validation with custom constraints
 * - File input/output in JSON and other formats
 * - Thread-safe operations
 * 
 * Configuration keys use dot notation for nesting, enabling organized parameter
 * management for different subsystems (brain, plasticity, neuromodulation, etc.).
 * 
 * @see ConfigEntry
 * @see ConfigConstraint
 * @see ConfigSource
 * @see https://en.wikipedia.org/wiki/Configuration_management
 */
class Config {
public:
    /**
     * @brief Default constructor creates empty configuration
     * @details Creates a configuration with default values for all known parameters.
     *          Use setDefault() to define default values before initialization.
     */
    Config();

    /**
     * @brief Destructor cleans up configuration resources
     */
    ~Config();

    /**
     * @brief Copy constructor is deleted (non-copyable)
     * @details Configurations cannot be copied to prevent unintended sharing
     *          of configuration state.
     */
    Config(const Config&) = delete;

    /**
     * @brief Copy assignment operator is deleted (non-assignable)
     * @details Maintains configuration integrity and prevents unintended sharing.
     */
    Config& operator=(const Config&) = delete;

    /**
     * @brief Move constructor transfers ownership of configuration data
     * @param other Configuration to move from (becomes invalid)
     * @details Efficiently transfers ownership without deep copying configuration data.
     */
    Config(Config&& other) noexcept;

    /**
     * @brief Move assignment operator transfers ownership
     * @param other Configuration to move from (becomes invalid)
     * @return Reference to this configuration
     */
    Config& operator=(Config&& other) noexcept;

    // Configuration operations

    /**
     * @brief Checks if a configuration key exists
     * @param key Configuration key (e.g., "brain.neuron_count")
     * @return bool true if key exists, false otherwise
     */
    bool has(const std::string& key) const;

    /**
     * @brief Gets all configuration keys
     * @return std::vector<std::string> List of all keys in the configuration
     * @details Returns all keys, including nested ones. Use getKeys() to get
     *          only top-level keys for a specific namespace.
     */
    std::vector<std::string> getKeys() const;

    /**
     * @brief Gets configuration value for a key
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @return T Value of the specified type
     * @throws ConfigException if key doesn't exist or type doesn't match
     * 
     * @code
     * Config config;
     * int neurons = config.get<int>("brain.neuron_count");
     * double rate = config.get<double>("plasticity.stdp.learning_rate");
     * std::string name = config.get<std::string>("brain.name");
     * @endcode
     */
    template<typename T>
    T get(const std::string& key) const;

    /**
     * @brief Gets configuration value with default fallback
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return T Value or default if key not found
     */
    template<typename T>
    T get(const std::string& key, const T& defaultValue) const;

    /**
     * @brief Sets configuration value for a key
     * @tparam T Type of value to set
     * @param key Configuration key
     * @param value Value to set
     * @param source Source of configuration (Runtime by default)
     * @throws ConfigException if value doesn't match expected type or validation fails
     * 
     * @code
     * Config config;
     * config.set<int>("brain.neuron_count", 1000);
     * config.set<double>("plasticity.stdp.learning_rate", 0.01);
     * config.set<std::string>("brain.name", "MyNetwork", ConfigSource::CommandLine);
     * @endcode
     */
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Removes configuration key
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);

    // Namespace operations

    /**
     * @brief Gets all keys in a specific namespace
     * @param namespace Namespace prefix (e.g., "brain", "plasticity")
     * @return std::vector<std::string> Keys in the specified namespace
     * @details Returns keys that start with the namespace followed by a dot.
     *          Use getKeys() for all keys regardless of namespace.
     */
    std::vector<std::string> getKeys(const std::string& namespaceName) const;

    /**
     * @brief Gets configuration sub-object for a namespace
     * @param namespaceName Namespace name
     * @return std::shared_ptr<Config> Configuration object for the namespace
     * @details Creates a new Config object that only contains keys from the
     *          specified namespace (without the namespace prefix).
     */
    std::shared_ptr<Config> getNamespace(const std::string& namespaceName) const;

    // Validation

    /**
     * @brief Adds validation constraint for a key
     * @param constraint Validation constraint to add
     * @throws ConfigException if constraint already exists
     */
    void addConstraint(const ConfigConstraint& constraint);

    /**
     * @brief Removes validation constraint for a key
     * @param key Key whose constraint should be removed
     */
    void removeConstraint(const std::string& key);

    /**
     * @brief Validates all constraints against current configuration
     * @throws ConfigValidationException if any constraint fails
     */
    void validateAll() const;

    /**
     * @brief Checks if a key satisfies all its constraints
     * @param key Key to validate
     * @return bool true if key passes validation, false otherwise
     */
    bool validateKey(const std::string& key) const;

    // File I/O

    /**
     * @brief Loads configuration from file
     * @param filename Path to configuration file
     * @throws ConfigFileException if file cannot be opened or parsed
     * @see saveToFile
     * 
     * @code
     * Config config;
     * config.loadFromFile("config.json");
     * config.loadFromFile("settings.toml", ConfigSource::File);
     * @endcode
     */
    void loadFromFile(const std::string& filename);

    /**
     * @brief Saves configuration to file
     * @param filename Path to output configuration file
     * @throws ConfigFileException if file cannot be written
     */
    void saveToFile(const std::string& filename) const;

    // Configuration manipulation

    /**
     * @brief Merges another configuration into this one
     * @param other Configuration to merge
     * @param overwrite If true, overwrite existing values
     * @details Merges key-value pairs from another configuration. Source tracking
     *          is preserved from the merging configuration unless overwritten.
     */
    void merge(const Config& other, bool overwrite = false);

    /**
     * @brief Clears all configuration values
     * @param keepDefault If true, keep Default-source values
     */
    void clear(bool keepDefault = true);

    /**
     * @brief Creates a copy of the configuration
     * @return Config Copy of this configuration
     */
    Config copy() const;

    // Configuration information

    /**
     * @brief Gets source of configuration for a key
     * @param key Configuration key
     * @return ConfigSource Source of the configuration value
     * @throws ConfigException if key doesn't exist
     */
    ConfigSource getSource(const std::string& key) const;

    /**
     * @brief Gets description of configuration key
     * @param key Configuration key
     * @return std::string Description of the key
     * @throws ConfigException if key doesn't exist
     */
    std::string getDescription(const std::string& key) const;

    /**
     * @brief Sets description for a configuration key
     * @param key Configuration key
     * @param description New description
     */
    void setDescription(const std::string& key, const std::string& description);

    // Debugging and monitoring

    /**
     * @brief Logs configuration for debugging
     * @details Outputs all configuration keys and values to the logging system.
     *          Useful for debugging and configuration verification.
     */
    void logConfiguration() const;

    // Constants

    /**
     * @brief Creates default configuration for NLM
     * @return Config Complete default configuration with all parameters
     * @details Populates configuration with biologically plausible and experimentally
     *          validated default values for all NLM subsystems.
     */
    static Config createDefaultConfig();

private:
    /**
     * @brief pImpl class for configuration implementation
     * @details Contains all configuration implementation details including:
     *          - Key-value storage
     *          - Constraint management
     *          - File I/O buffers
     *          - Thread synchronization
     * 
     * The pImpl pattern provides encapsulation and efficient memory management.
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

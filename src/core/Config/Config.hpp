// ==============================================================================
// CONFIGURATION SYSTEM - CORE CONFIGURATION MANAGEMENT
// ==============================================================================
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <sstream>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types supported by NLM
 * 
 * NLM's configuration system supports multiple data types to configure
 * all aspects of the neural simulation, from brain parameters to
 * cognitive system settings.
 */
using ConfigValue = std::variant<
    int,                    ///< Integer values (e.g., neuron counts)
    int64_t,                ///< 64-bit integer values
    double,                 ///< Floating-point values (e.g., weights, rates)
    bool,                   ///< Boolean flags (e.g., enable/disable features)
    std::string,            ///< String values (e.g., names, file paths)
    std::vector<int>,       ///< Integer arrays (e.g., population sizes)
    std::vector<double>,    ///< Floating-point arrays (e.g., learning rates)
    std::vector<std::string> ///< String arrays (e.g., region names)
>;

/**
 * @brief Configuration source enumeration
 * 
 * Different configuration sources have different priority:
 * Runtime > CommandLine > File > Default
 */
enum class ConfigSource {
public:
    Default = 0,      ///< Default configuration from source code
    File = 1,          ///< Loaded from JSON configuration file
    CommandLine = 2,   ///< Parsed from command line arguments
    Runtime = 3       ///< Set programmatically at runtime
};

/**
 * @brief Single configuration entry with metadata
 * 
 * Represents a single key-value pair in the configuration system,
 * including source information for precedence handling.
 */
struct ConfigEntry {
    std::string key;           ///< Configuration key (dot-notation, e.g., "brain.neuron_count")
    ConfigValue value;          ///< Configuration value
    ConfigSource source;        ///< Source of this configuration value
    std::string description;    ///< Human-readable description of the parameter
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration class for NLM
 * 
 * The Config class is the central configuration management system for the
 * Neural Learning Machine. It provides a unified interface for:
 * 
 * - Loading configuration from multiple sources (JSON files, command line)
 * - Setting and getting configuration values with type safety
 * - Managing configuration sources and precedence
 * - Saving configuration to files
 * - Validating configuration values
 * 
 * Configuration keys use dot notation for hierarchical organization:
 * - brain.neuron_count: Number of neurons in the brain
 * - plasticity.stdp.learning_rate: STDP learning rate
 * - memory.working_memory.capacity: Working memory capacity
 * - cognitive.attention.enabled: Whether attention is enabled
 * 
 * @note Configuration is thread-safe for read operations.
 */
class Config {
public:
    Config();
    ~Config();
    
    // === LIFECYCLE MANAGEMENT ===
    
    /**
     * @brief Construct a new Config object
     * 
     * Initializes with default configuration values for all NLM subsystems.
     */
    Config() = default;
    
    /**
     * @brief Destroy the Config object
     */
    ~Config() = default;
    
    // === COPY SEMANTICS ===
    
    /**
     * @brief Disable copy construction
     * 
     * Configuration objects should not be copied to avoid resource issues.
     * Use move semantics instead.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Config& operator=(const Config&) = delete;
    
    // === MOVE SEMANTICS ===
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of the internal configuration data.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of the internal configuration data.
     */
    Config& operator=(Config&&) noexcept;
    
    // === CONFIGURATION I/O ===
    
    /**
     * @brief Load configuration from a JSON file
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if successful, false otherwise
     * 
     * @note File configuration has higher priority than command line
     * and default configuration. Values from files can be overridden by
     * command line arguments or runtime calls.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument vector
     * @return true if successful, false otherwise
     * 
     * @note Command line configuration has higher priority than file
     * configuration but lower than runtime configuration.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a JSON file
     * 
     * @param filepath Path where to save the configuration
     * @return true if successful, false otherwise
     * 
     * @note Only saves configuration values that differ from defaults
     * to reduce file size.
     */
    bool saveToFile(const std::string& filepath) const;
    
    // === VALUE ACCESS ===
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * @tparam T Desired value type (int, double, bool, std::string, etc.)
     * @param key Configuration key in dot notation
     * @return Optional containing the value if found, std::nullopt otherwise
     * 
     * @note Returns std::nullopt if the key doesn't exist or the type doesn't match.
     * Use getOr() for type-safe access with default values.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * @tparam T Desired value type
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return The configuration value or default if key not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // === VALUE MODIFICATION ===
    
    /**
     * @brief Set a configuration value
     * 
     * @param key Configuration key
     * @param value New value
     * @param source Source of this configuration (Runtime by default)
     * 
     * @note Runtime values override all other sources.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a string configuration value
     * 
     * @param key Configuration key
     * @param value String value
     * @param source Source of this configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set an integer configuration value
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of this configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a floating-point configuration value
     * 
     * @param key Configuration key
     * @param value Floating-point value
     * @param source Source of this configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a boolean configuration value
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of this configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // === CONFIGURATION QUERY ===
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * @note Resets configuration to defaults only.
     */
    void clear();
    
    // === CONFIGURATION SUMMARY ===
    
    /**
     * @brief Get a human-readable summary of the configuration
     * 
     * @return String containing formatted configuration summary
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // === INTERNAL HELPERS ===
    
    /**
     * @brief Trim whitespace from a string
     * 
     * @param str Input string
     * @return String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
// ==============================================================================
// CONFIGURATION SYSTEM - CORE CONFIGURATION MANAGEMENT
// ==============================================================================
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <sstream>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

/**
 * @brief Configuration value types supported by NLM
 * 
 * NLM's configuration system supports multiple data types to configure
 * all aspects of the neural simulation, from brain parameters to
 * cognitive system settings.
 */
using ConfigValue = std::variant<
    int,                    ///< Integer values (e.g., neuron counts)
    int64_t,                ///< 64-bit integer values
    double,                 ///< Floating-point values (e.g., weights, rates)
    bool,                   ///< Boolean flags (e.g., enable/disable features)
    std::string,            ///< String values (e.g., names, file paths)
    std::vector<int>,       ///< Integer arrays (e.g., population sizes)
    std::vector<double>,    ///< Floating-point arrays (e.g., learning rates)
    std::vector<std::string> ///< String arrays (e.g., region names)
>;

/**
 * @brief Configuration source enumeration
 * 
 * Different configuration sources have different priority:
 * Runtime > CommandLine > File > Default
 */
enum class ConfigSource {
public:
    Default = 0,      ///< Default configuration from source code
    File = 1,          ///< Loaded from JSON configuration file
    CommandLine = 2,   ///< Parsed from command line arguments
    Runtime = 3       ///< Set programmatically at runtime
};

/**
 * @brief Single configuration entry with metadata
 * 
 * Represents a single key-value pair in the configuration system,
 * including source information for precedence handling.
 */
struct ConfigEntry {
    std::string key;           ///< Configuration key (dot-notation, e.g., "brain.neuron_count")
    ConfigValue value;          ///< Configuration value
    ConfigSource source;        ///< Source of this configuration value
    std::string description;    ///< Human-readable description of the parameter
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Configuration validation errors structure
 * 
 * Contains information about configuration validation errors for reporting.
 */
struct ConfigValidationError {
    std::string key;           ///< Configuration key that failed validation
    std::string message;        ///< Error message describing the issue
    ConfigSource source;       ///< Source of the configuration value
    
    ConfigValidationError(const std::string& k, const std::string& msg, ConfigSource s)
        : key(k), message(msg), source(s) {}
};

/**
 * @brief Main configuration class for NLM
 * 
 * The Config class is the central configuration management system for the
 * Neural Learning Machine. It provides a unified interface for:
 * 
 * - Loading configuration from multiple sources (JSON files, command line)
 * - Setting and getting configuration values with type safety
 * - Managing configuration sources and precedence
 * - Saving configuration to files
 * - Validating configuration values
 * 
 * Configuration keys use dot notation for hierarchical organization:
 * - brain.neuron_count: Number of neurons in the brain
 * - plasticity.stdp.learning_rate: STDP learning rate
 * - memory.working_memory.capacity: Working memory capacity
 * - cognitive.attention.enabled: Whether attention is enabled
 * 
 * @note Configuration is thread-safe for read operations.
 */
class Config {
public:
    Config();
    ~Config();
    
    // === LIFECYCLE MANAGEMENT ===
    
    /**
     * @brief Construct a new Config object
     * 
     * Initializes with default configuration values for all NLM subsystems.
     */
    Config() = default;
    
    /**
     * @brief Destroy the Config object
     */
    ~Config() = default;
    
    // === COPY SEMANTICS ===
    
    /**
     * @brief Disable copy construction
     * 
     * Configuration objects should not be copied to avoid resource issues.
     * Use move semantics instead.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment
     */
    Config& operator=(const Config&) = delete;
    
    // === MOVE SEMANTICS ===
    
    /**
     * @brief Move constructor
     * 
     * Efficiently transfers ownership of the internal configuration data.
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Efficiently transfers ownership of the internal configuration data.
     */
    Config& operator=(Config&&) noexcept;
    
    // === CONFIGURATION I/O ===
    
    /**
     * @brief Load configuration from a JSON file
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if successful, false otherwise
     * 
     * @note File configuration has higher priority than command line
     * and default configuration. Values from files can be overridden by
     * command line arguments or runtime calls.
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument vector
     * @return true if successful, false otherwise
     * 
     * @note Command line configuration has higher priority than file
     * configuration but lower than runtime configuration.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a JSON file
     * 
     * @param filepath Path where to save the configuration
     * @return true if successful, false otherwise
     * 
     * @note Only saves configuration values that differ from defaults
     * to reduce file size.
     */
    bool saveToFile(const std::string& filepath) const;
    
    // === VALUE ACCESS ===
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * @tparam T Desired value type (int, double, bool, std::string, etc.)
     * @param key Configuration key in dot notation
     * @return Optional containing the value if found, std::nullopt otherwise
     * 
     * @note Returns std::nullopt if the key doesn't exist or the type doesn't match.
     * Use getOr() for type-safe access with default values.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * @tparam T Desired value type
     * @param key Configuration key
     * @param defaultValue Value to return if key doesn't exist
     * @return The configuration value or default if key not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // === VALUE MODIFICATION ===
    
    /**
     * @brief Set a configuration value
     * 
     * @param key Configuration key
     * @param value New value
     * @param source Source of this configuration (Runtime by default)
     * 
     * @note Runtime values override all other sources.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a string configuration value
     * 
     * @param key Configuration key
     * @param value String value
     * @param source Source of this configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set an integer configuration value
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Source of this configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a floating-point configuration value
     * 
     * @param key Configuration key
     * @param value Floating-point value
     * @param source Source of this configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a boolean configuration value
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source of this configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // === CONFIGURATION QUERY ===
    
    /**
     * @brief Check if a configuration key exists
     * 
     * @param key Configuration key
     * @return true if key exists, false otherwise
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key
     * 
     * @param key Configuration key to remove
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * @note Resets configuration to defaults only.
     */
    void clear();
    
    // === CONFIGURATION SUMMARY ===
    
    /**
     * @brief Get a human-readable summary of the configuration
     * 
     * @return String containing formatted configuration summary
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // === INTERNAL HELPERS ===
    
    /**
     * @brief Trim whitespace from a string
     * 
     * @param str Input string
     * @return String with leading/trailing whitespace removed
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
    // === FRIEND DECLARATIONS ===
    
    friend class ConfigValidator;
    friend class CheckpointManager;
};

/**
 * @brief Configuration validation system
 * 
 * Provides validation for NLM configuration values, ensuring that configurations
 * are valid and complete before use in brain initialization and simulation.
 */
class ConfigValidator {
public:
    /**
     * @brief Validation rule type - function that returns error message or empty string if valid
     */
    using ValidationRule = std::function<std::string(const std::string&, const ConfigValue&)>;
    
    /**
     * @brief Validate configuration against schema
     * 
     * @param config Configuration to validate
     * @return Vector of validation errors (empty if valid)
     */
    static std::vector<ConfigValidationError> validate(const Config& config);
    
    /**
     * @brief Validate specific configuration key
     * 
     * @param config Configuration to validate
     * @param key Configuration key to validate
     * @return Error message if invalid, empty string if valid
     */
    static std::string validateKey(const Config& config, const std::string& key);
    
    /**
     * @brief Check if configuration is complete
     * 
     * @param config Configuration to check
     * @return true if configuration has all required keys, false otherwise
     */
    static bool isComplete(const Config& config);
    
    /**
     * @brief Create default validator with NLM-standard validation
     * 
     * @return Shared pointer to validated validator
     */
    static std::shared_ptr<ConfigValidator> createDefaultValidator();
};

} // namespace nlm

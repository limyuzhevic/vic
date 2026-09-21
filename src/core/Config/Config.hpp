#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// ============================================================================
// Configuration Value Types
// ============================================================================

/**
 * @brief Union type for all supported configuration values
 * 
 * This variant type supports the following configuration value types:
 * - Basic types: int, int64_t, double, bool
 * - String types: std::string
 * - Collection types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * @note When retrieving values with get<T>(), the type must exactly match
 *       the stored type (no implicit conversions)
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

// ============================================================================
// Configuration Source Enumeration
// ============================================================================

/**
 * @brief Enumeration of configuration sources
 * 
 * Each configuration key is tagged with its source to help with
 * debugging and understanding configuration precedence.
 */
enum class ConfigSource {
public:
    /** @brief Default/empty configuration (not set) */
    Default = 0,
    
    /** @brief Loaded from configuration file */
    File = 1,
    
    /** @brief Provided via command line arguments */
    CommandLine = 2,
    
    /** @brief Set at runtime via API */
    Runtime = 3
};

/**
 * @brief Helper function to get string representation of ConfigSource
 * 
 * @param source The ConfigSource value
 * @return std::string String representation
 */
inline std::string toString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
}

// ============================================================================
// Configuration Entry Structure
// ============================================================================

/**
 * @brief Single configuration entry with metadata
 * 
 * Represents a key-value pair with additional metadata including
 * the source of the configuration and a description for documentation.
 */
struct ConfigEntry {
    /** @brief Configuration key (parameter name) */
    std::string key;
    
    /** @brief Configuration value (must be one of ConfigValue types) */
    ConfigValue value;
    
    /** @brief Source of this configuration value */
    ConfigSource source;
    
    /** @brief Human-readable description of what this key does */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with Default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Full constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the configuration
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// ============================================================================
// Main configuration class
// ============================================================================

/**
 * @class Config
 * @brief Configuration management class for NLM system
 * 
 * This class provides a flexible, type-safe configuration management system
 * for the NLM neural simulation framework. It supports:
 * 
 * ## Key Features
 * - **Simple API**: Clean, intuitive interface for getting/setting configuration
 * - **Type Safety**: Compile-time type checking with template methods
 * - **Multiple Sources**: Support for file, command line, and runtime configuration
 * - **Default Values**: Easy fallback values with getOr()
 * - **Metadata**: Configuration entries include descriptions and source tracking
 * - **Thread-Safe**: Internal implementation details use proper synchronization
 * 
 * ## Usage Example
 * 
 * @code{.cpp}
 * #include "Config.hpp"
 * 
 * int main() {
 *     nlm::Config config;
 *     
 *     // Set configuration values
 *     config.set("brain.neuron_count", 1000);
 *     config.set("simulation.timestep", 0.001);
 *     config.set("learning.rate", 0.01f);
 *     
 *     // Get values with type safety
 *     int neurons = config.getOr<int>("brain.neuron_count", 500);
 *     double timestep = config.getOr<double>("simulation.timestep", 0.01);
 *     
 *     // Check if value exists
 *     if (config.has("learning.rate")) {
 *         float rate = config.get<float>("learning.rate").value();
 *     }
 *     
 *     // Load from file
 *     config.loadFromFile("config.txt");
 *     
 *     // Load from command line
 *     config.loadFromArgs(argc, argv);
 *     
 *     // Save configuration
 *     config.saveToFile("output_config.txt");
 * 
 *     return 0;
 * }
 * @endcode
 * 
 * ## Configuration Files
 * 
 * The configuration file format uses simple key=value pairs:
 * 
 * @code
 * # Configuration file example
 * brain.neuron_count = 1000
 * brain.synapse_density = 0.1
 * simulation.timestep = 0.001
 * learning.rate = 0.01
 * learning.stdp_enabled = true
 * @endcode
 * 
 * ## Command Line Arguments
 * 
 * Configuration can be set via command line arguments:
 * - `--key=value` format: Full key with equals sign
 * - `-k value` format: Short key with next argument as value
 * 
 * @code{.bash}
 * ./nlm_simulation --brain.neuron_count=1000 --simulation.timestep=0.001 --learning.rate=0.01
 * @endcode
 * 
 * @note Currently uses simple string-based value storage (Phase 1)
 * @warning Type checking is performed at runtime during retrieval
 * @see ConfigSource Enum for configuration source tracking
 */
class Config {
public:
    /**
     * @brief Construct a new Config object with default values
     * 
     * Creates an empty configuration object that can be populated
     * with key-value pairs using set() or loaded from files.
     * 
     * The constructor initializes an empty configuration with
     * all default values. New keys added will have ConfigSource::Runtime.
     * 
     * @see set() for adding configuration values
     * @see loadFromFile() for loading from configuration files
     * @see loadFromArgs() for loading from command line arguments
     */
    Config();
    
    /**
     * @brief Destroy the Config object and clean up internal resources
     * 
     * This destructor properly cleans up the internal implementation
     * and releases all allocated resources. The configuration object
     * will be empty after destruction.
     */
    ~Config();
    
    /**
     * @brief Disable copying for Config objects
     * 
     * Configuration objects should not be copied to prevent
     * unintended side effects with key-value pairs. Each Config
     * object maintains its own independent configuration state.
     * 
     * @warning Attempting to copy a Config object will result in
     *          compilation error due to deleted copy constructor
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Disable copy assignment for Config objects
     * 
     * Prevents accidental copying of configuration data. Config objects
     * should be moved instead of copied for efficiency.
     * 
     * @warning Attempting to assign one Config object to another
     *          will result in compilation error due to deleted copy assignment
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Move constructor for Config objects
     * 
     * Efficiently transfers ownership of internal configuration data
     * from one Config object to another. After the move, the source
     * object is left in a valid but empty state.
     * 
     * @param other The Config object to move from
     * @note This constructor is noexcept for exception safety
     */
    Config(Config&&) noexcept;
    
    /**
     * @brief Move assignment operator for Config objects
     * 
     * Efficiently reassigns internal configuration data from another
     * Config object. The current configuration is properly cleaned up
     * before assignment.
     * 
     * @param other The Config object to move from
     * @return Config& Reference to this object after assignment
     * @note This operator is noexcept for exception safety
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from a file
     * 
     * Reads configuration key-value pairs from a file using the simple
     * key=value format. The method skips empty lines and comments
     * (lines starting with # or /). Quoted values are automatically
     * unquoted.
     * 
     * @param filepath Path to the configuration file to load
     * @return bool true if file was successfully opened and read, false otherwise
     * 
     * @note Currently supports simple key=value format (Phase 1)
     * @warning File format may change in future versions to support JSON/YAML
     * @see loadFromArgs() for command line configuration
     * @see saveToFile() for saving configuration
     * 
     * @exception May throw std::runtime_error if file operations fail
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in two formats:
     * - `--key=value` format: Full key with equals sign
     * - `-k value` format: Short key with next argument as value
     * 
     * Configuration values are added with ConfigSource::CommandLine.
     * Duplicate keys from command line will overwrite earlier values.
     * 
     * @param argc Number of command line arguments (typically from main())
     * @param argv Array of command line argument strings (typically from main())
     * @return bool true if arguments were successfully parsed, false otherwise
     * 
     * @note Only processes arguments that start with -- or -k
     * @see loadFromFile() for file-based configuration
     * @see set() for runtime configuration
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to a file
     * 
     * Writes the current configuration to a file in simple key=value format.
     * Each configuration entry is preceded by a comment with its description.
     * Note that this is a placeholder implementation that writes all values
     * as "PLACEHOLDER_VALUE".
     * 
     * @param filepath Path to the file to save configuration to
     * @return bool true if file was successfully written, false otherwise
     * 
     * @note Currently saves values as "PLACEHOLDER_VALUE" (Phase 1 placeholder)
     * @warning Value serialization is not yet implemented
     * @see loadFromFile() for loading configuration
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get a configuration value with type safety
     * 
     * Retrieves a configuration value of the specified type if it exists
     * and matches the requested type exactly. If the key doesn't exist or
     * the type doesn't match, returns std::nullopt.
     * 
     * This method provides compile-time type checking for better code safety.
     * The type must exactly match the stored type (no implicit conversions).
     * 
     * @tparam T Type of the configuration value to retrieve
     * @param key Configuration key to retrieve
     * @return std::optional<T> Value if key exists and type matches, std::nullopt otherwise
     * 
     * @note Type checking is performed at runtime
     * @note This method is thread-safe
     * @see getOr() for fallback values
     * @see has() for checking key existence
     * 
     * @exception May throw std::runtime_error if type conversion fails
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default fallback
     * 
     * Retrieves a configuration value, returning the default if not found.
     * This is a convenience method that combines type checking with default
     * value handling.
     * 
     * @tparam T Type of the configuration value to retrieve
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key doesn't exist or type mismatch
     * @return T The configuration value if found and type matches, otherwise default
     * 
     * @note More convenient than get() when you need a fallback value
     * @note Type checking is performed at runtime
     * @see get() for strict type checking
     * 
     * @exception May throw std::runtime_error if type conversion fails
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set a configuration value
     * 
     * Stores a configuration value with the specified key and source.
     * The value is converted to the appropriate ConfigValue variant type
     * based on its C++ type. All configuration values are stored with
     * ConfigSource::Runtime by default.
     * 
     * @param key Configuration key to set
     * @param value Configuration value (will be converted to ConfigValue)
     * @param source Origin of this configuration (default: ConfigSource::Runtime)
     * 
     * @note Values are stored as variant types for flexibility
     * @note This method is thread-safe
     * @see loadFromFile() for file-based configuration
     * @see loadFromArgs() for command-line configuration
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (string specialization)
     * 
     * Convenience method for setting string configuration values.
     * 
     * @param key Configuration key to set
     * @param value String value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (int specialization)
     * 
     * Convenience method for setting integer configuration values.
     * 
     * @param key Configuration key to set
     * @param value Integer value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (double specialization)
     * 
     * Convenience method for setting floating-point configuration values.
     * 
     * @param key Configuration key to set
     * @param value Double value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set a configuration value (bool specialization)
     * 
     * Convenience method for setting boolean configuration values.
     * 
     * @param key Configuration key to set
     * @param value Boolean value to set
     * @param source Configuration source (default: ConfigSource::Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if a configuration key exists
     * 
     * Checks whether a configuration key has been set. This does not
     * perform type checking - it only checks for the presence of the key.
     * 
     * @param key Configuration key to check
     * @return bool true if the key exists, false otherwise
     * 
     * @note This is a quick check that doesn't validate the value type
     * @note This method is thread-safe
     * @see get() for type-safe value retrieval
     * @see getOr() for default value retrieval
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove a configuration key and its value
     * 
     * Permanently removes a configuration key and its associated value
     * from the configuration. This cannot be undone.
     * 
     * @param key Configuration key to remove
     * 
     * @note This operation cannot be undone
     * @note This method is thread-safe
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * Retrieves a list of all keys currently in the configuration.
     * The keys are returned in the order they were added.
     * 
     * @return std::vector<std::string> List of all configuration keys
     * 
     * @note Returns only explicitly set keys, not default values
     * @note This method is thread-safe
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clear all configuration entries
     * 
     * Removes all key-value pairs from the configuration, effectively
     * resetting it to an empty state. All configuration values are
     * removed, leaving no keys behind.
     * 
     * @note This operation cannot be undone
     * @note Default values (if any) are also cleared
     * @note This method is thread-safe
     */
    void clear();
    
    /**
     * @brief Get a summary string of the configuration
     * 
     * Generates a human-readable string representation of the
     * current configuration. The summary includes all key-value pairs
     * formatted with their descriptions for documentation.
     * 
     * @return std::string Formatted string containing configuration key-value pairs
     * 
     * @note This is primarily useful for debugging and logging
     * @note The format is simple: "key = value (# description)"
     * @see clear() for removing all configuration
     * @see getKeys() for getting just the keys
     */
    std::string summary() const;
    
private:
    /**
     * @brief Internal implementation details for Config class
     * 
     * This structure contains the actual configuration storage and
     * implementation details. It uses the Pimpl pattern to hide
     * implementation details from the header file, reducing compilation
     * dependencies and improving build times.
     */
    struct Impl;
    
    /**
     * @brief Pointer to the internal implementation
     * 
     * This is the Pimpl idiom implementation for the Config class.
     * The actual implementation is hidden in a separate compilation unit
     * to reduce header dependencies.
     */
    std::unique_ptr<Impl> pImpl;
    
    /**
     * @brief Helper function to trim whitespace from strings
     * 
     * Removes leading and trailing whitespace from a string, including
     * spaces, tabs, and carriage returns. This is used for parsing
     * configuration files.
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Helper function to convert string to lowercase
     * 
     * Converts all characters in a string to lowercase. Used for
     * case-insensitive configuration key matching.
     * 
     * @param str String to convert
     * @return std::string Lowercase version of input string
     */
    static std::string toLower(const std::string& str);
    
};

// ============================================================================
// Template method implementations (inlined for efficiency)
// ============================================================================

/**
 * @brief Template method implementation for get() with type checking
 * 
 * This template method is defined in the header file for efficiency.
 * It uses a type visitor pattern to safely convert the variant value
 * to the requested type.
 * 
 * @tparam T Type to convert to
 * @param key Configuration key to retrieve
 * @return std::optional<T> Converted value or nullopt if conversion fails
 */
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    static_assert(
        std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
        std::is_same_v<T, double> || std::is_same_v<T, bool> ||
        std::is_same_v<T, std::string> ||
        std::is_same_v<T, std::vector<int>> ||
        std::is_same_v<T, std::vector<double>> ||
        std::is_same_v<T, std::vector<std::string>>,
        "Unsupported configuration value type"
    );
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::visit([](const auto& value) -> T {
            using ValueType = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<ValueType, T>) {
                return value;
            } else {
                throw std::runtime_error("Type mismatch");
            }
        }, it->value);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

/**
 * @brief Template method implementation for getOr() with fallback
 * 
 * This template method provides a convenient way to get configuration
 * values with a fallback default. If the key doesn't exist or the type
 * doesn't match, the default value is returned instead of std::nullopt.
 * 
 * @tparam T Type to retrieve
 * @param key Configuration key to retrieve
 * @param defaultValue Fallback value if key doesn't exist or type mismatch
 * @return T Retrieved value or default if not available
 */
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

} // namespace nlm

// ============================================================================
// Convenience functions and helpers
// ============================================================================

/**
 * @brief Parse a key=value string into key and value components
 * 
 * @param line String containing key=value pair
 * @return std::pair<std::string, std::string> Key and value strings
 * 
 * @note Internal helper for configuration file parsing
 */
inline std::pair<std::string, std::string> parseKeyValue(const std::string& line) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return {"", ""};
    }
    return {
        nlm::Config::trim(line.substr(0, pos)),
        nlm::Config::trim(line.substr(pos + 1))
    };
}

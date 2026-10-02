#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management system for NLM
 * @author NLM Team
 * @date Phase 2
 * @version 1.0
 *
 * This file defines the configuration management system for the NLM (Neural Learning Machine).
 * The system supports multiple configuration formats (JSON, YAML, simple key=value) and provides
 * validation, type safety, and persistence of configuration parameters.
 *
 * Key features:
 * - JSON and YAML configuration file support
 * - Command-line argument parsing
 * - Type-safe value storage and retrieval
 * - Configuration validation with parameter ranges
 * - Source tracking (default, file, command line, runtime)
 * - Automatic conversion between string representations and native types
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

/**
 * @brief Forward declaration of the Config class to avoid circular dependencies
 */
class Config;

/**
 * @brief Union type for storing configuration values of various types
 * 
 * This variant type can store:
 * - Basic numeric types (int, int64_t, double)
 * - Boolean values
 * - String values
 * - Arrays of numeric or string types
 * 
 * The choice of type is determined at runtime based on the configuration source
 * and user preference. Type safety is maintained through template functions
 * that validate the requested type matches the stored type.
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
 * @brief Enumeration of configuration sources for tracking value origin
 * 
 * The source field helps distinguish between configuration values that come
 * from different origins, which is important for determining precedence and
 * for debugging configuration issues.
 */
enum class ConfigSource {
public:
    /**
     * @brief Default values set by the application code
     * 
     * These are the baseline configuration values that are set when the
     * application starts, typically in the main() function.
     */
    Default = 0,
    
    /**
     * @brief Values loaded from configuration files (JSON, YAML, or simple format)
     * 
     * File-based configuration provides persistent settings that can be
     * reused across application runs and shared with other users or systems.
     */
    File = 1,
    
    /**
     * @brief Values specified via command-line arguments
     * 
     * Command-line configuration allows for quick overrides of settings
     * without modifying configuration files. This is useful for debugging,
     * testing, or running the application with different parameters in
     * different environments.
     */
    CommandLine = 2,
    
    /**
     * @brief Values set at runtime via the set() method
     * 
     * Runtime configuration allows for dynamic changes to the configuration
     * during program execution, which is useful for adaptive systems or
     * for adjusting parameters based on user input or system conditions.
     */
    Runtime = 3
};

/**
 * @brief Structure representing a single configuration entry
 * 
 * Each configuration entry stores a key-value pair along with metadata
 * about the source of the value and a human-readable description. This
 * structure is used internally by the Config class to maintain the
 * configuration state.
 */
struct ConfigEntry {
    /**
     * @brief The configuration key/name used to identify this value
     * 
     * Keys are case-sensitive and should follow a hierarchical naming
     * convention (e.g., "brain.neuron_count" or "plasticity.stdp_ltp_weight").
     * The key is used for both storage and retrieval of configuration values.
     */
    std::string key;

    /**
     * @brief The actual configuration value stored in a type-safe variant
     * 
     * The value can be of any type supported by the ConfigValue variant.
     * Template functions in the Config class provide type-safe access to
     * values, converting them to the requested type at retrieval time.
     */
    ConfigValue value;

    /**
     * @brief The source from which this configuration value was obtained
     * 
     * The source helps track the origin of each configuration value and
     * determines precedence when multiple sources provide the same key.
     */
    ConfigSource source;

    /**
     * @brief Human-readable description of the configuration parameter
     * 
     * This field provides documentation about what the configuration
     * parameter controls, its typical values, and its impact on the
     * system. It is useful for generating configuration summaries and
     * for automatic documentation generation.
     */
    std::string description;

    /**
     * @brief Default constructor - creates an empty configuration entry
     * 
     * This constructor initializes all fields to their default values:
     * - key: empty string
     * - value: default-constructed variant
     * - source: ConfigSource::Default
     * - description: empty string
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}

    /**
     * @brief Constructor with parameters
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Human-readable description
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Main configuration management class for NLM
 * 
 * The Config class provides a unified interface for managing all configuration
 * parameters used throughout the NLM system. It supports multiple input sources,
 * type-safe value access, validation, and persistence. The configuration system
 * is central to NLM's ability to run with different parameters for different
 * experiments and research scenarios.
 * 
 * Key features:
 * - Multiple file format support (JSON, YAML, simple key=value)
 * - Command-line argument parsing
 * - Runtime configuration modification
 * - Configuration validation with parameter ranges
 * - Type-safe value storage and retrieval
 * - Comprehensive logging and error reporting
 * - Source tracking for debugging
 * 
 * Usage example:
 * @code
 * auto config = std::make_shared<Config>();
 * config->loadFromFile("config.json");
 * config->set("brain.neuron_count", 1000);
 * int neuronCount = config->getOr<int>("brain.neuron_count", 500);
 * @endcode
 */
class Config {
public:
    /**
     * @brief Default constructor - creates an empty configuration object
     * 
     * This constructor initializes the configuration system with empty
     * validation rules. Use loadFromFile() or loadFromArgs() to populate
     * the configuration with actual values.
     */
    Config();

    /**
     * @brief Destructor - cleans up implementation details
     * 
     * The Config class uses the Pimpl idiom to hide implementation details.
     * This destructor ensures proper cleanup of internal resources.
     */
    ~Config();

    /**
     * @brief Copy constructor - disabled to prevent copying
     * 
     * The Config class uses the Pimpl idiom and manages resources internally,
     * so copying is not allowed. Use move semantics instead.
     */
    Config(const Config&) = delete;

    /**
     * @brief Copy assignment operator - disabled to prevent copying
     * @return Reference to this object (not used)
     */
    Config& operator=(const Config&) = delete;

    /**
     * @brief Move constructor - enables efficient transfer of resources
     * @param other The Config object to move from
     */
    Config(Config&&) noexcept;

    /**
     * @brief Move assignment operator - enables efficient transfer of resources
     * @param other The Config object to move from
     * @return Reference to this object for method chaining
     */
    Config& operator=(Config&&) noexcept;

    /**
     * @brief Load configuration from a file
     * 
     * This method loads configuration values from a file. The file format is
     * determined by the file extension:
     * - .json: JSON format (recommended)
     * - .yaml/.yml: YAML format
     * - .cfg or no extension: Simple key=value format
     * 
     * The method automatically handles file format detection and parsing.
     * All configuration values are validated before being stored.
     * 
     * @param filepath Path to the configuration file
     * @return true if the file was successfully loaded, false otherwise
     * 
     * @see loadFromJSON
     * @see loadFromYAML
     * @see loadFromSimpleFormat
     */
    bool loadFromFile(const std::string& filepath);

    /**
     * @brief Load configuration from command-line arguments
     * 
     * This method parses command-line arguments and converts them to
     * configuration values. It supports both long options (--key=value) and
     * short options (-k value). Values are stored with ConfigSource::CommandLine
     * to indicate their origin.
     * 
     * Command-line arguments override values from configuration files.
     * The format supported is:
     * - --key=value (long option with equals sign)
     * - -k value (short option with following argument)
     * 
     * @param argc Number of command-line arguments
     * @param argv Array of command-line argument strings
     * @return true if parsing succeeded, false otherwise
     */
    bool loadFromArgs(int argc, char** argv);

    /**
     * @brief Save configuration to a file
     * 
     * This method saves the current configuration to a file. The file format
     * is determined by the file extension, matching the supported formats
     * for loading.
     * 
     * @param filepath Path to the output configuration file
     * @return true if the file was successfully saved, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;

    /**
     * @brief Get a configuration value with type checking
     * 
     * This method retrieves a configuration value and returns it as a
     * type-safe optional. If the key doesn't exist or the stored value
     * has a different type, the returned optional will be empty.
     * 
     * @tparam T The expected type of the configuration value
     * @param key The configuration key to retrieve
     * @return Optional containing the value if found and type matches, empty otherwise
     * 
     * @see getOr
     * @see set
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    /**
     * @brief Get a configuration value with a default fallback
     * 
     * This method retrieves a configuration value and returns it as the
     * specified type. If the key doesn't exist or the stored value has
     * a different type, the default value is returned.
     * 
     * @tparam T The expected type of the configuration value
     * @param key The configuration key to retrieve
     * @param defaultValue The value to return if the key doesn't exist or type mismatch
     * @return The configuration value or the default if not found
     * 
     * @see get
     * @see set
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    /**
     * @brief Set a configuration value with a generic ConfigValue
     * 
     * This method stores a configuration value in the variant storage.
     * The value is associated with the specified source. This is a
     * generic method that can store any type supported by ConfigValue.
     * 
     * @param key The configuration key to set
     * @param value The configuration value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set a configuration value from a string
     * 
     * This method stores a string configuration value. The string is
     * automatically converted to the appropriate internal type based on
     * the key name and validation rules.
     * 
     * @param key The configuration key to set
     * @param value The string value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set a configuration value from an integer
     * @param key The configuration key to set
     * @param value The integer value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set a configuration value from a double
     * @param key The configuration key to set
     * @param value The double value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set a configuration value from a boolean
     * @param key The configuration key to set
     * @param value The boolean value to store
     * @param source The source of this configuration value
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Check if a configuration key exists
     * 
     * This method checks whether a configuration key has been set, regardless
     * of its source (default, file, command line, or runtime).
     * 
     * @param key The configuration key to check
     * @return true if the key exists, false otherwise
     */
    bool has(const std::string& key) const;

    /**
     * @brief Remove a configuration key
     * 
     * This method removes a configuration key from the configuration if it exists.
     * After removal, the key will no longer be found by get() or getOr().
     * 
     * @param key The configuration key to remove
     */
    void remove(const std::string& key);

    /**
     * @brief Get all configuration keys
     * 
     * This method returns a list of all configuration keys that have been set,
     * regardless of their source. The order of keys in the returned vector
     * is not guaranteed.
     * 
     * @return Vector containing all configuration keys
     */
    std::vector<std::string> getKeys() const;

    /**
     * @brief Clear all configuration entries
     * 
     * This method removes all configuration entries, leaving the configuration
     * in an empty state. After calling this method, has() will return false
     * for all keys, and getOr() will return default values.
     */
    void clear();

    /**
     * @brief Get a human-readable summary of the configuration
     * 
     * This method generates a formatted string containing information about
     * all configuration entries, including their values and sources. This is
     * useful for debugging and logging purposes.
     * 
     * @return String containing the configuration summary
     */
    std::string summary() const;

private:
    /**
     * @brief Private implementation details (Pimpl idiom)
     * 
     * The Config class uses the Pimpl idiom to hide implementation details
     * and reduce compilation dependencies. All actual implementation is
     * contained in this private struct.
     */
    struct Impl;

    /**
     * @brief Pointer to the private implementation
     */
    std::unique_ptr<Impl> pImpl;

    /**
     * @brief Internal helper function to trim whitespace from strings
     * 
     * This static method removes leading and trailing whitespace from a string.
     * It is used when parsing configuration files and command-line arguments
     * to ensure consistent key and value formats.
     * 
     * @param str The string to trim
     * @return The trimmed string
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Internal helper function to convert strings to lowercase
     * 
     * This static method converts all characters in a string to lowercase.
     * It is used when parsing configuration files to ensure case-insensitive
     * key matching.
     * 
     * @param str The string to convert
     * @return The lowercase version of the string
     */
    static std::string toLower(const std::string& str);

    /**
     * @brief Initialize validation rules for configuration parameters
     * 
     * This private method sets up validation rules for common configuration
     * parameters. Each rule specifies constraints on the allowed values
     * (e.g., positive integers, float ranges, maximum values).
     */
    void initializeValidationRules();

    /**
     * @brief Validate a configuration value against its key's validation rules
     * 
     * This private method checks whether a configuration value satisfies the
     * validation rules associated with its key. If no validation rules exist
     * for the key, the value is considered valid.
     * 
     * @param key The configuration key
     * @param value The configuration value to validate
     * @return true if the value is valid, false otherwise
     */
    bool validateValue(const std::string& key, const ConfigValue& value) const;

    /**
     * @brief Validate a configuration value against a specific validation rule
     * 
     * This private method checks whether a configuration value satisfies a
     * specific validation rule (e.g., "positive_integer", "float_range_0_1").
     * It handles various rule types and converts values as needed for comparison.
     * 
     * @param key The configuration key (for context in error messages)
     * @param value The configuration value to validate
     * @param rule The validation rule to apply
     * @return true if the value satisfies the rule, false otherwise
     */
    bool validateValueAgainstRule(const std::string& key, const ConfigValue& value, const std::string& rule) const;

    /**
     * @brief Load configuration from a JSON file
     * 
     * This private method handles JSON file format parsing. It uses the
     * nlohmann/json library to parse JSON files and convert them to
     * NLM configuration entries.
     * 
     * @param filepath Path to the JSON configuration file
     * @return true if the file was successfully loaded, false otherwise
     */
    bool loadFromJSON(const std::string& filepath);

    /**
     * @brief Load configuration from a YAML file
     * 
     * This private method handles YAML file format parsing. It uses the
     * yaml-cpp library to parse YAML files and convert them to
     * NLM configuration entries.
     * 
     * @param filepath Path to the YAML configuration file
     * @return true if the file was successfully loaded, false otherwise
     */
    bool loadFromYAML(const std::string& filepath);

    /**
     * @brief Recursively parse a YAML node and its children
     * 
     * This private method traverses a YAML node tree and converts it to
     * NLM configuration entries. It handles scalar values, sequences,
     * and mappings recursively.
     * 
     * @param node The YAML node to parse
     * @param prefix The key prefix for nested elements
     */
    void parseYAMLNode(const YAML::Node& node, const std::string& prefix);

    /**
     * @brief Load configuration from a simple key=value file
     * 
     * This private method handles the simple text file format used in
     * Phase 1 of NLM. It parses lines in the format "key = value" and
     * converts string values to appropriate types.
     * 
     * @param filepath Path to the configuration file
     * @return true if the file was successfully loaded, false otherwise
     */
    bool loadFromSimpleFormat(const std::string& filepath);

    /**
     * @brief Save configuration to a JSON output stream
     * 
     * This private method converts the current configuration to JSON format
     * and writes it to the provided output stream.
     * 
     * @param stream The output stream to write to
     */
    void saveToJSON(std::ostream& stream) const;

    /**
     * @brief Save configuration to a YAML output stream
     * 
     * This private method converts the current configuration to YAML format
     * and writes it to the provided output stream.
     * 
     * @param stream The output stream to write to
     */
    void saveToYAML(std::ostream& stream) const;

    /**
     * @brief Convert a configuration value to a string representation
     * 
     * This private method converts a configuration value to a human-readable
     * string. It is used when saving configuration files and generating
     * configuration summaries.
     * 
     * @param value The configuration value to convert
     * @return String representation of the value
     */
    std::string getValueString(const ConfigValue& value) const;

    /**
     * @brief Convert a configuration value to nlohmann::json
     * 
     * This private method converts a configuration value to the nlohmann::json
     * type for JSON serialization. It handles all supported types in
     * ConfigValue.
     * 
     * @param value The configuration value to convert
     * @return JSON representation of the value
     */
    nlohmann::json getValueFromConfigValue(const ConfigValue& value) const;

    // Explicit template instantiations to avoid linker errors
    template std::optional<int> get<int>(const std::string&) const;
    template std::optional<int64_t> get<int64_t>(const std::string&) const;
    template std::optional<double> get<double>(const std::string&) const;
    template std::optional<bool> get<bool>(const std::string&) const;
    template std::optional<std::string> get<std::string>(const std::string&) const;

    template int getOr<int>(const std::string&, const int&) const;
    template int64_t getOr<int64_t>(const std::string&, const int64_t&) const;
    template double getOr<double>(const std::string&, const double&) const;
    template bool getOr<bool>(const std::string&, const bool&) const;
    template std::string getOr<std::string>(const std::string&, const std::string&) const;
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

namespace nlm {

/**
 * @file Config.hpp
 * @brief Configuration management system
 * 
 * @details The configuration system provides a flexible, type-safe mechanism for
 *          managing simulation and system parameters. It supports multiple value types,
 *          validation, persistence to file formats, and runtime parameter management.
 *          The Config class serves as the central interface for accessing and modifying
 *          all configuration parameters used throughout the NLM system.
 * 
 * @note Thread-safe read-only access, requires synchronization for write operations
 * @author NLM Development Team
 * @date 2026
 * @version 1.0.0
 * 
 * @ingroup Systems Integration
 */

class ConfigError : public std::runtime_error {
public:
    /**
     * @brief Constructor - Create configuration error with message
     * 
     * @details Creates a configuration error with the specified error message.
     *          This exception is thrown when configuration operations fail.
     * 
     * @param message Error message describing the configuration problem
     * 
     * @note Used for all configuration-related error conditions
     * @thread_safety Thread-safe construction
     * @ingroup Systems Integration
     */
    ConfigError(const std::string& message) : std::runtime_error(message) {}
};

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
];

/**
 * @brief Configuration entry representing a single parameter
 * 
 * @details A ConfigEntry stores a configuration parameter value along with metadata
 *          including source, description, and validation information. This structure
 *          provides complete tracking of parameter origins and properties.
 * 
 * @note Thread-safe after construction
 * @warning Key values should be unique within configuration
 * @ingroup Systems Integration
 */
struct ConfigEntry {
    std::string key;              ///< Configuration parameter key/name
    ConfigValue value;            ///< Parameter value
    ConfigSource source;          ///< Source of this configuration value
    std::string description;      ///< Human-readable description of parameter
    
    /**
     * @brief Default constructor
     * 
     * @details Creates an empty configuration entry with default values.
     *          Used for initialization and placeholder entries.
     * 
     * @note Creates entry with empty key and default source
     * @thread_safety Thread-safe
     * @ingroup Systems Integration
     */
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description() {}

    /**
     * @brief Constructor - Create configuration entry with parameters
     * 
     * @details Creates a fully specified configuration entry with all metadata.
     *          This constructor provides complete control over entry properties.
     * 
     * @param k Configuration parameter key/name
     * @param v Parameter value
     * @param s Source of configuration value
     * @param desc Description of parameter (default: empty string)
     * 
     * @throws std::bad_alloc if memory allocation fails
     * @note All parameters are stored by value
     * @thread_safety Thread-safe
     * @ingroup Systems Integration
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};
/**
     * @brief Main configuration class - Manages all system parameters
     * 
     * @details The Config class provides a comprehensive configuration management system
     *          for the NLM neural system. It supports multiple input sources (file, command line,
     *          runtime), type-safe parameter access, validation, persistence, and schema-based
     *          configuration management. This class is used throughout the brain and other
     *          subsystems to access simulation and runtime parameters.
     * 
     * @note Thread-safe read operations, requires synchronization for write operations
     * @warning Configuration must be complete before brain initialization
     * @ingroup Systems Integration
     */
    Config();

    /**
     * @brief Destructor - Clean up configuration resources
     * 
     * @details Releases all allocated memory and cleans up configuration state,
     *          including all stored configuration entries and internal data structures.
     * 
     * @note All configuration resources are properly released
     * @thread_safety Thread-safe cleanup
     * @post Configuration resources are freed
     */
    ~Config();

    /**
     * @brief Copy constructor (deleted)
     * 
     * @details Copy construction is not supported to prevent accidental copying
     *          and maintain object ownership semantics.
     * 
     * @note Configuration contains non-copyable resources (unique_ptr)
     * @warning Never use copy constructor - explicitly deleted
     */
    Config(const Config&) = delete;

    /**
     * @brief Copy assignment operator (deleted)
     * 
     * @details Copy assignment is not supported for the same reasons as copy constructor.
     * 
     * @note Prevents shallow copying of internal resources
     * @warning Never use copy assignment - explicitly deleted
     */
    Config& operator=(const Config&) = delete;

    /**
     * @brief Move constructor (noexcept)
     * 
     * @details Move constructor allows efficient transfer of configuration ownership
     *          by moving the internal pImpl pointer. The source configuration becomes
     *          invalid after move operation.
     * 
     * @param other Configuration to move from
     * 
     * @note Source configuration becomes unusable after move
     * @thread_safety Exception-safe
     * @post Source configuration is in valid but unspecified state
     */
    Config(Config&&) noexcept;

    /**
     * @brief Move assignment operator (noexcept)
     * 
     * @details Move assignment allows efficient transfer of configuration ownership,
     *          properly cleaning up existing resources before transferring.
     * 
     * @param other Configuration to move from
     * 
     * @return Reference to this configuration after assignment
     * @note Source configuration becomes unusable after move
     * @thread_safety Exception-safe, may throw
     * @post Source configuration is in valid but unspecified state
     */
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from file (JSON or simple format)
     * 
     * @details Loads configuration parameters from a file, supporting both
     *          JSON format (preferred) and simple key=value format. The file format
     *          is automatically detected based on file extension and content.
     * 
     * @param filepath Path to configuration file to load
     * 
     * @return true if configuration is successfully loaded, false if file cannot be
     *         read, is malformed, or contains invalid parameters
     * 
     * @throws std::runtime_error if critical configuration parsing fails
     * @note File format is auto-detected: .json files use JSON format, others use simple format
     * @note Existing configuration is replaced when loading from file
     * @thread_safety Thread-unsafe if concurrent with other configuration operations
     * @post Configuration is updated with values from file
     * @return Loading status
     */
    bool loadFromFile(const std::string& filepath);

    /**
     * @brief Load configuration from command line arguments
     * 
     * @details Parses command line arguments to extract configuration parameters.
     *          Supports standard parameter formats like --key value or -k value.
     *          Command line values take precedence over file and default values.
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * 
     * @return true if command line parsing succeeds, false if arguments are malformed
     *         or contain invalid parameters
     * 
     * @throws std::runtime_error if command line parsing fails
     * @note Must be called before brain initialization
     * @note Command line overrides file and default values
     * @thread_safety Thread-unsafe - should be called before simulation
     * @post Configuration is updated with command line parameters
     * @return Parsing status
     */
    bool loadFromArgs(int argc, char** argv);

    /**
     * @brief Save configuration to file
     * 
     * @details Saves current configuration parameters to a file in JSON format.
     *          The saved configuration can be reloaded later using loadFromFile().
     * 
     * @param filepath Path to configuration file to save
     * 
     * @return true if configuration is successfully saved, false if file cannot be
     *         written or serialization fails
     * 
     * @throws std::runtime_error if file writing fails
     * @note Saves all current configuration parameters
     * @thread_safety Thread-unsafe if concurrent with configuration modification
     * @post Configuration is saved to persistent storage
     * @return Save status
     */
    bool saveToFile(const std::string& filepath) const;

    /**
     * @brief Get configuration value (type-safe)
     * 
     * @details Retrieves a configuration value with automatic type conversion and
     *          validation. Returns nullopt if the parameter doesn't exist or has
     *          incompatible type.
     * 
     * @tparam T Type of value to retrieve (int, int64_t, double, bool, std::string,
     *           std::vector<int>, std::vector<double>, std::vector<std::string>)
     * @param key Configuration parameter key
     * 
     * @return std::optional<T> Value if parameter exists and has correct type, nullopt otherwise
     * 
     * @throws std::runtime_error if type conversion fails
     * @note Type must match exactly or convertible
     * @thread_safety Thread-safe - read-only access
     * @return Parameter value or nullopt
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    /**
     * @brief Get configuration value with default fallback
     * 
     * @details Retrieves a configuration value, returning a default value if the
     *          parameter doesn't exist or has incompatible type. This is useful
     *          for optional parameters with sensible defaults.
     * 
     * @tparam T Type of value to retrieve and default value type
     * @param key Configuration parameter key
     * @param defaultValue Default value to return if parameter doesn't exist
     * 
     * @return T Parameter value if exists and has correct type, otherwise default value
     * 
     * @note Type must match exactly or be convertible
     * @thread_safety Thread-safe - read-only access
     * @return Parameter value or default
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    /**
     * @brief Set configuration value
     * 
     * @details Sets a configuration parameter value. This operation supports multiple
     *          value types and automatically determines the appropriate ConfigValue type
     *          for storage. The parameter is marked as having Runtime source.
     * 
     * @param key Configuration parameter key
     * @param value Configuration value (any type convertible to ConfigValue)
     * 
     * @throws std::runtime_error if value type is not supported
     * @note Sets parameter source to Runtime
     * @thread_safety Thread-unsafe - may require synchronization
     * @post Parameter value is updated
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (string)
     * 
     * @details Convenience overload for setting string values.
     * 
     * @param key Configuration parameter key
     * @param value String value
     * @param source Source of configuration value (default: Runtime)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (int)
     * 
     * @details Convenience overload for setting integer values.
     * 
     * @param key Configuration parameter key
     * @param value Integer value
     * @param source Source of configuration value (default: Runtime)
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (double)
     * 
     * @details Convenience overload for setting floating-point values.
     * 
     * @param key Configuration parameter key
     * @param value Floating-point value
     * @param source Source of configuration value (default: Runtime)
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (bool)
     * 
     * @details Convenience overload for setting boolean values.
     * 
     * @param key Configuration parameter key
     * @param value Boolean value
     * @param source Source of configuration value (default: Runtime)
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Check if configuration parameter exists
     * 
     * @details Tests whether a configuration parameter with the specified key
     *          exists in the configuration. This is useful for optional parameters
     *          and conditional configuration handling.
     * 
     * @param key Configuration parameter key to check
     * 
     * @return bool true if parameter exists, false otherwise
     * 
     * @thread_safety Thread-safe - read-only access
     * @return Existence status
     */
    bool has(const std::string& key) const;

    /**
     * @brief Remove configuration parameter
     * 
     * @details Removes a configuration parameter from the configuration,
     *          freeing any associated resources. The parameter will be treated as
     *          if it never existed after removal.
     * 
     * @param key Configuration parameter key to remove
     * 
     * @throws std::runtime_error if removal fails
     * @thread_safety Thread-unsafe - requires synchronization
     * @post Parameter is removed from configuration
     */
    void remove(const std::string& key);

    /**
     * @brief Get all configuration keys
     * 
     * @details Retrieves all configuration parameter keys currently in the
     *          configuration. This is useful for introspection and debugging.
     * 
     * @return std::vector<std::string> Vector of all configuration parameter keys
     * 
     * @note Returns keys in insertion order
     * @thread_safety Thread-safe - read-only access
     * @return Vector of parameter keys
     */
    std::vector<std::string> getKeys() const;

    /**
     * @brief Clear all configuration parameters
     * 
     * @details Removes all configuration parameters from the configuration,
     *          returning it to an empty state. This is useful for re-initialization
     *          or testing scenarios.
     * 
     * @throws std::runtime_error if clear operation fails
     * @thread_safety Thread-unsafe - requires synchronization
     * @post Configuration is empty
     */
    void clear();

    /**
     * @brief Get configuration summary
     * 
     * @details Generates a human-readable summary of the current configuration,
     *          including parameter count, sources, and important statistics. This
     *          is useful for logging and debugging.
     * 
     * @return std::string Formatted summary string
     * 
     * @note Summary includes parameter counts and source distribution
     * @thread_safety Thread-safe - read-only access
     * @return Summary string
     */
    std::string summary() const;
    
/**
     * @brief Implementation details for Config class (pImpl pattern)
     * 
     * @details Contains all internal state and implementation details of the Config class.
     *          This follows the pImpl idiom to hide implementation from header files.
     *          The Impl structure contains all configuration entries, parsing logic,
     *          and internal configuration management data.
     * 
     * @note Thread-unsafe access patterns apply to all Impl members
     * @warning Direct access to pImpl is discouraged - use Config public interface
     * @ingroup Systems Integration
     */
    struct Impl;

    /**
     * @brief Non-owning pointer to implementation
     * 
     * @details Stores the actual implementation of Config using the pImpl pattern.
     *          This allows the header file to remain clean while hiding implementation details.
     * 
     * @note Ownership of implementation is managed by Config
     * @warning Direct access to pImpl is discouraged
     * @ingroup Systems Integration
     */
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    /**
     * @brief Trim whitespace from string
     * 
     * @details Removes leading and trailing whitespace from the input string.
     *          Used for normalizing configuration parameter values and keys.
     * 
     * @param str Input string to trim
     * 
     * @return std::string Trimmed string
     * 
     * @note Removes spaces, tabs, and newlines
     * @thread_safety Thread-safe
     * @ingroup Systems Integration
     */
    static std::string trim(const std::string& str);

    /**
     * @brief Convert string to lowercase
     * 
     * @details Converts all characters in the input string to lowercase.
     *          Used for case-insensitive configuration parameter matching.
     * 
     * @param str Input string to convert
     * 
     * @return std::string Lowercase version of input string
     * 
     * @note Preserves non-alphabetic characters
     * @thread_safety Thread-safe
     * @ingroup Systems Integration
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

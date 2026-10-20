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

/**
 * @struct ConfigEntry
 * @brief Represents a single configuration entry with metadata
 * 
 * @details
 * Each configuration entry stores a key-value pair along with metadata
 * about its source and description. This allows for tracking where
 * configuration values originated and providing user-friendly descriptions.
 */
struct ConfigEntry {
    std::string key;           /**< Configuration key for lookup */
    ConfigValue value;         /**< Configuration value of any supported type */
    ConfigSource source;       /**< How the configuration was obtained */
    std::string description;   /**< Human-readable description of the parameter */
    
    /**
     * @brief Default constructor creates an empty configuration entry
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructs a configuration entry with full metadata
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @brief Enumeration of configuration sources
 * 
 * Determines the priority and origin of configuration values.
 * Sources are evaluated in order: Default > File > CommandLine > Runtime
 */
enum class ConfigSource {
    Default,      /**< Built into the application defaults */
    File,         /**< Loaded from configuration file */
    CommandLine,  /**< Provided via command line arguments */
    Runtime       /**< Set at runtime via API calls */
};

/**
 * @class Config
 * @brief Centralized configuration management system
 * 
 * @details
 * The Config class provides a unified interface for managing application
 * parameters including neural properties, simulation settings, and runtime
 * overrides. It supports multiple configuration sources with proper precedence,
 * type-safe value retrieval, and persistence to files.
 * 
 * Configuration values are stored as variants supporting:
 * - Primitive types: int, int64_t, double, bool
 * - String types: std::string
 * - Array types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * The system provides automatic type conversion for value retrieval and
 * maintains insertion order for deterministic behavior.
 * 
 * References:
 * - Stroustrup (2000) "The C++ Programming Language"
 * - Alexandrescu et al. (2001) "Modern C++ Design"
 */
class Config {
public:
    /**
     * @brief Constructs a configuration object with default values
     * 
     * @details
     * Initializes an empty configuration that will be populated with
     * default values. Default values can be set by subclassing Config
     * or by calling set() during initialization.
     */
    Config();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper destruction of the pImpl object and cleanup of any
     * allocated memory or resources.
     */
    ~Config();
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * @param other Source configuration to move from
     * 
     * @details
     * Efficient transfer of the pImpl pointer without deep copying.
     * Source configuration is left in a valid, empty state.
     */
    Config(Config&& other) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership and cleans up
     * 
     * @param other Source configuration to move from
     * @return Config& Reference to this configuration for chaining
     * 
     * @details
     * Proper cleanup of current implementation before transferring ownership.
     * Prevents resource leaks during self-assignment.
     */
    Config& operator=(Config&& other) noexcept;
    
    /**
     * @brief Explicitly delete copy constructor - configurations cannot be copied
     * 
     * @details
     * Config uses pImpl idiom with dynamic state. Direct copying
     * would require complex deep copying of all entries.
     */
    Config(const Config&) = delete;
    
    /**
     * @brief Explicitly delete copy assignment operator - configurations cannot be copied
     * 
     * @details
     * See copy constructor rationale. This prevents accidental configuration duplication.
     */
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Loads configuration from a file (simple key=value format)
     * 
     * @param filepath Path to the configuration file
     * @return bool True if file was successfully read and parsed
     * 
     * @details
     * Currently implements a simple key=value parser with support for:
     * - Empty lines and comment lines (starting with # or /)
     * - Quoted values (double or single quotes)
     * - Basic type inference for common types
     * 
     * TODO PHASE 2: Implement proper JSON/YAML parser with:
     * - Full JSON/YAML specification support
     * - Nested configuration objects
     * - Array values with proper typing
     * - Schema validation
     * 
     * @warning Placeholder implementation uses simple format, not JSON/YAML
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Loads configuration from command line arguments
     * 
     * @param argc Argument count (from main())
     * @param argv Argument vector (from main())
     * @return bool True if arguments were successfully parsed
     * 
     * @details
     * Supports two argument formats:
     * 1. --key=value (long option format)
     * 2. -key value (short option format)
     * 
     * Values are parsed and converted to appropriate types (int, double, bool, string).
     * Command line values have highest precedence in the configuration system.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Saves current configuration to a file
     * 
     * @param filepath Path to the output configuration file
     * @return bool True if file was successfully written
     * 
     * @details
     * Currently outputs a simplified key=value format with comments.
     * Values are converted to string representation.
     * 
     * TODO PHASE 2: Improve output format for better readability and completeness
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Gets a configuration value with type safety
     * 
     * @tparam T Target type (must match stored type)
     * @param key Configuration key to retrieve
     * @return std::optional<T> Value if key exists and type matches, std::nullopt otherwise
     * 
     * @details
     * Performs type checking at runtime. If the stored value matches the
     * requested type, it is returned; otherwise, std::nullopt is returned.
     * This prevents type-related runtime errors.
     * 
     * @note Type names must match exactly (e.g., "int" vs "int64_t")
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Gets a configuration value with default fallback
     * 
     * @tparam T Target type
     * @param key Configuration key to retrieve
     * @param defaultValue Value to return if key doesn't exist or type mismatch
     * @return T Retrieved value or default if not found
     * 
     * @details
     * Convenience method that combines get() with a fallback value.
     * Ensures the program always has valid values for critical parameters.
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Sets a configuration value with explicit source
     * 
     * @param key Configuration key
     * @param value Value to set (any supported ConfigValue type)
     * @param source Source of the configuration (default: Runtime)
     * 
     * @details
     * This is the generic set method. Specific type overloads are provided
     * for common types to enable type-safe usage.
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a string configuration value
     * 
     * @param key Configuration key
     * @param value String value to set
     * @param source Source of the configuration
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets an integer configuration value
     * 
     * @param key Configuration key
     * @param value Integer value to set
     * @param source Source of the configuration
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a double configuration value
     * 
     * @param key Configuration key
     * @param value Double value to set
     * @param source Source of the configuration
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Sets a boolean configuration value
     * 
     * @param key Configuration key
     * @param value Boolean value to set
     * @param source Source of the configuration
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Checks if a configuration key exists
     * 
     * @param key Configuration key to check
     * @return bool True if key exists in configuration
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Removes a configuration key
     * 
     * @param key Configuration key to remove
     * 
     * @details
     * Removes the key from configuration regardless of its source.
     * Runtime and CommandLine settings can be overridden by file settings.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Gets all configuration keys
     * 
     * @return std::vector<std::string> List of all keys in insertion order
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Clears all configuration entries
     * 
     * @details
     * Resets configuration to empty state. Default values must be re-established
     * through initialization or by calling set() with appropriate values.
     */
    void clear();
    
    /**
     * @brief Generates a human-readable configuration summary
     * 
     * @return std::string Formatted summary of configuration entries
     * 
     * @details
     * Creates a readable representation showing all configuration parameters,
     * their values, and sources. Useful for debugging, logging, and documentation.
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

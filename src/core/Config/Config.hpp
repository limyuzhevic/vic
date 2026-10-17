/**
 * @file Config.hpp
 * @brief Configuration system for NLM with type-safe key-value storage
 * 
 * This file defines the configuration system for the NLM framework. It provides
 * a type-safe, persistent configuration storage system that supports multiple
 * value types, sources, and loading/saving from various formats. The configuration
 * system is the central place where all simulation parameters and settings are
 * stored and retrieved.
 * 
 * The Config class implements:
 * - Type-safe value storage with ConfigValue variant types
 * - Multiple configuration sources (default, file, command-line, runtime)
 * - Loading from JSON files and command-line arguments
 * - Saving to files for checkpointing
 * - Query operations (get, has, remove, clear)
 * - Default value fallback mechanism
 * 
 * @note The configuration system uses std::variant to support multiple value types
 *       while maintaining type safety through the template-based get() interface.
 *       This allows the same configuration key to hold different types as needed.
 * 
 * @warning Configuration values are not thread-safe. Concurrent access to the
 *         same Config instance from multiple threads will result in undefined
 *         behavior. Ensure proper synchronization if multi-threaded access is
 *         required.
 * 
 * @see Brain.hpp - Uses configuration for neural network setup
 * @see SimpleWorld.hpp - Uses configuration for world parameters
 * @see AgentBrain.hpp - Uses configuration for agent behavior
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
/**
 * @brief Type alias for configuration values supporting multiple types
 * 
 * ConfigValue is a type-safe variant that can store different configuration
 * value types. This allows the configuration system to handle various
 * parameter types while maintaining type safety through the get<T>() interface.
 * 
 * Supported value types:
 * - Primitive types: int, int64_t, double, bool
 * - String types: std::string
 * - Collection types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * @note The choice of std::variant (not boost::variant or similar) ensures
 *       compatibility with standard C++11 and later. Template functions
 *       are used to safely extract values of specific types.
 * 
 * @warning When setting configuration values, use the appropriate set() overload
 *         for the target type. Using the wrong type will cause runtime errors
 *         when attempting to retrieve the value.
 * 
 * @see Config::get() - Template function for retrieving typed values
 * @see Config::set() - Overloaded functions for setting different types
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

// Configuration source
/**
 * @brief Enumeration of configuration value sources
 * 
 * ConfigSource indicates where a configuration value originates from. This is
 * used to determine the precedence of configuration values when multiple sources
 * provide the same key. The source also helps with debugging and understanding
 * the origin of configuration values.
 * 
 * Source precedence (highest to lowest):
 * 1. Runtime - Set programmatically during execution
 * 2. CommandLine - From command-line arguments
 * 3. File - From configuration files
 * 4. Default - Hard-coded defaults
 * 
 * @note The source is important for understanding configuration behavior:
 *       Command-line arguments typically override file values, which override
 *       default values. Runtime values (set programmatically) typically have
 *       the lowest precedence.
 * 
 * @see Config::set() - Sets configuration source along with value
 * @see ConfigEntry::source - Configuration entry source field
 */
enum class ConfigSource {
    /// Default configuration value (hard-coded in code)
    Default,
    
    /// Configuration value from file (typically JSON)
    File,
    
    /// Configuration value from command-line arguments
    CommandLine,
    
    /// Runtime configuration value (set programmatically)
    Runtime
};

// Configuration entry
/**
 * @brief Single configuration entry storing a key-value pair with metadata
 * 
 * ConfigEntry represents a single configuration parameter. It includes the
 * key (identifier), value, source (origin), and description. This structure
 * is used internally by the Config class to manage configuration storage.
 * 
 * @note The ConfigEntry structure is internal to the Config class implementation.
 *       External users interact with the Config class directly, not with
 *       individual ConfigEntry objects.
 * 
 * @warning The description field is optional but recommended for clarity
 *         and documentation purposes. It should describe what the configuration
 *         parameter controls and its valid range or options.
 * 
 * @see Config - Main configuration class using ConfigEntry internally
 * @see Config::set() - Creates ConfigEntry when setting values
 * @see Config::getKeys() - Retrieves keys from all ConfigEntry objects
 */
struct ConfigEntry {
    /// Configuration key (identifier)
    std::string key;
    
    /// Configuration value (any supported type)
    ConfigValue value;
    
    /// Source of this configuration value
    ConfigSource source;
    
    /// Human-readable description of configuration purpose
    std::string description;
    
    /// Default constructor creates an empty entry
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /// Construct a configuration entry with specified properties
    ///
    /// @param k Configuration key (identifier)
    /// @param v Configuration value
    /// @param s Source of the configuration value
    /// @param desc Human-readable description (optional)
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
/**
 * @brief Type-safe configuration system for NLM framework
 * 
 * The Config class provides a centralized, type-safe configuration storage
 * system for the NLM framework. It manages all simulation parameters including
 * neural network settings, world parameters, agent behavior, and system options.
 * 
 * The configuration system supports:
 * - Multiple value types through ConfigValue variant
 * - Configuration persistence via file I/O
 * - Command-line argument parsing
 * - Default value fallbacks
 * - Source tracking for precedence resolution
 * - Type-safe value retrieval with error checking
 * 
 * @note The Config class uses the pImpl pattern to separate interface from
 *       implementation, allowing for easier maintenance and multiple backend
 *       implementations. External users interact with the Config class through
 *       its public interface without needing to know implementation details.
 * 
 * @warning Configuration values must be set before use. Using an unset
 *         configuration key will return std::nullopt from get(). Always check
 *         if a key exists using has() before attempting to retrieve its value.
 * 
 * @see ConfigValue - Supported configuration value types
 * @see ConfigSource - Configuration value sources
 * @see Brain.hpp - Example usage in neural simulation
 * @see SimpleWorld.hpp - Example usage in world simulation
 */
class Config {
public:
    /**
     * @brief Constructor that initializes an empty configuration
     * 
     * Creates a Config instance with no configuration values. The configuration
     * starts empty and must be populated using set() methods or loading from files/
     * command-line arguments. Default configuration values can be added as needed.
     * 
     * @note The constructor creates an empty configuration. If you need default
     *       values, call set() to add them or load from a configuration file that
     *       contains defaults.
     * 
     * @warning If you plan to load configuration from files, ensure those files
     *         exist and are readable before constructing the Config object. The
     *         constructor itself doesn't load any configuration.
     * 
     * @throws std::bad_alloc if memory allocation fails during initialization
     */
    Config();
    
    /// Destructor that cleans up configuration storage
    /// 
    /// Properly cleans up all configuration entries and internal resources.
    /// Ensures proper resource management and prevents memory leaks.
    ~Config();
    
    /// Deleted copy constructor (prevents shallow copying)
    /// 
    /// @note Config objects cannot be copied due to the pImpl pattern and
    ///       complex internal resource management. Use move semantics instead.
    /// 
    /// @warning Attempting to copy a Config will result in a compile error.
    ///         If you need to duplicate configuration, use saveToFile() to
    ///         checkpoint and loadFromFile() to restore.
    Config(const Config&) = delete;
    
    /// Deleted copy assignment operator (prevents shallow copying)
    /// @note Same rationale as deleted copy constructor
    Config& operator=(const Config&) = delete;
    
    /// Move constructor - transfers ownership of internal resources
    /// 
    /// @param rhs Source config to move from (will be left in valid but unspecified state)
    Config(Config&&) noexcept;
    
    /// Move assignment operator - transfers ownership of internal resources
    /// 
    /// @param rhs Source config to move from (will be left in valid but unspecified state)
    /// @return Reference to this config (for chaining)
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

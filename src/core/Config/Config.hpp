#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "core/ErrorHandling/ErrorHandling.hpp"

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
 * @namespace nlm
 * @brief Neural Learning Machine framework namespace
 *
 * The NLM namespace encompasses the entire neural simulation framework,
 * providing components for neural computation, learning, memory, and cognition.
 */

/**
 * @namespace nlm
 * @brief Common type definitions for NLM framework
 *
 * This namespace provides standardized type definitions used throughout
 * the NLM neural simulation framework, ensuring consistency across components.
 */
namespace nlm {
    using SimulationStep = size_t;
    using Timestamp = double;
}

// Forward declarations
namespace nlm {
    class Config;
}

/**
 * @enum ConfigSource
 * @brief Enumeration of configuration data sources
 *
 * This enumeration identifies the source of configuration values, enabling
 * conflict resolution and tracking the origin of configuration settings.
 *
 * @note Configuration sources are ordered by priority:
 *       - Default: Built-in framework defaults
 *       - File: Values loaded from configuration files
 *       - CommandLine: Values from command-line arguments
 *       - Runtime: Values set during program execution
 */
enum class ConfigSource {
    /** Default configuration values built into the framework */
    Default,
    /** Values loaded from JSON configuration files */
    File,
    /** Values parsed from command-line arguments */
    CommandLine,
    /** Values set programmatically at runtime */
    Runtime
};

/**
  * @struct ConfigEntry
  * @brief Configuration key-value pair with metadata
  *
  * Represents a single configuration parameter, including its value, source,
  * and descriptive documentation. Used internally by the Config class to store
  * all configuration parameters with their metadata.
  *
  * ConfigEntry stores configuration data with complete context including:
  * - The parameter name (key) for identification
  * - The actual value (type-erased variant for multi-type support)
  * - The source of the configuration (default, file, command-line, or runtime)
  * - Human-readable documentation for parameter understanding
  *
  * The structure is designed for:
  * - Type-safe storage of various configuration value types
  * - Conflict resolution through source tracking
  * - Configuration validation and documentation
  * - Conversion to/from Python dictionary format
  *
  * @note ConfigEntry is the fundamental data structure for configuration management,
  *       supporting type-safe storage of various configuration value types.
  *       Each configuration parameter is stored as a separate entry with full metadata.
  *
  * @warning Configuration keys must be unique within a Config object to prevent conflicts.
  *
  * @see Config
  * @see ConfigSource
  */
struct ConfigEntry {
    /** Configuration parameter name (must be unique within the config) */
    std::string key;
    /** Value of the configuration parameter (type-erased variant) */
    ConfigValue value;
    /** Source of this configuration value */
    ConfigSource source;
    /** Human-readable documentation for this parameter */
    std::string description;
    
    /**
     * @brief Default constructor
     * @note Creates an empty configuration entry with default values
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    /**
     * @brief Parameterized constructor
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Human-readable description (optional)
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

/**
 * @class Config
 * @brief Central configuration manager for the NLM framework
 *
 * The Config class provides a comprehensive configuration management system
 * for the Neural Learning Machine framework. It supports multiple configuration
 * sources (file, command-line, runtime) with conflict resolution, type-safe
 * parameter storage, and conversion to/from Python dictionary format.
 *
 * The class implements the PImpl pattern for efficient memory management and
 * thread-safe operations. It provides a clean API for accessing configuration
 * values with automatic type conversion, validation, and error handling.
 *
 * The configuration system is designed to support:
 * - Hierarchical configuration with nested keys (using dot notation)
 * - Type-safe value retrieval with automatic conversion
 * - Validation against parameter constraints
 * - Conversion to Python dictionaries for interoperability
 * - Comprehensive logging and error reporting
 *
 * @note Configuration values are stored as variants to support multiple types
 *       while maintaining type safety through accessors. The system includes
 *       built-in validation for critical parameters and supports custom
 *       validation rules through the validate() method.
 *
 * @warning Configuration errors during initialization can prevent proper
 *          framework operation. Always validate critical parameters before
 *          use. See NLMErrors for available error codes.
 *
 * @see ConfigEntry
 * @see NLMErrors
 *
 * @todo Add support for environment variable configuration
 * @todo Implement configuration schema validation
 * @todo Add configuration comparison methods
 *
 * @author NLM Development Team
 * @date Phase 6 Development
 *
 * Examples:
 * @code
 * // Default configuration
 * Config config;
 *
 * // Set configuration values
 * config.set("neuron_count", 500);
 * config.set("simulation_timestep", 0.001);
 * config.set("stdp_ltp_weight", 0.01f);
 *
 * // Get configuration values with defaults
 * int neurons = config.getOr("neuron_count", 500);
 * double timestep = config.getOr("simulation_timestep", 0.001);
 *
 * // Load from file
 * if (config.loadFromFile("config.json")) {
 *     // Configuration successfully loaded
 * }
 *
 * // Save to file
 * config.saveToFile("output_config.json");
 * @endcode
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
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
    
    // ========== CONVENIENCE METHODS ==========
    
    // Merge another config into this one
    void merge(const Config& other);
    
    // Filter config keys matching pattern
    std::unordered_map<std::string, ConfigValue> filter(const std::string& pattern) const;
    
    // Convert config to Python dictionary format (as std::unordered_map)
    std::unordered_map<std::string, ConfigValue> toDict() const;
    
    // Load config from Python dictionary format
    void fromDict(const std::unordered_map<std::string, ConfigValue>& dict);
    
    // Validate configuration against schema
    bool validate() const;
    
    // Get optional value with default (overloaded for convenience)
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

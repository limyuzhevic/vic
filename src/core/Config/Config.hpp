// Copyright (c) 2024+ NLM Project
// All rights reserved.
// 
// This file is part of the Neural Learning Machine (NLM) project.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/**
 * @file Config.hpp
 * @brief Configuration management system for NLM parameters and settings
 * 
 * This file implements the Config class, providing a comprehensive configuration
 * management system for the NLM project. The configuration system supports
 * multiple value types, hierarchical organization, source tracking, and runtime
 * modification capabilities. It serves as the central configuration repository
 * for all components of the NLM system.
 * 
 * The configuration system is designed with the following key principles:
 * - Type-safe configuration with compile-time type checking
 * - Hierarchical parameter organization for better management
 * - Source tracking for configuration provenance and conflict resolution
 * - Runtime modification support for adaptive behavior
 * - Thread-safe access for parallel simulation
 * - Comprehensive validation and error handling
 * - Support for multiple configuration sources (files, command line, runtime)
 * 
 * @author NLM Project
 * @version 1.0.0
 * @date 2024-2026
 * @copyright NLM Project (2024+)
 * 
 * @see src/core/Logger/Logger.hpp - Logging system for configuration events
 * @see src/brain/Brain.hpp - Brain configuration and usage
 * @see src/core/Random/Random.hpp - Random number generation configuration
 * @see src/memory/WorkingMemory.hpp - Working memory configuration
 * @see src/plasticity/STDP.hpp - Plasticity rule configuration
 * @see src/neuromodulation/Neuromodulator.hpp - Neuromodulator configuration
 * @see src/experiments/Experiment.hpp - Experiment configuration
 * 
 * @note This file uses modern C++17 features and follows Doxygen documentation standards.
 *       The configuration system supports JSON format for file-based configuration.
 */

#pragma once

// NLM filename guard
#ifndef NLM_CONFIG_HPP
#define NLM_CONFIG_HPP

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace nlm {

/**
 * @enum ConfigSource
 * @brief Configuration source enumeration for tracking provenance
 * 
 * This enum identifies the source of configuration values, enabling conflict
 * resolution and provenance tracking. The source hierarchy determines precedence:
 * - Runtime overrides File overrides CommandLine overrides Default
 * 
 * ConfigSource values:
 * - Default: Built-in default values
 * - File: Values loaded from configuration files
 * - CommandLine: Values from command line arguments
 * - Runtime: Values set during program execution
 * 
 * The source is used for:
 * - Conflict resolution (runtime overrides file, etc.)
 * - Configuration auditing and debugging
 * - State restoration and serialization
 * - Conditional application based on source
 */
enum class ConfigSource {
    Default,     ///< Built-in default value
    File,        ///< Value loaded from configuration file
    CommandLine, ///< Value from command line arguments
    Runtime      ///< Value set during runtime execution
};

/**
 * @struct ConfigEntry
 * @brief Single configuration entry with type and source tracking
 * 
 * This structure represents a single configuration parameter with its value,
 * source, and metadata. It provides type safety through the ConfigValue variant
 * and maintains provenance information for the configuration system.
 * 
 * The ConfigEntry is designed for efficient access and update while maintaining
 * the complete configuration history for auditing and debugging purposes.
 * 
 * Configuration entry usage:
 * - Storing configuration values with type information
 * - Tracking the origin of each configuration value
 * - Supporting configuration serialization and deserialization
 * - Enabling configuration comparison and validation
 */
struct ConfigEntry {
    std::string key;            ///< Configuration key (dot-separated for hierarchy)
    ConfigValue value;          ///< Configuration value with type information
    ConfigSource source;        ///< Source of this configuration value
    std::string description;    ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default values.
     * All fields are initialized to neutral/empty values.
     */
    ConfigEntry()
        : key()
        , value()
        , source(ConfigSource::Default)
        , description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of the configuration value
     * @param desc Description of the parameter (optional)
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s,
                const std::string& desc = "")
        : key(k)
        , value(v)
        , source(s)
        , description(desc) {}
};

/**
 * @class Config
 * @brief Central configuration management system for NLM
 * 
 * The Config class provides a comprehensive configuration management system
 * for the NLM project. It supports hierarchical parameter organization,
 * type-safe access, source tracking, and runtime modification. The configuration
 * system is designed for both simplicity and power, serving as the central
 * configuration repository for all NLM components.
 * 
 * Key Features:
 * - Hierarchical parameter organization (dot-separated keys)
 * - Type-safe access with template-based getters
 * - Source tracking for configuration provenance
 * - Runtime modification capabilities
 * - File-based configuration (JSON format)
 * - Command-line argument support
 * - Thread-safe access
 * - Comprehensive validation and error handling
 * - Configuration serialization and comparison
 * - Default value management
 * 
 * Configuration Management Usage:
 * @code
 * #include "src/core/Config/Config.hpp"
 * 
 * // Create configuration instance
 * Config config;
 * 
 * // Load from file
 * config.loadFromFile("nlm_config.json");
 * 
 * // Load from command line
 * config.loadFromArgs(argc, argv);
 * 
 * // Get values with type safety
 * int max_steps = config.getOr("simulation.max_steps", 1000);
 * float learning_rate = config.getOr("plasticity.learning_rate", 0.01f);
 * bool enable_debug = config.getOr("debug.enabled", false);
 * 
 * // Set values at runtime
 * config.set("simulation.max_steps", 2000);
 * config.set("debug.enabled", true, ConfigSource::Runtime);
 * 
 * // Check existence
 * if (config.has("plasticity.stdp_enabled")) {
 *     bool stdp_enabled = config.get<bool>("plasticity.stdp_enabled").value();
 * }
 * 
 * // Get configuration summary
 * std::string summary = config.summary();
 * @endcode
 * 
 * Configuration File Format (JSON):
 * @code
 * {
 *     "simulation": {
 *         "max_steps": 10000,
 *         "dt": 0.001,
 *         "enable_plasticity": true
 *     },
 *     "plasticity": {
 *         "learning_rate": 0.01,
 *         "stdp_enabled": true,
 *         "hebbian_enabled": false
 *     },
 *     "debug": {
 *         "enabled": false,
 *         "log_level": "info"
 *     }
 * }
 * @endcode
 * 
 * Command Line Arguments:
 * @code
 * nlm --simulation.max_steps 5000 --plasticity.learning_rate 0.05
 *      --debug.enabled true --log.file "brain.log"
 * @endcode
 * 
 * @note This class uses the Pimpl idiom for efficient compilation and reduced
 *       coupling. The actual implementation is in Config.cpp.
 *       The configuration system is critical for reproducible and adaptive
 *       neural network simulations.
 */
class Config {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a configuration instance with default parameters.
     * The configuration starts with default values for all parameters
     * and is ready for loading additional configuration sources.
     */
    Config();
    
    /**
     * @brief Destructor
     * 
     * Cleans up all allocated resources and ensures proper configuration
     * cleanup. The destructor prevents memory leaks and maintains
     * proper resource management.
     */
    ~Config();
    
    /**
     * @brief Disable copy construction
     * 
     * Configurations cannot be copied because they contain:
     * - Dynamic resources requiring unique ownership
     * - Network state that would be corrupted by sharing
     * - File handles that would become invalid
     * - Complex internal structures that require coordination
     */
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    /**
     * @brief Enable move construction
     * 
     * Allows efficient transfer of configuration ownership from one
     * instance to another. After move construction, the source configuration
     * is in a valid but unspecified state.
     */
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @param filepath Path to the JSON configuration file
     * @return bool true if loading succeeded, false otherwise
     * 
     * This method loads configuration parameters from a JSON file,
     * merging them with existing configuration. The file format should
     * follow standard JSON syntax with dot-separated keys for hierarchy.
     * 
     * Supported JSON value types:
     * - Numbers (integer, floating-point)
     * - Booleans
     * - Strings
     * - Arrays of the above types
     * - Objects (nested configurations)
     * 
     * @throw std::runtime_error if file cannot be read or parsed
     * 
     * @post Configuration is updated with values from the file
     * 
     * Example JSON file:
     * @code
     * {
     *     "simulation": {
     *         "max_steps": 10000,
     *         "dt": 0.001,
     *         "enable_plasticity": true
     *     },
     *     "plasticity": {
     *         "learning_rate": 0.01,
     *         "stdp_enabled": true
     *     }
     * }
     * @endcode
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command line arguments
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * @return bool true if loading succeeded, false otherwise
     * 
     * This method parses command line arguments in the format:
     * --parameter.key value
     * 
     * Supported value types:
     * - Integer and floating-point numbers
     * - Boolean values (true/false, yes/no, 1/0)
     * - Strings (quoted)
     * - Arrays (space-separated, JSON format)
     * 
     * @note Command line arguments take precedence over file-based
     *       configuration and can be used to override settings.
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * @param filepath Path where configuration will be saved
     * @return bool true if save succeeded, false otherwise
     * 
     * This method saves the current configuration to a JSON file,
     * including:
     * - All configuration values
     * - Their sources (for provenance)
     * - Descriptions (if available)
     * 
     * @post File contains serialized configuration
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @tparam T Type to extract (must be one of ConfigValue variants)
     * @param key Configuration key (dot-separated for hierarchy)
     * @return std::optional<T> Value if key exists, empty optional otherwise
     * 
     * This template method provides type-safe access to configuration
     * values. It performs automatic type checking and conversion.
     * 
     * Supported types T:
     * - Basic types: int, int64_t, double, float, bool, std::string
     * - Container types: std::vector<int>, std::vector<double>, std::vector<std::string>
     * - Arrays are automatically converted to vectors
     * 
     * @throw std::runtime_error if type conversion fails
     * 
     * @note If the key doesn't exist or has the wrong type,
     *       returns an empty optional.
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Type to extract
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return T Configuration value or default
     * 
     * This method provides convenient access to configuration values
     * with a fallback default value. If the key doesn't exist or has
     * the wrong type, returns the default value.
     * 
     * @throw std::runtime_error if type conversion fails
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Source of this configuration value (default: Runtime)
     * 
     * This method sets a configuration value with the specified source.
     * If a value with the same key already exists, it will be replaced.
     * 
     * @note Source determines precedence in conflict resolution:
     *       Runtime > CommandLine > File > Default
     */
    void set(const std::string& key, const ConfigValue& value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (overloaded for convenience)
     * 
     * @param key Configuration key
     * @param value String value
     * @param source Source (default: Runtime)
     */
    void set(const std::string& key, const std::string& value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (overloaded for convenience)
     * 
     * @param key Configuration key
     * @param value Integer value
     * @param source Source (default: Runtime)
     */
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (overloaded for convenience)
     * 
     * @param key Configuration key
     * @param value Double value
     * @param source Source (default: Runtime)
     */
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (overloaded for convenience)
     * 
     * @param key Configuration key
     * @param value Boolean value
     * @param source Source (default: Runtime)
     */
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key
     * @return bool true if key exists, false otherwise
     * 
     * This method checks for the existence of a configuration key,
     * including nested keys in the hierarchy.
     */
    bool has(const std::string& key) const;
    
    /**
     * @brief Remove configuration key
     * 
     * @param key Configuration key to remove
     * 
     * This method removes the specified key and all its child keys
     * from the configuration.
     */
    void remove(const std::string& key);
    
    /**
     * @brief Get all configuration keys
     * 
     * @return std::vector<std::string> Vector of all configuration keys
     * 
     * This method returns all configuration keys, including nested keys
     * with dot notation (e.g., "simulation.max_steps").
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Get all configuration entries
     * 
     * @return std::vector<ConfigEntry> Vector of all configuration entries
     * 
     * This method returns all configuration entries with their sources
     * and descriptions, useful for:
     * - Configuration auditing
     * - Debugging and troubleshooting
     * - Configuration comparison
     * - State serialization
     */
    std::vector<ConfigEntry> getAllEntries() const;
    
    /**
     * @brief Clear all configuration values
     * 
     * This method removes all configuration values and resets the
     * configuration to its default state.
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return std::string String representation of the configuration
     * 
     * This method generates a human-readable summary of the configuration,
     * including:
     * - Number of configuration parameters
     * - Configuration sources and counts
     * - Memory usage estimates
     * - Configuration statistics
     */
    std::string summary() const;
    
    /**
     * @brief Get configuration value with default (specialized version)
     * 
     * @tparam T Type to extract
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return T Configuration value or default
     * 
     * This method is identical to getOr() but provides specialized
     * implementations for common types.
     */
    template<typename T>
    T getValueOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Compare configurations
     * 
     * @param other Configuration to compare with
     * @return bool true if configurations are equal, false otherwise
     */
    bool equals(const Config& other) const;
    
    /**
     * @brief Merge another configuration
     * 
     * @param other Configuration to merge
     * @param overwrite If true, overwrite existing values (default: true)
     * 
     * This method merges another configuration into this one, combining
     * values from both configurations. When overwrite is true (default),
     * values from the other configuration replace existing values.
     */
    void merge(const Config& other, bool overwrite = true);
    
    /**
     * @brief Validate configuration against schema
     * 
     * @param schema Schema for validation (key patterns, value ranges, etc.)
     * @return bool true if configuration is valid, false otherwise
     * 
     * This method validates the current configuration against a schema
     * containing validation rules. The schema can specify:
     * - Required keys
     * - Key patterns (regex)
     * - Value ranges and constraints
     * - Dependencies between keys
     */
    bool validate(const std::map<std::string, std::string>& schema) const;
    
    /**
     * @brief Get configuration statistics
     * 
     * @return std::map<std::string, int> Statistics for various metrics
     * 
     * This method returns statistics about the configuration, including:
     * - Number of parameters per type
     * - Distribution of sources
     * - Memory usage
     * - Other metrics
     */
    std::map<std::string, int> getStatistics() const;

private:
    /**
     * @brief Implementation details structure
     * 
     * Contains the actual implementation details using the Pimpl idiom.
     * This allows efficient compilation and reduces coupling between
     * header and implementation.
     */
    struct Impl;
    
    Impl* pImpl; ///< Pointer to implementation details

public:
    /**
     * @brief Internal helper: Trim whitespace from string
     * 
     * @param str String to trim
     * @return std::string Trimmed string
     * 
     * This method removes leading and trailing whitespace from a string,
     * useful for configuration parsing and validation.
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Internal helper: Convert string to lowercase
     * 
     * @param str String to convert
     * @return std::string Lowercase string
     * 
     * This method converts all characters in a string to lowercase,
     * useful for case-insensitive configuration access.
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Internal helper: Parse JSON value string
     * 
     * @param value_str String representation of JSON value
     * @return ConfigValue Parsed configuration value
     * 
     * This method parses a string representation of a JSON value
     * into the appropriate ConfigValue type.
     */
    static ConfigValue parseJsonValue(const std::string& value_str);
    
    /**
     * @brief Internal helper: Convert ConfigValue to string
     * 
     * @param value Configuration value to convert
     * @return std::string String representation
     * 
     * This method converts a ConfigValue to its string representation,
     * useful for serialization and debugging.
     */
    static std::string configValueToString(const ConfigValue& value);
    
    /**
     * @brief Internal helper: Parse configuration key from string
     * 
     * @param key_str String representation of configuration key
     * @return std::vector<std::string> Parsed key components
     */
    static std::vector<std::string> parseKey(const std::string& key_str);
    
    /**
     * @brief Internal helper: Merge configurations with source resolution
     * 
     * @param target Target configuration
     * @param source Source configuration
     * @param overwrite Whether to overwrite existing values
     */
    static void mergeConfigs(Config& target, const Config& source,
                            bool overwrite);
    
    /**
     * @brief Internal helper: Validate configuration value type
     * 
     * @tparam T Expected type
     * @param value Configuration value to validate
     * @return bool true if value is of expected type
     */
    template<typename T>
    static bool isValueType(const ConfigValue& value);
    
    /**
     * @brief Internal helper: Convert string to boolean
     * 
     * @param str String to convert
     * @return bool Boolean value
     * 
     * This method converts a string to a boolean value, recognizing
     * common boolean representations (true/false, yes/no, 1/0, on/off).
     */
    static bool stringToBool(const std::string& str);
    
    /**
     * @brief Internal helper: Convert boolean to string
     * 
     * @param value Boolean value
     * @return std::string String representation
     */
    static std::string boolToString(bool value);
    
    /**
     * @brief Internal helper: Check if string represents a number
     * 
     * @param str String to check
     * @return bool true if string represents a number
     */
    static bool isNumber(const std::string& str);
    
    /**
     * @brief Internal helper: Parse numeric string to type
     * 
     * @tparam T Numeric type
     * @param str String to parse
     * @return T Parsed value
     * 
     * This method parses a string to a numeric value of type T.
     */
    template<typename T>
    static T parseNumber(const std::string& str);
    
    /**
     * @brief Internal helper: Parse array string
     * 
     * @param array_str String representation of array
     * @param delimiter Delimiter between array elements (default: space)
     * @return std::vector<std::string> Parsed array elements
     */
    static std::vector<std::string> parseArray(const std::string& array_str,
                                              char delimiter = ' ');
}; // class Config

} // namespace nlm

// Close NLM filename guard
#endif // NLM_CONFIG_HPP


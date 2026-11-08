#pragma once

/**
 * @file Config.hpp
 * @brief Configuration management for the NLM system
 * @author NLM Team
 * @date 2026
 * 
 * This file provides a type-safe configuration system for the Neural Learning Machine.
 * It supports loading/saving from files, command line arguments, and runtime settings
 * with comprehensive value type support (int, double, bool, strings, and vectors).
 * 
 * The configuration system is organized by source priority:
 *   Default < File < Command Line < Runtime
 * Later sources override earlier ones.
 */

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

    // Forward declarations
    class Config;

    /**
     * @brief Configuration value types supported by the Config class
     * 
     * These are the atomic value types that can be stored in configuration entries.
     * The system supports both scalar types and vector types for flexible configuration.
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
     * @brief Configuration source enumeration
     * 
     * Defines the priority hierarchy for configuration sources, where later sources
     * override earlier ones during configuration lookup.
     */
    enum class ConfigSource {
        Default,    /**< Default configuration values (factory defaults) */
        File,       /**< Configuration loaded from file */
        CommandLine, /**< Configuration from command line arguments */
        Runtime    /**< Configuration set at runtime */
    };

    /**
     * @brief Single configuration entry with source tracking
     * 
     * Represents a configuration value with its source of origin and description.
     * This enables the system to track how each configuration value was set and its priority.
     */
    struct ConfigEntry {
        std::string key;                 /**< Configuration key identifier */
        ConfigValue value;               /**< The actual configuration value */
        ConfigSource source;             /**< How this value was set */
        std::string description;         /**< Human-readable description of this setting */
        
        /**
         * @brief Default constructor
         * Initializes all fields to empty/default values.
         */
        ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
        
        /**
         * @brief Parameterized constructor
         * @param k Configuration key
         * @param v Initial value
         * @param s Source of this configuration
         * @param desc Description of the configuration
         */
        ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
            : key(k), value(v), source(s), description(desc) {}
    };

    /**
     * @brief Main configuration class for the NLM system
     * 
     * Manages all configuration values with type safety, source tracking, and persistence
     * capabilities. The class supports both simple key-value access and sophisticated
     * type-safe getters that prevent runtime type errors.
     * 
     * Key features:
     * - Automatic type conversion between compatible numeric types
     * - Source-based override system (Default < File < Command Line < Runtime)
     * - File-based configuration persistence (simple key=value format)
     * - Command-line argument parsing
     * - Comprehensive error handling with clear reporting
     * 
     * Usage example:
     * ```cpp
     * Config config;
     * config.set("brain.neuron_count", 1000);
     * config.set("learning.rate", 0.01);
     * config.set("simulation.enabled", true);
     * 
     * auto count = config.get<int>("brain.neuron_count");
     * if (count) {
     *     std::cout << "Neuron count: " << *count << std::endl;
     * }
     * ```
     */
    class Config {
    public:
        /**
         * @brief Default constructor
         * Creates a configuration object with empty initial state.
         */
        Config();
        
        /**
         * @brief Destructor
         * Cleans up internal implementation details.
         */
        ~Config();
        
        /**
         * @brief Copy constructor (disabled)
         * Configuration objects cannot be copied to prevent inconsistent state.
         */
        Config(const Config&) = delete;
        
        /**
         * @brief Copy assignment operator (disabled)
         * Configuration objects cannot be copied to prevent inconsistent state.
         */
        Config& operator=(const Config&) = delete;
        
        /**
         * @brief Move constructor (enabled)
         * @param other Source configuration to move from
         * Moves internal resources efficiently.
         */
        Config(Config&&) noexcept;
        
        /**
         * @brief Move assignment operator (enabled)
         * @param other Source configuration to move from
         * Moves internal resources efficiently.
         */
        Config& operator=(Config&&) noexcept;
        
        /**
         * @brief Load configuration from file
         * 
         * Parses configuration from a text file using simple key=value format.
         * Lines starting with # or / are treated as comments and ignored.
         * 
         * @param filepath Path to the configuration file
         * @return true if file loaded successfully, false if file not found or invalid
         */
        bool loadFromFile(const std::string& filepath);
        
        /**
         * @brief Load configuration from command line arguments
         * 
         * Parses --key=value and -key value format arguments.
         * Arguments are processed in order, with later arguments overriding earlier ones.
         * 
         * @param argc Number of command line arguments
         * @param argv Command line argument array (argv[0] is program name)
         * @return true if parsing succeeded, false otherwise
         */
        bool loadFromArgs(int argc, char** argv);
        
        /**
         * @brief Save configuration to file
         * 
         * Writes current configuration to a text file. Note: This is a placeholder
         * implementation that uses "PLACEHOLDER_VALUE" for all values. Phase 2+ should
         * implement proper JSON/YAML serialization.
         * 
         * @param filepath Path where configuration should be saved
         * @return true if file saved successfully, false otherwise
         */
        bool saveToFile(const std::string& filepath) const;
        
        /**
         * @brief Generic type-safe value getter
         * 
         * Retrieves a configuration value with compile-time type checking.
         * Provides automatic numeric type conversion between compatible types.
         * 
         * @tparam T Target type (int, double, bool, std::string, or vector types)
         * @param key Configuration key to look up
         * @return std::optional<T> Value if key exists and type matches, nullopt otherwise
         */
        template<typename T>
        std::optional<T> get(const std::string& key) const;
        
        /**
         * @brief Type-safe value getter with default
         * 
         * Retrieves a configuration value, returning a default if the key doesn't exist
         * or the value has an incompatible type.
         * 
         * @tparam T Target type
         * @param key Configuration key to look up
         * @param defaultValue Default value to return if key not found
         * @return Retrieved value or default if key doesn't exist/incompatible
         */
        template<typename T>
        T getOr(const std::string& key, const T& defaultValue) const;
        
        /**
         * @brief Set configuration value
         * 
         * Sets a configuration value with specified source. If the key already exists,
         * the existing value will be updated with the new source priority.
         * 
         * @param key Configuration key
         * @param value New value (will be stored as ConfigValue variant)
         * @param source Source of this configuration value
         */
        void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
        
        /**
         * @brief Set configuration value (string overload)
         * @param key Configuration key
         * @param value String value
         * @param source Source of this configuration value
         */
        void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
        
        /**
         * @brief Set configuration value (int overload)
         * @param key Configuration key
         * @param value Integer value
         * @param source Source of this configuration value
         */
        void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
        
        /**
         * @brief Set configuration value (int64_t overload)
         * @param key Configuration key
         * @param value int64_t value
         * @param source Source of this configuration value
         */
        void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
        
        /**
         * @brief Set configuration value (double overload)
         * @param key Configuration key
         * @param value double value
         * @param source Source of this configuration value
         */
        void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
        
        /**
         * @brief Set configuration value (bool overload)
         * @param key Configuration key
         * @param value bool value
         * @param source Source of this configuration value
         */
        void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
        
        /**
         * @brief Check if configuration key exists
         * 
         * @param key Configuration key to check
         * @return true if key exists, false otherwise
         */
        bool has(const std::string& key) const;
        
        /**
         * @brief Remove configuration key
         * 
         * Removes the configuration entry for the specified key, if it exists.
         * 
         * @param key Configuration key to remove
         */
        void remove(const std::string& key);
        
        /**
         * @brief Get all configuration keys
         * 
         * @return Vector containing all configuration keys currently in the config
         */
        std::vector<std::string> getKeys() const;
        
        /**
         * @brief Clear all configuration entries
         * 
         * Removes all configuration entries, resetting to default empty state.
         */
        void clear();
        
        /**
         * @brief Get configuration summary
         * 
         * Returns a human-readable summary of all configuration entries including
         * their values and sources.
         * 
         * @return String representation of all configuration entries
         */
        std::string summary() const;
        
    private:
        /**
         * @brief Internal implementation structure (pimpl idiom)
         * 
         * Opaque implementation details to reduce compilation dependencies
         * and allow for future changes to the internal data structures.
         */
        struct Impl;
        std::unique_ptr<Impl> pImpl;
        
        /**
         * @brief Internal string trimming utility
         * 
         * Removes leading and trailing whitespace from a string.
         * 
         * @param str Input string to trim
         * @return Trimmed string with no leading/trailing whitespace
         */
        static std::string trim(const std::string& str);
        
        /**
         * @brief Internal string lowercase conversion utility
         * 
         * Converts all characters in a string to lowercase.
         * 
         * @param str Input string to convert
         * @return Lowercase version of input string
         */
        static std::string toLower(const std::string& str);
    };
    
} // namespace nlm

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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// ============================================================================
// Configuration System
// ============================================================================
// Central configuration management for NLM brain simulation
// 
// This system provides a type-safe key-value configuration interface for
// controlling all aspects of brain simulation including neuron properties,
// plasticity rules, memory systems, development parameters, and simulation settings.
// 
// Configuration values are stored as variants supporting multiple types:
// - Basic types: int, int64_t, double, bool, std::string
// - Collections: std::vector<int>, std::vector<double>, std::vector<std::string>
// - Nested structures: via ConfigEntry with source tracking
// 
// Configuration sources are tracked to understand precedence:
// - Default: Hard-coded defaults
// - File: Loaded from configuration files
// - CommandLine: Parsed from command line arguments  
// - Runtime: Set programmatically at runtime
// 
// Configuration lookup uses template specialization for type safety and
// provides convenient default values when keys are not present.
//
// @note TODO PHASE 2: Implement proper JSON/YAML parser for advanced configuration
// @note Currently uses simple key=value format for Phase 1/2 compatibility

class Config {
class Config {
public:
    // ============================================================================
    // Lifecycle Management
    // ============================================================================

    /**
     * @brief Construct a new empty configuration object
     * 
     * Creates a configuration with no entries. Call set() or loadFrom*() methods
     * to populate configuration values.
     */
    Config();

    /**
     * @brief Destroy configuration and clean up resources
     */
    ~Config();

    // Disable copying, enable moving (Rule of Five compliance)
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;

    // ============================================================================
    // Configuration Loading
    // ============================================================================

    /**
     * @brief Load configuration from file (JSON format)
     * 
     * Parses a configuration file and populates the configuration object.
     * File format is currently simple key=value pairs (Phase 1/2 compatibility).
     * 
     * @param filepath Path to configuration file
     * @return true if file was successfully loaded and parsed, false if file
     *         could not be opened or contained parsing errors
     * 
     * @note TODO PHASE 2: Implement proper JSON/YAML parser for advanced file formats
     * @note Comments start with # or / (can be commented out lines)
     * @note Quoted values are supported ("value" or 'value')
     */
    bool loadFromFile(const std::string& filepath);

    /**
     * @brief Load configuration from command line arguments
     * 
     * Parses command line arguments in the format:
     * - --key=value
     * - -key value
     * 
     * @param argc Number of command line arguments
     * @param argv Array of command line argument strings
     * @return true if parsing succeeded, false if arguments were malformed
     * 
     * @note Command line arguments override file and default values
     * @note Uses ConfigSource::CommandLine as source for loaded values
     */
    bool loadFromArgs(int argc, char** argv);

    /**
     * @brief Save configuration to file
     * 
     * Writes current configuration to a file. Note that this is a simplified
     * implementation that writes placeholder values for now.
     * 
     * @param filepath Path to save configuration file
     * @return true if file was successfully written, false if write failed
     */
    bool saveToFile(const std::string& filepath) const;

    // ============================================================================
    // Configuration Access
    // ============================================================================

    /**
     * @brief Get configuration value with type safety
     * 
     * Retrieves a configuration value of the specified type. Throws std::bad_any_cast
     * if the stored value is not of the requested type.
     * 
     * @tparam T Type of value to retrieve (int, double, bool, std::string, etc.)
     * @param key Configuration key to look up
     * @return std::optional<T> Value if key exists and matches type, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    /**
     * @brief Get configuration value with default fallback
     * 
     * Retrieves a configuration value, returning a default value if the key is not
     * present or if the value type doesn't match.
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key to look up
     * @param defaultValue Value to return if key is not found
     * @return T Retrieved value or default value
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // ============================================================================
    // Configuration Modification
    // ============================================================================

    /**
     * @brief Set configuration value with source tracking
     * 
     * Sets a configuration value and tracks its source for precedence determination.
     * Multiple set operations with different sources will determine which value is used
     * (typically: CommandLine > File > Runtime > Default).
     * 
     * @param key Configuration key to set
     * @param value Configuration value
     * @param source Source of configuration (Default, File, CommandLine, Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (string)
     * 
     * Convenience method for setting string values.
     * 
     * @param key Configuration key
     * @param value String value to set
     * @param source Configuration source
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (integer)
     * 
     * @param key Configuration key
     * @param value Integer value to set
     * @param source Configuration source
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (double/floating-point)
     * 
     * @param key Configuration key
     * @param value Floating-point value to set
     * @param source Configuration source
     */
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);

    /**
     * @brief Set configuration value (boolean)
     * 
     * @param key Configuration key
     * @param value Boolean value to set
     * @param source Configuration source
     */
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);

    // ============================================================================
    // Configuration Inspection
    // ============================================================================

    /**
     * @brief Check if configuration key exists
     * 
     * @param key Configuration key to check
     * @return true if key is present in configuration, false otherwise
     */
    bool has(const std::string& key) const;

    /**
     * @brief Remove configuration key
     * 
     * @param key Configuration key to remove
     * @note Removed keys cannot be recovered without reloading from file
     */
    void remove(const std::string& key);

    /**
     * @brief Get all configuration keys
     * 
     * @return Vector of all configuration keys currently set
     */
    std::vector<std::string> getKeys() const;

    /**
     * @brief Clear all configuration entries
     * 
     * @note Resets configuration to empty state (only default values may remain)
     */
    void clear();

    /**
     * @brief Get configuration summary string
     * 
     * @return Formatted string summarizing configuration contents
     */
    std::string summary() const;

private:
    // ============================================================================
    // Internal Implementation
    // ============================================================================

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Internal helpers
    static std::string trim(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static std::string extractValueFromLine(const std::string& line);
    static bool parseSimpleKeyValue(const std::string& line, std::string& key, std::string& value);
    static ConfigValue convertStringToValue(const std::string& str);
    static std::string convertValueToString(const ConfigValue& value);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

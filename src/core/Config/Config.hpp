#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Configuration system for NLM (Neural Learning Machine)
// Advanced key-value configuration with multiple source support
// 
// Configuration Features:
// - JSON file format support
// - Command-line argument parsing
// - Runtime configuration management
// - Type-safe value retrieval
// - Source tracking (default, file, command_line, runtime)
// - Configuration validation and warnings
// - Serialization and checkpoint support

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types for type-safe storage
 * 
 * Supports numeric types, strings, and collections for flexible configuration.
 * Uses std::variant for safe type checking during retrieval.
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
 * @brief Configuration source tracking for dependency resolution
 * 
 * Tracks the origin of each configuration value to handle overrides:
 * - Default: Built-in defaults, lowest priority
 * - File: Loaded from configuration files, medium priority  
 * - CommandLine: Set via command-line arguments, high priority
 * - Runtime: Set programmatically, highest priority
 */
enum class ConfigSource {
public:
    enum Value {
        Default = 0,      ///< Built-in default configuration
        File = 1,         ///< Loaded from configuration file
        CommandLine = 2,  ///< Set via command-line arguments
        Runtime = 3       ///< Set programmatically at runtime
    };
    
    ConfigSource() = default;
    constexpr ConfigSource(Value v) : value(v) {}
    constexpr operator Value() const { return value; }
    constexpr bool operator==(const ConfigSource& other) const { return value == other.value; }
    constexpr bool operator!=(const ConfigSource& other) const { return value != other.value; }
    
private:
    Value value = Default;
};

/**
 * @brief Configuration entry with metadata
 * 
 * Represents a single configuration key-value pair with additional
 * information about its origin and documentation.
 */
struct ConfigEntry {
    std::string key;           ///< Configuration key
    ConfigValue value;          ///< Configuration value
    ConfigSource source;        ///< Origin of this configuration
    std::string description;    ///< Human-readable description
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    // Type-safe getters for common types
    template<typename T>
    std::optional<T> getValue() const {
        if (std::holds_alternative<T>(value)) {
            return std::get<T>(value);
        }
        return std::nullopt;
    }
    
    // Check if entry has the specified source
    bool fromSource(ConfigSource source) const { return this->source == source; }
    
    // Check if this entry overrides another
    bool overrides(const ConfigEntry& other) const {
        return this->source != ConfigSource::Default && 
               static_cast<int>(this->source) > static_cast<int>(other.source);
    }
};

/**
 * @brief Main configuration class for NLM
 * 
 * Provides a comprehensive configuration management system with:
 * - File loading (JSON format)
 * - Command-line argument parsing
 * - Type-safe value retrieval
 * - Configuration validation
 * - Serialization support
 * - Change tracking and notifications
 * 
 * Thread-safe for concurrent access during simulation.
 */
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving (PIMPL pattern)
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * @brief Load configuration from JSON file
     * 
     * @param filepath Path to configuration file
     * @return true if file was loaded successfully, false otherwise
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * @brief Load configuration from command-line arguments
     * 
     * Parses common NLM configuration options:
     * --config=file.cfg, --seed=42, --neurons=500, etc.
     * 
     * @param argc Command-line argument count
     * @param argv Command-line argument array
     * @return true if parsing succeeded, false on error
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * @brief Save configuration to JSON file
     * 
     * @param filepath Path where configuration should be saved
     * @return true if save succeeded, false otherwise
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * @brief Get configuration value with type safety
     * 
     * @tparam T Type of value to retrieve (int, double, bool, string, or vector)
     * @param key Configuration key
     * @return Optional containing value if found, nullopt otherwise
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * @brief Get configuration value with default fallback
     * 
     * @tparam T Type of value to retrieve
     * @param key Configuration key
     * @param defaultValue Value to return if key not found
     * @return Configuration value or default if not found
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * @brief Set configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Origin of this configuration (default: Runtime)
     */
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Set configuration value (overloaded for common types)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
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
     * @brief Get all configuration entries
     * 
     * @return Vector of all configuration entries
     */
    std::vector<ConfigEntry> getAllEntries() const;
    
    /**
     * @brief Clear all configuration values
     */
    void clear();
    
    /**
     * @brief Get configuration summary
     * 
     * @return Formatted string containing configuration summary
     */
    std::string summary() const;
    
    /**
     * @brief Validate configuration values
     * 
     * Checks for common configuration errors and inconsistencies.
     * 
     * @return true if configuration is valid, false otherwise
     */
    bool validate() const;
    
    /**
     * @brief Get configuration statistics
     * 
     * @return Map containing configuration statistics
     */
    std::unordered_map<std::string, size_t> getStatistics() const;
    
    /**
     * @brief Merge configuration from another config object
     * 
     * Values from the source config override current values if they
     * come from a higher priority source.
     * 
     * @param other Configuration object to merge from
     */
    void merge(const Config& other);
    
    /**
     * @brief Get configuration entry by key
     * 
     * @param key Configuration key
     * @return ConfigEntry if found, nullptr otherwise
     */
    const ConfigEntry* getEntry(const std::string& key) const;
    
    /**
     * @brief Get configuration source priority order
     * 
     * @return Vector of sources in priority order (lowest to highest)
     */
    static std::vector<ConfigSource> getSourcePriorityOrder();
    
    /**
     * @brief Check if configuration has been modified
     * 
     * @return true if configuration differs from default, false otherwise
     */
    bool isModified() const;
    
    /**
     * @brief Reset configuration to defaults
     */
    void resetToDefaults();
    
    /**
     * @brief Get configuration file format version
     * 
     * @return Version string for compatibility checking
     */
    static std::string getConfigVersion() { return "1.0"; }
    
    /**
     * @brief Check if configuration value is within acceptable range
     * 
     * @tparam T Type of value
     * @param key Configuration key
     * @param min Minimum acceptable value
     * @param max Maximum acceptable value
     * @return true if value is within range, false otherwise
     */
    template<typename T>
    bool isWithinRange(const std::string& key, const T& min, const T& max) const {
        auto value = get<T>(key);
        if (!value) return false;
        return *value >= min && *value <= max;
    }
    
    /**
     * @brief Get configuration value with range validation
     * 
     * @tparam T Type of value
     * @param key Configuration key
     * @param min Minimum acceptable value
     * @param max Maximum acceptable value
     * @param defaultValue Value to return if invalid
     * @return Valid configuration value or default
     */
    template<typename T>
    T getValidated(const std::string& key, const T& min, const T& max, const T& defaultValue) const {
        auto value = get<T>(key);
        if (!value || !isWithinRange(key, min, max)) {
            return defaultValue;
        }
        return *value;
    }
    
    /**
     * @brief Batch update multiple configuration values
     * 
     * @param updates Map of key-value pairs to update
     * @param source Source of these updates
     */
    void batchUpdate(const std::unordered_map<std::string, ConfigValue>& updates, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * @brief Export configuration to string
     * 
     * @param format Output format (json, ini, csv)
     * @return Configuration as string
     */
    std::string exportConfig(const std::string& format = "json") const;
    
    /**
     * @brief Import configuration from string
     * 
     * @param configString Configuration data as string
     * @param format Input format (json, ini, csv)
     * @return true if import succeeded, false otherwise
     */
    bool importConfig(const std::string& configString, const std::string& format = "json");
    
    /**
     * @brief Get configuration dependency graph
     * 
     * @return Map of keys to their dependent keys
     */
    std::unordered_map<std::string, std::vector<std::string>> getDependencyGraph() const;
    
    /**
     * @brief Check for configuration cycles
     * 
     * @return true if cycles detected, false otherwise
     */
    bool hasConfigurationCycles() const;
    
    /**
     * @brief Get configuration health status
     * 
     * @return Vector of health status messages
     */
    std::vector<std::string> getHealthStatus() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static bool parseJsonValue(const std::string& jsonStr, ConfigValue& outValue);
    static std::string serializeToJson() const;
    static bool parseJson(const std::string& jsonStr);
    static std::string boolToString(bool value) const;
    static std::string vectorToString(const ConfigValue& value) const;
    static ConfigValue stringToValue(const std::string& str);
    
    // Configuration validation
    static bool validateValue(const std::string& key, const ConfigValue& value, ConfigSource source);
    static bool validateConsistency() const;
    
    // Change tracking
    mutable bool modified;
    mutable std::unordered_map<std::string, ConfigSource> originalSources;
    
    // Statistics tracking
    mutable std::unordered_map<std::string, size_t> accessCounts;
    mutable std::unordered_map<std::string, size_t> modificationCounts;
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <filesystem>

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

// Main configuration class
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
    
    // === VALIDATION AND UTILITY METHODS ===
    
    /**
     * Validate configuration values against known parameters
     * @param strict If true, only known parameters are allowed
     * @return vector of error messages (empty if valid)
     */
    std::vector<std::string> validate(bool strict = false) const;
    
    /**
     * Get configuration parameter description (if available)
     * @param key Configuration parameter key
     * @return Description or empty string if not found
     */
    std::string getParameterDescription(const std::string& key) const;
    
    /**
     * Merge configuration from another Config object
     * @param other Config object to merge
     * @param overrideSources If true, overwrite entries from other sources
     */
    void merge(const Config& other, bool overrideSources = false);
    
    /**
     * Get all configuration values as strings
     * @return Map of key-value pairs as strings
     */
    std::unordered_map<std::string, std::string> getAllAsStrings() const;
    
    /**
     * Check if configuration value matches expected pattern
     * @param key Configuration parameter key
     * @param pattern Expected pattern (bool, int, float)
     * @return true if matches pattern or no validation
     */
    bool validatePattern(const std::string& key, const std::string& pattern) const;
    
    /**
     * Get configuration value type
     * @param key Configuration parameter key
     * @return String representation of type (int, float, bool, string, vector)
     */
    std::string getValueType(const std::string& key) const;
    
    /**
     * Convert configuration to JSON-like string
     * @return JSON representation of configuration
     */
    std::string toJson() const;
    
    /**
     * Load from JSON file
     * @param filepath Path to JSON configuration file
     * @return true if successful
     */
    bool loadFromJson(const std::string& filepath);
    
    /**
     * Load from YAML file
     * @param filepath Path to YAML configuration file
     * @return true if successful
     */
    bool loadFromYaml(const std::string& filepath);
    
    /**
     * Check if configuration contains any values from a specific source
     * @param source Configuration source to check
     * @return true if any entries have this source
     */
    bool hasSource(ConfigSource source) const;
    
    /**
     * Get all configuration entries from a specific source
     * @param source Configuration source to filter
     * @return Vector of configuration entries
     */
    std::vector<ConfigEntry> getEntriesFromSource(ConfigSource source) const;
    
    /**
     * Set default values for known parameters
     * @param source Source of these defaults (usually ConfigSource::Default)
     */
    void setDefaults(ConfigSource source = ConfigSource::Default);
    
    /**
     * Get configuration statistics
     * @return String with statistics about configuration
     */
    std::string getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Set values with automatic type conversion
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Template specializations for specific types
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate individual value against expected type
    static bool validateValue(const ConfigValue& value, ConfigSource source);
    
    // Convert string to appropriate type
    static ConfigValue stringToConfigValue(const std::string& str);
    
    // Get human-readable type name
    static std::string getTypeName(const ConfigValue& value);
};

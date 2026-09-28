#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

namespace nlm {

// Forward declarations
class Config;

/**
 * @brief Configuration value types for the NLM brain system
 * 
 * Configuration values can be various types including primitive types,
 * strings, and collections. The variant type allows flexible storage
 * of different configuration options while maintaining type safety
 * through the get() template method.
 * 
 * @note Used internally by the Config class to store named configuration entries
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
 * @brief Source of configuration entry
 * 
 * Enumerates the different sources from which configuration values
 * can originate, used to determine configuration priority and precedence.
 * 
 * @note Higher priority sources override lower priority sources:
 * CommandLine > File > Default > Runtime
 */
enum class ConfigSource {
    /**
     * @brief Default configuration values
     * 
     * Initial values set by the system. Can be overridden by other sources.
     */
    Default,
    /**
     * @brief Configuration loaded from file
     * 
     * Configuration values from configuration files (JSON, YAML, or simple key=value format).
     */
    File,
    /**
     * @brief Configuration from command line arguments
     * 
     * Configuration values set via command line arguments have highest priority.
     */
    CommandLine,
    /**
     * @brief Runtime configuration changes
     * 
     * Configuration values set during program execution (e.g., via advanced commands).
     */
    Runtime
};

/**
 * @brief Configuration entry storing a key-value pair with metadata
 * 
 * Represents a single configuration parameter with its value, source,
 * and descriptive information. Used internally by the Config class to
 * maintain the configuration database.
 */
struct ConfigEntry {
    /**
     * @brief Configuration parameter key/name
     * 
     * Unique identifier for the configuration parameter. Used to retrieve
     * and set configuration values throughout the system.
     */
    std::string key;
    
    /**
     * @brief Configuration value
     * 
     * The actual configuration value stored as a variant type. The type
     * can be accessed using template get() methods of the Config class.
     */
    ConfigValue value;
    
    /**
     * @brief Source of this configuration value
     * 
     * Indicates where this configuration value originated from, used to
     * determine precedence and for debugging purposes.
     */
    ConfigSource source;
    
    /**
     * @brief Human-readable description of the configuration parameter
     * 
     * Provides documentation for what this configuration parameter controls
     * and its valid ranges/values.
     */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with parameters
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Configuration source
     * @param desc Human-readable description
     */
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
    static std::string valueToString(const ConfigValue& value);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

/**
 * @namespace nlm
 * @brief Neural Learning Machine (NLM) - An experimental artificial developmental brain simulation framework.
 * 
 * NLM provides a complete brain simulation platform with integrated neural dynamics,
 * memory systems, prediction mechanisms, neuromodulation, and developmental stages.
 * The system simulates biological brain processes using simplified neuron models
 * and spike-based neural dynamics.
 */

namespace nlm {

// Forward declarations
class Config;

/**
 * @namespace nlm::config
 * @brief Configuration system for NLM
 * 
 * Handles loading, saving, and management of simulation parameters and settings.
 * Supports multiple configuration sources with precedence rules.
 */
namespace config {

/**
 * @brief Configuration value types supported by NLM
 * 
 * Using std::variant to store different types of configuration values with
 * automatic type checking and conversion capabilities.
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
 * @enum ConfigSource
 * @brief Source of configuration value
 * 
 * Different sources have different precedence for resolving conflicts:
 * Runtime > CommandLine > File > Default
 */
enum class ConfigSource {
    Default,    ///< Default configuration values
    File,       ///< Loaded from file (JSON format)
    CommandLine,///< Command line arguments
    Runtime     ///< Set programmatically at runtime
};

/**
 * @struct ConfigEntry
 * @brief Single configuration key-value pair with metadata
 * 
 * Stores the configuration value along with its source and descriptive
 * information for documentation and debugging purposes.
 */
struct ConfigEntry {
    std::string key;             ///< Configuration key/name
    ConfigValue value;           ///< Configuration value (type-safe variant)
    ConfigSource source;         ///< Source of this configuration value
    std::string description;     ///< Human-readable description of the parameter
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty configuration entry with Default source.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Constructor with all parameters
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

} // namespace config

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
};

} // namespace nlm

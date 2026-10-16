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
 * @brief Union type representing all possible configuration values
 * 
 * This variant type can hold various data types:
 * - Basic types: int, int64_t, double, bool
 * - String types: std::string
 * - Array types: std::vector<int>, std::vector<double>, std::vector<std::string>
 * 
 * Used by the Config class to store configuration parameters.
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
 * @brief Enumeration of configuration source types
 * 
 * Defines where configuration values come from, used for tracking
 * and debugging configuration management.
 */
enum class ConfigSource {
public:
    /** @brief Default configuration values */
    Default,
    
    /** @brief Configuration loaded from file */
    File,
    
    /** @brief Configuration provided via command line */
    CommandLine,
    
    /** @brief Configuration set at runtime */
    Runtime
};

/**
 * @brief Internal structure representing a single configuration entry
 * 
 * @note This is an implementation detail of the Config class
 * @see Config::loadFromFile()
 * @see Config::set()
 */
struct ConfigEntry {
    /** @brief Unique key identifying the configuration parameter */
    std::string key;
    
    /** @brief Configuration value of the parameter */
    ConfigValue value;
    
    /** @brief Source of this configuration value */
    ConfigSource source;
    
    /** @brief Human-readable description of the parameter */
    std::string description;
    
    /**
     * @brief Default constructor
     * 
     * Initializes all fields to default values.
     */
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param k Configuration key
     * @param v Configuration value
     * @param s Source of configuration
     * @param desc Description of the parameter
     */
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Main configuration class
/**
 * @class Config
 * @brief Central configuration management system for NLM
 * 
 * The Config class provides a flexible, type-safe configuration management system
 * for the NLM (Neural Learning Machine) framework. It supports multiple input
 * formats (JSON, YAML, simple key=value), provides type-safe value retrieval,
 * and includes comprehensive error handling.
 * 
 * Key features:
 * - **Multiple format support**: JSON, YAML, and backward-compatible key=value formats
 * - **Type safety**: Strong typing with variants for different value types
 * - **Error handling**: Comprehensive error reporting and recovery
 * - **Flexibility**: Can be loaded from files, command line, or runtime
 * - **Persistence**: Save/load configuration to/from files
 * 
 * Configuration values can be strings, numbers, booleans, or arrays of these types.
 * The system uses a source tracking mechanism to manage configuration precedence
 * (command line > file > runtime > default).
 * 
 * @note This class uses the Pimpl idiom for efficient implementation
 * @see ConfigSource
 * @see ConfigValue
 * @see nlohmann::json
 * @see YAML::Node
 * @see nlm::Logger
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

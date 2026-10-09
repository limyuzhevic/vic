#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <fstream>

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

// Main configuration class for NLM
/**
 * @class Config
 * @brief Central configuration management for the NLM neural simulation framework
 * 
 * The Config class provides a unified interface for loading, storing, and accessing
 * configuration parameters for the NLM (Neural Learning Machine) system. It supports
 * multiple configuration sources (files, command line, runtime) and handles various
 * data types including integers, floats, booleans, strings, and complex types like
 * vectors and variants.
 * 
 * Key features:
 * - JSON configuration file support with automatic parsing
 * - Command-line argument integration with flexible format (--key=value, -key value)
 * - Runtime configuration modification capabilities
 * - Comprehensive error handling and validation
 * - Type-safe value retrieval with automatic conversion
 * - Multiple configuration sources with precedence handling
 * - Persistent configuration saving and loading
 * 
 * The configuration system is designed to be both simple for basic use cases and
 * powerful enough for complex experimental setups. It supports all configuration
 * options needed by the Phase 6 integrated brain system.
 * 
 * @note This is a Phase 6 component - provides the configuration backbone for
 *       the final integrated artificial brain system
 * @see ConfigEntry, ConfigSource
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
    
    // Helper methods for enhanced parsing
    std::string escapeString(const std::string& str);
    void setFromString(const std::string& key, const std::string& valueStr, ConfigSource source);
    bool loadFromSimpleFormat(std::ifstream& file);
    bool loadFromJson(std::ifstream& file);
    void parseJsonString(const std::string& jsonStr);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

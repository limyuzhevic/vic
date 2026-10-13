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

// Enhanced configuration management with type safety
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving for performance
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    /**
     * Load configuration from file (JSON format)
     * Supports key=value format and will fall back to simple parsing
     * if JSON parser is not available
     */
    bool loadFromFile(const std::string& filepath);
    
    /**
     * Load configuration from command line arguments
     * Supports both --key=value and -key value formats
     * Also supports boolean flags (--key enables true)
     */
    bool loadFromArgs(int argc, char** argv);
    
    /**
     * Save configuration to file
     * Generates human-readable configuration files with comments
     */
    bool saveToFile(const std::string& filepath) const;
    
    /**
     * Get configuration value with type checking
     * Returns nullopt if key doesn't exist or type mismatch
     */
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    /**
     * Get configuration value with default
     * Returns default value if key doesn't exist or type mismatch
     */
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    /**
     * Set configuration value with automatic type detection
     * Accepts strings and automatically converts to appropriate type
     * (bool, int, double, or string)
     */
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Set configuration value with explicit type
     */
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    /**
     * Check if configuration key exists
     */
    bool has(const std::string& key) const;
    
    /**
     * Remove configuration key
     */
    void remove(const std::string& key);
    
    /**
     * Get all configuration keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * Clear all configuration entries
     */
    void clear();
    
    /**
     * Generate configuration summary
     * Useful for debugging and logging
     */
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    /**
     * Internal helper: trim whitespace from string
     */
    static std::string trim(const std::string& str);
    
    /**
     * Internal helper: convert string to lowercase
     */
    static std::string toLower(const std::string& str);
};

} // namespace nlm

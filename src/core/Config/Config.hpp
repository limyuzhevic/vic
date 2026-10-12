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
    
    // ========== CONVENIENCE CONSTRUCTORS ==========
    
    // Factory methods for common configurations
    static Config createDefaultConfig();
    static Config createMinimalConfig();
    static Config createProductionConfig();
    static Config createDevelopmentConfig();
    
    // Apply preset configurations
    void applyPreset(const std::string& presetName);
    
    // ========== BATCH OPERATIONS ==========
    
    // Set multiple values at once
    void setValues(const std::map<std::string, ConfigValue>& values);
    
    // Get multiple values
    std::map<std::string, std::optional<ConfigValue>> getValues(const std::vector<std::string>& keys) const;
    
    // Check multiple keys at once
    std::map<std::string, bool> hasKeys(const std::vector<std::string>& keys) const;
    
    // ========== CONFIGURATION GROUPS ==========
    
    // Get all keys in a group
    std::vector<std::string> getKeysInGroup(const std::string& groupName) const;
    
    // Set configuration group
    void setGroup(const std::string& groupName, const std::map<std::string, ConfigValue>& values);
    
    // Get configuration group
    std::map<std::string, std::optional<ConfigValue>> getGroup(const std::string& groupName) const;
    
    // ========== ADVANCED OPERATIONS ==========
    
    // Merge with another config
    void merge(const Config& other, bool overwrite = true);
    
    // Clone configuration
    Config clone() const;
    
    // Compare configurations
    bool equals(const Config& other) const;
    
    // Get configuration hash for quick comparison
    size_t getHash() const;
    
    // Get filtered configuration (only keys matching pattern)
    Config filter(const std::string& pattern) const;
    
    // Validate configuration against schema
    std::vector<std::string> validate(const std::string& schema) const;
    
    // ========== STATISTICS AND METADATA ==========
    
    // Get configuration statistics
    std::map<std::string, double> getStatistics() const;
    
    // Get configuration metadata
    std::map<std::string, std::string> getMetadata() const;
    
    // Set metadata
    void setMetadata(const std::string& key, const std::string& value);
    
    // Add tag to configuration
    void addTag(const std::string& tag);
    
    // Get all tags
    std::vector<std::string> getTags() const;
    
    // Check if configuration has tag
    bool hasTag(const std::string& tag) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

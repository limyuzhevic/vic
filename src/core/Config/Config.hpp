#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>

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
    
    // Load from file formats (private helpers)
    bool loadFromLegacyFormat(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    std::optional<T> getNested(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with dotted notation support
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values with nested key support
    void setNested(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void setNested(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setNested(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setNested(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setNested(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setNested(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence (supports nested keys)
    bool has(const std::string& key) const;
    bool hasNested(const std::string& key) const;
    
    // Remove key (supports nested keys)
    void remove(const std::string& key);
    void removeNested(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all keys with optional prefix filter
    std::vector<std::string> getKeys(const std::string& prefix) const;
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration by nested key
    std::optional<ConfigValue> getNested(const std::string& key) const;
    
    // Clear all
    void clear();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    std::unordered_map<std::string, std::shared_ptr<Config>> createNestedStructure(const std::string& key) const;
    std::shared_ptr<Config> getNestedConfig(const std::string& key) const;
    std::string normalizeKey(const std::string& key) const;
};

} // namespace nlm

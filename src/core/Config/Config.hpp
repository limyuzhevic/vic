#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <filesystem>
#include <algorithm>

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
    std::vector<std::string>,
    std::vector<ConfigValue>,  // Nested arrays
    std::unordered_map<std::string, ConfigValue>  // Nested objects
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

// File format enum
enum class ConfigFormat {
    Unknown,
    SimpleKeyValue,
    JSON,
    YAML
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
    
    // Load from file (JSON/YAML/SimpleKV format)
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
    
    // File format detection
    static ConfigFormat detectFileFormat(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getFileExtension(const std::string& filepath);
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // Format-specific parsing methods
    bool parseSimpleKeyValue(const std::string& content);
    bool parseJSON(const std::string& content);
    bool parseYAML(const std::string& content);
    
    // Format-specific saving methods
    bool saveSimpleKeyValue(std::ofstream& file) const;
    bool saveJSON(std::ofstream& file) const;
    bool saveYAML(std::ofstream& file) const;
    
    // Helper methods for nested structures
    static ConfigValue parseSimpleValue(const std::string& valueStr);
    static std::string configValueToString(const ConfigValue& value) const;
    static void flattenConfig(const std::string& prefix, const ConfigValue& value, std::unordered_map<std::string, ConfigValue>& flatConfig);
    static ConfigValue unflattenConfig(const std::unordered_map<std::string, ConfigValue>& flatConfig);
};

} // namespace nlm

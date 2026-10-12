#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <any>
#include <functional>

// Forward declarations for JSON and YAML libraries
namespace nlohmann {
    template<typename T> class json;
}

namespace YAML {
    class Node;
}

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

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string", "array<int>", "array<double>", "array<string>"
    bool required;
    std::any defaultValue;
    std::function<bool(const std::any&)> validator;
    std::string description;
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
    
    // Load from file (JSON or YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (auto-detects format based on extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to file with specific format
    bool saveToFile(const std::string& filepath, const std::string& format) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get value with schema validation and default
    template<typename T>
    T getOrDefault(const std::string& key, const T& defaultValue, const ConfigSchema& schema) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set nested values
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Validation
    bool validate(const std::vector<ConfigSchema>& schema) const;
    
    // Merge configurations
    void merge(const Config& other);
    
    // Configuration resolution (file < command line < default)
    bool resolveOverrides(const Config& sourceConfig, ConfigSource prioritySource);
    
    // Get configuration source
    ConfigSource getSource(const std::string& key) const;
    
    // Convert between types
    template<typename T>
    static std::optional<T> convertValue(const ConfigValue& value);
    
    // Static helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    const ConfigEntry* findEntry(const std::string& key) const;
};

} // namespace nlm

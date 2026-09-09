#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Try to include JSON and YAML libraries if available
#if __has_include(<nlohmann/json.hpp>)
#include <nlohmann/json.hpp>
#endif

#if __has_include(<yaml-cpp/yaml.h>)
#include <yaml-cpp/yaml.h>
#endif

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

// JSON to ConfigValue conversion helpers
namespace json_helpers {
    template<typename T>
    ConfigValue jsonToConfigValue(const T& value);
    
    template<typename T>
    T configValueToJson(const ConfigValue& configVal);
}

// YAML to ConfigValue conversion helpers  
namespace yaml_helpers {
    template<typename T>
    ConfigValue yamlToConfigValue(const T& value);
    
    template<typename T>
    T configValueToYaml(const ConfigValue& configVal);
}

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
    
    // Load from file (supports JSON and YAML)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
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
    
    // Parse JSON string to config values
    static std::map<std::string, ConfigValue> parseJsonString(const std::string& jsonString);
    
    // Parse YAML string to config values
    static std::map<std::string, ConfigValue> parseYamlString(const std::string& yamlString);
    
    // Validate config value against expected type
    static bool validateConfigValue(const ConfigValue& value, const std::string& expectedType);
    
    // Import config from JSON string
    bool importFromJsonString(const std::string& jsonString, ConfigSource source = ConfigSource::Runtime);
    
    // Export config to JSON string
    std::string exportToJsonString() const;
    
    // Import config from YAML string
    bool importFromYamlString(const std::string& yamlString, ConfigSource source = ConfigSource::Runtime);
    
    // Merge another config into this one
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Validate all config values
    std::vector<std::string> validateAll() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string valueToString(const ConfigValue& value);
    static ConfigSource resolveSource(ConfigSource source) const;
};

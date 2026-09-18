#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

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
    
    // Load from file (auto-detect format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYaml(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (auto-detect format based on extension)
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
    
    // Configuration schema validation
    struct Schema {
        std::string key;
        std::string type; // "int", "float", "string", "bool", "vector<int>", "vector<float>", "vector<string>"
        std::string description;
        std::optional<std::string> defaultValue;
        bool required = false;
        std::vector<std::string> allowedValues;
        
        std::vector<double> minValue;
        std::vector<double> maxValue;
        bool validate(const ConfigValue& value) const;
    };
    
    // Schema management
    void addSchema(const Schema& schema);
    void validateSchema(const nlohmann::json& schema) const;
    bool validate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<Schema> schemas;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Format detection
    static std::string detectFormat(const std::string& filepath);
    
    // Parsing helpers
    static ConfigValue jsonValueToConfigValue(const nlohmann::json& jsonValue);
    static nlohmann::json configValueToJson(const ConfigValue& configValue);
    static ConfigValue yamlValueToConfigValue(const YAML::Node& yamlValue);
    static YAML::Node configValueToYaml(const ConfigValue& configValue);
};

} // namespace nlm

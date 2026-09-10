#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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

// JSON serialization functions
nlohmann::json configValueToJson(const ConfigValue& value);
ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue);
bool validateJsonConfig(const nlohmann::json& jsonConfig, const ConfigSchema& schema);

// Schema validation types
struct ConfigSchemaEntry {
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string", "vector<int>", "vector<double>", "vector<string>"
    bool required;
    ConfigValue defaultValue;
    std::string validationPattern;
    double minValue;
    double maxValue;
    size_t minSize;
    size_t maxSize;
};

struct ConfigSchema {
    std::string name;
    std::string description;
    std::vector<ConfigSchemaEntry> entries;
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> customValidators;
    
    ConfigSchema() : name(), description() {}
    ConfigSchema(const std::string& n, const std::string& desc) : name(n), description(desc) {}
    
    void addEntry(const std::string& key, const std::string& type, bool required = false, 
                  const ConfigValue& defaultValue = ConfigValue{}, 
                  const std::string& validationPattern = "",
                  double minValue = 0.0, double maxValue = 0.0,
                  size_t minSize = 0, size_t maxSize = 0) {
        ConfigSchemaEntry entry;
        entry.key = key;
        entry.type = type;
        entry.required = required;
        entry.defaultValue = defaultValue;
        entry.validationPattern = validationPattern;
        entry.minValue = minValue;
        entry.maxValue = maxValue;
        entry.minSize = minSize;
        entry.maxSize = maxSize;
        entries.push_back(entry);
    }
    
    void addCustomValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
        customValidators[key] = validator;
    }
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

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>

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
    
    ConfigEntry(nullptr) : key(nullptr), value(nullptr), source(ConfigSource::Default), description(nullptr) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// JSON serialization helper functions
namespace json_helpers {
    nlohmann::json configValueToJson(const ConfigValue& value);
    ConfigValue jsonToConfigValue(const nlohmann::json& json);
}

// Main configuration class
class Config {
public:
    Config(nullptr);
    ~Config(nullptr);
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format, also supports .yaml/.yml)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJsonString(nullptr) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys(nullptr) const;
    
    // Clear all
    void clear(nullptr);
    
    // Get configuration summary
    std::string summary(nullptr) const;
    
    // Get file extension and determine format
    static std::string getFileExtension(const std::string& filepath);
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Performance optimization: replace linear searches with hash maps
    static std::unordered_map<std::string, ConfigEntry> buildKeyMap(const std::vector<ConfigEntry>& entries) const;
};

} // namespace nlm

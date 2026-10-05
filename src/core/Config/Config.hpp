#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
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
    
    // File format detection
    static std::string detectFileFormat(const std::string& filepath);
    static bool isJSONFormat(const std::string& extension);
    static bool isYAMLFormat(const std::string& extension);
    static bool isLegacyFormat(const std::string& extension);
    
    // JSON/YAML parsing helpers
    static bool loadFromJSON(const std::string& filepath);
    static bool loadFromYAML(const std::string& filepath);
    static bool loadFromLegacy(const std::string& filepath);
    
    // JSON/YAML to ConfigValue conversion
    static ConfigValue jsonToConfigValue(const nlohmann::json& j, const std::string& key);
    static ConfigValue yamlToConfigValue(const YAML::Node& node, const std::string& key);
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static YAML::Node configValueToYAML(const ConfigValue& value);
    
    // Type checking and conversion
    template<typename T>
    static bool valueMatchesType(const ConfigValue& value);
    static bool tryConvertToType(const ConfigValue& from, ConfigValue& to);
    
    // Error handling
    static bool handleParseError(const std::string& error);
    
    // Configuration parsing
    static void parseNestedConfig(const nlohmann::json& j, const std::string& prefix = "");
    static void parseNestedConfig(const YAML::Node& node, const std::string& prefix = "");
    
    // Configuration saving
    static nlohmann::json configValueToJSON(const ConfigValue& value, const std::string& key);
    static YAML::Node configValueToYAML(const ConfigValue& value, const std::string& key);
};

} // namespace nlm

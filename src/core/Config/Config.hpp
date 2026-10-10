#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <any>

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
    Environment,
    Runtime
};

// Configuration validation types
enum class ConfigType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    VectorInt,
    VectorDouble,
    VectorString
};

// Configuration validation rule
struct ConfigValidationRule {
    std::string key;
    ConfigType type;
    bool required = false;
    std::any defaultValue;
    std::string description;
    std::function<bool(const std::any&)> validator;
    
    ConfigValidationRule(const std::string& k, ConfigType t, bool req = false, const std::string& desc = "")
        : key(k), type(t), required(req), description(desc) {}
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

// Type-safe configuration accessor
class ConfigParameter {
public:
    template<typename T>
    static T get(const Config& config, const std::string& key, const T& defaultValue = T{});
    
    template<typename T>
    static void set(Config& config, const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    template<typename T>
    static bool has(const Config& config, const std::string& key);
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars = {});
    
    // Load from configuration schema
    bool loadFromSchema(const std::vector<ConfigValidationRule>& schema);
    
    // Validate configuration
    bool validate(const std::vector<ConfigValidationRule>& schema) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with source tracking
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    
    // Set default values from schema
    void setDefaults(const std::vector<ConfigValidationRule>& schema);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration by source
    std::string getSummaryBySource(ConfigSource source) const;
    
    // Configuration helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string replaceEnvVars(const std::string& str, const std::unordered_map<std::string, std::string>& envVars = {});
    static ConfigType getConfigType(const std::string& typeStr);
    static std::string configTypeToString(ConfigType type);
    static bool parseValue(const std::string& valueStr, ConfigValue& outValue);
    
    // Static helper methods for CLI
    static std::vector<std::string> parseArgs(int argc, char** argv);
    static std::unordered_map<std::string, std::string> parseKeyValueArgs(const std::vector<std::string>& args);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

// Type-safe configuration accessor implementations
namespace nlm {

template<typename T>
T ConfigParameter::get(const Config& config, const std::string& key, const T& defaultValue) {
    auto value = config.getOr<T>(key, defaultValue);
    return value;
}

template<typename T>
void ConfigParameter::set(Config& config, const std::string& key, const T& value, ConfigSource source) {
    config.set(key, value, source);
}

template<typename T>
bool ConfigParameter::has(const Config& config, const std::string& key) {
    return config.has(key);
}

} // namespace nlm

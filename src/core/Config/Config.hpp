#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

// Include JSON and YAML libraries for configuration parsing
#ifdef CONFIG_USE_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef CONFIG_USE_YAML
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON/YAML/key=value format)
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
    
    // JSON/YAML parsing helpers
#ifdef CONFIG_USE_JSON
    bool loadFromJSON(const std::string& filepath);
    bool saveToJSON(const std::string& filepath) const;
    void processJSONConfig(const nlohmann::json& json, ConfigSource source);
    nlohmann::json getJSONValue(const ConfigValue& value) const;
#endif
    
#ifdef CONFIG_USE_YAML
    bool loadFromYAML(const std::string& filepath);
    bool saveToYAML(const std::string& filepath) const;
    void processYAMLConfig(const YAML::Node& node, ConfigSource source);
    YAML::Node getYAMLValue(const ConfigValue& value) const;
#endif
    
    void setDescription(const std::string& key, const std::string& description);
    
    // Prevent accidental mixing of different library versions
    static const int EXPECTED_LIBRARY_VERSION = 1;
};

} // namespace nlm

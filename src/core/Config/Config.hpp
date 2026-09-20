#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

namespace nlm {

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string
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
    
    // Move constructor
    ConfigEntry(ConfigEntry&& other) noexcept = default;
    // Move assignment operator
    ConfigEntry& operator=(ConfigEntry&& other) noexcept = default;
};

// Main configuration class
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    // Move constructor
    Config(Config&&) noexcept;
    
    // Move assignment operator
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv) noexcept;
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) noexcept;
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) noexcept;
    
    // Check existence
    bool has(const std::string& key) const noexcept;
    
    // Remove key
    void remove(const std::string& key) noexcept;
    
    // Get all keys
    std::vector<std::string> getKeys() const noexcept;
    
    // Clear all
    void clear() noexcept;
    
    // Get configuration summary
    std::string summary() const noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str) noexcept;
    static std::string toLower(const std::string& str) noexcept;
    
    // JSON parsing helpers
    static bool parseJson(const std::string& content, nlohmann::json& result);
    static bool parseYaml(const std::string& content, nlohmann::json& result);
    static bool convertJsonToConfig(const nlohmann::json& j, std::vector<ConfigEntry>& entries);
    static std::string configValueToString(const ConfigValue& value);
    static ConfigValue stringToConfigValue(const std::string& str);
    static bool validateConfigEntry(const ConfigEntry& entry);
};

} // namespace nlm

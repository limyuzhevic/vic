#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>

// Include error system
#include "ErrorSystem.hpp"

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

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Nested configuration structure for dot notation
class NestedConfig {
private:
    std::map<std::string, std::variant<ConfigValue, NestedConfig>> data;
    
public:
    void set(const std::string& key, const ConfigValue& value);
    std::optional<ConfigValue> get(const std::string& key) const;
    bool has(const std::string& key) const;
    std::vector<std::string> getKeys() const;
    std::string toJson() const;
    static NestedConfig fromJson(const std::string& jsonStr);
    static NestedConfig fromDotNotation(const std::map<std::string, std::string>& keyValuePairs);
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
    [[nodiscard]] std::string loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    [[nodiscard]] std::string loadFromArgs(int argc, char** argv);
    
    // Save to file
    [[nodiscard]] std::string saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    [[nodiscard]] std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe convenience methods
    [[nodiscard]] std::optional<std::string> getString(const std::string& key) const;
    [[nodiscard]] std::optional<int> getInt(const std::string& key) const;
    [[nodiscard]] std::optional<double> getDouble(const std::string& key) const;
    [[nodiscard]] std::optional<bool> getBool(const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe convenience setters
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Convenience setters with conditions
    bool setIfExists(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool setIfNotExists(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration merging
    bool merge(const Config& other, bool override = true);
    
    // Check existence
    [[nodiscard]] bool has(const std::string& key) const;
    
    // Dot notation access
    [[nodiscard]] std::optional<ConfigValue> getDotNotation(const std::string& key) const;
    bool hasDotNotation(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    [[nodiscard]] std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    [[nodiscard]] std::string summary() const;
    
    // Static factory methods
    static Config createMinimalConfig();
    static Config createDevelopmentConfig();
    static Config createProductionConfig();
    static Config createResearchConfig();
    
    // Preset configuration groups
    static Config createConfigWithPreset(const std::string& presetName);
    
    // Simple convenience methods for common types
    static Config createConfigWithValues(const std::map<std::string, std::string>& keyValuePairs);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

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

// Advanced configuration class with JSON/YAML support
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
    
    // Load from string (JSON or YAML)
    bool loadFromString(const std::string& content, const std::string& format = "json");
    
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
    
    // Advanced setters with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get all entries
    const std::vector<ConfigEntry>& getEntries() const { return pImpl->entries; }
    
    // File I/O utilities
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLFile(const std::string& filepath);
    static bool isTextFile(const std::string& filepath);
    
    // Validation
    static bool validateKey(const std::string& key);
    static bool validateValue(const ConfigValue& value);
    
    // Get configuration statistics
    size_t entryCount() const { return pImpl->entries.size(); }
    std::vector<ConfigSource> getSources() const;
    
    // Merge configurations
    void merge(const Config& other);
    
    // Get default configuration values
    static Config getDefaultConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON conversion helpers
    static std::string configValueToJSON(const ConfigValue& value);
    static bool jsonToConfigValue(const nlohmann::json& json, ConfigValue& value);
    static std::string configValueToYAML(const ConfigValue& value);
    static bool yamlToConfigValue(const YAML::Node& yaml, ConfigValue& value);
    
    // Validation helpers
    static bool isValidPrimitiveType(const ConfigValue& value);
    static bool isValidVectorType(const ConfigValue& value);
};

} // namespace nlm

#include "Config.cpp"

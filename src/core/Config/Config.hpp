#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>
#include <map>
#include <unordered_map>
#include <algorithm>

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

// JSON/YAML parser interface
class JSONParser {
public:
    static bool parse(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string serialize(const std::vector<ConfigEntry>& entries);
    static bool has_support();
};

class YAMLParser {
public:
    static bool parse(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string serialize(const std::vector<ConfigEntry>& entries);
    static bool has_support();
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
    
    // Load from file (auto-detects JSON/YAML/key=value format)
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
    
    // Get available parser support
    bool hasJSONSupport() const;
    bool hasYAMLSupport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Parser helper functions
    static bool tryParseJSON(const std::string& content, std::vector<ConfigEntry>& entries);
    static bool tryParseYAML(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string entriesToJSON(const std::vector<ConfigEntry>& entries);
    static std::string entriesToYAML(const std::vector<ConfigEntry>& entries);
    static std::string entriesToKeyValue(const std::vector<ConfigEntry>& entries);
};

} // namespace nlm

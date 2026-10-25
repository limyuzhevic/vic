#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <filesystem>

// Include nlohmann::json for JSON parsing
#include "nlohmann/json.hpp"

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
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from simple YAML format
    bool loadFromSimpleYAML(const std::string& filepath);
    
    // Load from simple text format
    bool loadFromSimpleTextFormat(const std::string& filepath);
    
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
    
    // Set description for existing key
    void setDescription(const std::string& key, const std::string& description);
    
    // Get description for existing key
    std::string getDescription(const std::string& key) const;
    
    // Validation methods
    bool validate() const;
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const std::string& value);
    
    // Convert JSON to ConfigValue
    static ConfigValue jsonToConfigValue(const nlohmann::json& json);
    
    // Convert ConfigValue to string
    static std::string configValueToString(const ConfigValue& value);
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        std::unordered_map<ConfigSource, size_t> entriesBySource;
        size_t stringEntries;
        size_t intEntries;
        size_t doubleEntries;
        size_t boolEntries;
        size_t vectorEntries;
    };
    
    ConfigStats getStats() const;
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Find entries by source
    std::vector<std::string> findKeysBySource(ConfigSource source) const;
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Configuration parser for JSON/YAML
class JSONParser {
public:
    static bool parse(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string toJSONString(const ConfigValue& value);
    static bool isJSON(const std::string& content);
};

} // namespace nlm

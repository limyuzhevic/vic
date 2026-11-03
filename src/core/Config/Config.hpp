#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include <nlohmann/json.hpp>  // Include JSON library

namespace nlm {

using json = nlohmann::json;

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

// Helper function to convert ConfigValue to JSON
inline json configValueToJson(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return std::get<std::vector<int>>(value);
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return std::get<std::vector<double>>(value);
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return std::get<std::vector<std::string>>(value);
    }
    return json(nullptr);
}

// Helper function to convert JSON to ConfigValue
inline ConfigValue jsonToConfigValue(const json& j) {
    if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return static_cast<int64_t>(j.get<uint64_t>());
    } else if (j.is_number()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_array()) {
        if (!j.empty() && j[0].is_number_integer()) {
            return j.get<std::vector<int>>();
        } else if (!j.empty() && j[0].is_number()) {
            return j.get<std::vector<double>>();
        } else if (!j.empty() && j[0].is_string()) {
            return j.get<std::vector<std::string>>();
        }
    }
    return std::string(); // Default empty string
}

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
    
    // Load from JSON file
    bool loadFromJsonFile(const std::string& filepath);
    
    // Load from TOML file (alternative format)
    bool loadFromTomlFile(const std::string& filepath);
    
    // Save to JSON file
    bool saveToJsonFile(const std::string& filepath) const;
    
    // Save to human-readable text file
    bool saveToTextFile(const std::string& filepath) const;
    
    // Import from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Export to JSON string
    std::string toJsonString() const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Merge another configuration
    void merge(const Config& other);
    
    // Get configuration statistics
    std::string getStatistics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool fileExists(const std::string& filepath);
    static std::string getFileExtension(const std::string& filename);
    
    // JSON conversion helpers
    static json configMapToJson(const std::map<std::string, ConfigEntry>& configMap) const;
    static void jsonToConfigMap(const json& j, std::map<std::string, ConfigEntry>& configMap);
    
    // TOML conversion helpers (if toml library available)
    static bool loadFromToml(const std::string& content, std::map<std::string, ConfigEntry>& configMap);
};

} // namespace nlm

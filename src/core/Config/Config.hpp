#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

// Include nlohmann/json for JSON parsing
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
    Environment,
    Runtime
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string configPath; // For nested access (e.g., "brain.neuron_count")
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), configPath() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), configPath(k) {}
};

// Schema validation types
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string", "array_int", "array_double", "array_string"
    bool required = false;
    std::string description;
    std::variant<int, int64_t, double, bool, std::string> defaultValue;
    std::vector<std::string> allowedValues;
    std::string validationPattern; // For string pattern validation
    
    // Nested schema support
    std::vector<ConfigSchema> nestedSchema;
};

// Main configuration class with JSON support
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
    
    // Load from environment variables
    bool loadFromEnv(const std::string& prefix = "NLM_");
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Advanced type-safe getters with nested support
    template<typename T>
    std::optional<T> get(const std::vector<std::string>& path) const;
    
    template<typename T>
    T getOr(const std::vector<std::string>& path, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set nested value
    bool setNested(const std::vector<std::string>& path, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    bool hasNested(const std::vector<std::string>& path) const;
    
    // Remove key
    void remove(const std::string& key);
    void removeNested(const std::vector<std::string>& path);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::vector<std::string>> getNestedPaths() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Schema validation
    bool validateSchema(const std::vector<ConfigSchema>& schema) const;
    void addSchema(const std::vector<ConfigSchema>& schema);
    bool validateValue(const ConfigSchema& schema, const ConfigValue& value) const;
    
    // Configuration path utilities
    static std::vector<std::string> parsePath(const std::string& pathStr);
    static std::string joinPath(const std::vector<std::string>& path);
    
    // Get value as JSON for debugging
    nlohmann::json toJson() const;
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonStr);
    
    // Get all configuration entries
    std::vector<ConfigEntry> getAllEntries() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Convert ConfigValue to JSON
    static nlohmann::json configValueToJson(const ConfigValue& value);
    
    // Convert JSON to ConfigValue
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& json, const std::string& expectedType);
    
    // Convert string to specific type
    template<typename T>
    static std::optional<T> stringToValue(const std::string& str);
    
    // Template specializations for conversion
    template<>
    static std::optional<int> stringToValue(const std::string& str) {
        try {
            size_t pos;
            int val = std::stoi(str, &pos);
            if (pos != str.size()) return std::nullopt;
            return val;
        } catch (...) {
            return std::nullopt;
        }
    }
    
    template<>
    static std::optional<double> stringToValue(const std::string& str) {
        try {
            size_t pos;
            double val = std::stod(str, &pos);
            if (pos != str.size()) return std::nullopt;
            return val;
        } catch (...) {
            return std::nullopt;
        }
    }
    
    template<>
    static std::optional<bool> stringToValue(const std::string& str) {
        if (str == "true" || str == "1" || str == "yes" || str == "on") return true;
        if (str == "false" || str == "0" || str == "no" || str == "off") return false;
        return std::nullopt;
    }
};

} // namespace nlm
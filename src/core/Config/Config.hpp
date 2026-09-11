#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <limits>
#include <nlohmann/json.hpp>

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
    std::string type;  // JSON type string (optional)
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const std::string& t = "")
        : key(k), value(v), source(s), description(desc), type(t) {}
};

// Configuration schema definition
struct ConfigSchemaEntry {
    std::string key;
    std::string type;  // "int", "double", "bool", "string", "array"
    std::string description;
    std::string defaultValue;
    bool required = false;
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    std::vector<std::string> allowedValues;
};

// Type checking utilities
namespace ConfigUtils {
    bool isNumberType(const ConfigValue& value);
    bool isStringType(const ConfigValue& value);
    bool isBoolType(const ConfigValue& value);
    bool isArrayType(const ConfigValue& value);
    
    std::string getTypeString(const ConfigValue& value);
    
    // JSON conversion helpers
    ConfigValue fromJson(const nlohmann::json& j);
    nlohmann::json toJson(const ConfigValue& value);
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get JSON string representation
    std::string toJsonString() const;
    
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
    
    // Validation
    std::vector<std::string> validate(const std::vector<ConfigSchemaEntry>& schema) const;
    
    // Apply schema with defaults
    void applySchema(const std::vector<ConfigSchemaEntry>& schema);
    
    // Set default schema
    void setDefaultSchema(const std::vector<ConfigSchemaEntry>& schema);
    
    // Get default schema
    std::vector<ConfigSchemaEntry> getDefaultSchema() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

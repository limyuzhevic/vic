#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
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

// Configuration validation error
struct ValidationError {
    std::string key;
    std::string message;
    
    ValidationError(const std::string& k, const std::string& m) : key(k), message(m) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string description;
    ConfigValue defaultValue;
    std::function<bool(const ConfigValue&)> validator;
    bool required;
    std::string typeName;
    
    ConfigSchemaEntry() : required(false) {}
    ConfigSchemaEntry(const std::string& k, const std::string& d, const ConfigValue& def,
                      const std::function<bool(const ConfigValue&)>& val, bool req, const std::string& type)
        : key(k), description(d), defaultValue(def), validator(val), required(req), typeName(type) {}
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
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
    
    // Validate all configuration values
    std::vector<ValidationError> validate() const;
    
    // Get configuration schema
    const std::map<std::string, ConfigSchemaEntry>& getSchema() const { return schema; }
    
    // Add schema entry (for internal use)
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    
    // Load default configuration from file
    bool loadDefaults(const std::string& filepath = "configs/default.cfg");
    
    // Convert ConfigValue to JSON
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    
    // Convert JSON to ConfigValue
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& json, const std::string& key = "");
    
    // Error recovery - fix invalid values
    static bool fixInvalidValue(ConfigValue& value, const std::string& key = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::map<std::string, ConfigSchemaEntry> schema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getTypeName(const ConfigValue& value);
    
    // Type conversion helpers
    static std::optional<ConfigValue> stringToConfigValue(const std::string& str, const std::string& key = "");
    static std::string configValueToString(const ConfigValue& value);
    
    // Validation helpers
    static bool validateInt(const ConfigValue& value, const ConfigSchemaEntry& schema);
    static bool validateDouble(const ConfigValue& value, const ConfigSchemaEntry& schema);
    static bool validateBool(const ConfigValue& value, const ConfigSchemaEntry& schema);
    static bool validateString(const ConfigValue& value, const ConfigSchemaEntry& schema);
    static bool validateVector(const ConfigValue& value, const ConfigSchemaEntry& schema);
    
    // Initialize default schema
    void initializeDefaultSchema();
};

} // namespace nlm

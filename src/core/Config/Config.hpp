#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

// JSON support via pybind11
namespace pybind11 {
    template<typename T>
    class class_

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

// Configuration schema entry for type safety
struct ConfigSchemaEntry {
    std::string key;
    std::string type;  // "int", "double", "bool", "string", "array<int>", etc.
    std::string description;
    std::variant<int, double, bool, std::string> defaultValue;
    bool required = false;
    
    ConfigSchemaEntry() = default;
    ConfigSchemaEntry(const std::string& k, const std::string& t, const std::string& desc = "")
        : key(k), type(t), description(desc), required(false) {}
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
    
    // Load from file (JSON or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Convert to JSON string
    std::string toJson() const;
    
    // Parse JSON string and load
    bool parseJson(const std::string& jsonStr);
    
    // Get values with type safety
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
    
    // Type-safe setters
    template<typename T>
    void setTyped(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // JSON validation and conversion
    static bool isValidJson(const std::string& jsonStr);
    static std::string toJson(const ConfigEntry& entry);
    static bool fromJson(const std::string& jsonStr, ConfigEntry& entry);
    
    // Schema management
    void addSchema(const ConfigSchemaEntry& schema);
    bool validateSchema() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<ConfigSchemaEntry> schema;
    mutable std::vector<std::string> validationErrors;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string jsonValueToString(const ConfigValue& value);
    static bool stringToJsonValue(const std::string& str, ConfigValue& value);
    
    // JSON helpers
    static nlohmann::json configToJson(const ConfigEntry& entry);
    static bool jsonToConfig(const nlohmann::json& json, ConfigEntry& entry);
};

} // namespace nlm

} // namespace nlm

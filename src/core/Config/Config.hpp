#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <any>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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

// Type conversion utilities
class ConfigConverter {
public:
    static bool stringToValue(const std::string& str, ConfigValue& out);
    static std::string valueToString(const ConfigValue& value);
    static bool isNumericType(const ConfigValue& value);
    static std::string getTypeName(const ConfigValue& value);
};

// Configuration validation
class ConfigValidator {
public:
    static bool validate(const Config& config);
    static std::vector<std::string> getValidationErrors(const Config& config);
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
    
    // Load from file (JSON/YAML/simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr);
    
    // Load from simple key=value format (legacy)
    bool loadFromSimpleFormat(const std::string& content);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Export/import
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced getters for common types
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
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
    
    // Configuration validation
    bool validate(const ConfigSchema& schema) const;
    std::vector<std::string> validate(const ConfigSchema& schema, std::string& errorMsg) const;
    
    // Add description to entries
    void addDescription(const std::string& key, const std::string& description);
    
    // Statistics
    size_t getEntryCount() const;
    size_t getEntryCountBySource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // File format detection and parsing
    bool parseFileFormat(const std::string& filepath);
    bool parseJSONFile(std::ifstream& file);
    bool parseYAMLFile(std::ifstream& file);
    bool parseSimpleFormat(std::ifstream& file);
    
    // String conversion utilities
    static ConfigValue parseSimpleLine(const std::string& line);
    static std::string serializeConfigValue(const ConfigValue& value);
};

// Configuration schema for validation
class ConfigSchema {
public:
    struct SchemaEntry {
        std::string key;
        std::string type;
        std::string description;
        std::any defaultValue;
        bool required;
        std::function<bool(const ConfigValue&)> validator;
        
        SchemaEntry() : key(), type("unknown"), description(), required(false) {}
        SchemaEntry(const std::string& k, const std::string& t, const std::string& desc = "", 
                   const std::any& def = nullptr, bool req = false)
            : key(k), type(t), description(desc), defaultValue(def), required(req) {}
    };
    
    std::vector<SchemaEntry> entries;
    
    void addEntry(const std::string& key, const std::string& type, 
                  const std::string& description = "", 
                  const std::any& defaultValue = nullptr, 
                  bool required = false) {
        entries.emplace_back(key, type, description, defaultValue, required);
    }
    
    bool validate(const Config& config) const;
    std::vector<std::string> getErrors(const Config& config) const;
};

} // namespace nlm

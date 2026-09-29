#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <chrono>

namespace nlm {

// Configuration value types for JSON/YAML parsing
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::map<std::string, ConfigValue>  // JSON objects
>;

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry with validation support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    std::string validationPattern;
    std::string validationErrorMessage;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), required(false),
          validationPattern(), validationErrorMessage() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), required(req),
          validationPattern(), validationErrorMessage() {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::string type;  // "int", "double", "string", "bool", "array", "object"
    std::string description;
    std::optional<ConfigValue> defaultValue;
    bool required;
    std::string validationPattern;
    std::map<std::string, ConfigSchema> properties;  // For objects
    std::vector<ConfigSchema> items;  // For arrays
    double min;  // For numeric types
    double max;  // For numeric types
    int minItems;  // For arrays
    int maxItems;  // For arrays
    
    ConfigSchema() : required(false), min(0.0), max(0.0), minItems(0), maxItems(0) {}
};

// Main configuration system with JSON/YAML support
class ConfigSystem {
public:
    ConfigSystem();
    ~ConfigSystem();
    
    // Disable copying, enable moving
    ConfigSystem(const ConfigSystem&) = delete;
    ConfigSystem& operator=(const ConfigSystem&) = delete;
    ConfigSystem(ConfigSystem&&) noexcept;
    ConfigSystem& operator=(ConfigSystem&&) noexcept;
    
    // Load from JSON/YAML file
    bool loadFromFile(const std::string& filepath, bool isYAML = false);
    
    // Load from JSON/YAML string
    bool loadFromString(const std::string& content, bool isYAML = false);
    
    // Save as JSON
    bool saveToJSON(const std::string& filepath) const;
    
    // Save as YAML
    bool saveToYAML(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "");
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Configuration schema management
    void addSchema(const std::string& key, const ConfigSchema& schema);
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration merge
    void merge(const ConfigSystem& other, bool overwriteExisting = true);
    
    // Command line argument loading
    bool loadFromArgs(int argc, char** argv);
    
    // Environment variable loading
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration entry for inspection
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Configuration validation
    bool validateEntry(const ConfigEntry& entry) const;
    std::string validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Get source information
    ConfigSource getSource(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal JSON/YAML parsing helpers
    bool parseJSON(const std::string& content, ConfigValue& root);
    bool parseYAML(const std::string& content, ConfigValue& root);
    std::string serializeJSON() const;
    std::string serializeYAML() const;
    
    // Schema validation helpers
    bool validateSchema(const ConfigValue& value, const ConfigSchema& schema) const;
    bool validateAgainstSchema(const std::string& key) const;
    
    // Configuration entry management
    void addEntry(const ConfigEntry& entry);
    bool hasEntry(const std::string& key) const;
    const ConfigEntry& getEntryInternal(const std::string& key) const;
    
    // Configuration source management
    static std::string sourceToString(ConfigSource source);
};

} // namespace nlm
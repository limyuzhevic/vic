#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <filesystem>
#include <functional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

// Custom exceptions for configuration errors
class ConfigException : public std::runtime_error {
public:
    explicit ConfigException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigParseException : public ConfigException {
public:
    explicit ConfigParseException(const std::string& message) : ConfigException(message) {}
};

class ConfigValidationException : public ConfigException {
public:
    explicit ConfigValidationException(const std::string& message) : ConfigException(message) {}
};

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
    std::vector<std::string> dependsOn;  // Keys that must be set before this one
    bool validated;                     // Whether this entry has passed validation
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), dependsOn(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::vector<std::string>& deps = {})
        : key(k), value(v), source(s), description(desc), dependsOn(deps), validated(false) {}
};

// Configuration validation function type
using ValidationFunc = std::function<bool(const ConfigValue&)>;

// Configuration range specification for validation
struct ConfigRange {
    enum class Type { Min, Max, MinMax, InclusiveMin, InclusiveMax };
    
    Type type;
    double value;
    
    ConfigRange(Type t, double v) : type(t), value(v) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    ConfigRange range;
    ValidationFunc validator;
    std::string validationMessage;
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
    
    // Load from YAML file
    bool loadFromYamlFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJsonFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYamlString(const std::string& yamlString);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Save to JSON file
    bool saveToJsonFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Save to YAML file
    bool saveToYamlFile(const std::string& filepath, bool prettyPrint = false) const;
    
    // Get values with type-safe accessors
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-specific getters
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<int> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::vector<std::string>> getStringArray(const std::string& key) const;
    std::optional<std::vector<int>> getIntArray(const std::string& key) const;
    std::optional<std::vector<double>> getDoubleArray(const std::string& key) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Type-specific setters
    bool set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    bool validate(const std::string& key);
    bool validateAll();
    
    // Schema management
    void addSchemaEntry(const std::string& key, const ConfigRange& range,
                       ValidationFunc validator, const std::string& message = "");
    void removeSchemaEntry(const std::string& key);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get entries by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration inheritance
    bool loadFromFileWithInheritance(const std::string& filepath);
    bool loadFromYamlFileWithInheritance(const std::string& filepath);
    bool loadFromJsonFileWithInheritance(const std::string& filepath);
    
    // File-based configuration with includes
    void setIncludeDirectory(const std::string& directory);
    bool loadFromFileWithIncludes(const std::string& filepath);
    
    // Configuration merging
    void merge(const Config& other);
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    
    // Get configuration statistics
    struct Stats {
        size_t totalEntries;
        size_t fileEntries;
        size_t commandLineEntries;
        size_t runtimeEntries;
        size_t validatedEntries;
    };
    
    Stats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // File parsing helpers
    bool parseJsonFile(const std::string& filepath, std::map<std::string, ConfigValue>& data);
    bool parseYamlFile(const std::string& filepath, std::map<std::string, ConfigValue>& data);
    
    // Validation helpers
    bool validateRange(const ConfigValue& value, const ConfigRange& range) const;
    bool validateCustom(const ConfigValue& value, const ValidationFunc& validator) const;
    
    // Inheritance helpers
    bool loadConfigFile(const std::string& filepath, std::map<std::string, ConfigValue>& data,
                       const std::string& basePath = "");
};
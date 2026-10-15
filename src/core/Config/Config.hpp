#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <chrono>

namespace nlm {

// Enhanced configuration value types with better type safety
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
    std::unordered_map<std::string, std::string> // For nested JSON-like objects
>;

// Enhanced configuration source tracking
enum class ConfigSource {
    Default,      // Hard-coded defaults
    File,         // Loaded from configuration file (JSON/YAML)
    CommandLine,  // Override from command line
    Runtime,      // Set programmatically at runtime
    Environment   // From environment variables
};

// Enhanced configuration entry with metadata and validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string typeHint;  // "int", "float", "string", "bool", "array", "object"
    std::string validationRegex;  // For string validation
    std::optional<double> minValue;  // For numeric validation
    std::optional<double> maxValue;  // For numeric validation
    bool required;            // Whether this key is required
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), typeHint(),
          validationRegex(), minValue(), maxValue(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& type = "",
                const std::string& regex = "", const std::optional<double>& min = std::nullopt,
                const std::optional<double>& max = std::nullopt, bool req = false)
        : key(k), value(v), source(s), description(desc), typeHint(type),
          validationRegex(regex), minValue(min), maxValue(max), required(req) {}
    
    // Type checking helper
    bool isType(const std::string& expectedType) const;
    
    // Value validation
    bool validate() const;
    
    // Get value as specific type with error handling
    template<typename T>
    std::optional<T> getValueAs() const;
    
    // Set value with type conversion
    void setValue(const ConfigValue& newValue, ConfigSource source);
};

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "float", "string", "bool", "array", "object"
    bool required = false;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::string validationRegex;
    std::string description;
    
    // Type validation function
    std::function<bool(const ConfigValue&)> validator;
};

// Main configuration class with schema support and multi-format loading
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from multiple file formats (JSON/YAML/TOML)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonString);
    bool loadFromYAML(const std::string& yamlString);
    bool loadFromTOML(const std::string& tomlString);
    
    // Load from command line arguments (enhanced)
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file in multiple formats
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToTOML(const std::string& filepath) const;
    
    // Get values with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type conversion and validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced type-specific setters
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setArray(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and type
    bool has(const std::string& key) const;
    std::string getType(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Configuration schema and validation
    bool validate() const;
    bool validateSchema() const;
    
    // Get configuration summary
    std::string summary() const;
    
    // Get entries by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration debugging
    std::string debugInfo() const;
    
    // Set default values programmatically
    void setDefault(const std::string& key, const ConfigValue& value, const std::string& description = "", const std::string& type = "");
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    bool hasSchema(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    bool validateEntry(const ConfigEntry& entry) const;
    std::string getDefaultDescription(const std::string& key) const;
    std::string valueToString(const ConfigValue& value) const;
};

} // namespace nlm

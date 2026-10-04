#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>

// JSON library
#include <nlohmann/json.hpp>

// YAML library
#include <yaml-cpp/yaml.h>

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

// Schema validation rule
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Configuration file format enum
enum class ConfigFileFormat {
    Unknown,
    KeyValue,
    JSON,
    YAML
};

// Schema for configuration validation
struct ConfigSchema {
    std::vector<ValidationRule> validationRules;
    std::unordered_map<std::string, ConfigValue> defaultValues;
    bool validateOnLoad = false;
    bool validateOnSet = true;
    
    void addValidationRule(const std::string& key, 
                          std::function<bool(const ConfigValue&)> validator,
                          const std::string& errorMessage = "") {
        validationRules.emplace_back(key, validator, errorMessage);
    }
    
    void setDefaultValue(const std::string& key, const ConfigValue& value) {
        defaultValues[key] = value;
    }
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
    
    // Load from file with format detection
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with explicit format
    bool loadFromFile(const std::string& filepath, ConfigFileFormat format);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with automatic format detection
    bool saveToFile(const std::string& filepath) const;
    
    // Save to file with explicit format
    bool saveToFile(const std::string& filepath, ConfigFileFormat format) const;
    
    // Save as JSON
    bool saveToJSON(const std::string& filepath) const;
    
    // Save as YAML
    bool saveToYAML(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
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
    
    // Format detection
    static ConfigFileFormat detectFormatFromExtension(const std::string& filepath);
    
    // Schema validation
    void setSchema(const ConfigSchema& schema) { schema_ = schema; }
    const ConfigSchema& getSchema() const { return schema_; }
    bool validate() const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Apply defaults from schema
    void applyDefaults();
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Private methods for format-specific parsing
    bool parseKeyValue(const std::string& content);
    bool parseJSON(const nlohmann::json& jsonData);
    bool parseYAML(const YAML::Node& yamlData);
    
    // Private methods for format-specific serialization
    std::string toKeyValue() const;
    nlohmann::json toJSON() const;
    YAML::Node toYAML() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema_;
    
    // Helper functions for type conversion
    static ConfigValue toConfigValue(int value) { return value; }
    static ConfigValue toConfigValue(int64_t value) { return value; }
    static ConfigValue toConfigValue(double value) { return value; }
    static ConfigValue toConfigValue(bool value) { return value; }
    static ConfigValue toConfigValue(const std::string& value) { return value; }
    static ConfigValue toConfigValue(const std::vector<int>& value) { return value; }
    static ConfigValue toConfigValue(const std::vector<double>& value) { return value; }
    static ConfigValue toConfigValue(const std::vector<std::string>& value) { return value; }
};

} // namespace nlm

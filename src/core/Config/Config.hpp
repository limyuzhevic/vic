#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;

// JSON/YAML schema validation types
struct JSONSchema {
    std::string type; // "string", "number", "integer", "boolean", "array", "object"
    std::optional<std::string> pattern;
    std::optional<std::string> description;
    std::optional<double> min;
    std::optional<double> max;
    std::optional<int> minLength;
    std::optional<int> maxLength;
    std::optional<std::vector<JSONSchema>> items;
    std::optional<std::unordered_map<std::string, JSONSchema>> properties;
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
    Runtime,
    Inherited,
    EnvironmentVariable
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::string filePath;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   timestamp(std::chrono::system_clock::now()), filePath() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", 
                const std::string& fp = "") 
        : key(k), value(v), source(s), description(desc), 
          timestamp(std::chrono::system_clock::now()), filePath(fp) {}
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
    
    // Load from file (JSON or YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr);
    
    // Load from JSON file with schema validation
    bool loadFromJSONWithValidation(const std::string& filepath, const JSONSchema& schema);
    
    // Load from YAML file with schema validation
    bool loadFromYAMLWithValidation(const std::string& filepath, const JSONSchema& schema);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save as JSON
    std::string saveToJSON() const;
    
    // Save as YAML
    std::string saveToYAML() const;
    
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
    
    // Environment variable substitution
    bool resolveEnvironmentVariables();
    
    // Configuration inheritance
    bool inheritFrom(const std::string& parentConfigPath);
    
    // Schema validation
    bool validate(const JSONSchema& schema, std::vector<std::string>& errors) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get entries by source
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
    // Get configuration summary
    std::string summary() const;
    
    // JSON/YAML format detection
    static std::string detectFormat(const std::string& filepath);
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string formatValue(const ConfigValue& value) const;
    static ConfigValue parseValue(const std::string& valueStr) const;
    
    // JSON/YAML conversion helpers
    static ConfigValue jsonToConfigValue(const nlohmann::json& jsonVal);
    static nlohmann::json configValueToJSON(const ConfigValue& configVal) const;
    static ConfigValue yamlToConfigValue(const YAML::Node& yamlNode);
    static YAML::Node configValueToYAML(const ConfigValue& configVal) const;
    
    // Schema validation helpers
    static bool validateSchema(const JSONSchema& schema, const std::string& key, const ConfigValue& value, std::vector<std::string>& errors);
    
    // Environment variable substitution helpers
    static std::string substituteEnvironmentVariables(const std::string& str);
    static bool isEnvironmentVariable(const std::string& str);
    static std::string getEnvironmentVariableValue(const std::string& varName);
};

} // namespace nlm

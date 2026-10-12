#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <any>

// JSON and YAML includes
#include <nlohmann/json.hpp>
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
    Runtime,
    JSON,
    YAML
};

// JSON/YAML parsing errors
struct ConfigParserError {
    std::string message;
    std::string source;
    size_t line = 0;
    size_t column = 0;
};

// JSON schema for validation
struct JSONSchema {
    std::string type;
    std::string description;
    std::map<std::string, JSONSchema> properties;
    bool required = false;
    std::vector<std::string> requiredFields;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string filePath;
    size_t lineNumber = 0;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), filePath(), lineNumber(0) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& fp = "", size_t ln = 0)
        : key(k), value(v), source(s), description(desc), filePath(fp), lineNumber(ln) {}
};

// Helper function to convert ConfigEntry to JSON
nlohmann::json configEntryToJSON(const ConfigEntry& entry);

// Helper function to convert JSON to ConfigEntry
ConfigEntry jsonToConfigEntry(const nlohmann::json& jsonData);

// Helper function to convert ConfigEntry to YAML
YAML::Node configEntryToYAML(const ConfigEntry& entry);

// Helper function to convert YAML to ConfigEntry
ConfigEntry yamlToConfigEntry(const YAML::Node& yamlData);

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
    
    // Load from file (JSON/YAML format auto-detected)
    bool loadFromFile(const std::string& filepath);
    
    // Load from string (JSON/YAML format auto-detected)
    bool loadFromString(const std::string& data, const std::string& format = "auto");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON/YAML format)
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
    // Load from JSON
    bool loadFromJSONFile(const std::string& filepath);
    bool loadFromJSONString(const std::string& jsonString);
    std::string toJSON() const;
    bool toJSONFile(const std::string& filepath) const;
    
    // Load from YAML
    bool loadFromYAMLFile(const std::string& filepath);
    bool loadFromYAMLString(const std::string& yamlString);
    std::string toYAML() const;
    bool toYAMLFile(const std::string& filepath) const;
    
    // Static factory methods
    static std::shared_ptr<Config> fromJSON(const std::string& jsonString);
    static std::shared_ptr<Config> fromYAML(const std::string& yamlString);
    static std::shared_ptr<Config> mergeJSON(const std::shared_ptr<Config>& base, 
                                           const std::string& jsonString);
    static std::shared_ptr<Config> mergeYAML(const std::shared_ptr<Config>& base,
                                           const std::string& yamlString);
    
    // Comparison and analysis
    bool compareJSON(const std::string& jsonString) const;
    std::string diffJSON(const std::string& jsonString) const;
    std::string getJSONSchema() const;
    
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
    
    // Get all entries
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration as JSON string
    std::string exportJSON() const;
    
    // Get configuration as YAML string
    std::string exportYAML() const;
    
    // Format detection
    static std::string detectFormat(const std::string& filepath);
    static bool isJSON(const std::string& data);
    static bool isYAML(const std::string& data);
    static bool isSimple(const std::string& data);
    
    // Error handling
    std::vector<ConfigParserError> getParserErrors() const;
    void clearParserErrors();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseSimpleFormat(const std::string& data);
    static std::string convertValueToString(const ConfigValue& value);
    static ConfigValue convertStringToValue(const std::string& str, const std::string& typeHint = "");
    
    // JSON/YAML helpers
    bool parseJSON(const nlohmann::json& jsonData);
    bool parseYAML(const YAML::Node& yamlData);
    nlohmann::json exportToJSON() const;
    YAML::Node exportToYAML() const;
};

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <any>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <exception>
#include <iostream>
#include <unordered_set>
#include <set>

// Forward declare JSON and YAML libraries
namespace nlohmann {
    template<typename T> class basic_json;
    using json = basic_json<>;
}

namespace YAML {
    class Node;
}

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

// Configuration value types - Enhanced for nested structures
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<std::any>,
    std::unordered_map<std::string, ConfigValue>,  // Nested object 1
    std::map<std::string, ConfigValue>             // Nested object 2 (sorted)
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

// Configuration schema validation types
enum class ValidationRule {
    Required,
    Optional,
    Range,
    Pattern,
    Custom
};

struct ValidationError {
    std::string path;
    std::string message;
    ValidationRule rule;
    
    ValidationError(const std::string& p, const std::string& m, ValidationRule r)
        : path(p), message(m), rule(r) {}
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
    
    // Load from file (JSON/YAML format with fallback)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from key=value format (backward compatibility)
    bool loadFromKeyValue(const std::string& content);
    
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
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Nested configuration support
    bool hasNested(const std::vector<std::string>& keys) const;
    std::optional<ConfigValue> getNested(const std::vector<std::string>& keys) const;
    void setNested(const std::vector<std::string>& keys, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    void removeNested(const std::vector<std::string>& keys);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getNestedKeys(const std::string& parentKey) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // JSON/YAML serialization/deserialization
    static std::string toJson(const ConfigValue& value);
    static std::string toYaml(const ConfigValue& value);
    static std::optional<ConfigValue> fromJson(const std::string& jsonStr);
    static std::optional<ConfigValue> fromYaml(const std::string& yamlStr);
    
    // Configuration validation
    std::vector<ValidationError> validate() const;
    void setSchema(const ConfigSchema& schema);
    
    // Backward compatibility helpers
    static bool isKeyValueFormat(const std::string& content);
    static bool isJsonFormat(const std::string& content);
    static bool isYamlFormat(const std::string& content);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string detectFormat(const std::string& content);
    static std::optional<ConfigValue> parseValue(const std::string& str);
    static std::string valueToString(const ConfigValue& value) const;
    static std::string configEntryToString(const ConfigEntry& entry) const;
    static void collectValidationErrors(const std::string& path, const ConfigValue& value, std::vector<ValidationError>& errors) const;
    
    // JSON/YAML helper functions
    static std::string jsonToString(const nlohmann::json& jsonValue);
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& jsonValue);
    
    static std::string yamlToString(const YAML::Node& yamlNode);
    static YAML::Node configValueToYaml(const ConfigValue& value);
    static std::optional<ConfigValue> yamlToConfigValue(const YAML::Node& yamlNode);
};

} // namespace nlm

// Implementation headers
#include "ConfigImpl.hpp"
#include "ConfigSchema.hpp"
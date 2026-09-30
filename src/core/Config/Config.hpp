#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cmath>
#include "ConfigException.hpp"

// JSON support
#ifdef __cplusplus
#include <nlohmann/json.hpp>
#endif

// YAML support
#ifdef __cplusplus
#include <yaml-cpp/yaml.h>
#endif

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
    std::vector<std::string>,
    std::vector<bool>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation rules
typedef std::function<bool(const ConfigValue&)> ValidationRule;

struct ConfigValidation {
    std::string name;
    std::string description;
    std::vector<ValidationRule> rules;
    std::vector<std::string> dependentKeys; // Keys that affect validation
    
    ConfigValidation() : name(), description(), rules(), dependentKeys() {}
    ConfigValidation(const std::string& name, const std::string& desc)
        : name(name), description(desc) {}
    
    ConfigValidation& addRule(const ValidationRule& rule) {
        rules.push_back(rule);
        return *this;
    }
    
    ConfigValidation& addDependentKey(const std::string& key) {
        dependentKeys.push_back(key);
        return *this;
    }
    
    bool validate(const ConfigValue& value, const std::map<std::string, ConfigValue>& context = {}) const {
        for (const auto& rule : rules) {
            if (!rule(value)) {
                return false;
            }
        }
        return true;
    }
};

// Configuration entry for schema validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable bool validated; // Cached validation status
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validated(false) {}
};

// Schema for configuration validation
typedef std::map<std::string, ConfigValidation> ConfigSchema;

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Schema and validation
    void addSchema(const ConfigSchema& schema);
    bool validate(const std::map<std::string, ConfigValue>& context = {}) const;
    
    // Load from file (supports JSON, YAML, and simple key=value formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<bool>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Statistics and diagnostics
    struct Stats {
        size_t totalEntries;
        size_t memoryUsage;
        std::map<std::string, size_t> typeCounts;
        std::map<std::string, double> averageValues;
    };
    Stats getStats() const;
    
    // Schema introspection
    std::vector<std::string> getValidationRules() const;
    std::string getKeyDocumentation(const std::string& key) const;
    std::map<std::string, std::string> getAllDocumentation() const;
    
    // Help and documentation
    std::string help() const;
    
    // Size and access methods
    size_t size() const;
    
    // Iteration support
    struct Item {
        std::string key;
        ConfigValue value;
    };
    std::vector<Item> items() const;
    std::vector<ConfigValue> values() const;
    
    // Internal access methods (for Python bindings)
    const std::vector<ConfigEntry>& getEntries() const;
    const ConfigSchema& getSchema() const;
    
    // Helper functions
    static bool validateValue(const ConfigValue& value, ConfigSource source);
    static std::string formatError(const std::string& key, const std::string& expectedType, const std::string& actualType);
    
    // Parse functions
    static std::optional<ConfigValue> parseSimpleValue(const std::string& str);
    static bool saveValueToStream(std::ostream& out, const ConfigValue& value);
    
    // JSON/YAML parsing functions
    static std::optional<ConfigValue> parseJsonValue(const nlohmann::json& json);
    static std::optional<ConfigValue> parseYamlValue(const YAML::Node& yaml);
    static void flattenObject(const std::string& prefix, const nlohmann::json& obj, std::map<std::string, nlohmann::json>& flat);
    static void flattenYamlObject(const std::string& prefix, const YAML::Node& obj, std::map<std::string, YAML::Node>& flat);
    static std::string detectFileFormat(const std::string& filepath);
    static nlohmann::json jsonFromFile(const std::string& filepath);
    static YAML::Node yamlFromFile(const std::string& filepath);
    
    // Serialization/deserialization
    std::string toJson() const;
    bool fromJson(const std::string& json);
    std::map<std::string, ConfigValue> toDict() const;
    bool fromDict(const std::map<std::string, ConfigValue>& d);
    
    // Configuration merging
    void merge(const Config& other);
    
    // Configuration diff
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> diff(const Config& other) const;
};
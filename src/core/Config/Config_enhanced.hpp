// Enhanced NLM Configuration System
// Supports JSON, YAML, and key=value formats with advanced features

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <functional>
#include <regex>

namespace nlm {

// Configuration value types with enhanced support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>,
    std::vector<bool>,
    std::map<std::string, std::string>
>;

// Configuration source tracking
enum class ConfigSource {
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3,
    Environment = 4,
    Auto = 5
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    std::string group;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;  // "int", "double", "string", "bool", "list", "map"
    std::string defaultValue;
    std::string validation;  // Regex pattern for validation
    bool required = false;
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    std::vector<std::string> choices;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry()
        : key(), group(), value(), source(ConfigSource::Default), description(),
          type("string"), defaultValue(), validation(), required(false),
          min(std::numeric_limits<double>::lowest()),
          max(std::numeric_limits<double>::max()), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "",
                const std::string& t = "string", const std::string& def = "",
                bool req = false)
        : key(k), group(), value(v), source(s), description(desc), type(t), defaultValue(def),
          validation(), required(req), timestamp(std::chrono::system_clock::now()) {}
    
    // Helper methods
    bool hasValidValue() const;
    std::string getStringValue() const;
    double getNumericValue() const;
    bool getBoolValue() const;
    bool isValid() const;
    std::string toString() const;
};

// Configuration schema for validation
struct ConfigSchema {
    std::string name;
    std::string description;
    std::map<std::string, ConfigEntry> schema;
    std::vector<std::string> requiredGroups;
    bool strict = false;
    
    ConfigSchema(const std::string& n, const std::string& desc = "")
        : name(n), description(desc), strict(false) {}
    
    void addEntry(const ConfigEntry& entry);
    bool validate(const Config& config) const;
    std::vector<std::string> getValidationErrors(const Config& config) const;
    void setStrict(bool strict) { this->strict = strict; }
};

// Enhanced configuration class with groups, validation, and merging
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from different formats
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromKeyValue(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    bool loadFromJSONString(const std::string& jsonString);
    bool loadFromYAMLString(const std::string& yamlString);
    
    // Save to different formats
    bool saveToFile(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToKeyValue(const std::string& filepath) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Get values with enhanced type conversion
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced setting methods
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration groups
    void setGroup(const std::string& key, const std::string& group);
    std::string getGroup(const std::string& key) const;
    std::vector<std::string> getKeysInGroup(const std::string& group) const;
    std::vector<std::string> getAllGroups() const;
    
    // Configuration management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    
    // Schema validation
    bool validateSchema(const ConfigSchema& schema) const;
    std::vector<std::string> getValidationErrors(const ConfigSchema& schema) const;
    
    // Configuration comparison and merging
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    Config merge(const Config& other) const;
    Config diff(const Config& other) const;
    
    // Configuration summary and documentation
    std::string summary() const;
    std::string summary(const std::string& group) const;
    std::string getDocumentation() const;
    
    // Configuration presets
    bool loadPreset(const std::string& presetName);
    bool savePreset(const std::string& presetName);
    std::vector<std::string> getPresetNames() const;
    
    // Environment-specific configurations
    bool loadEnvironmentConfig(const std::string& environment = "development");
    void setEnvironment(const std::string& environment);
    std::string getEnvironment() const;
    
    // File format detection
    static std::string detectFileFormat(const std::string& filepath);
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLLFile(const std::string& filepath);
    static bool isKeyValueFile(const std::string& filepath);
    
    // Configuration cloning
    Config clone() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string detectFileFormatFromContent(const std::string& content);
    static bool validateConfigValue(const ConfigEntry& entry, const ConfigValue& value);
    static std::string valueToString(const ConfigValue& value);
    static ConfigValue stringToValue(const std::string& str, const std::string& type);
    
    // CLI parsing helpers
    static bool parseCLIArgument(const std::string& arg, std::string& key, std::string& value);
    static std::vector<std::string> parseListValue(const std::string& value);
    static bool parseBooleanValue(const std::string& value);
};

} // namespace nlm

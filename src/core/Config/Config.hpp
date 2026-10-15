#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
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

// Configuration template for advanced users
struct ConfigTemplate {
    std::string name;
    std::string description;
    nlohmann::json schema;        // JSON schema for validation
    nlohmann::json defaultValues; // Default values for template
    
    ConfigTemplate() = default;
    ConfigTemplate(const std::string& n, const std::string& desc, 
                   const nlohmann::json& schema, const nlohmann::json& defaults)
        : name(n), description(desc), schema(schema), defaultValues(defaults) {}
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
    
    // Load from file (JSON, YAML, or .cfg format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (automatic format detection)
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
    
    // Batch operations for advanced users
    bool setMultiple(const std::vector<std::pair<std::string, ConfigValue>>& keyValues, ConfigSource source = ConfigSource::Runtime);
    bool setFromJson(const std::string& jsonString);
    bool setFromYaml(const std::string& yamlString);
    
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
    
    // Template management
    bool addTemplate(const ConfigTemplate& template);
    bool removeTemplate(const std::string& name);
    bool applyTemplate(const std::string& name);
    std::vector<std::string> getTemplateNames() const;
    std::string getTemplateInfo(const std::string& name) const;
    
    // Configuration validation and schema checking
    bool validate(const nlohmann::json& schema) const;
    bool validateAgainstTemplate(const std::string& templateName) const;
    std::string getLastError() const;
    
    // Auto-save functionality for advanced users
    void enableAutoSave(const std::string& filepath, int intervalSeconds = 60);
    void disableAutoSave();
    bool isAutoSaveEnabled() const;
    std::string getAutoSavePath() const;
    
    // Configuration comparison and diff
    bool equals(const Config& other) const;
    std::string diff(const Config& other) const;
    
    // Serialization for network transfer or storage
    std::string toJsonString() const;
    std::string toYamlString() const;
    bool fromJsonString(const std::string& jsonString);
    bool fromYamlString(const std::string& yamlString);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    bool validateValue(const std::string& key, const ConfigValue& value, const nlohmann::json& schema) const;
    nlohmann::json configValueToJson(const ConfigValue& value) const;
    ConfigValue jsonToConfigValue(const nlohmann::json& j) const;
};

} // namespace nlm

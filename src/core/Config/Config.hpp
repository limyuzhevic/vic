#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <functional>

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

// Validation function type
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type; // For schema validation
    std::string validation; // Validation rule string
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), validation() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type(), validation() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc,
                const std::string& t, const std::string& val)
        : key(k), value(v), source(s), description(desc), type(t), validation(val) {}
    
    bool operator==(const ConfigEntry& other) const {
        return key == other.key && value == other.value && source == other.source;
    }
};

// Configuration validation schema
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Add configuration parameter to schema
    void addParameter(const std::string& key, const std::string& type,
                     const std::string& description, const std::string& validation = "",
                     bool required = false, const ConfigValue& defaultValue = {});
    
    // Check if schema is valid
    bool isValid() const;
    
    // Validate configuration against schema
    bool validate(const Config& config, std::vector<std::string>& errors) const;
    
    // Get parameter info
    struct ParameterInfo {
        std::string key;
        std::string type;
        std::string description;
        std::string validation;
        bool required;
        ConfigValue defaultValue;
        bool isValid;
        std::string error;
    };
    
    std::vector<ParameterInfo> getParameterInfo() const;
    
    // Get schema summary
    std::string getSummary() const;
    
    // Reset schema
    void clear();
    
    // Load schema from JSON
    bool loadFromJson(const std::string& filepath);
    bool loadFromJson(const nlohmann::json& json);
    
    // Save schema to JSON
    bool saveToJson(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Default configuration schema
ConfigSchema createDefaultSchema();

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
    
    // Load from file (supports JSON, YAML, and simple key=value formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (auto-detects format from extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to specific formats
    bool saveToJson(const std::string& filepath) const;
    bool saveToYaml(const std::string& filepath) const;
    bool saveToSimpleFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with helper methods
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set methods
    bool setFromString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration validation and management
    bool validate() const;
    bool validate(const ConfigSchema& schema) const;
    std::string getConfigSummary() const;
    
    // Configuration merging
    Config& merge(const Config& other);
    Config& merge(const std::string& jsonFile);
    bool equals(const Config& other) const;
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getFileExtension(const std::string& filename);
    static std::string getCurrentTimestamp();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool loadFromJson(const std::string& filepath);
    bool loadFromYaml(const std::string& filepath);
    bool loadFromJsonInternal(const nlohmann::json& json, ConfigSource source);
    bool loadFromSimpleFile(const std::string& filepath);
    bool setFromJsonValue(const std::string& key, const nlohmann::json& value, 
                         ConfigSource source, std::string& error);
    nlohmann::json convertToJsonValue(const ConfigValue& value) const;
    std::string convertToStringValue(const ConfigValue& value) const;
    
    // Default configuration initialization
    void initializeDefaults();
};

// Json helper class
class ConfigJsonHelper {
public:
    static bool validateJsonSchema(const nlohmann::json& json);
    static std::string getSchemaDescription();
};

} // namespace nlm

} // namespace nlm

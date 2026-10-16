// src/core/Config/Config.hpp
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>
#include "ConfigurationSchema.hpp"
#include "ConfigurationValidator.hpp"

namespace nlm {

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

// Enhanced configuration for nested structures
struct NestedConfigValue {
    std::string name;
    nlohmann::json value;
    std::string description;
    std::vector<NestedConfigValue> children;
    
    NestedConfigValue(const std::string& n, const nlohmann::json& v, const std::string& desc = "")
        : name(n), value(v), description(desc) {}
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
    
    // Enhanced loading from file (JSON/YAML support)
    bool loadFromFile(const std::string& filepath, bool validate = false);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString, bool validate = false);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get nested configuration values
    std::optional<nlohmann::json> getNested(const std::string& section, const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set nested configuration
    void setNested(const std::string& section, const std::string& key, const nlohmann::json& value);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by section
    std::vector<std::string> getKeys(const std::string& section) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get validation report
    std::string getValidationReport() const;
    
    // Set schema for validation
    void setSchema(std::unique_ptr<ConfigurationSchema> schema) {
        validator.setSchema(std::move(schema));
    }
    
    // Enable/disable auto-fix mode
    void setAutoFix(bool enabled) {
        validator.setAutoFix(enabled);
    }
    
    // Validate current configuration
    bool validate(std::string& errorMessage) const;
    
    // Get the validator
    const ConfigurationValidator& getValidator() const { return validator; }
    
    // Runtime modification capabilities
    void mergeConfig(const Config& other);
    void patchConfig(const nlohmann::json& patch);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Enhanced validator
    ConfigurationValidator validator;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Convert between ConfigValue and JSON
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static std::optional<ConfigValue> jsonToConfigValue(const nlohmann::json& value);
    
    // Parse configuration file (support JSON/YAML)
    bool parseConfigFile(const std::string& filepath, nlohmann::json& config);
    bool parseConfigString(const std::string& content, nlohmann::json& config);
    
    // Convert flat config to nested structure
    std::vector<NestedConfigValue> createNestedStructure() const;
};

// JSON serialization support
void to_json(nlohmann::json& j, const ConfigEntry& entry);
void from_json(const nlohmann::json& j, ConfigEntry& entry);
}
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <nlohmann/json.hpp>

namespace nlm {

using json = nlohmann::json;

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
    Environment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validation;
    bool required;
    std::string defaultValue;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                    validation(), required(false), defaultValue() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validation(), required(false), defaultValue() {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::unordered_map<std::string, std::string> typePatterns;  // key -> type regex pattern
    std::unordered_map<std::string, std::string> validationRules;
    std::unordered_map<std::string, std::string> descriptions;
    std::unordered_map<std::string, std::string> defaultValues;
    std::unordered_map<std::string, bool> requiredKeys;
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from YAML file (new method)
    bool loadFromYamlFile(const std::string& filepath, bool validate = true);
    
    // Load from JSON file (new method)
    bool loadFromJsonFile(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_", bool validate = true);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool pretty = true) const;
    
    // Get values (defined here so templates instantiate in every TU)
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
    
    // Set with description and validation
    void set(const std::string& key, const std::string& value, const std::string& description, 
             const std::string& validation = "", ConfigSource source = ConfigSource::Runtime);
    
    // Set default configuration
    static Config createDefaultConfig();
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get raw JSON representation
    json toJson() const;
    
    // Validate configuration
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Get configuration schema
    static ConfigSchema getDefaultSchema();
    
    // Merge with another config
    void merge(const Config& other);
    
    // Copy values from string (e.g., from command line)
    void setFromString(const std::string& keyValuePair);
    
    // Get config value as string
    std::string getAsString(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static std::string detectFileFormat(const std::string& filepath);
    
    // Validation helpers
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    bool validateType(const std::string& key, const ConfigValue& value) const;
    
    // JSON conversion helpers
    static ConfigValue jsonToConfigValue(const json& j);
    static json configValueToJson(const ConfigValue& value);
    
    // Schema validation helpers
    static bool validateAgainstSchema(const json& configData, const ConfigSchema& schema);
};

} // namespace nlm

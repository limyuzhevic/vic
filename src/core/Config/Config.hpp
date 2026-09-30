#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations for JSON and YAML
class ConfigValidator;

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

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) 
        : std::runtime_error(message) {}
};

// Configuration schema definition
struct ConfigSchema {
    using Validator = std::function<bool(const ConfigValue&)>;
    using Converter = std::function<ConfigValue(const std::string&)>;
    
    std::string name;
    std::string description;
    bool required;
    ConfigValue defaultValue;
    Validator validator;
    Converter converter;
    
    ConfigSchema() 
        : name(), description(), required(false), defaultValue(), 
          validator(nullptr), converter(nullptr) {}
    
    ConfigSchema(const std::string& n, const std::string& d, bool r = false)
        : name(n), description(d), required(r), defaultValue(), 
          validator(nullptr), converter(nullptr) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type("auto") {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& t = "auto")
        : key(k), value(v), source(s), description(desc), type(t) {}
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
    
    // Load from file with automatic format detection (JSON, YAML, or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Save to file with automatic format detection (JSON, YAML, or simple key=value format)
    bool saveToFile(const std::string& filepath) const;
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Get values with type conversion
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
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Advanced configuration features
    bool loadFromFileWithInheritance(const std::string& filepath, const std::vector<std::string>& basePaths = {});
    bool mergeFromFile(const std::string& filepath, bool override = true);
    bool substituteVariables();
    bool expandEnvironmentVariables();
    bool validateSchema() const;
    
    // Schema management
    void addSchema(const std::string& key, const ConfigSchema& schema);
    ConfigSchema getSchema(const std::string& key) const;
    bool hasSchema(const std::string& key) const;
    void removeSchema(const std::string& key);
    
    // Format detection
    static std::string detectFileFormat(const std::string& filepath);
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    static bool isKeyValueFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeJsonString(const std::string& input);
    
    // Helper functions for parsing
    bool parseKeyValueFormat(const std::string& filepath, std::vector<ConfigEntry>& entries);
    bool parseJsonFormat(const std::string& filepath, std::vector<ConfigEntry>& entries);
    bool parseYamlFormat(const std::string& filepath, std::vector<ConfigEntry>& entries);
    
    // Helper functions for saving
    bool saveToJson(const std::string& filepath, bool pretty) const;
    bool saveToYaml(const std::string& filepath, bool pretty) const;
    bool saveToKeyValue(const std::string& filepath) const;
    
    // Helper functions for conversion
    static ConfigValue jsonToConfigValue(const nlohmann::json& j);
    static nlohmann::json configToJson(const ConfigValue& value);
    
    // Helper functions for validation
    static bool isValueValid(const ConfigValue& value, const ConfigSchema& schema);
    
    // Helper functions for variable substitution
    static bool substituteInValue(std::string& value);
    static bool expandEnvInValue(std::string& value);
};

} // namespace nlm

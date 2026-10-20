#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
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

// Configuration validation
struct ConfigValidationError {
    std::string key;
    std::string message;
    ConfigValidationError(const std::string& k, const std::string& msg) : key(k), message(msg) {}
};

// Configuration schema definition
struct ConfigSchemaEntry {
    std::string key;
    ConfigValue defaultValue;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    bool required;
    std::string type;
    
    ConfigSchemaEntry(const std::string& k, const ConfigValue& def, const std::string& desc, 
                     const std::function<bool(const ConfigValue&)>& val, bool req, const std::string& typeName)
        : key(k), defaultValue(def), description(desc), validator(val), required(req), type(typeName) {}
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonContent);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlContent);
    
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
    
    // Configuration validation
    std::vector<ConfigValidationError> validate() const;
    void setSchema(const std::vector<ConfigSchemaEntry>& schema);
    void clearSchema();
    
    // Schema-based validation helpers
    static bool isValidValue(const ConfigValue& value, const std::string& type);
    static bool validateString(const ConfigValue& value);
    static bool validateInt(const ConfigValue& value);
    static bool validateDouble(const ConfigValue& value);
    static bool validateBool(const ConfigValue& value);
    static bool validateIntArray(const ConfigValue& value);
    static bool validateDoubleArray(const ConfigValue& value);
    static bool validateStringArray(const ConfigValue& value);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    static std::vector<ConfigValidationError> validateAgainstSchema() const;
    static bool isValueOfType(const ConfigValue& value, const std::string& typeName);
    static std::string valueToString(const ConfigValue& value);
};

} // namespace nlm

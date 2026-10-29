#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <unordered_set>

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

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string description;
    std::string type;  // "int", "double", "bool", "string", "array"
    std::string defaultValue;
    bool required;
    double minValue;
    double maxValue;
    std::vector<std::string> allowedValues;
    std::string validationRegex;
    
    ConfigSchema(const std::string& k, const std::string& desc, const std::string& t = "string",
                 const std::string& def = "", bool req = false, double min = -1e9, double max = 1e9)
        : key(k), description(desc), type(t), defaultValue(def), required(req),
          minValue(min), maxValue(max) {}
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
    
    // Configuration validation helpers
    bool isValidJson(const std::string& json) const;
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    ConfigValue parseJsonValue(const std::string& value) const;
    bool validateConfigEntry(const std::string& key, const ConfigValue& value) const;
    bool validateType(const std::string& key, const ConfigValue& value, const std::string& expectedType) const;
    bool validateRange(const std::string& key, const ConfigValue& value, const ConfigSchema& schema) const;
    bool validateStringWithRegex(const std::string& value, const std::string& regex) const;
    bool loadFromJson(const std::string& jsonContent);
    bool loadFromSimpleFormat(const std::string& content);
};

} // namespace nlm

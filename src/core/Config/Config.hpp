#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
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

// Configuration validation result
struct ConfigValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string path; // Configuration key path for nested configs
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::set<std::string> allowedTypes;
    std::optional<std::string> defaultValue;
    bool required;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    
    ConfigSchema() : key(), required(false), description() {}
    
    ConfigSchema(const std::string& k, const std::set<std::string>& types, 
                 bool req, const std::string& desc,
                 const std::function<bool(const ConfigValue&)>& validator = nullptr)
        : key(k), allowedTypes(types), required(req), description(desc), validator(validator) {}
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
    
    // Save to file (fixed to save actual values)
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
    static ConfigValidationResult validate(const Config& config, const std::vector<ConfigSchema>& schema);
    static bool validateValue(const ConfigValue& value, const std::string& typeName);
    static std::string typeToString(const ConfigValue& value);
    
    // Schema management
    static void registerSchema(const std::string& category, const std::vector<ConfigSchema>& schema);
    static std::vector<ConfigSchema> getSchema(const std::string& category);
    static std::map<std::string, std::vector<ConfigSchema>> getAllSchemas();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str, double& outValue);
    static bool isInteger(const std::string& str, int64_t& outValue);
    static bool isBoolean(const std::string& str, bool& outValue);
    
    // Schema validation helper
    static ConfigValidationResult validateAgainstSchema(const ConfigEntry& entry, const ConfigSchema& schema);
};

} // namespace nlm

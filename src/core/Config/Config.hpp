#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>

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
    std::vector<std::vector<int>>,  // 2D int array
    std::vector<std::vector<double>>,  // 2D double array
    std::vector<std::vector<std::string>>,  // 2D string array
    std::vector<bool>  // vector of booleans
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
    
    // Validation methods
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Schema management
    void addSchema(const std::string& name, const std::string& type,
                   const std::optional<std::string>& defaultValue = std::nullopt,
                   bool required = false, const std::string& validation = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // File parsing methods
    bool parseJSON(const std::string& content);
    bool parseYAML(const std::string& content);
    bool parseSimpleFormat(const std::string& content);
    
    // Schema validation
    struct ConfigSchema {
        std::string name;
        std::string type;  // "int", "double", "string", "bool", "list", "vector"
        std::optional<std::string> defaultValue;
        bool required = false;
        std::string validation;  // Regex pattern for strings
    };
    
    std::vector<ConfigSchema> schema_;
    
    // Thread safety
    mutable std::mutex mutex_;
    
    // Value parsing helpers
    static ConfigValue parseValue(const std::string& str);
    static std::string serializeValue(const ConfigValue& value);
    
    // Entry validation
    std::string validateEntry(const ConfigEntry& entry) const;
    bool validateStringWithRegex(const std::string& str, const std::string& pattern) const;
}; // namespace nlm
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>

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
    Environment,
    Runtime
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    ValidationResult() : valid(true), errorMessage() {}
    ValidationResult(bool v, const std::string& msg) : valid(v), errorMessage(msg) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    ConfigSource defaultSource;
    std::string defaultValue;
    std::string description;
    bool required;
    
    ConfigSchema(const std::string& k = "", ConfigSource src = ConfigSource::Default,
                 const std::string& defVal = "", const std::string& desc = "",
                 bool req = false)
        : key(k), defaultSource(src), defaultValue(defVal), description(desc), required(req) {}
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
    
    // Load from environment variables
    void loadFromEnv(const std::string& prefix = "NLM_");
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
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
    
    // Get configuration summary with validation
    std::string summary() const;
    
    // Validate configuration against schema
    bool validate() const;
    
    // Get configuration schema validation errors
    std::string getValidationErrors() const;
    
    // Add default configuration schema entry
    static void addDefault(const std::string& key, ConfigSource source, const std::string& value, const std::string& description);
    
    // Get all default keys
    static std::vector<std::string> getDefaultKeys();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJSONValue(const std::string& value, ConfigValue& outValue);
    static std::string configValueToString(const ConfigValue& value);
};

} // namespace nlm

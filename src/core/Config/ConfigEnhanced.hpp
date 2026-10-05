#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <any>
#include <stdexcept>
#include <unordered_set>

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
    Runtime,
    Environment
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& key, const std::string& reason)
        : std::runtime_error("Configuration validation error for key '" + key + "': " + reason) {}
};

// Configuration schema entry
struct ConfigSchema {
    std::string key;
    ConfigValue type; // Default value used for type checking
    std::any defaultValue;
    std::string validationRegex;
    std::string description;
    bool required;
    
    ConfigSchema() : key(), type(), defaultValue(), validationRegex(), description(), required(false) {}
    
    ConfigSchema(const std::string& k, const ConfigValue& t, const std::any& def,
                 const std::string& regex = "", const std::string& desc = "",
                 bool req = false)
        : key(k), type(t), defaultValue(def), validationRegex(regex),
          description(desc), required(req) {}
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
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get value with error handling
    template<typename T>
    T getRequired(const std::string& key) const;
    
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
    std::vector<std::string> validate(const std::vector<ConfigSchema>& schema) const;
    
    // Configuration schema management
    static void setDefaultSchema(const std::vector<ConfigSchema>& schema);
    static const std::vector<ConfigSchema>& getDefaultSchema();
    
    // Environment variable utilities
    static bool isEnvironmentVariable(const std::string& key);
    static std::string getEnvironmentVariable(const std::string& key);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Schema validation helpers
    static bool matchesRegex(const std::string& value, const std::string& regex);
    static bool validateValueType(const ConfigValue& value, const ConfigValue& type);
};

// Exception hierarchy
namespace config {
    class ConfigException : public std::runtime_error {
    public:
        ConfigException(const std::string& message) : std::runtime_error(message) {}
    };
    
    class ConfigNotFoundException : public ConfigException {
    public:
        ConfigNotFoundException(const std::string& key)
            : ConfigException("Configuration key not found: '" + key + "'") {}
    };
    
    class ConfigValidationException : public ConfigException {
    public:
        ConfigValidationException(const std::string& key, const std::string& reason)
            : ConfigException("Configuration validation error for key '" + key + "': " + reason) {}
    };
} // namespace config

} // namespace nlm

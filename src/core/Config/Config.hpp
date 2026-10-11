#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <map>

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

// Validation result structure
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v) : valid(v) {}
    ValidationResult(bool v, const std::vector<std::string>& errs) 
        : valid(v), errors(errs) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Validation error exception class
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Configuration schema definition support
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "double", "bool", "string", "vector"
    std::string description;
    bool required;
    
    // Validation parameters
    std::optional<int> min_int;
    std::optional<int> max_int;
    std::optional<double> min_double;
    std::optional<double> max_double;
    std::optional<std::string> pattern;
    std::vector<std::string> allowed_values;
    
    ConfigSchema() : required(false) {}
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc = "", bool req = false)
        : key(k), type(t), description(desc), required(req) {}
};

// Known configuration schema
class ConfigSchemaRegistry {
public:
    static ConfigSchemaRegistry& getInstance() {
        static ConfigSchemaRegistry instance;
        return instance;
    }
    
    void registerSchema(const ConfigSchema& schema) {
        schemas[schema.key] = schema;
    }
    
    std::vector<ConfigSchema> getAllSchemas() const {
        std::vector<ConfigSchema> result;
        result.reserve(schemas.size());
        for (const auto& pair : schemas) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    const ConfigSchema* getSchema(const std::string& key) const {
        auto it = schemas.find(key);
        return it != schemas.end() ? &it->second : nullptr;
    }
    
    bool hasSchema(const std::string& key) const {
        return schemas.find(key) != schemas.end();
    }
    
private:
    ConfigSchemaRegistry() = default;
    std::unordered_map<std::string, ConfigSchema> schemas;
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
    
    // Configuration validation functions
    ValidationResult validate() const;
    ValidationResult validateKey(const std::string& key) const;
    static ValidationResult validateValue(const std::string& key, const ConfigValue& value);
    static ValidationResult validateAgainstSchema(const ConfigSchema& schema, const ConfigValue& value);
    
    // Configuration integrity checks
    static bool checkIntegrity(const std::vector<ConfigEntry>& entries);
    static ValidationResult checkKeyUniqueness(const std::vector<ConfigEntry>& entries);
    
    // Validation utilities
    static std::string getLogLevelValue(const std::string& level);
    static bool isValidBoolean(const std::string& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validate configuration values based on key type
    static ValidationResult validateConfigValue(const std::string& key, const ConfigValue& value);
    
    // Initialize default schema registry
    static void initializeSchemaRegistry();
};

} // namespace nlm

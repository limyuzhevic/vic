#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
struct ConfigSchemaEntry {
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string", "float", "vector_int", "vector_double", "vector_string"
    ConfigSource source; // Required source for this field
    std::string description;
    bool required;
    std::string defaultValue; // Default value if field is missing
    
    ConfigSchemaEntry() : key(), type(), source(ConfigSource::Default), description(), required(false), defaultValue() {}
    ConfigSchemaEntry(const std::string& k, const std::string& t, ConfigSource s, const std::string& desc, bool req = false, const std::string& def = "")
        : key(k), type(t), source(s), description(desc), required(req), defaultValue(def) {}
};

// Configuration validation result
struct ConfigValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : valid(true), errors(), warnings() {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
};

// Configuration class with validation
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
    
    // Configuration validation
    static const std::vector<ConfigSchemaEntry>& getDefaultSchema();
    ConfigValidationResult validate(const std::vector<ConfigSchemaEntry>& schema = getDefaultSchema()) const;
    bool validateField(const std::string& key, const std::string& expectedType) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Schema definition
    static std::vector<ConfigSchemaEntry> defaultSchema;
    
    // Type checking helpers
    static bool checkType(const ConfigValue& value, const std::string& typeName);
    static std::string valueToString(const ConfigValue& value);
};

} // namespace nlm

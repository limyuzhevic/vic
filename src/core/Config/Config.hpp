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
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
};

// Type for type-safe configuration validation
using ConfigValidator = std::function<bool(const ConfigValue&)>;
// Validation results with error messages
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    ValidationResult(bool v = true, const std::string& error = "") 
        : valid(v), errorMessage(error) {}
};
// Default configuration schema definitions
struct ConfigSchema {
    std::string name;
    std::string description;
    ConfigSource defaultSource;
    std::string defaultValue;
    std::string type; // "int", "int64", "double", "bool", "string", "vector<int>", "vector<double>"
    std::string units;
    double minValue;
    double maxValue;
    ConfigValidator validator;
    
    ConfigSchema() : defaultSource(ConfigSource::Default), minValue(-1e100), maxValue(1e100) {}
    
    ConfigSchema(const std::string& name, const std::string& desc, ConfigSource src, 
                const std::string& def, const std::string& type, const std::string& units = "",
                double min = -1e100, double max = 1e100)
        : name(name), description(desc), defaultSource(src), defaultValue(def), 
          type(type), units(units), minValue(min), maxValue(max) {}
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
    
    // Enhanced configuration management
    std::string getDescription(const std::string& key) const;
    ConfigSource getDefaultSource(const std::string& key) const;
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    bool hasDescription(const std::string& key) const;
    std::string getType(const std::string& key) const;
    std::string getUnits(const std::string& key) const;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    std::vector<std::string> getAllSchemaKeys() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Initialize default schema
    void initializeDefaultSchema();
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // String to value conversions (friend functions)
    static ConfigValue stringToValue(const std::string& valueStr, const std::string& type);
    static ValidationResult validateValue(const ConfigSchema& schema, const ConfigValue& value);
};

} // namespace nlm

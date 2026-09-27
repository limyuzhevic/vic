// Config.hpp - Updated with JSON/YAML support and schema validation
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
    std::vector<std::string>,
    std::vector<bool>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    std::string validationPattern;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description()
        , required(false), validationPattern() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false,
                const std::string& pattern = "")
        : key(k), value(v), source(s), description(desc)
        , required(req), validationPattern(pattern) {}
};

// Schema validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs,
                    const std::vector<std::string>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
};

// Type descriptors for schema validation
enum class ConfigType {
    Integer,
    Integer64,
    Float,
    Boolean,
    String,
    IntArray,
    FloatArray,
    StringArray,
    BoolArray,
    Object  // Nested configuration (future extension)
};

struct ConfigSchemaEntry {
    std::string key;
    ConfigType type;
    ConfigValue defaultValue;
    bool required;
    std::string description;
    std::string validationPattern;
    double minValue;
    double maxValue;
    std::vector<ConfigValue> allowedValues;
    
    ConfigSchemaEntry(const std::string& k, ConfigType t, const ConfigValue& def,
                     bool req = false, const std::string& desc = "",
                     const std::string& pattern = "", double min = 0.0,
                     double max = 0.0)
        : key(k), type(t), defaultValue(def), required(req), description(desc)
        , validationPattern(pattern), minValue(min), maxValue(max) {}
};

class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars);
    
    // Save to file (JSON format by default)
    bool saveToFile(const std::string& filepath, bool prettyPrint = true) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and type
    bool has(const std::string& key) const;
    ConfigType getType(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    ValidationResult validate(const std::vector<ConfigSchemaEntry>& schema) const;
    ValidationResult validateAgainst(const Config& referenceConfig) const;
    
    // Schema management
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    void clearSchema();
    std::vector<std::string> getSchemaKeys() const;
    
    // Environment and file path helpers
    static std::string getEnvironmentVariable(const std::string& name);
    static std::string expandPath(const std::string& path);
    static bool fileExists(const std::string& path);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool matchesPattern(const std::string& value, const std::string& pattern);
    static std::string convertToString(const ConfigValue& value);
    static std::string configTypeToString(ConfigType type);
    
    // JSON/YAML parsing helpers
    static bool parseKeyValueLine(const std::string& line, std::string& key, std::string& value);
    static bool validateJSONValue(const std::string& value, ConfigType expectedType);
    static bool validateYAMLScalar(const std::string& value, ConfigType expectedType);
    
    // Schema validation helpers
    static bool validateAgainstSchema(const ConfigEntry& entry, const ConfigSchemaEntry& schema, std::vector<std::string>& errors, std::vector<std::string>& warnings);
    static bool convertConfigValue(const ConfigValue& from, ConfigValue& to, std::vector<std::string>& errors);
};
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
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

// Configuration schema definition
struct ConfigSchema {
    using ValidatorFunc = std::function<bool(const ConfigValue&)>;
    using NormalizerFunc = std::function<ConfigValue(const ConfigValue&)>;
    
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string", "vector<int>", etc.
    ConfigSource source; // Allowed sources for this key
    ConfigValue defaultValue;
    std::string description;
    ValidatorFunc validator;
    NormalizerFunc normalizer;
    bool required;
    std::vector<std::string> allowedValues; // For string types
    double minValue; // For numeric types
    double maxValue; // For numeric types
    
    ConfigSchema() : source(ConfigSource::Runtime), required(false), minValue(-1e9), maxValue(1e9) {}
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
    
    // Get values (defined here so templates instantiate in every TU)
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
    
    // Validation and schema methods
    bool validateSchema() const;
    bool validate(const std::string& key) const;
    ConfigValue normalize(const std::string& key, const ConfigValue& value) const;
    bool addSchema(const ConfigSchema& schema);
    bool removeSchema(const std::string& key);
    std::vector<ConfigSchema> getSchema() const;
    void clearSchema();
    
    // Configuration validation
    bool isValid(const std::string& key) const;
    std::vector<std::string> getInvalidKeys() const;
    
    // Configuration value type conversion
    std::optional<double> toDouble(const std::string& key) const;
    std::optional<int> toInt(const std::string& key) const;
    std::optional<bool> toBool(const std::string& key)�טא类似
    std::optional<std::string> toString(const std::string& key) const;
    
    // Range validation helpers
    static bool validateRange(const ConfigValue& value, double min, double max);
    static bool validateChoices(const ConfigValue& value, const std::vector<std::string>& choices);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Schema management
    struct SchemaImpl;
    std::unique_ptr<SchemaImpl> schemaImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumericType(const ConfigValue& value);
    static bool isStringType(const ConfigValue& value);
    static bool isVectorType(const ConfigValue& value);
    static bool parseValue(const std::string& str, ConfigValue& outValue);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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

// Configuration validation types
struct ValidationError {
    std::string key;
    std::string message;
    std::string expectedType;
    std::string actualType;
    
    ValidationError() : key(), message(), expectedType(), actualType() {}
    ValidationError(const std::string& k, const std::string& msg, 
                   const std::string& expected, const std::string& actual)
        : key(k), message(msg), expectedType(expected), actualType(actual) {}
};

struct ConfigSchema {
    std::string key;
    std::string type;  // "int", "double", "string", "bool", "vector"
    std::optional<std::string> description;
    std::optional<double> minValue;  // For numeric types
    std::optional<double> maxValue;  // For numeric types
    std::optional<size_t> minSize;    // For vector types
    std::optional<size_t> maxSize;    // For vector types
    bool required;
    
    ConfigSchema() : key(), type(), description(), minValue(), maxValue(), 
                     minSize(), maxSize(), required(false) {}
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
    
    // Configuration validation
    std::vector<ValidationError> validate() const;
    bool validate(const std::vector<ConfigSchema>& schema) const;
    bool addSchema(const ConfigSchema& schema);
    std::vector<ConfigSchema> getSchemas() const;
    void clearSchemas();
    
    // Type checking helpers
    static bool isTypeCompatible(const ConfigValue& value, const std::string& type);
    static std::string getValueTypeName(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

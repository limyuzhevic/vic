#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Configuration schema entry for validation
struct ConfigSchemaEntry {
    std::string key;
    std::type_index type;
    bool required;
    std::string description;
    std::vector<std::string> allowedValues;
    
    ConfigSchemaEntry(const std::string& k, const std::type_index& t, bool req = false, 
                     const std::string& desc = "", const std::vector<std::string>& allowed = {})
        : key(k), type(t), required(req), description(desc), allowedValues(allowed) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Disable copying, enable moving
    ConfigSchema(const ConfigSchema&) = delete;
    ConfigSchema& operator=(const ConfigSchema&) = delete;
    ConfigSchema(ConfigSchema&&) noexcept;
    ConfigSchema& operator=(ConfigSchema&&) noexcept;
    
    // Add schema entry
    void addEntry(const std::string& key, const std::type_index& type, bool required = false,
                 const std::string& description = "", const std::vector<std::string>& allowedValues = {});
    
    // Validate configuration against schema
    ValidationResult validate(const Config& config) const;
    
    // Get all required keys
    std::vector<std::string> getRequiredKeys() const;
    
    // Get all keys
    std::vector<std::string> getAllKeys() const;
    
    // Check if key exists in schema
    bool hasKey(const std::string& key) const;
    
    // Get schema description for key
    std::string getDescription(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Configuration validator utilities
class ConfigValidator {
public:
    // Validate value against expected type
    static bool validateValueType(const ConfigValue& value, const std::type_index& expectedType);
    
    // Validate value against allowed values
    static bool validateAllowedValues(const ConfigValue& value, const std::vector<std::string>& allowedValues);
    
    // Convert string to appropriate type
    static bool tryConvertToType(const std::string& str, const std::type_index& targetType, ConfigValue& outValue);
    
    // Get human-readable type name
    static std::string getTypeName(const std::type_index& type);
};

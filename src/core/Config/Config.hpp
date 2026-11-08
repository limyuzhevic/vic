#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <any>
#include <ctime>

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

// Schema validation types
enum class ConfigType {
    Integer,
    Integer64,
    Double,
    Boolean,
    String,
    IntVector,
    DoubleVector,
    StringVector
};

enum class ValidationResult {
    Valid,
    Invalid,
    Missing,
    TypeMismatch,
    OutOfRange
};

// Configuration validation error
struct ValidationError {
    std::string key;
    std::string message;
    ValidationResult result;
};

// Configuration schema entry
struct ConfigSchemaEntry {
    ConfigType type;
    std::string description;
    std::any defaultValue;
    bool required;
    std::vector<std::any> allowedValues;
    std::vector<std::pair<std::any, std::any>> range;
    std::string validationRegex;
};

// Configuration schema class for validation
class ConfigSchema {
public:
    ConfigSchema();
    
    // Add a schema entry
    void addEntry(const std::string& key, ConfigType type, 
                  const std::string& description = "",
                  const std::any& defaultValue = nullptr,
                  bool required = false,
                  const std::vector<std::any>& allowedValues = {},
                  const std::vector<std::pair<std::any, std::any>>& range = {},
                  const std::string& validationRegex = "");
    
    // Validate configuration against schema
    std::vector<ValidationError> validate(const Config& config) const;
    
    // Check if a key exists in schema
    bool hasKey(const std::string& key) const;
    
    // Get type for a key
    ConfigType getType(const std::string& key) const;
    
    // Get default value for a key
    std::any getDefaultValue(const std::string& key) const;
    
    // Get all keys in schema
    std::vector<std::string> getKeys() const;
    
private:
    std::unordered_map<std::string, ConfigSchemaEntry> schema_;
    
    // Validation helper methods
    ValidationResult validateType(const std::string& key, ConfigType expectedType, const std::any& value) const;
    ValidationResult validateRange(const std::string& key, const std::vector<std::pair<std::any, std::any>>& range, const std::any& value) const;
    ValidationResult validateAllowedValues(const std::string& key, const std::vector<std::any>& allowedValues, const std::any& value) const;
    ConfigType inferTypeFromAny(const std::any& value) const;
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
    
    // Schema validation
    static ConfigSchema createDefaultSchema();
    bool validateAgainstSchema(const ConfigSchema& schema) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getCurrentTimestamp();
    static ConfigType inferType(const std::string& value);
    static std::string valueToString(const ConfigValue& value) const;
    
    // File format parsers
    bool loadFromJSON(const std::string& content);
    bool loadFromYAML(const std::string& content);
    bool loadFromSimpleFormat(const std::string& content);
    
    // Simple parser implementations
    bool parseSimpleJSON(const std::string& content);
    bool parseSimpleYAML(const std::string& content);
};

} // namespace nlm

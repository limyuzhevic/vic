#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <vector>
#include <unordered_map>

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

// Configuration validation error
struct ConfigValidationError {
    std::string key;
    std::string message;
    std::string errorCode;
    
    ConfigValidationError(const std::string& k, const std::string& msg, const std::string& code)
        : key(k), message(msg), errorCode(code) {}
};

// Parameter type enumeration
enum class ParameterType {
    Int,
    Double,
    String,
    Bool,
    IntVector,
    DoubleVector,
    StringVector
};

// Parameter information for schema validation
struct ParameterInfo {
    std::string name;
    ParameterType type;
    std::string defaultValue;
    std::string description;
    bool required;
    std::vector<std::string> validValues;
    double minValue;
    double maxValue;
    bool isPositive;
    
    ParameterInfo() : type(ParameterType::String), required(false), 
                      minValue(std::numeric_limits<double>::lowest()),
                      maxValue(std::numeric_limits<double>::max()), isPositive(false) {}
};

// Configuration schema class for validation
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Disable copying, enable moving
    ConfigSchema(const ConfigSchema&) = delete;
    ConfigSchema& operator=(const ConfigSchema&) = delete;
    ConfigSchema(ConfigSchema&&) noexcept;
    ConfigSchema& operator=(ConfigSchema&&) noexcept;
    
    // Add parameter definition
    void addParameter(const std::string& name, const std::string& typeStr, 
                      const std::string& defaultValue, const std::string& description,
                      bool required = false);
    
    void addParameterWithRange(const std::string& name, const std::string& typeStr,
                               double minValue, double maxValue, 
                               const std::string& defaultValue, const std::string& description);
    
    void addParameterWithChoices(const std::string& name, const std::string& typeStr,
                                const std::vector<std::string>& validValues,
                                const std::string& defaultValue, const std::string& description);
    
    // Get parameter info
    const ParameterInfo* getParameter(const std::string& name) const;
    
    // Get all parameters
    std::vector<ParameterInfo> getAllParameters() const;
    
    // Validate configuration against schema
    std::vector<ConfigValidationError> validate(const Config& config) const;
    
    // Load schema from JSON file
    bool loadFromFile(const std::string& filepath);
    
    // Save schema to JSON file
    bool saveToFile(const std::string& filepath) const;
    
    // Get schema summary
    std::string summary() const;
    
    // Get parameter type from string
    static ParameterType stringToParameterType(const std::string& typeStr);
    
    // Get parameter type string
    static std::string parameterTypeToString(ParameterType type);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main enhanced configuration class
class Config {
public:
    Config();
    Config(ConfigSchema& schema);
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
    
    // Set values with validation
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values without validation (for advanced users)
    void setUnsafe(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void setUnsafe(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setUnsafe(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setUnsafe(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setUnsafe(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
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
    std::vector<ConfigValidationError> validate(const ConfigSchema& schema) const;
    bool isValid(const ConfigSchema& schema) const;
    
    // Set schema for this config instance
    void setSchema(ConfigSchema& schema);
    
    // Get schema (const version)
    const ConfigSchema* getSchema() const;
    
    // Helper methods
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    bool validateValue(const std::string& key, const ConfigValue& value, ConfigSource source) const;
    ParameterType inferParameterType(const std::string& value) const;
    bool isValueWithinRange(const std::string& key, const ConfigValue& value) const;
    bool isValueInValidChoices(const std::string& key, const ConfigValue& value) const;
};

} // namespace nlm

// Template implementations for convenience
namespace nlm {
    template<typename T>
    inline std::optional<T> Config::get(const std::string& key) const {
        // Implementation would go here
        return std::nullopt;
    }
    
    template<typename T>
    inline T Config::getOr(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
}

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <set>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::string key;
    
    ValidationResult(bool v = true, const std::string& error = "", const std::string& key = "")
        : valid(v), errorMessage(error), key(key) {}
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

// Configuration schema constraint
struct ConfigConstraint {
    std::string key;
    std::function<ValidationResult(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ConfigConstraint(const std::string& k, std::function<ValidationResult(const ConfigValue&)> val, const std::string& err)
        : key(k), validator(val), errorMessage(err) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema();
    ~ConfigSchema();
    
    // Add a configuration option with validation
    void addOption(const std::string& key, const std::string& description,
                   std::function<ValidationResult(const ConfigValue&)> validator = nullptr,
                   const std::string& errorMessage = "Invalid value",
                   bool required = false);
    
    // Add a type constraint
    template<typename T>
    void addType(const std::string& key, bool required = false, const std::string& description = "") {
        auto validator = [this](const ConfigValue& value) -> ValidationResult {
            if constexpr (std::is_same_v<T, int>) {
                return std::holds_alternative<int>(value) ? ValidationResult(true) : ValidationResult(false, "Expected integer", key);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::holds_alternative<int64_t>(value) ? ValidationResult(true) : ValidationResult(false, "Expected int64_t", key);
            } else if constexpr (std::is_same_v<T, double>) {
                return (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) || std::holds_alternative<double>(value)) 
                    ? ValidationResult(true) : ValidationResult(false, "Expected numeric", key);
            } else if constexpr (std::is_same_v<T, bool>) {
                return std::holds_alternative<bool>(value) ? ValidationResult(true) : ValidationResult(false, "Expected boolean", key);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return std::holds_alternative<std::string>(value) ? ValidationResult(true) : ValidationResult(false, "Expected string", key);
            }
            return ValidationResult(false, "Unsupported type", key);
        };
        
        auto typeValidator = [validator, key](const ConfigValue& value) -> ValidationResult {
            ValidationResult result = validator(value);
            if (!result.valid) {
                result.key = key;
            }
            return result;
        };
        
        addOption(key, description, typeValidator, errorMessage + " (type mismatch)", required);
    }
    
    // Add range constraint
    void addRange(const std::string& key, double min, double max, const std::string& error = "Value out of range");
    
    // Add enum constraint
    void addEnum(const std::string& key, const std::set<std::string>& allowedValues, const std::string& error = "Value not in allowed set");
    
    // Validate a configuration value
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    
    // Check if a key is present in schema
    bool hasKey(const std::string& key) const;
    
    // Get description for a key
    std::string getDescription(const std::string& key) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Generate schema documentation
    std::string generateDocumentation() const;
    
    // Load schema from file
    bool loadFromFile(const std::string& filepath);
    
    // Save schema to file
    bool saveToFile(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

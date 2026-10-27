#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;
class ConfigValidator;

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

// Validation result
enum class ValidationResult {
    Success,
    Warning,
    Error
};

// Validation error
struct ValidationError {
    std::string key;
    std::string message;
    ValidationResult severity;
    
    ValidationError() : key(), message(), severity(ValidationResult::Error) {}
    ValidationError(const std::string& k, const std::string& msg, ValidationResult sev)
        : key(k), message(msg), severity(sev) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string type;
    std::string description;
    std::variant<std::monostate, double, int, std::vector<std::string>> defaultValue;
    bool required;
    std::function<ValidationResult(const ConfigValue&)> validator;
    
    ConfigSchemaEntry() : key(), type(), description(), defaultValue(), required(false), validator(nullptr) {}
    ConfigSchemaEntry(const std::string& k, const std::string& t, const std::string& desc,
                     const std::variant<std::monostate, double, int, std::vector<std::string>>& def = std::monostate(),
                     bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), required(req), validator(nullptr) {}
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

// Configuration validator class
class ConfigValidator;

// Configuration validator class
class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Disable copying, enable moving
    ConfigValidator(const ConfigValidator&) = delete;
    ConfigValidator& operator=(const ConfigValidator&) = delete;
    ConfigValidator(ConfigValidator&&) noexcept;
    ConfigValidator& operator=(ConfigValidator&&) noexcept;
    
    // Add a schema entry
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    
    // Validate configuration against schema
    std::vector<ValidationError> validate(const Config& config) const;
    
    // Validate a single value
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Get all schema entries
    const std::vector<ConfigSchemaEntry>& getSchemaEntries() const;
    
    // Clear all schema entries
    void clearSchema();
    
    // Load default schema
    void loadDefaultSchema();
    
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
    
    // Configuration validation
    std::vector<ValidationError> validateWith(const ConfigValidator& validator) const;
    
    // Configuration validation with default validator
    std::vector<ValidationError> validate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

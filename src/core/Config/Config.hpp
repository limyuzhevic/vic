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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v) : valid(v) {}
};

// Configuration value validator interface
class IValidator {
public:
    virtual ~IValidator() = default;
    virtual ValidationResult validate(const std::string& key, const ConfigValue& value) const = 0;
};

// Range validator
class RangeValidator : public IValidator {
public:
    RangeValidator(const std::string& min, const std::string& max) : min_(min), max_(max) {}
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override;
    
private:
    std::string min_;
    std::string max_;
};

// Enum validator
class EnumValidator : public IValidator {
public:
    EnumValidator(const std::vector<std::string>& allowed) : allowed_(allowed) {}
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override;
    
private:
    std::vector<std::string> allowed_;
};

// Pattern validator
class PatternValidator : public IValidator {
public:
    PatternValidator(const std::string& pattern) : pattern_(pattern) {}
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override;
    
private:
    std::string pattern_;
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::string ruleType;  // "range", "enum", "pattern", "required"
    std::string ruleData;  // JSON string containing rule parameters
    std::string errorMessage;
    
    ValidationRule() : key(), ruleType(), ruleData(), errorMessage() {}
    ValidationRule(const std::string& k, const std::string& type, const std::string& data, const std::string& error)
        : key(k), ruleType(type), ruleData(data), errorMessage(error) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::vector<std::string> requiredKeys;
    std::vector<ValidationRule> validationRules;
    std::vector<std::string> optionalKeys;
    
    ConfigSchema() : name() {}
    ConfigSchema(const std::string& n) : name(n) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable bool validated;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validated(false) {}
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

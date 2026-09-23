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
    std::vector<std::string>,
    std::vector<bool>
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
    InvalidType,
    OutOfRange,
    InvalidFormat,
    MissingRequired
};

// Configuration validation function
using ValidationFunction = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ValidationFunction validator;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, const ValidationFunction& val)
        : key(k), value(v), source(s), description(desc), validator(val) {}
};

// Configuration constraints
struct ConfigConstraints {
    std::vector<std::string> requiredKeys;
    std::unordered_map<std::string, ValidationFunction> validators;
    double minFloat = std::numeric_limits<double>::lowest();
    double maxFloat = std::numeric_limits<double>::max();
    int minInt = std::numeric_limits<int>::min();
    int maxInt = std::numeric_limits<int>::max();
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
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by prefix (hierarchical support)
    std::vector<std::string> getKeysWithPrefix(const std::string& prefix) const;
    
    // Validation and constraints
    void setValidator(const std::string& key, const ValidationFunction& validator);
    void setConstraints(const ConfigConstraints& constraints);
    ValidationResult validate() const;
    bool isValid() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration statistics
    std::string stats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigConstraints constraints;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateValue(const ConfigEntry& entry);
    static std::string valueToString(const ConfigValue& value);
};

} // namespace nlm

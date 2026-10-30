#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_set>
#include <chrono>

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
    bool isValid;
    std::string errorMessage;
    
    ValidationResult(bool valid = true, const std::string& error = "") 
        : isValid(valid), errorMessage(error) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    std::string dataType;
    bool isRequired;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   validationRegex(), dataType(), isRequired(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationRegex(), dataType(), isRequired(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc,
                const std::string& regex, const std::string& type, bool required)
        : key(k), value(v), source(s), description(desc), validationRegex(regex), dataType(type), isRequired(required) {}
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
    
    // Set values with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values (legacy)
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Add predefined config with metadata
    bool addConfig(const std::string& key, const ConfigValue& value, const std::string& description,
                   const std::string& dataType, bool isRequired = false, const std::string& validationRegex = "");
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get config entry metadata
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Get all config entries
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate all required config entries
    std::vector<ValidationResult> validateAll() const;
    
    // Get current timestamp for change tracking
    static std::string getCurrentTimestamp();
    
    // Enhanced string value parsing (JSON/YAML support)
    static ConfigValue parseConfigValue(const std::string& value);
    
    // Type conversion utilities
    static bool isNumericType(const ConfigValue& value);
    static std::string configValueToString(const ConfigValue& value) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string trimQuotes(const std::string& str);
    static bool matchesValidation(const ConfigValue& value, const std::string& regex);
    static std::string getTypeString(const ConfigValue& value);
    static std::string getCurrentTimeString();
};

} // namespace nlm

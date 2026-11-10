#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
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

// Validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getMessage() const {
        std::string msg;
        if (!errors.empty()) {
            msg = "Validation errors:\n";
            for (const auto& error : errors) {
                msg += "  ERROR: " + error + "\n";
            }
        }
        if (!warnings.empty()) {
            if (!msg.empty()) msg += "\n";
            msg += "Validation warnings:\n";
            for (const auto& warning : warnings) {
                msg += "  WARNING: " + warning + "\n";
            }
        }
        return msg;
    }
};

// Configuration parameter validation rules
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    std::string warningMessage;
    std::function<void(ConfigValue&)> defaultGenerator;
    std::pair<double, double> numericRange;
    std::vector<std::string> allowedValues;
};

// Configuration schema
struct ConfigSchema {
    std::map<std::string, ValidationRule> rules;
    std::string description;
    
    void addRule(const std::string& key,
                 std::function<bool(const ConfigValue&)> validator,
                 const std::string& errorMsg,
                 const std::string& warningMsg = "",
                 std::function<void(ConfigValue&)> defaultGen = nullptr,
                 const std::pair<double, double>& range = {-1e6, 1e6},
                 const std::vector<std::string>& allowed = {});
    
    ValidationResult validate(const Config& config) const;
    
    static ConfigSchema createDefaultSchema();
    static std::string getSchemaDocumentation();
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
    ValidationResult validate() const;
    
    // Apply validation rules
    bool applyValidation(ConfigSchema& schema);
    
    // Get schema for documentation
    static ConfigSchema getDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Configuration value extraction helpers
    static bool getBool(const ConfigValue& value);
    static int64_t getInt(const ConfigValue& value);
    static double getDouble(const ConfigValue& value);
    static std::string getString(const ConfigValue& value);
};

} // namespace nlm

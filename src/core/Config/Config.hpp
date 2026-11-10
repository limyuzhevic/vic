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
    
    // Load from file (JSON/YAML format)
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
    
    // Error handling
    const std::string& getError() const { return error_; }
    void clearError() { error_.clear(); }
    
    // Configuration validation
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Configuration templates and defaults
    bool setTemplate(const std::string& name, const Config& templateConfig);
    Config getTemplate(const std::string& name) const;
    std::vector<std::string> getTemplateNames() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string error_;  // Error message for last operation
    
    // Templates and validation
    std::vector<ConfigEntry> templates;
    std::vector<std::string> validationErrors;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    std::string getCurrentTimestamp() const;
    
    // Helper functions for improved functionality
    bool parseSimpleConfig(const std::string& content);
    bool setWithValidation(const std::string& key, const std::string& valueStr);
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // JSON/YAML parsing helpers
    bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& out);
    bool parseJSONString(const std::string& json, size_t& pos, std::string& out);
    bool parseJSONNumber(const std::string& json, size_t& pos, ConfigValue& out);
    bool parseJSONBoolean(const std::string& json, size_t& pos, ConfigValue& out);
    bool parseJSONArray(const std::string& json, size_t& pos, ConfigValue& out);
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source);
    Config getMergedConfig(const std::vector<Config>& configs) const;
    
    // Utility functions
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
};

// Configuration template for common use cases
struct ConfigTemplate {
    std::string name;
    std::shared_ptr<Config> config;
    std::string description;
    bool required;
    
    ConfigTemplate(const std::string& name, const std::string& description = "")
        : name(name), config(std::make_shared<Config>()), description(description), required(false) {}
};

// Configuration validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
};

// Common configuration validators
class RequiredValidator : public ConfigValidator {
    std::set<std::string> requiredKeys;
public:
    RequiredValidator(const std::set<std::string>& keys) : requiredKeys(keys) {}
    
    bool validate(const std::string& key, const ConfigValue&) const override {
        return requiredKeys.find(key) == requiredKeys.end();
    }
    
    std::string getErrorMessage(const std::string& key, const ConfigValue&) const override {
        return "Required configuration key missing: " + key;
    }
};

class RangeValidator : public ConfigValidator {
    std::string key;
    double minValue;
    double maxValue;
    bool inclusive;
public:
    RangeValidator(const std::string& key, double min, double max, bool inclusive = true)
        : key(key), minValue(min), maxValue(max), inclusive(inclusive) {}
    
    bool validate(const std::string& key, const ConfigValue& value) const override {
        if (this->key != key) return true;
        
        bool valid = std::visit([this](auto&& v) -> bool {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_arithmetic_v<T>) {
                double val = static_cast<double>(v);
                if (inclusive) {
                    return val >= minValue && val <= maxValue;
                } else {
                    return val > minValue && val < maxValue;
                }
            } else {
                return false;
            }
        }, value);
        
        return valid;
    }
    
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        if (this->key != key) return "";
        
        bool valid = std::visit([this](auto&& v) -> bool {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_arithmetic_v<T>) {
                double val = static_cast<double>(v);
                if (inclusive) {
                    return val >= minValue && val <= maxValue;
                } else {
                    return val > minValue && val < maxValue;
                }
            } else {
                return false;
            }
        }, value);
        
        if (!valid) {
            return "Value for key '" + key + "' must be between " + 
                   std::to_string(minValue) + " and " + std::to_string(maxValue);
        }
        return "";
    }
};

} // namespace nlm

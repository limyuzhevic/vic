#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <string>
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

// Configuration validation rule
enum class ValidationRule {
    None,
    Positive,
    NonNegative,
    Range,
    StringLength,
    Enum
};

// Parameter validation configuration
struct ParameterValidation {
    ValidationRule rule;
    std::string errorMessage;
    double minValue;
    double maxValue;
    size_t minLength;
    size_t maxLength;
    std::vector<std::string> allowedValues;
    
    ParameterValidation() 
        : rule(ValidationRule::None), minValue(0.0), maxValue(0.0), minLength(0), maxLength(0) {}
    
    bool validate(const ConfigValue& value) const {
        try {
            switch (rule) {
                case ValidationRule::None:
                    return true;
                case ValidationRule::Positive:
                    if constexpr (std::is_same_v<int, decltype(*std::get_if<int>(&value))>) {
                        return std::get<int>(value) > 0;
                    } else if constexpr (std::is_same_v<double, decltype(*std::get_if<double>(&value))>) {
                        return std::get<double>(value) > 0.0;
                    }
                    return true;
                case ValidationRule::NonNegative:
                    if constexpr (std::is_same_v<int, decltype(*std::get_if<int>(&value))>) {
                        return std::get<int>(value) >= 0;
                    } else if constexpr (std::is_same_v<double, decltype(*std::get_if<double>(&value))>) {
                        return std::get<double>(value) >= 0.0;
                    }
                    return true;
                case ValidationRule::Range:
                    if constexpr (std::is_same_v<int, decltype(*std::get_if<int>(&value))>) {
                        double val = static_cast<double>(std::get<int>(value));
                        return val >= minValue && val <= maxValue;
                    } else if constexpr (std::is_same_v<double, decltype(*std::get_if<double>(&value))>) {
                        return std::get<double>(value) >= minValue && std::get<double>(value) <= maxValue;
                    }
                    return true;
                case ValidationRule::StringLength:
                    if constexpr (std::is_same_v<std::string, decltype(*std::get_if<std::string>(&value))>) {
                        size_t len = std::get<std::string>(value).length();
                        return len >= minLength && len <= maxLength;
                    }
                    return true;
                case ValidationRule::Enum:
                    if constexpr (std::is_same_v<std::string, decltype(*std::get_if<std::string>(&value))>) {
                        return std::find(allowedValues.begin(), allowedValues.end(), std::get<std::string>(value)) != allowedValues.end();
                    }
                    return true;
                default:
                    return true;
            }
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
};

// Configuration entry with validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ParameterValidation validation;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const ParameterValidation& val = ParameterValidation())
        : key(k), value(v), source(s), description(desc), validation(val) {}
};

// Configuration parameter specification
struct ParameterSpec {
    std::string key;
    std::string description;
    ConfigSource defaultSource;
    ParameterValidation validation;
    std::string defaultValueStr;
    
    ParameterSpec(const std::string& k, const std::string& desc, ConfigSource src, 
                  const ParameterValidation& val, const std::string& defaultStr = "")
        : key(k), description(desc), defaultSource(src), validation(val), defaultValueStr(defaultStr) {}
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
    bool validateConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration documentation
    void setParameterDescription(const std::string& key, const std::string& description);
    std::string getParameterDescription(const std::string& key) const;
    std::vector<std::string> getAllParameterNames() const;
    
    // Parameter validation rules
    void addParameterValidation(const std::string& key, const ParameterValidation& validation);
    
    // Initialize with standard parameters
    void initializeStandardParameters();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toString(const ConfigValue& value);
    static std::optional<ConfigValue> parseValue(const std::string& str);
};

} // namespace nlm

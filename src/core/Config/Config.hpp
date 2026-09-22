#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <regex>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <mutex>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types - enhanced with more type safety and conversion
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>
>;

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Enhanced configuration entry with validation support
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Validation rule for configuration values
struct ValidationRule {
    enum class RuleType {
        Range,
        Regex,
        AllowedValues,
        MinLength,
        MaxLength,
        Positive,
        NonNegative,
        Custom
    };
    
    RuleType type;
    std::string errorMessage;
    
    // Range validation
    struct RangeRule {
        double min;
        double max;
        bool inclusive = true;
    };
    
    // Regex validation
    struct RegexRule {
        std::regex pattern;
    };
    
    // Allowed values validation
    struct AllowedValuesRule {
        std::vector<ConfigValue> allowedValues;
    };
    
    // Length validation
    struct LengthRule {
        size_t min;
        size_t max;
        bool inclusive = true;
    };
    
    // Custom validation function type
    using CustomValidator = std::function<bool(const ConfigValue&)>;
    
    union RuleData {
        RangeRule range;
        RegexRule regex;
        AllowedValuesRule allowedValues;
        LengthRule length;
        CustomValidator custom;
    } data;
    
    ValidationRule() : type(RuleType::Custom), errorMessage("Validation failed") {}
    
    static ValidationRule range(double min, double max, bool inclusive = true, const std::string& msg = "Value out of range") {
        ValidationRule rule;
        rule.type = RuleType::Range;
        rule.errorMessage = msg;
        rule.data.range = {min, max, inclusive};
        return rule;
    }
    
    static ValidationRule regex(const std::string& pattern, const std::string& msg = "Value does not match pattern") {
        ValidationRule rule;
        rule.type = RuleType::Regex;
        rule.errorMessage = msg;
        rule.data.regex = {std::regex(pattern)};
        return rule;
    }
    
    static ValidationRule allowedValues(const std::vector<ConfigValue>& values, const std::string& msg = "Value not in allowed list") {
        ValidationRule rule;
        rule.type = RuleType::AllowedValues;
        rule.errorMessage = msg;
        rule.data.allowedValues = {values};
        return rule;
    }
    
    static ValidationRule length(size_t min, size_t max, bool inclusive = true, const std::string& msg = "Length out of range") {
        ValidationRule rule;
        rule.type = RuleType::Length;
        rule.errorMessage = msg;
        rule.data.length = {min, max, inclusive};
        return rule;
    }
    
    bool validate(const ConfigValue& value) const {
        switch (type) {
            case RuleType::Range:
                return validateRange(value);
            case RuleType::Regex:
                return validateRegex(value);
            case RuleType::AllowedValues:
                return validateAllowedValues(value);
            case RuleType::Length:
                return validateLength(value);
            case RuleType::Positive:
                return validatePositive(value);
            case RuleType::NonNegative:
                return validateNonNegative(value);
            case RuleType::Custom:
                if (std::holds_alternative<CustomValidator>(data.custom)) {
                    return std::get<CustomValidator>(data.custom)(value);
                }
                return true;
            default:
                return true;
        }
    }
    
private:
    bool validateRange(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) {
            double val = static_cast<double>(std::get<int>(value));
            if (!data.range.inclusive) {
                return val > data.range.min && val < data.range.max;
            }
            return val >= data.range.min && val <= data.range.max;
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            if (!data.range.inclusive) {
                return val > data.range.min && val < data.range.max;
            }
            return val >= data.range.min && val <= data.range.max;
        } else if (std::holds_alternative<float>(value)) {
            double val = static_cast<double>(std::get<float>(value));
            if (!data.range.inclusive) {
                return val > data.range.min && val < data.range.max;
            }
            return val >= data.range.min && val <= data.range.max;
        }
        return false;
    }
    
    bool validateRegex(const ConfigValue& value) const {
        if (std::holds_alternative<std::string>(value)) {
            return std::regex_search(std::get<std::string>(value), data.regex.pattern);
        }
        return false;
    }
    
    bool validateAllowedValues(const ConfigValue& value) const {
        for (const auto& allowed : data.allowedValues.allowedValues) {
            if (value == allowed) {
                return true;
            }
        }
        return false;
    }
    
    bool validateLength(const ConfigValue& value) const {
        if (std::holds_alternative<std::string>(value)) {
            size_t len = std::get<std::string>(value).size();
            if (!data.length.inclusive) {
                return len > data.length.min && len < data.length.max;
            }
            return len >= data.length.min && len <= data.length.max;
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            size_t len = std::get<std::vector<int>>(value).size();
            if (!data.length.inclusive) {
                return len > data.length.min && len < data.length.max;
            }
            return len >= data.length.min && len <= data.length.max;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            size_t len = std::get<std::vector<double>>(value).size();
            if (!data.length.inclusive) {
                return len > data.length.min && len < data.length.max;
            }
            return len >= data.length.min && len <= data.length.max;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            size_t len = std::get<std::vector<std::string>>(value).size();
            if (!data.length.inclusive) {
                return len > data.length.min && len < data.length.max;
            }
            return len >= data.length.min && len <= data.length.max;
        }
        return false;
    }
    
    bool validatePositive(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) > 0;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) > 0.0;
        } else if (std::holds_alternative<float>(value)) {
            return std::get<float>(value) > 0.0f;
        }
        return false;
    }
    
    bool validateNonNegative(const ConfigValue& value) const {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) >= 0;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= 0.0;
        } else if (std::holds_alternative<float>(value)) {
            return std::get<float>(value) >= 0.0f;
        }
        return false;
    }
};

// Main configuration class with advanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced file loading with format detection
    bool loadFromFile(const std::string& filepath);
    bool loadFromJson(const std::string& filepath);
    bool loadFromYaml(const std::string& filepath);
    bool loadFromAdvanced(const std::string& filepath);
    
    // Enhanced load from command line arguments with better error handling
    bool loadFromArgs(int argc, char** argv);
    
    // Enhanced save to file with better formatting and type preservation
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Enhanced getOr with validation support
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getValidated(const std::string& key, const T& defaultValue = T()) const;
    
    // Set values with validation support
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Validation support
    bool validate(const std::string& key) const;
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    void addValidationRule(const std::string& key, const ValidationRule& rule);
    
    // Advanced features
    bool saveToJson(const std::string& filepath) const;
    bool saveToYaml(const std::string& filepath) const;
    
    // Configuration merging and inheritance
    void mergeFrom(const Config& other, ConfigSource source = ConfigSource::File);
    void inheritFrom(const Config& parent, const std::string& prefix = "");
    
    // Statistics and diagnostics
    size_t getEntryCount() const;
    std::vector<ConfigEntry> getEntriesBySource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    mutable std::mutex mutex_; // Thread safety
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static bool isInteger(const std::string& str);
    static bool isFloat(const std::string& str);
    static bool stringToBool(const std::string& str);
    template<typename T>
    static T stringToNumber(const std::string& str);
    static ConfigValue parseStringToConfigValue(const std::string& str);
    
    // Helper methods for JSON/YAML conversion
    void convertJsonToConfig(const std::string& jsonStr, const std::string& prefix);
    void convertYamlToConfig(const std::string& yamlStr, const std::string& prefix);
    
    // Validation helpers
    bool validateValue(const ConfigValue& value, const ValidationRule& rule) const;
};

} // namespace nlm

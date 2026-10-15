#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>

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
    std::string errorMessage;
    
    ValidationResult(bool valid = true, const std::string& error = "")
        : isValid(valid), errorMessage(error) {}
    
    static ValidationResult success() { return ValidationResult(true); }
    static ValidationResult failure(const std::string& error) { return ValidationResult(false, error); }
};

// Configuration validation rule
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual ValidationResult validate(const std::string& key, const ConfigValue& value) const = 0;
};

// Range validation rule
class RangeRule : public ValidationRule {
public:
    enum class Type { INT, INT64, DOUBLE };
    
    RangeRule(Type type, double min, double max, const std::string& errorMsg)
        : type(type), min(min), max(max), errorMsg(errorMsg) {}
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override {
        try {
            switch (type) {
                case Type::INT:
                    if (const auto* val = std::get_if<int>(&value)) {
                        if (*val < min || *val > max) {
                            return ValidationResult(false, 
                                errorMsg + " (key: " + key + ", value: " + std::to_string(*val) + ")");
                        }
                    }
                    break;
                case Type::INT64:
                    if (const auto* val = std::get_if<int64_t>(&value)) {
                        if (*val < min || *val > max) {
                            return ValidationResult(false,
                                errorMsg + " (key: " + key + ", value: " + std::to_string(*val) + ")");
                        }
                    }
                    break;
                case Type::DOUBLE:
                    if (const auto* val = std::get_if<double>(&value)) {
                        if (*val < min || *val > max) {
                            return ValidationResult(false,
                                errorMsg + " (key: " + key + ", value: " + std::to_string(*val) + ")");
                        }
                    }
                    break;
            }
        } catch (const std::bad_variant_access&) {}
        return ValidationResult::success();
    }
    
private:
    Type type;
    double min;
    double max;
    std::string errorMsg;
};

// Allowed values validation rule
class AllowedValuesRule : public ValidationRule {
public:
    AllowedValuesRule(const std::vector<std::string>& values, const std::string& errorMsg)
        : allowedValues(values), errorMsg(errorMsg) {}
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const override {
        try {
            if (const auto* strVal = std::get_if<std::string>(&value)) {
                if (std::find(allowedValues.begin(), allowedValues.end(), *strVal) == allowedValues.end()) {
                    std::string valuesStr = "";
                    for (const auto& v : allowedValues) {
                        valuesStr += "\"" + v + "\", ";
                    }
                    valuesStr.erase(valuesStr.size() - 2); // Remove last ", "
                    return ValidationResult(false,
                        errorMsg + " (key: " + key + ", value: \"" + *strVal + "\"). Allowed: " + valuesStr);
                }
            }
        } catch (const std::bad_variant_access&) {}
        return ValidationResult::success();
    }
    
private:
    std::vector<std::string> allowedValues;
    std::string errorMsg;
};

// Configuration schema
class ConfigSchema {
public:
    void addRule(const std::string& key, std::unique_ptr<ValidationRule> rule) {
        validationRules[key] = std::move(rule);
    }
    
    void addIntRange(const std::string& key, int min, int max, const std::string& errorMsg = "Value must be within range") {
        addRule(key, std::make_unique<RangeRule>(RangeRule::Type::INT, min, max, errorMsg));
    }
    
    void addInt64Range(const std::string& key, int64_t min, int64_t max, const std::string& errorMsg = "Value must be within range") {
        addRule(key, std::make_unique<RangeRule>(RangeRule::Type::INT64, min, max, errorMsg));
    }
    
    void addDoubleRange(const std::string& key, double min, double max, const std::string& errorMsg = "Value must be within range") {
        addRule(key, std::make_unique<RangeRule>(RangeRule::Type::DOUBLE, min, max, errorMsg));
    }
    
    void addAllowedValues(const std::string& key, const std::vector<std::string>& values, const std::string& errorMsg = "Value not allowed") {
        addRule(key, std::make_unique<AllowedValuesRule>(values, errorMsg));
    }
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const {
        auto it = validationRules.find(key);
        if (it != validationRules.end()) {
            return it->second->validate(key, value);
        }
        return ValidationResult::success(); // No rule for this key is OK
    }
    
    void clear() {
        validationRules.clear();
    }
    
private:
    std::unordered_map<std::string, std::unique_ptr<ValidationRule>> validationRules;
    friend class Config;
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
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type conversion
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
    
    // Set validation schema
    void setSchema(const ConfigSchema& schema) { configSchema = schema; }
    
    // Validate configuration
    ValidationResult validate() const;
    
    // Validate a specific key
    ValidationResult validate(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Schema for validation
    ConfigSchema configSchema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON parsing helpers
    bool parseJSONValue(const std::string& json, size_t& pos, ConfigValue& outValue);
    bool parseJSONString(const std::string& json, size_t& pos, std::string& outString);
    bool parseJSONNumber(const std::string& json, size_t& pos, ConfigValue& outValue);
    bool parseJSONBoolean(const std::string& json, size_t& pos, ConfigValue& outValue);
    bool parseJSONArray(const std::string& json, size_t& pos, ConfigValue& outValue);
    void skipWhitespace(const std::string& json, size_t& pos);
    char peekChar(const std::string& json, size_t pos);
    
    friend class Config;
};

} // namespace nlm

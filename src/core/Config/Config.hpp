#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <nlohmann/json.hpp>

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

// Validation result for configuration values
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    ValidationResult(bool v = true, const std::string& msg = "")
        : valid(v), errorMessage(msg) {}
    
    static ValidationResult Valid() { return ValidationResult(true); }
    static ValidationResult Invalid(const std::string& msg) { return ValidationResult(false, msg); }
};

// Configuration constraint for validation
enum class ConstraintType {
    MinInt,
    MaxInt,
    MinDouble,
    MaxDouble,
    MinLength,
    MaxLength,
    Pattern,  // For strings (regex)
    Choices,  // For strings or enums
    Custom    // User-defined validation function
};

struct ConfigConstraint {
    ConstraintType type;
    std::string errorMessage;
    std::variant<int, double, size_t, std::string> value;
    std::function<ValidationResult(const ConfigValue&)> customValidator;
    
    ConfigConstraint(ConstraintType t, const std::string& msg, const std::variant<int, double, size_t, std::string>& val)
        : type(t), errorMessage(msg), value(val) {}
};

// Configuration entry with constraints
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<ConfigConstraint> constraints;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    ValidationResult validate() const {
        for (const auto& constraint : constraints) {
            ValidationResult result = validateConstraint(constraint);
            if (!result.valid) {
                return result;
            }
        }
        return ValidationResult::Valid();
    }
    
private:
    ValidationResult validateConstraint(const ConfigConstraint& constraint) const {
        switch (constraint.type) {
            case ConstraintType::MinInt:
                if (const int* val = std::get_if<int>(&value)) {
                    if (*val < std::get<int>(constraint.value)) {
                        return ValidationResult::Invalid(constraint.errorMessage);
                    }
                }
                break;
            case ConstraintType::MaxInt:
                if (const int* val = std::get_if<int>(&value)) {
                    if (*val > std::get<int>(constraint.value)) {
                        return ValidationResult::Invalid(constraint.errorMessage);
                    }
                }
                break;
            case ConstraintType::MinDouble:
                if (const double* val = std::get_if<double>(&value)) {
                    if (*val < std::get<double>(constraint.value)) {
                        return ValidationResult::Invalid(constraint.errorMessage);
                    }
                }
                break;
            case ConstraintType::MaxDouble:
                if (const double* val = std::get_if<double>(&value)) {
                    if (*val > std::get<double>(constraint.value)) {
                        return ValidationResult::Invalid(constraint.errorMessage);
                    }
                }
                break;
            case ConstraintType::MinLength:
                if (const std::string* val = std::get_if<std::string>(&value)) {
                    if (val->length() < std::get<size_t>(constraint.value)) {
                        return ValidationResult::Invalid(constraint.errorMessage);
                    }
                }
                break;
            case ConstraintType::MaxLength:
                if (const std::string* val = std::get_if<std::string>(&value)) {
                    if (val->length() > std::get<size_t>(constraint.value)) {
                        return ValidationResult::Invalid(constraint.errorMessage);
                    }
                }
                break;
            case ConstraintType::Pattern:
                if (const std::string* val = std::get_if<std::string>(&value)) {
                    const std::string& pattern = std::get<std::string>(constraint.value);
                    if (pattern != "*" && pattern != ".*" && pattern != ".*") {
                        bool matches = true;
                        if (pattern.size() >= 2 && pattern[0] == '*' && pattern[pattern.size() - 1] == '*') {
                            std::string middle = pattern.substr(1, pattern.size() - 2);
                            if (val->find(middle) == std::string::npos) {
                                matches = false;
                            }
                        }
                        if (!matches) {
                            return ValidationResult::Invalid(constraint.errorMessage);
                        }
                    }
                }
                break;
            case ConstraintType::Choices:
                if (const std::string* val = std::get_if<std::string>(&value)) {
                    const std::string& choicesStr = std::get<std::string>(constraint.value);
                    size_t pos = choicesStr.find(',');
                    while (pos != std::string::npos) {
                        std::string choice = choicesStr.substr(0, pos);
                        if (choice == *val) {
                            break;
                        }
                        choicesStr.erase(0, pos + 1);
                        pos = choicesStr.find(',');
                    }
                    if (pos == std::string::npos && choicesStr != *val) {
                        return ValidationResult::Invalid(constraint.errorMessage);
                    }
                }
                break;
            case ConstraintType::Custom:
                if (constraint.customValidator) {
                    return constraint.customValidator(value);
                }
                break;
        }
        return ValidationResult::Valid();
    }
};

// Configuration schema for validation and documentation
class ConfigSchema {
public:
    void addField(const std::string& key, 
                  const std::string& description,
                  ConfigSource defaultSource = ConfigSource::Default,
                  const std::vector<ConfigConstraint>& constraints = {});
    
    void setDefaultValue(const std::string& key, const ConfigValue& value);
    void setDefaultValue(const std::string& key, int value);
    void setDefaultValue(const std::string& key, double value);
    void setDefaultValue(const std::string& key, bool value);
    void setDefaultValue(const std::string& key, const std::string& value);
    
    bool hasField(const std::string& key) const;
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    ValidationResult validateAll() const;
    
    const ConfigEntry* getEntry(const std::string& key) const;
    std::vector<std::string> getFieldKeys() const;
    
    bool validateFileEntry(const std::string& key, const std::string& value) const;
    
    std::string getSummary() const;
    
private:
    std::unordered_map<std::string, ConfigEntry> fields_;
    std::unordered_map<std::string, ConfigValue> defaultValues_;
};

// Main configuration class with schema validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Set schema for validation
    void setSchema(std::shared_ptr<ConfigSchema> schema);
    
    // Load from file (supports JSON, YAML, or key=value format)
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
    bool set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Set values without validation (for runtime)
    void setNoValidate(const std::string& key, const ConfigValue& value, 
                      ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Validate all entries
    bool validate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // File parsing helpers
    bool parseJSONFile(const std::string& filepath);
    bool parseYAMLLikeFile(const std::string& filepath);
    bool parseSimpleFile(const std::string& filepath);
    
    // Type conversion helpers
    static bool stringToBool(const std::string& str);
    static int stringToInt(const std::string& str);
    static double stringToDouble(const std::string& str);
};

// Explicit template instantiations
template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

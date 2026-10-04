#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
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

// Configuration source enum
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration error handling
class ConfigError : public std::runtime_error {
public:
    enum class ErrorCode {
        SUCCESS = 0,
        FILE_NOT_FOUND = 1,
        INVALID_FORMAT = 2,
        INVALID_KEY = 3,
        INVALID_VALUE = 4,
        TYPE_MISMATCH = 5,
        VALUE_OUT_OF_RANGE = 6,
        DUPLICATE_KEY = 7,
        VALIDATION_FAILED = 8
    };

    ConfigError(ErrorCode code, const std::string& message)
        : std::runtime_error(message), code_(code) {}

    ErrorCode getCode() const { return code_; }

private:
    ErrorCode code_;
};

// Configuration validation rule
class ValidationRule {
public:
    virtual ~ValidationRule() = default;
    virtual bool validate(const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage() const = 0;
};

// Numeric range validation
class RangeValidationRule : public ValidationRule {
public:
    RangeValidationRule(double min, double max, const std::string& desc)
        : min_(min), max_(max), description_(desc) {}

    bool validate(const ConfigValue& value) const override {
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value)) >= min_ && 
                   static_cast<double>(std::get<int>(value)) <= max_;
        } else if (std::holds_alternative<int64_t>(value)) {
            return static_cast<double>(std::get<int64_t>(value)) >= min_ && 
                   static_cast<double>(std::get<int64_t>(value)) <= max_;
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= min_ && 
                   std::get<double>(value) <= max_;
        }
        return false;
    }

    std::string getErrorMessage() const override {
        return "Value must be between " + std::to_string(min_) + " and " + 
               std::to_string(max_) + " (" + description_ + ")";
    }

private:
    double min_;
    double max_;
    std::string description_;
};

// String enumeration validation
class EnumValidationRule : public ValidationRule {
public:
    EnumValidationRule(const std::vector<std::string>& validValues, const std::string& desc)
        : validValues_(validValues), description_(desc) {}

    bool validate(const ConfigValue& value) const override {
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            return std::find(validValues_.begin(), validValues_.end(), strValue) != validValues_.end();
        }
        return false;
    }

    std::string getErrorMessage() const override {
        std::string validStr = "";
        for (size_t i = 0; i < validValues_.size(); ++i) {
            if (i > 0) validStr += ", ";
            validStr += "\"" + validValues_[i] + "\"";
        }
        return "Value must be one of: " + validStr + " (" + description_ + ")";
    }

private:
    std::vector<std::string> validValues_;
    std::string description_;
};

// Configuration entry with validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<std::shared_ptr<ValidationRule>> validators;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
    
    bool validate() const {
        for (const auto& validator : validators) {
            if (!validator->validate(value)) {
                return false;
            }
        }
        return true;
    }
    
    std::string getValidationError() const {
        for (const auto& validator : validators) {
            if (!validator->validate(value)) {
                return validator->getErrorMessage();
            }
        }
        return "";
    }
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string type;  // "int", "int64", "double", "bool", "string"
    ConfigValue defaultValue;
    std::string description;
    bool required;
    std::vector<std::shared_ptr<ValidationRule>> validators;
};

// Configuration class with full validation and error handling
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
    bool loadFromFile(const std::string& filepath, std::string& error);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, std::string& error);
    
    // Save to file
    bool saveToFile(const std::string& filepath, std::string& error) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, int64_t value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime, bool validate = true);
    
    // Add validation rule for a key
    void addValidationRule(const std::string& key, std::shared_ptr<ValidationRule> rule);
    
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
    
    // Get last error
    std::string getLastError() const { return lastError_; }
    
    // Check if last operation was successful
    bool wasLastOperationSuccessful() const { return lastOperationSuccess_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Error tracking
    mutable std::string lastError_;
    mutable bool lastOperationSuccess_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseConfigValue(const std::string& str, ConfigValue& outValue);
    
    // Schema management
    static std::map<std::string, ConfigSchemaEntry> getDefaultSchema();
    static bool validateKeyValue(const std::string& key, const ConfigValue& value);
    static std::string getValidationError(const std::string& key);
    static bool validateBasicValue(const std::string& key, const ConfigValue& value);
    static void initializeDefaults();
    static std::map<std::string, ConfigSchemaEntry> schema_;
    static std::map<std::string, ConfigSchemaEntry>& getSchema();
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <stdexcept>
#include <limits>

namespace nlm {

// Forward declarations
class Config;

// Configuration validation errors
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Configuration validator interface
class ConfigValidator {
public:
    virtual ~ConfigValidator() = default;
    virtual bool validate(const std::string& key, const ConfigValue& value) const = 0;
    virtual std::string getErrorMessage(const std::string& key, const ConfigValue& value) const = 0;
};

// Simple range validator for numeric values
class RangeValidator : public ConfigValidator {
public:
    RangeValidator(const std::string& key, double min, double max, const std::string& desc)
        : key_(key), min_(min), max_(max), description_(desc) {}
    
    bool validate(const std::string& key, const ConfigValue& value) const override {
        if (key != this->key_) return true;
        
        if (auto* v = std::get_if<double>(&value)) {
            return *v >= min_ && *v <= max_;
        } else if (auto* v = std::get_if<int>(&value)) {
            return static_cast<double>(*v) >= min_ && static_cast<double>(*v) <= max_;
        } else if (auto* v = std::get_if<int64_t>(&value)) {
            return static_cast<double>(*v) >= min_ && static_cast<double>(*v) <= max_;
        }
        return true; // Can't validate non-numeric types
    }
    
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        if (key != this->key_) return "";
        return "Value for '" + key + "' must be between " + std::to_string(min_) + " and " + 
               std::to_string(max_) + ". " + description_;
    }
    
private:
    std::string key_;
    double min_;
    double max_;
    std::string description_;
};

// Enumeration validator for valid values
class EnumValidator : public ConfigValidator {
public:
    EnumValidator(const std::string& key, const std::vector<std::string>& validValues, const std::string& desc)
        : key_(key), validValues_(validValues), description_(desc) {}
    
    bool validate(const std::string& key, const ConfigValue& value) const override {
        if (key != this->key_) return true;
        
        if (auto* v = std::get_if<std::string>(&value)) {
            for (const auto& valid : validValues_) {
                if (*v == valid) return true;
            }
            return false;
        } else if (auto* v = std::get_if<int>(&value)) {
            // Convert to string for comparison
            std::string strVal = std::to_string(*v);
            for (const auto& valid : validValues_) {
                if (strVal == valid) return true;
            }
        }
        return true; // Can't validate non-string types
    }
    
    std::string getErrorMessage(const std::string& key, const ConfigValue& value) const override {
        if (key != this->key_) return "";
        std::string validList = "Valid values: ";
        for (size_t i = 0; i < validValues_.size(); ++i) {
            if (i > 0) validList += ", ";
            validList += "'" + validValues_[i] + "'";
        }
        return "Value for '" + key + "' must be one of: " + validList + ". " + description_;
    }
    
private:
    std::string key_;
    std::vector<std::string> validValues_;
    std::string description_;
};

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
    
    // Validate a key-value pair against all validators
    bool validate(const std::string& key, const ConfigValue& value) const;
    std::string getValidationError(const std::string& key, const ConfigValue& value) const;
};

// Convenience validator functions
inline std::unique_ptr<ConfigValidator> createRangeValidator(const std::string& key, double min, double max, const std::string& desc) {
    return std::make_unique<RangeValidator>(key, min, max, desc);
}

inline std::unique_ptr<ConfigValidator> createEnumValidator(const std::string& key, 
    const std::vector<std::string>& validValues, const std::string& desc) {
    return std::make_unique<EnumValidator>(key, validValues, desc);
}

inline std::unique_ptr<ConfigValidator> createPositiveValidator(const std::string& key, double min, const std::string& desc) {
    return std::make_unique<RangeValidator>(key, min, std::numeric_limits<double>::max(), desc);
}

inline std::unique_ptr<ConfigValidator> createNonNegativeValidator(const std::string& key, const std::string& desc) {
    return std::make_unique<RangeValidator>(key, 0.0, std::numeric_limits<double>::max(), desc);
}

inline std::unique_ptr<ConfigValidator> createProbabilityValidator(const std::string& key, const std::string& desc) {
    return std::make_unique<RangeValidator>(key, 0.0, 1.0, desc);
}

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <limits>

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

// Validation mode
enum class ValidationMode {
    Strict,
    Lenient,
    None
};

// Validation error types
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, 
                   const std::string& key = "",
                   const std::string& value = "")
        : std::runtime_error(message), key_(key), value_(value) {}
    
    const std::string& key() const { return key_; }
    const std::string& value() const { return value_; }
    
private:
    std::string key_;
    std::string value_;
};

// Type validation result
enum class ValidationResult {
    Valid,
    TypeMismatch,
    ValueOutOfRange,
    KeyFormatInvalid,
    EnumValueInvalid,
    PathInvalid,
    DefaultInvalid
};

// Value bounds for numeric types
struct NumericBounds {
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    bool has_min = false;
    bool has_max = false;
};

// Validation callback signature
using ValidationCallback = std::function<ValidationResult(
    const std::string& key,
    const ConfigValue& value,
    const std::string& operation
)>;

// Key validation rules
struct KeyValidationRules {
    bool allow_empty = false;
    size_t max_length = 0; // 0 means no limit
    std::string allowed_pattern;
    bool allow_special_chars = true;
    std::vector<std::string> reserved_prefixes;
};

// Value validation rules
struct ValueValidationRules {
    bool require_non_empty_string = false;
    bool allow_negative_numbers = true;
    bool allow_zero = true;
    std::unordered_map<std::string, NumericBounds> numeric_bounds;
    std::unordered_map<std::string, std::vector<std::string>> allowed_enums;
    bool validate_file_paths = false;
    bool validate_array_non_empty = false;
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
    
public:
    // Validation settings
    void setValidationMode(ValidationMode mode);
    ValidationMode getValidationMode() const;
    
    // Key validation rules
    void setKeyValidationRules(const std::string& key, const KeyValidationRules& rules);
    
    // Value validation rules
    void setValueValidationRules(const std::string& key, const ValueValidationRules& rules);
    
    // Validation callbacks
    void addValidationCallback(const std::string& key, const ValidationCallback& callback);
    
    // Strict validation control
    void setStrictValidation(bool strict);
    bool hasStrictValidation() const;
    
    // Deprecated validation methods (keeping for backward compatibility)
    // Use setKeyValidationRules/setValueValidationRules for new code
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    ValidationResult validate(const std::string& key, const std::string& value) const;
    ValidationResult validate(const std::string& key, int value) const;
    ValidationResult validate(const std::string& key, double value) const;
    ValidationResult validate(const std::string& key, bool value) const;
    
private:
    // Internal validation helpers
    ValidationResult validateKey(const std::string& key) const;
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    ValidationResult validateValueInternal(const std::string& key, const ConfigValue& value, const ValueValidationRules& rules) const;
    bool isValidFilePath(const std::string& path) const;
};

} // namespace nlm

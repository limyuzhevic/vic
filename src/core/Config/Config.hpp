#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <stdexcept>
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

// Configuration validation result
enum class ValidationResult {
    Success,
    InvalidValue,
    MissingRequired,
    TypeMismatch,
    RangeViolation,
    CustomError
};

// Validation error details
struct ValidationError {
    ValidationResult result;
    std::string message;
    std::string key;
    std::string expected;
    std::string actual;
    
    ValidationError() : result(ValidationResult::Success), message("") {}
    ValidationError(ValidationResult r, const std::string& msg, const std::string& k = "")
        : result(r), message(msg), key(k), expected(""), actual("") {}
    ValidationError(ValidationResult r, const std::string& msg, const std::string& k, 
                   const std::string& exp, const std::string& act)
        : result(r), message(msg), key(k), expected(exp), actual(act) {}
};

// Configuration validation callback type
using ValidationCallback = std::function<ValidationError(const std::string& key, const ConfigValue& value)>;

// Configuration constraint information
struct ConfigConstraint {
    std::string key;
    std::string description;
    std::string type;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::vector<std::string> allowedValues;
    bool required = false;
    std::string defaultValue;
    
    ConfigConstraint() : key(), description(), type(), required(false) {}
    ConfigConstraint(const std::string& k, const std::string& desc, const std::string& t, bool req = false)
        : key(k), description(desc), type(t), required(req) {}
};

// Enhanced configuration entry with validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::vector<std::string> allowedValues;
    bool required = false;
    std::vector<ValidationCallback> validators;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type(getTypeName(v)), required(false) {}
    
    // Helper to get type name from ConfigValue
    static std::string getTypeName(const ConfigValue& value);
    
    // Validation methods
    bool validate() const;
    std::vector<ValidationError> validateAll() const;
    
    // Constraint methods
    void addConstraint(const ConfigConstraint& constraint);
    bool hasConstraint(const std::string& key) const;
    ConfigConstraint getConstraint(const std::string& key) const;
    
    // Type checking
    bool isType(const std::string& expectedType) const;
    bool isNumeric() const;
    bool isBoolean() const;
    bool isString() const;
    bool isVector() const;
    
    // Value access helpers
    template<typename T>
    std::optional<T> getAs() const;
    
    template<typename T>
    bool hasValueType() const;
    
    // Comparison
    bool operator==(const ConfigEntry& other) const;
    bool operator!=(const ConfigEntry& other) const;
};

// Main enhanced configuration class
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
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation and schema
    void addConstraint(const ConfigConstraint& constraint);
    void addValidator(const std::string& key, const ValidationCallback& validator);
    void setRequired(const std::string& key, bool required);
    std::vector<ValidationError> validateAll() const;
    std::vector<ValidationError> validate(const std::string& key) const;
    bool isValid(const std::string& key) const;
    bool isValidAll() const;
    
    // Configuration presets
    static std::vector<ConfigConstraint> getDefaultConstraints();
    void applyPreset(const std::string& presetName);
    void createPreset(const std::string& presetName, const std::vector<ConfigConstraint>& constraints);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getRequiredKeys() const;
    std::vector<std::string> getOptionalKeys() const;
    
    // Get constraints for a key
    std::vector<ConfigConstraint> getConstraints(const std::string& key) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string validationSummary() const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    std::vector<std::string> diff(const Config& other) const;
    
    // File format helpers
    static bool isValidJsonKey(const std::string& key);
    static std::string sanitizeJsonKey(const std::string& key);
    static std::string formatValue(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static std::string boolToString(bool value);
    
    // Validation helpers
    static std::vector<ValidationError> validateConstraints(const ConfigEntry& entry) const;
    static std::vector<ValidationError> validateCustom(const ConfigEntry& entry) const;
    static std::string getValueTypeName(const ConfigValue& value);
    static bool valueMatchesType(const ConfigValue& value, const std::string& expectedType);
    
    // Configuration file helpers
    static std::string generateConfigTemplate(const std::vector<ConfigConstraint>& constraints);
    static std::vector<ConfigConstraint> parseConfigTemplate(const std::string& templateContent);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

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

// Parameter validation error codes
enum class ConfigValidationError {
    None,
    TypeMismatch,
    RangeViolation,
    MissingRequired,
    InvalidFormat,
    ValueOutOfBounds
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable ConfigValidationError validationError;
    mutable std::string validationMessage;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   validationError(ConfigValidationError::None), validationMessage() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          validationError(ConfigValidationError::None), validationMessage() {}
};

// Parameter metadata for enhanced configuration management
struct ParameterMetadata {
    std::string key;
    std::string type;
    std::string description;
    bool required;
    ConfigValue defaultValue;
    double minValue;
    double maxValue;
    std::string validationRule;
    bool validated;
    
    ParameterMetadata(const std::string& k, const std::string& t, const std::string& desc, 
                     bool req = false, const ConfigValue& def = {}, double min = 0.0, 
                     double max = 0.0, const std::string& rule = "")
        : key(k), type(t), description(desc), required(req), defaultValue(def),
          minValue(min), maxValue(max), validationRule(rule), validated(false) {}
};

// Enhanced configuration class with validation and schema support
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
    
    // Enhanced configuration management methods
    bool validate() const;
    bool validateParameter(const std::string& key) const;
    void validateAllParameters();
    ConfigValidationError getParameterValidationError(const std::string& key) const;
    std::string getParameterValidationMessage(const std::string& key) const;
    
    // Configuration schema and parameter management
    struct ParameterSchema {
        std::string key;
        std::string type;
        std::string description;
        bool required;
        ConfigValue defaultValue;
        double minValue;
        double maxValue;
        std::string validationRule;
        
        ParameterSchema(const std::string& k, const std::string& t, const std::string& desc, 
                       bool req = false, const ConfigValue& def = {}, double min = 0.0, 
                       double max = 0.0, const std::string& rule = "")
            : key(k), type(t), description(desc), required(req), defaultValue(def),
              minValue(min), maxValue(max), validationRule(rule) {}
    };
    
    // Get all parameter schemas
    static std::vector<ParameterSchema> getParameterSchemas();
    
    // Advanced parameter validation and utility methods
    bool validateRange(const std::string& key) const;
    bool validateType(const std::string& key) const;
    bool validateRequired(const std::string& key) const;
    std::string getParameterTypeInfo(const std::string& key) const;
    void printParameterInfo(const std::string& key) const;
    static std::string getParameterValidationError(const std::string& key, const ConfigValue& value, ConfigValidationError error);
    
    // Parameter-specific validation helpers (static methods)
    static bool validateRange(const ConfigValue& value, double min, double max);
    static bool validatePositive(const ConfigValue& value);
    static bool validateNonNegative(const ConfigValue& value);
    static bool validateStringLength(const ConfigValue& value, size_t minLen, size_t maxLen);
    static bool validateTypeMatch(const ConfigValue& value, const std::string& expectedType);
    static bool isValueNumeric(const ConfigValue& value);
    static double extractNumericValue(const ConfigValue& value);
    
    // Configuration validation framework
    struct ValidationResult {
        std::string parameter;
        bool isValid;
        ConfigValidationError error;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
        
        ValidationResult(const std::string& param, bool valid, ConfigValidationError err, 
                        const std::string& msg)
            : parameter(param), isValid(valid), error(err), message(msg), 
              timestamp(std::chrono::system_clock::now()) {}
    };
    
    std::vector<ValidationResult> validateAllParametersWithDetails() const;
    
    // Batch operations for efficiency
    void setMultiple(const std::map<std::string, ConfigValue>& values);
    void applyConfiguration(const std::map<std::string, ConfigValue>& values);
    
    // Configuration information and statistics
    size_t getParameterCount() const;
    size_t getValidParameterCount() const;
    double getValidationCoverage() const;
    std::vector<std::string> getInvalidParameters() const;
    
    // Source management
    ConfigSource getParameterSource(const std::string& key) const;
    std::vector<std::string> getParametersBySource(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toString(const ConfigValue& value);
    
    // Parameter existence and management
    bool parameterExists(const std::string& key) const;
    size_t getParameterIndex(const std::string& key) const;
    
    // Validation helpers
    bool validateParameterType(const std::string& key, const ConfigValue& value, const std::string& expectedType) const;
    bool validateParameterRange(const std::string& key, const ConfigValue& value) const;
    bool validateParameterRequired(const std::string& key) const;
    
    // Parameter schema management
    static std::vector<ParameterMetadata> getParameterMetadata();
    ParameterMetadata getParameterMetadata(const std::string& key) const;
    
    // Source management helpers
    std::vector<std::string> getParameterKeysBySource(ConfigSource source) const;
};

} // namespace nlm

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
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>,
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
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string suggestion;
    
    ValidationResult(bool valid, const std::string& error, const std::string& suggestion = "")
        : isValid(valid), errorMessage(error), suggestion(suggestion) {}
    
    static ValidationResult success() {
        return ValidationResult(true, "");
    }
    
    static ValidationResult error(const std::string& error, const std::string& suggestion = "") {
        return ValidationResult(false, error, suggestion);
    }
};

// Configuration constraint for validation
struct ConfigConstraint {
    std::string key;
    std::string typeName;
    std::string description;
    std::string defaultValue;
    std::string minValue;
    std::string maxValue;
    bool required;
    std::vector<std::string> allowedValues;
    
    ConfigConstraint(const std::string& k, const std::string& type, const std::string& desc = "")
        : key(k), typeName(type), description(desc), required(false) {}
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string typeName;
    std::string validationRule;
    bool isValidated;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), typeName(), validationRule(), isValidated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const std::string& type = "")
        : key(k), value(v), source(s), description(desc), typeName(type), isValidated(false) {}
};

// Configuration schema for validation and documentation
class ConfigSchema {
public:
    void addConstraint(const ConfigConstraint& constraint);
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    bool hasConstraint(const std::string& key) const;
    std::optional<ConfigConstraint> getConstraint(const std::string& key) const;
    std::vector<ConfigConstraint> getAllConstraints() const;
    std::string getTypeName(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    std::vector<std::string> getAllowedKeys() const;
    
private:
    std::map<std::string, ConfigConstraint> constraints_;
};

// Main configuration class with validation
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
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Specialized getters for common types
    template<typename T>
    T getFloat(const std::string& key, float defaultValue = 0.0f) const;
    template<typename T>
    size_t getSizeT(const std::string& key, size_t defaultValue = 0) const;
    template<typename T>
    int64_t getInt64(const std::string& key, int64_t defaultValue = 0) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, size_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate and set with constraint checking
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get configuration schema
    const ConfigSchema& getSchema() const { return schema_; }
    ConfigSchema& getSchema() { return schema_; }
    
    // Initialize with default values
    void initializeDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isFloatString(const std::string& str);
    static float stringToFloat(const std::string& str);
    static size_t stringToSizeT(const std::string& str);
    static int64_t stringToInt64(const std::string& str);
    
    // Validation helpers
    ValidationResult validateType(const std::string& key, const ConfigValue& value) const;
    ValidationResult validateRange(const std::string& key, const ConfigValue& value) const;
    ValidationResult validateEnum(const std::string& key, const ConfigValue& value) const;
};

} // namespace nlm
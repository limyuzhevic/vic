#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <fstream>
#include <iostream>

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
    Runtime,
    Experimental
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult(bool v = true) : valid(v) {}
    ValidationResult(const std::vector<std::string>& errs) : valid(false), errors(errs) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Configuration schema definition
struct ConfigSchemaEntry {
    std::string key;
    std::string type;
    ConfigValue defaultValue;
    std::string description;
    std::function<ValidationResult(const ConfigValue&)> validator;
    bool required;
    bool experimental;
    
    ConfigSchemaEntry() : key(), type(), defaultValue(), description(), validator(nullptr), required(false), experimental(false) {}
    
    ConfigSchemaEntry(const std::string& k, const std::string& t, const ConfigValue& def,
                     const std::string& desc, bool req = false, bool exp = false)
        : key(k), type(t), defaultValue(def), description(desc), required(req), experimental(exp) {
        validator = nullptr;  // Default validator - no validation
    }
    
    ConfigSchemaEntry(const std::string& k, const std::string& t, const ConfigValue& def,
                     const std::string& desc, std::function<ValidationResult(const ConfigValue&)> val,
                     bool req = false, bool exp = false)
        : key(k), type(t), defaultValue(def), description(desc), validator(val), required(req), experimental(exp) {}
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    bool experimental;
    size_t usageCount;
    std::string lastModified;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(), experimental(false), usageCount(0), lastModified() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type(), experimental(false), usageCount(0), lastModified() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc,
                const std::string& t, bool exp = false)
        : key(k), value(v), source(s), description(desc), type(t), experimental(exp), usageCount(0), lastModified() {}
};

// Main configuration class with validation and schema
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or simple format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    
    // Load from Python dictionary-like object (for Python bindings)
    bool loadFromMap(const std::unordered_map<std::string, std::string>& map, bool validate = true);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath, bool includeExperimental = false) const;
    
    // Get values with type safety
    std::optional<int> getInt(const std::string& key) const;
    std::optional<int64_t> getInt64(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<std::vector<int>> getIntVector(const std::string& key) const;
    std::optional<std::vector<double>> getDoubleVector(const std::string& key) const;
    std::optional<std::vector<std::string>> getStringVector(const std::string& key) const;
    
    // Get values with default values
    int getIntOr(const std::string& key, int defaultValue) const;
    int64_t getInt64Or(const std::string& key, int64_t defaultValue) const;
    double getDoubleOr(const std::string& key, double defaultValue) const;
    bool getBoolOr(const std::string& key, bool defaultValue) const;
    std::string getStringOr(const std::string& key, const std::string& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values without validation (for internal use)
    void setInternal(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Validate configuration against schema
    ValidationResult validate() const;
    
    // Apply default configuration
    void applyDefaults();
    
    // Add schema entry
    void addSchemaEntry(const ConfigSchemaEntry& entry);
    
    // Get schema entry
    const ConfigSchemaEntry* getSchemaEntry(const std::string& key) const;
    
    // Check if key is experimental
    bool isExperimental(const std::string& key) const;
    
    // Increment usage counter for analytics
    void recordUsage(const std::string& key);
    
    // Get usage statistics
    std::string getUsageStats() const;
    
    // Get configuration metadata
    std::string getMetadata() const;
    
    // Phase 2 compatibility: legacy getOr method
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        auto result = get<T>(key);
        return result.has_value() ? result.value() : defaultValue;
    }
    
    // Phase 2 compatibility: legacy has method
    bool has(const std::string& key) const {
        return has(key);
    }
    
    // Phase 2 compatibility: legacy get method
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        return get<T>(key);
    }
    
    // Phase 2 compatibility: legacy set method
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime) {
        // Simplified validation for backward compatibility
        setInternal(key, value, source);
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toString(const ConfigValue& value);
    static bool matchesType(const ConfigValue& value, const std::string& typeName);
    
    // Type conversion utilities
    static int convertToInt(const ConfigValue& value);
    static int64_t convertToInt64(const ConfigValue& value);
    static double convertToDouble(const ConfigValue& value);
    static bool convertToBool(const ConfigValue& value);
    
    // JSON serialization
    static std::string toJson(const ConfigValue& value);
    static ConfigValue fromJson(const std::string& json, const std::string& typeName);
    
    // Configuration schema initialization
    static std::vector<ConfigSchemaEntry> initializeSchema();
    
    // Built-in schema for common configuration options
    static std::unordered_map<std::string, ConfigSchemaEntry> createBuiltInSchema();
};

// Helper functions for common configuration validation
ValidationResult validatePositiveInt(const ConfigValue& value);
ValidationResult validateNonNegativeInt(const ConfigValue& value);
ValidationResult validatePositiveDouble(const ConfigValue& value);
ValidationResult validateRangeDouble(const ConfigValue& value, double min, double max);
ValidationResult validateStringLength(const ConfigValue& value, size_t minLength, size_t maxLength);
ValidationResult validateEnum(const ConfigValue& value, const std::vector<std::string>& validValues);
ValidationResult validateConfigFileExists(const ConfigValue& value);

} // namespace nlm

// Template implementations for Config class
#include "Config.tpp"

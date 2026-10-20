#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include "Validation.hpp"
#include "ValidationUtils.hpp"

namespace nlm {

// Forward declarations
class Config;
class ConfigSchemaManager;
class ConfigValidator;

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

// Configuration category
enum class ConfigCategory {
    Neuromodulation,
    Simulation,
    Development,
    Memory,
    Environment,
    Logging,
    Visualization,
    Reward,
    Unknown
};

// Validation mode
enum class ValidationMode {
    Strict,
    Lenient,
    Permissive
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

// Validation error structure
struct ValidationError {
    std::string field;
    std::string category;
    std::string message;
    enum class Severity { Invalid, Warning, Info } severity;
    std::optional<std::string> fileLocation;
    int lineNumber;
    
    ValidationError(const std::string& f, const std::string& cat, const std::string& msg,
                   Severity sev = Severity::Invalid, 
                   const std::optional<std::string>& loc = std::nullopt, int line = 0)
        : field(f), category(cat), message(msg), severity(sev), 
          fileLocation(loc), lineNumber(line) {}
};

// Configuration schema
struct ValidationRule {
    std::string name;
    enum class ConfigDataType {
        Integer,
        Float,
        Boolean,
        String,
        IntegerVector,
        FloatVector,
        StringVector,
        Any
    } dataType;
    std::string description;
    bool required;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::optional<std::string> pattern;
    std::optional<std::function<bool(const ConfigValue&)>> customValidator;
    ValidationMode defaultMode;
    std::vector<std::string> relatedFields;
    
    ValidationRule(const std::string& n, ConfigDataType dt, const std::string& desc = "",
                   bool req = false, ValidationMode mode = ValidationMode::Strict)
        : name(n), dataType(dt), description(desc), required(req), 
          defaultMode(mode) {}
};

struct ConfigSchema {
    std::string name;
    ConfigCategory category;
    std::unordered_map<std::string, ValidationRule> rules;
    
    ConfigSchema(const std::string& n, ConfigCategory cat)
        : name(n), category(cat) {}
    
    void addRule(const ValidationRule& rule) {
        rules[rule.name] = rule;
    }
};

// Configuration validator
class ConfigValidator {
public:
    ConfigValidator() : currentMode(ValidationMode::Strict) {}
    
    void setMode(ValidationMode mode) {
        currentMode = mode;
    }
    
    ValidationMode getMode() const {
        return currentMode;
    }
    
    static ConfigValidator& getInstance() {
        static ConfigValidator instance;
        return instance;
    }
    
    std::vector<ValidationError> validate(const ConfigValue& value, const ValidationRule& rule, 
                                        ConfigCategory category, const std::string& field) const;
    
    ValidationResult validateValue(const std::string& fieldName, const ConfigValue& value,
                                 ConfigCategory category, std::vector<ValidationError>& errors) const;
    
    std::vector<ValidationError> validateAll(const std::unordered_map<std::string, ConfigValue>& configData,
                                           ConfigCategory category) const;
    
    static std::string dataTypeToString(ConfigDataType type);
    
private:
    ValidationMode currentMode;
    
    bool validateType(const ConfigValue& value, ConfigDataType expectedType) const;
    bool validateRange(const ConfigValue& value, const ValidationRule& rule) const;
    bool validateStringPattern(const ConfigValue& value, const ValidationRule& rule) const;
    bool validateCustom(const ConfigValue& value, const ValidationRule& rule) const;
    bool validateConsistency(const std::string& fieldName, const ConfigValue& value,
                           const std::unordered_map<std::string, ConfigValue>& allConfig,
                           const std::vector<std::string>& relatedFields) const;
    
    ValidationResult getSeverityForMode(bool isValid, ValidationMode mode) const;
    
    std::string formatValue(const ConfigValue& value) const;
};

// Configuration schema manager
class ConfigSchemaManager {
public:
    static ConfigSchemaManager& getInstance() {
        static ConfigSchemaManager instance;
        return instance;
    }
    
    void registerSchema(const ConfigSchema& schema) {
        schemas[schema.category] = schema;
    }
    
    const ConfigSchema& getSchema(ConfigCategory category) const {
        auto it = schemas.find(category);
        if (it != schemas.end()) {
            return it->second;
        }
        return defaultSchema;
    }
    
    const std::unordered_map<ConfigCategory, ConfigSchema>& getAllSchemas() const {
        return schemas;
    }
    
    void clear() {
        schemas.clear();
    }
    
    bool hasSchema(ConfigCategory category) const {
        return schemas.find(category) != schemas.end();
    }
    
private:
    ConfigSchemaManager() {
        initializeDefaultSchemas();
    }
    
    void initializeDefaultSchemas();
    
    std::unordered_map<ConfigCategory, ConfigSchema> schemas;
    ConfigSchema defaultSchema{"Default", ConfigCategory::Unknown};
};

// Configuration category helper functions
namespace ConfigCategories {
    ConfigCategory stringToCategory(const std::string& categoryStr);
    std::string categoryToString(ConfigCategory category);
    std::vector<std::string> getCategoryFields(ConfigCategory category);
}

// Forward declaration for friend function
class ValidationImplementation;

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
    
    // Load from file (JSON/YAML format)
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
    
    // Configuration validation
    bool validate(const std::string& category = "default") const;
    std::vector<ValidationError> getValidationErrors(const std::string& category = "default") const;
    void setValidationMode(ValidationMode mode);
    ValidationMode getValidationMode() const;
    
    // Category detection
    static ConfigCategory detectCategory(const std::string& key);
    static std::string categoryToString(ConfigCategory category);
    
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Friend declaration for implementation access
    friend class ValidationImplementation;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ValidationMode currentValidationMode;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Implementation class for internal details
class ValidationImplementation {
public:
    static bool validateConfig(const Config& config, ConfigCategory category, 
                              std::vector<ValidationError>& errors);
    static ValidationMode getValidationMode(const Config& config);
    static ConfigCategory detectCategory(const std::string& key);
};

} // namespace nlm

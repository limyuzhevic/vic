#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <stdexcept>
#include <set>

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

// Validation error types
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& key, const std::string& message)
        : std::runtime_error("Validation error for key \"" + key + "\": " + message),
          key(key), message(message) {}
    
    std::string getKey() const { return key; }
    std::string getMessage() const { return message; }
    
private:
    std::string key;
    std::string message;
};

// Validation rule types
enum class ValidationType {
    None,
    Range,
    Enum,
    StringPattern,
    MinLength,
    MaxLength,
    Custom
};

// Range validation
struct RangeValidation {
    double min;
    double max;
    bool inclusive = true;
    
    RangeValidation(double min, double max, bool inclusive = true)
        : min(min), max(max), inclusive(inclusive) {}
};

// Enum validation
struct EnumValidation {
    std::set<std::string> allowedValues;
    
    EnumValidation(const std::set<std::string>& values) : allowedValues(values) {}
};

// String pattern validation
struct StringPatternValidation {
    std::string pattern;
    bool caseSensitive = false;
    
    StringPatternValidation(const std::string& pattern, bool caseSensitive = false)
        : pattern(pattern), caseSensitive(caseSensitive) {}
};

// Custom validation function
using CustomValidator = std::function<bool(const ConfigValue&)>;

// Combined validation rule
struct ValidationRule {
    std::string key;
    ValidationType type;
    std::string errorMessage;
    
    // Union of all possible validation data
    std::variant<
        RangeValidation,
        EnumValidation,
        StringPatternValidation,
        CustomValidator
    > validationData;
    
    ValidationRule(const std::string& k, ValidationType t, const std::string& msg)
        : key(k), type(t), errorMessage(msg) {}
};

// Schema support
class Schema;

// Schema-based configuration manager
class SchemaBasedConfig {
public:
    SchemaBasedConfig(const Schema& schema);
    
    bool validate(const ConfigValue& value) const;
    
    std::string getErrorMessage(const ConfigValue& value) const;
    
    std::string schemaToJSON() const;
    
    std::string getSchemaName() const { return schema_.name; }
    std::string getSchemaDescription() const { return schema_.description; }
    std::string getSchemaVersion() const { return schema_.version; }
    
private:
    Schema schema_;
};

// Global schema registry
class SchemaRegistry {
public:
    static SchemaRegistry& getInstance() {
        static SchemaRegistry instance;
        return instance;
    }
    
    void registerSchema(const Schema& schema);
    
    SchemaBasedConfig* getSchema(const std::string& name);
    
    bool hasSchema(const std::string& name) const;
    
    std::vector<std::string> getSchemaNames() const;
    
    void unregisterSchema(const std::string& name);
    
private:
    SchemaRegistry() {}
    std::map<std::string, SchemaBasedConfig> schemas_;
};

// Schema definition
struct Schema {
    std::string name;
    std::string description;
    std::string version;
    bool strict;
    
    std::map<std::string, SchemaProperty> properties;
    std::vector<std::string> requiredProperties;
    
    Schema(const std::string& name, const std::string& desc = "", const std::string& ver = "1.0")
        : name(name), description(desc), version(ver), strict(true) {}
    
    void addProperty(const SchemaProperty& prop);
    
    bool validate(const ConfigValue& value) const;
    
    std::string toJSON() const;
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    std::string getVersion() const { return version; }
    
private:
    std::string schemaTypeToString(SchemaValueType type) const;
};

// Schema property
struct SchemaProperty {
    std::string name;
    SchemaValueType type;
    std::string description;
    std::string defaultValue;
    bool required;
    bool readOnly;
    
    std::vector<SchemaConstraint> constraints;
    
    SchemaProperty(const std::string& name, SchemaValueType type, const std::string& desc = "")
        : name(name), type(type), description(desc), required(false), readOnly(false) {}
};

// Schema constraint
struct SchemaConstraint {
    std::string constraintType;
    std::string constraintValue;
    std::string errorMessage;
    
    SchemaConstraint(const std::string& type, const std::string& value, const std::string& error)
        : constraintType(type), constraintValue(value), errorMessage(error) {}
};

// Schema value types
enum class SchemaValueType {
    Integer,
    Double,
    Boolean,
    String,
    IntArray,
    DoubleArray,
    StringArray,
    Object,
    Array
};

    // Schema-based validation
    void applySchema(const Schema& schema);
    void removeSchema(const std::string& schemaName);
    bool isSchemaValid(const std::string& schemaName) const;
    std::vector<ValidationError> getSchemaValidationErrors(const std::string& schemaName) const;
    
    // Schema validation helpers
    static ValidationRule createRangeValidation(const std::string& key, double min, double max,
                                                const std::string& errorMessage = "Value out of range");
    
    static ValidationRule createEnumValidation(const std::string& key,
                                               const std::set<std::string>& allowedValues,
                                               const std::string& errorMessage = "Value not in allowed list");
    
    static ValidationRule createStringPatternValidation(const std::string& key,
                                                       const std::string& pattern,
                                                       bool caseSensitive = false,
                                                       const std::string& errorMessage = "String does not match pattern");
    
    static ValidationRule createCustomValidation(const std::string& key,
                                                 CustomValidator validator,
                                                 const std::string& errorMessage = "Custom validation failed");
    
    // Schema creation helpers
    static Schema createBrainSchema();
    static Schema createWorldSchema();
    static Schema createAgentSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateValue(const ConfigValue& value, const ValidationRule& rule);
    static std::string schemaTypeToString(SchemaValueType type);
};

} // namespace nlm

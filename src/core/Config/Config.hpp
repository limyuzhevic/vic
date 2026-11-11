#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <nlohmann/json.hpp>
#include <pybind11/pybind11.h>

using json = nlohmann::json;

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
    Runtime,
    Python
};

// Configuration validation error
struct ValidationError {
    std::string key;
    std::string expectedType;
    std::string actualType;
    std::string message;
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "int64", "double", "bool", "string", "vector"
    std::string description;
    std::optional<ConfigValue> defaultValue;
    bool required = false;
    std::vector<ValidationError> validationErrors;
    
    ConfigSchema() = default;
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc, 
                 const std::optional<ConfigValue>& def = std::nullopt, bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), required(req) {}
    
    bool validate(const ConfigValue& value) const {
        validationErrors.clear();
        
        if (required && value.index() == std::variant_npos) {
            validationErrors.push_back({"Key not found", type, "none", "Required key is missing"});
            return false;
        }
        
        if (value.index() == std::variant_npos) {
            return true; // Optional and not present
        }
        
        switch (value.index()) {
            case 0: // int
                if (type != "int") {
                    validationErrors.push_back({key, type, "int", "Type mismatch: expected " + type + ", got int"});
                    return false;
                }
                break;
            case 1: // int64_t
                if (type != "int64") {
                    validationErrors.push_back({key, type, "int64", "Type mismatch: expected " + type + ", got int64"});
                    return false;
                }
                break;
            case 2: // double
                if (type != "double" && type != "float") {
                    validationErrors.push_back({key, type, "double", "Type mismatch: expected " + type + ", got double"});
                    return false;
                }
                break;
            case 3: // bool
                if (type != "bool") {
                    validationErrors.push_back({key, type, "bool", "Type mismatch: expected " + type + ", got bool"});
                    return false;
                }
                break;
            case 4: // std::string
                if (type != "string") {
                    validationErrors.push_back({key, type, "string", "Type mismatch: expected " + type + ", got string"});
                    return false;
                }
                break;
            case 5: // std::vector<int>
                if (type != "vector<int>") {
                    validationErrors.push_back({key, type, "vector<int>", "Type mismatch: expected " + type + ", got vector<int>"});
                    return false;
                }
                break;
            case 6: // std::vector<double>
                if (type != "vector<double>" && type != "vector<float>") {
                    validationErrors.push_back({key, type, "vector<double>", "Type mismatch: expected " + type + ", got vector<double>"});
                    return false;
                }
                break;
            case 7: // std::vector<std::string>
                if (type != "vector<string>") {
                    validationErrors.push_back({key, type, "vector<string>", "Type mismatch: expected " + type + ", got vector<string>"});
                    return false;
                }
                break;
        }
        
        return validationErrors.empty();
    }
    
    json to_json() const {
        json j;
        j["key"] = key;
        j["type"] = type;
        j["description"] = description;
        j["required"] = required;
        if (defaultValue.has_value()) {
            j["default"] = *defaultValue;
        }
        return j;
    }
    
    static ConfigSchema from_json(const json& j) {
        ConfigSchema schema;
        schema.key = j.value("key", "");
        schema.type = j.value("type", "string");
        schema.description = j.value("description", "");
        schema.required = j.value("required", false);
        
        if (j.contains("default")) {
            schema.defaultValue = j["default"].get<ConfigValue>();
        }
        
        return schema;
    }
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    time_t timestamp;
    bool validated;
    std::vector<ValidationError> validationErrors;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), timestamp(0), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(time(nullptr)), validated(false) {}
    
    json to_json() const {
        json j;
        j["key"] = key;
        j["value"] = value;
        j["source"] = static_cast<int>(source);
        j["description"] = description;
        j["timestamp"] = timestamp;
        j["validated"] = validated;
        return j;
    }
    
    static ConfigEntry from_json(const json& j) {
        ConfigEntry entry;
        entry.key = j.value("key", "");
        entry.value = j.value("value", ConfigValue());
        entry.source = static_cast<ConfigSource>(j.value("source", 0));
        entry.description = j.value("description", "");
        entry.timestamp = j.value("timestamp", 0);
        entry.validated = j.value("validated", false);
        return entry;
    }
};

// Validation result
struct ValidationResult {
    bool valid;
    std::vector<ValidationError> errors;
    std::map<std::string, std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    void addError(const ValidationError& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& key, const std::string& message) {
        warnings[key] = message;
    }
    
    std::string summary() const {
        std::ostringstream oss;
        if (valid) {
            oss << "✓ Configuration is valid";
        } else {
            oss << "✗ Configuration validation failed:";
            for (const auto& error : errors) {
                oss << "\n  - Key: " << error.key << " Message: " << error.message;
            }
        }
        oss << "\nWarnings:";
        for (const auto& warning : warnings) {
            oss << "\n  - " << warning.first << ": " << warning.second;
        }
        return oss.str();
    }
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
    
    // Enhanced loading from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJson(const json& j);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Enhanced saving to file
    bool saveToFile(const std::string& filepath) const;
    bool saveToJson(json& j) const;
    
    // Get values with enhanced type support
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-specific getters
    int getInt(const std::string& key) const;
    int64_t getInt64(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::string getString(const std::string& key) const;
    std::vector<int> getIntVector(const std::string& key) const;
    std::vector<double> getDoubleVector(const std::string& key) const;
    std::vector<std::string> getStringVector(const std::string& key) const;
    
    // Type-specific setters
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setInt64(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setIntVector(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void setDoubleVector(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void setStringVector(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Enhanced validation
    ValidationResult validate(const std::vector<ConfigSchema>& schema = {}) const;
    ValidationResult validateSchema(const std::vector<ConfigSchema>& schema) const;
    
    // Schema management
    void setSchema(const std::vector<ConfigSchema>& schema);
    void addSchema(const ConfigSchema& schema);
    const std::vector<ConfigSchema>& getSchema() const;
    std::vector<ConfigSchema> getValidEntries() const;
    
    // Configuration management
    void setSource(const std::string& key, ConfigSource source);
    ConfigSource getSource(const std::string& key) const;
    time_t getTimestamp(const std::string& key) const;
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration conversion
    json toJson() const;
    static Config fromJson(const json& j);
    
    // Utility functions
    static std::string configValueToString(const ConfigValue& value);
    static std::string configSourceToString(ConfigSource source);
    static std::string configTypeToString(const ConfigValue& value);
    
    // Python-specific utilities
    static Config fromPythonDict(const py::dict& d);
    py::dict toPythonDict() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static ConfigValue stringToConfigValue(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // JSON conversion helpers
    static void configValueToJson(const ConfigValue& value, json& j);
    static ConfigValue jsonToConfigValue(const json& j);
    
    // Validation helpers
    static bool validateType(const ConfigValue& value, const std::string& type) const;
    static ValidationError createValidationError(const std::string& key, const std::string& expected, const std::string& actual, const std::string& message);
};

} // namespace nlm
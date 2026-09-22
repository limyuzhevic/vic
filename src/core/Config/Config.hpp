#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <regex>
#include <chrono>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <functional>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

// Forward declarations
class Config;

// Enhanced error types
enum class ConfigErrorType {
    InvalidType,
    InvalidValue,
    MissingKey,
    FileNotFound,
    ParseError,
    ValidationError,
    SerializationError,
    EnvironmentError,
    MergeError,
    SchemaError
};

class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message, ConfigErrorType type = ConfigErrorType::InvalidValue)
        : std::runtime_error(message), errorType(type) {}
    
    ConfigErrorType getErrorType() const { return errorType; }
    
    static std::string errorTypeToString(ConfigErrorType type) {
        switch (type) {
            case ConfigErrorType::InvalidType: return "InvalidType";
            case ConfigErrorType::InvalidValue: return "InvalidValue";
            case ConfigErrorType::MissingKey: return "MissingKey";
            case ConfigErrorType::FileNotFound: return "FileNotFound";
            case ConfigErrorType::ParseError: return "ParseError";
            case ConfigErrorType::ValidationError: return "ValidationError";
            case ConfigErrorType::SerializationError: return "SerializationError";
            case ConfigErrorType::EnvironmentError: return "EnvironmentError";
            case ConfigErrorType::MergeError: return "MergeError";
            case ConfigErrorType::SchemaError: return "SchemaError";
            default: return "Unknown";
        }
    }

private:
    ConfigErrorType errorType;
};

// Configuration value types - enhanced with more flexibility
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<bool>,
    std::vector<std::vector<double>>
>;

// Configuration source with hierarchy
enum class ConfigSource {
    Default,
    File,
    Environment,
    CommandLine,
    Runtime,
    Preset,
    Inheritance
};

// Configuration metadata for tracking
struct ConfigMetadata {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::string defaultValue;
    std::string validationRule;
    std::string environmentVariable;
    bool required;
    double minValue;
    double maxValue;
    std::chrono::system_clock::time_point timestamp;
    std::string version;
    
    ConfigMetadata() : source(ConfigSource::Default), description(""), type(""), defaultValue(""), 
                      validationRule(""), environmentVariable(""), required(false), 
                      minValue(std::numeric_limits<double>::lowest()), 
                      maxValue(std::numeric_limits<double>::max()), version("1.0") {}
    
    ConfigMetadata(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type(""), defaultValue(""), 
          validationRule(""), environmentVariable(""), required(false), 
          minValue(std::numeric_limits<double>::lowest()), 
          maxValue(std::numeric_limits<double>::max()), version("1.0") {}
};

// Parameter validation rules
struct ValidationRule {
    std::string type; // "range", "regex", "enum", "custom"
    std::string rule; // format depends on type
    std::string errorMessage;
    bool operator()(const ConfigValue& value) const {
        return validate(value);
    }
    
private:
    bool validate(const ConfigValue& value) const {
        if (type == "range") {
            try {
                if (std::holds_alternative<int>(value)) {
                    double val = std::get<int>(value);
                    double min = std::stod(rule.substr(0, rule.find('-')));
                    double max = std::stod(rule.substr(rule.find('-') + 1));
                    return val >= min && val <= max;
                } else if (std::holds_alternative<double>(value)) {
                    double val = std::get<double>(value);
                    double min = std::stod(rule.substr(0, rule.find('-')));
                    double max = std::stod(rule.substr(rule.find('-') + 1));
                    return val >= min && val <= max;
                }
            } catch (...) {
                return false;
            }
        } else if (type == "regex") {
            try {
                std::string strValue;
                if (std::holds_alternative<std::string>(value)) strValue = std::get<std::string>(value);
                else if (std::holds_alternative<int>(value)) strValue = std::to_string(std::get<int>(value));
                else if (std::holds_alternative<double>(value)) strValue = std::to_string(std::get<double>(value));
                else if (std::holds_alternative<bool>(value)) strValue = std::get<bool>(value) ? "true" : "false";
                
                std::regex regex(rule);
                return std::regex_match(strValue, regex);
            } catch (...) {
                return false;
            }
        } else if (type == "enum") {
            try {
                std::string strValue;
                if (std::holds_alternative<std::string>(value)) strValue = std::get<std::string>(value);
                else if (std::holds_alternative<int>(value)) strValue = std::to_string(std::get<int>(value));
                else if (std::holds_alternative<double>(value)) strValue = std::to_string(std::get<double>(value));
                else if (std::holds_alternative<bool>(value)) strValue = std::get<bool>(value) ? "true" : "false";
                
                std::string allowedValues = rule;
                size_t pos = 0;
                while ((pos = allowedValues.find(',')) != std::string::npos) {
                    std::string enumValue = allowedValues.substr(0, pos);
                    if (trim(enumValue) == strValue) return true;
                    allowedValues.erase(0, pos + 1);
                }
                if (trim(allowedValues) == strValue) return true;
                return false;
            } catch (...) {
                return false;
            }
        }
        return true; // No validation or unknown type passes
    }
};

// Preset configurations
struct ConfigPreset {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> parameters;
    std::string category; // "performance", "accuracy", "memory", "experiment"
    
    ConfigPreset(const std::string& n, const std::string& desc = "") 
        : name(n), description(desc), category("general") {}
};

// Configuration comparison result
struct ConfigDiff {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    ConfigSource source;
    std::string changeType; // "added", "removed", "modified"
    std::chrono::system_clock::time_point timestamp;
    
    ConfigDiff(const std::string& k, const ConfigValue& oldV, const ConfigValue& newV, ConfigSource s, const std::string& change)
        : key(k), oldValue(oldV), newValue(newV), source(s), changeType(change), 
          timestamp(std::chrono::system_clock::now()) {}
};

// JSON schema definition
struct JsonSchema {
    std::string schema;
    std::string schemaPath;
    std::string validationType; // "json", "yaml", "custom"
    
    JsonSchema(const std::string& s = "", const std::string& p = "", const std::string& t = "json")
        : schema(s), schemaPath(p), validationType(t) {}
};

// Exception for unsupported types
class UnsupportedTypeException : public std::exception {
public:
    UnsupportedTypeException(const std::string& type) : message("Unsupported type: " + type) {}
    const char* what() const noexcept override { return message.c_str(); }
private:
    std::string message;
};

// Main configuration class with all enhancements
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format with fallback to key=value)
    bool loadFromFile(const std::string& filepath, bool merge = true);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_", bool merge = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonStr, bool merge = true);
    
    // Load from YAML string (if YAML parser available)
    bool loadFromYaml(const std::string& yamlStr, bool merge = true);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type conversion and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> get(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get value with type safety
    template<typename T>
    T getValue(const std::string& key) const;
    
    // Get value with error handling
    template<typename T>
    T getValue(const std::string& key, const std::string& errorContext) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<bool>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set with metadata
    void setWithMetadata(const std::string& key, const ConfigValue& value, 
                        const ConfigMetadata& metadata, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and get metadata
    bool has(const std::string& key) const;
    std::optional<ConfigMetadata> getMetadata(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all keys with metadata
    std::vector<std::pair<std::string, ConfigMetadata>> getKeysWithMetadata() const;
    
    // Clear all
    void clear();
    
    // Configuration presets
    static std::vector<ConfigPreset> getPresets();
    bool applyPreset(const std::string& presetName);
    bool addPreset(const ConfigPreset& preset);
    
    // Configuration inheritance
    bool inheritFrom(const Config& parent, bool merge = true);
    
    // Validation
    bool validate() const;
    bool validate(const std::string& key) const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration change tracking
    std::vector<ConfigDiff> getChanges(const Config& other) const;
    std::vector<ConfigDiff> getChangesSince(const Config& previous) const;
    bool hasChanges(const Config& other) const;
    void resetChangeTracking();
    
    // Backup and restore
    bool backup(const std::string& filepath) const;
    bool restore(const std::string& filepath);
    
    // JSON schema validation
    bool loadSchema(const JsonSchema& schema);
    bool validateWithSchema() const;
    
    // Environment-based configuration
    static std::string normalizeEnvVarName(const std::string& name);
    
    // Comparison
    bool equals(const Config& other) const;
    std::string toJson() const;
    
    // Get configuration summary
    std::string summary() const;
    std::string toString() const;
    
    // Configuration helper API
    static bool isNumericType(const std::string& valueStr);
    static bool isBooleanType(const std::string& valueStr);
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Version control
    void setVersion(const std::string& version);
    std::string getVersion() const;
    
    // Configuration history
    void enableHistory(bool enabled);
    std::vector<Config> getHistory() const;
    
    // Configuration merge
    void merge(const Config& other, bool mergeDefaults = true);
    
    // Error handling
    std::vector<std::string> getErrorMessages() const { return errorMessages; }
    void clearErrorMessages() { errorMessages.clear(); }
    
    // Type conversions
    static ConfigValue convertToVariant(const std::string& valueStr);
    template<typename T>
    static std::string convertToString(const T& value);
    
    // Configuration cleanup
    static void cleanupTempFiles();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string errorTypeToString(ConfigErrorType type);
    static bool isFileExists(const std::string& filepath);
    static std::string generateTempFile(const std::string& prefix = "config_backup_");
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static ConfigValue jsonToConfigValue(const nlohmann::json& json);
    
    // Error tracking
    mutable std::vector<std::string> errorMessages;
    mutable std::mutex errorMutex;
    
    // Configuration metadata storage
    mutable std::map<std::string, ConfigMetadata> metadataStore;
    mutable std::mutex metadataMutex;
    
    // Change tracking
    mutable std::vector<ConfigDiff> changeHistory;
    mutable std::mutex changeHistoryMutex;
    
    // Version control
    std::string version;
    bool historyEnabled;
    
    // Preset storage
    static std::map<std::string, ConfigPreset> presetStore;
    
    // Schema storage
    JsonSchema currentSchema;
    
    // Validation rules
    static std::map<std::string, ValidationRule> validationRules;
    
    // Configuration instances for comparison
    static std::vector<std::weak_ptr<Config>> historyInstances;
    
    // Thread safety
    mutable std::mutex configMutex;
    
    // Error helpers
    void addError(const std::string& error, ConfigErrorType type = ConfigErrorType::InvalidValue) {
        std::lock_guard<std::mutex> lock(errorMutex);
        errorMessages.push_back(error);
    }
    
    bool validateValue(const ConfigMetadata& metadata) const;
    std::string getValueAsString(const ConfigValue& value) const;
    
    // JSON/YAML parsing helpers
    bool parseJsonFile(const std::string& filepath, bool merge);
    bool parseKeyValueFile(const std::string& filepath, bool merge);
    std::string escapeString(const std::string& str) const;
    
    // Type conversion helpers
    template<typename T>
    static bool canConvert(const ConfigValue& from, T& to);
    
    // Preset initialization
    static void initializePresets();
    static void initializeValidationRules();
};

} // namespace nlm

// Explicit template instantiations
extern template std::optional<int> Config::get<int>(const std::string&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
extern template std::optional<double> Config::get<double>(const std::string&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;

extern template std::optional<int> Config::get<int>(const std::string&, const int&) const;
extern template std::optional<int64_t> Config::get<int64_t>(const std::string&, const int64_t&) const;
extern template std::optional<double> Config::get<double>(const std::string&, const double&) const;
extern template std::optional<bool> Config::get<bool>(const std::string&, const bool&) const;
extern template std::optional<std::string> Config::get<std::string>(const std::string&, const std::string&) const;

extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

extern template int Config::getValue<int>(const std::string&) const;
extern template int64_t Config::getValue<int64_t>(const std::string&) const;
extern template double Config::getValue<double>(const std::string&) const;
extern template bool Config::getValue<bool>(const std::string&) const;
extern template std::string Config::getValue<std::string>(const std::string&) const;

extern template int Config::getValue<int>(const std::string&, const std::string&) const;
extern template int64_t Config::getValue<int64_t>(const std::string&, const std::string&) const;
extern template double Config::getValue<double>(const std::string&, const std::string&) const;
extern template bool Config::getValue<bool>(const std::string&, const std::string&) const;
extern template std::string Config::getValue<std::string>(const std::string&, const std::string&) const;

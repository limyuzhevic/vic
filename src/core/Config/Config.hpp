#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <regex>

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

// Validation result struct for reporting validation errors
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs = {}, const std::vector<std::string>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getErrors() const {
        std::ostringstream oss;
        for (const auto& error : errors) {
            oss << error << "\n";
        }
        return oss.str();
    }
    
    std::string getWarnings() const {
        std::ostringstream oss;
        for (const auto& warning : warnings) {
            oss << warning << "\n";
        }
        return oss.str();
    }
};

// Configuration type enumeration
enum class ConfigType {
    INTEGER,
    DOUBLE,
    STRING,
    BOOLEAN,
    INTEGER_ARRAY,
    DOUBLE_ARRAY,
    STRING_ARRAY,
    OBJECT
};

// Schema definition for a single configuration option
struct ConfigSchema {
    std::string name;
    ConfigType type;
    std::string description;
    std::string defaultValue;
    std::vector<std::string> validators;
    bool required;
    std::map<std::string, ConfigSchema> nestedSchema;
    
    ConfigSchema() : type(ConfigType::STRING), required(false) {}
    
    ConfigSchema(const std::string& n, ConfigType t, const std::string& desc = "", 
                 const std::string& def = "", bool req = false)
        : name(n), type(t), description(desc), defaultValue(def), required(req) {}
    
    bool isValidType(const ConfigValue& value) const;
    std::string getTypeName() const;
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
    
    // Enhanced load methods
    bool loadFromFile(const std::string& filepath, bool validate = true);
    bool loadFromJSON(const std::string& filepath, bool validate = true);
    bool loadFromYAML(const std::string& filepath, bool validate = true);
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    bool loadFromJSON(const std::string& jsonString, bool validate = true);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    std::string toJSON() const;
    std::string toYAML() const;
    
    // Schema validation
    ValidationResult validate(const ConfigSchema& schema) const;
    bool validateAgainstSchema(const std::vector<ConfigSchema>& schema);
    
    // Schema management
    void setSchema(const std::vector<ConfigSchema>& schema);
    void addSchema(const ConfigSchema& schema);
    void clearSchema();
    
    // Get schema help
    std::string getSchemaHelp() const;
    std::vector<ConfigSchema> getSchema() const { return schema; }
    
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
    
    // Error handling
    std::string getLastError() const { return lastError; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Enhanced implementation details
    std::vector<ConfigSchema> schema;
    std::string lastError;
    
    // Parser helper methods
    bool parseJSON(const std::string& content);
    bool parseYAML(const std::string& content);
    bool parseKeyValue(const std::string& content);
    
    // Validation helper methods
    ValidationResult validateKey(const std::string& key) const;
    ValidationResult validateValue(const ConfigEntry& entry) const;
    
    // Error handling
    void setError(const std::string& error) { lastError = error; }
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeJSON(const std::string& str);
    static std::string escapeYAML(const std::string& str);
};

} // namespace nlm

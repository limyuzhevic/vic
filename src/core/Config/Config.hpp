#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <unordered_set>
#include <any>

// Forward declarations
namespace nlohmann {
    class json;
}

namespace simdjson {
    class ondemand_parser;
}

namespace nlm {

// Configuration value types with enhanced support
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<std::any>,  // For nested arrays
    std::map<std::string, std::any>  // For nested objects
>;

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs = {}, 
                    const std::vector<std::string>& warns = {}) 
        : valid(v), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) {
        valid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string toString() const {
        std::string result;
        if (!valid) {
            result += "INVALID: ";
            for (const auto& err : errors) {
                result += "ERROR: " + err + "; ";
            }
        }
        for (const auto& warn : warnings) {
            result += "WARNING: " + warn + "; ";
        }
        return result;
    }
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<bool(const std::any&)> validator;
    std::string errorMessage;
    bool required;
    std::string typeName;
    
    ValidationRule(const std::string& k, std::function<bool(const std::any&)> v,
                  const std::string& err, bool req = false, 
                  const std::string& type = "any")
        : key(k), validator(v), errorMessage(err), required(req), typeName(type) {}
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment,
    Imported
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string type;
    std::any defaultValue;
    bool required;
    std::vector<std::string> allowedValues;
    std::pair<double, double> numericRange;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), type("string"), 
          required(false), numericRange(std::make_pair(0.0, 0.0)) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& t = "string",
                const std::any& def = {}, bool req = false)
        : key(k), value(v), source(s), description(desc), type(t),
          defaultValue(def), required(req), numericRange(std::make_pair(0.0, 0.0)) {}
    
    bool isValid() const {
        // Type validation
        if (type == "int") {
            return std::holds_alternative<int>(value);
        } else if (type == "int64") {
            return std::holds_alternative<int64_t>(value);
        } else if (type == "double") {
            return std::holds_alternative<double>(value);
        } else if (type == "bool") {
            return std::holds_alternative<bool>(value);
        } else if (type == "string") {
            return std::holds_alternative<std::string>(value);
        } else if (type == "int_array") {
            return std::holds_alternative<std::vector<int>>(value);
        } else if (type == "double_array") {
            return std::holds_alternative<std::vector<double>>(value);
        } else if (type == "string_array") {
            return std::holds_alternative<std::vector<std::string>>(value);
        } else if (type == "any_array") {
            return std::holds_alternative<std::vector<std::any>>(value);
        } else if (type == "object") {
            return std::holds_alternative<std::map<std::string, std::any>>(value);
        }
        return false;
    }
    
    std::any getAnyValue() const {
        // Convert ConfigValue to any (simplified implementation)
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            return std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            return std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return std::get<std::vector<std::string>>(value);
        }
        return std::any();
    }
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
    
    // Load from file (JSON, YAML, or .properties format)
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& jsonContent);
    bool loadFromYAML(const std::string& yamlContent);
    bool loadFromProperties(const std::string& propertiesContent);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath, bool prettyPrint = false) const;
    std::string toJSON(bool prettyPrint = false) const;
    std::string toYAML() const;
    std::string toProperties() const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get raw ConfigValue for advanced usage
    std::optional<ConfigValue> getRaw(const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with validation
    bool setValidated(const std::string& key, const ConfigValue& value, 
                     ConfigSource source = ConfigSource::Runtime);
    
    // Configuration schema management
    void addValidationRule(const ValidationRule& rule);
    void addDefault(const std::string& key, const ConfigValue& defaultValue, 
                   const std::string& type = "string", const std::string& description = "");
    void addSchema(const std::string& name, const std::vector<ValidationRule>& rules);
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, const std::string& type) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysByPrefix(const std::string& prefix) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation
    ValidationResult validate() const;
    ValidationResult validate(const std::string& schemaName) const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Imported);
    void mergeFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Statistics
    size_t size() const;
    std::map<ConfigSource, size_t> getSourceCounts() const;
    
    // Environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // File format detection
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    static bool isProperties(const std::string& content);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string replaceAll(std::string str, const std::string& from, const std::string& to);
    
    // JSON serialization helpers
    static void jsonToConfig(const nlohmann::json& j, Config& config);
    static nlohmann::json configToJson(const Config& config);
    
    // Validation helpers
    ValidationResult validateAgainstRules(const std::vector<ValidationRule>& rules) const;
    
    friend class ConfigInternalAccess; // For testing and advanced usage
};

} // namespace nlm

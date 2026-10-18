#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <stdexcept>
#include <functional>
#include <regex>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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
    Environment,
    Runtime
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), required(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), required(req) {}
    
    // Type validation helper
    template<typename T>
    bool isValueType() const {
        try {
            std::get<T>(value);
            return true;
        } catch (const std::bad_variant_access&) {
            return false;
        }
    }
    
    // Get value with type safety
    template<typename T>
    T getValue() const {
        try {
            return std::get<T>(value);
        } catch (const std::bad_variant_access&) {
            throw std::runtime_error("Configuration value '" + key + "' is not of expected type");
        }
    }
};

// Configuration validation rule
struct ValidationRule {
    std::string ruleType; // "range", "pattern", "enum", "custom"
    std::string ruleValue;
    std::string errorMessage;
    
    ValidationRule(const std::string& type, const std::string& value, const std::string& error)
        : ruleType(type), ruleValue(value), errorMessage(error) {}
};

// Configuration schema for validation
struct ConfigSchemaEntry {
    std::string key;
    std::string description;
    bool required;
    ConfigSource defaultSource;
    std::string valueType; // "int", "float", "string", "bool", "array"
    std::vector<ValidationRule> validationRules;
    std::string defaultValue;
    
    ConfigSchemaEntry(const std::string& k, const std::string& desc, bool req = false, 
                     ConfigSource src = ConfigSource::Default,
                     const std::string& type = "string",
                     const std::string& defaultVal = "")
        : key(k), description(desc), required(req), defaultSource(src), 
          valueType(type), defaultValue(defaultVal) {}
    
    // Add validation rule
    void addValidation(const std::string& ruleType, const std::string& ruleValue, const std::string& errorMsg) {
        validationRules.emplace_back(ruleType, ruleValue, errorMsg);
    }
    
    // Validate value against rules
    bool validate(const ConfigValue& value) const {
        // Check type compatibility
        if (!isValidType(value)) {
            return false;
        }
        
        // Apply validation rules
        for (const auto& rule : validationRules) {
            if (!validateRule(rule, value)) {
                return false;
            }
        }
        
        return true;
    }
    
private:
    bool isValidType(const ConfigValue& value) const {
        if (valueType == "int" || valueType == "integer") {
            return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
        } else if (valueType == "float" || valueType == "double") {
            return std::holds_alternative<double>(value);
        } else if (valueType == "bool" || valueType == "boolean") {
            return std::holds_alternative<bool>(value);
        } else if (valueType == "string") {
            return std::holds_alternative<std::string>(value);
        } else if (valueType == "array") {
            return std::holds_alternative<std::vector<int>>(value) ||
                   std::holds_alternative<std::vector<double>>(value) ||
                   std::holds_alternative<std::vector<std::string>>(value);
        }
        return true; // Unknown type, assume valid
    }
    
    bool validateRule(const ValidationRule& rule, const ConfigValue& value) const {
        if (rule.ruleType == "range") {
            return validateRange(rule, value);
        } else if (rule.ruleType == "pattern") {
            return validatePattern(rule, value);
        } else if (rule.ruleType == "enum") {
            return validateEnum(rule, value);
        } else if (rule.ruleType == "min") {
            return validateMin(rule, value);
        } else if (rule.ruleType == "max") {
            return validateMax(rule, value);
        } else if (rule.ruleType == "length") {
            return validateLength(rule, value);
        }
        return true; // Unknown rule type
    }
    
    bool validateRange(const ValidationRule& rule, const ConfigValue& value) const {
        // Parse range format: "min,max"
        size_t pos = rule.ruleValue.find(',');
        if (pos == std::string::npos) return true;
        
        double min, max;
        try {
            min = std::stod(rule.ruleValue.substr(0, pos));
            max = std::stod(rule.ruleValue.substr(pos + 1));
        } catch (...) {
            return true; // Invalid range format
        }
        
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= min && std::get<double>(value) <= max;
        } else if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) >= static_cast<int>(min) && std::get<int>(value) <= static_cast<int>(max);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) >= static_cast<int64_t>(min) && std::get<int64_t>(value) <= static_cast<int64_t>(max);
        }
        
        return false;
    }
    
    bool validatePattern(const ValidationRule& rule, const ConfigValue& value) const {
        if (!std::holds_alternative<std::string>(value)) return false;
        
        const std::string& strValue = std::get<std::string>(value);
        std::regex pattern(rule.ruleValue);
        return std::regex_match(strValue, pattern);
    }
    
    bool validateEnum(const ValidationRule& rule, const ConfigValue& value) const {
        // Parse enum format: "value1,value2,value3"
        std::vector<std::string> enumValues;
        size_t start = 0;
        size_t pos;
        while ((pos = rule.ruleValue.find(',', start)) != std::string::npos) {
            enumValues.push_back(rule.ruleValue.substr(start, pos - start));
            start = pos + 1;
        }
        enumValues.push_back(rule.ruleValue.substr(start));
        
        if (std::holds_alternative<std::string>(value)) {
            const std::string& strValue = std::get<std::string>(value);
            return std::find(enumValues.begin(), enumValues.end(), strValue) != enumValues.end();
        } else if (std::holds_alternative<int>(value)) {
            int intValue = std::get<int>(value);
            for (const auto& enumStr : enumValues) {
                try {
                    int enumInt = std::stoi(enumStr);
                    if (enumInt == intValue) return true;
                } catch (...) {}
            }
        }
        
        return false;
    }
    
    bool validateMin(const ValidationRule& rule, const ConfigValue& value) const {
        double min = std::stod(rule.ruleValue);
        
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) >= min;
        } else if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) >= static_cast<int>(min);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) >= static_cast<int64_t>(min);
        }
        
        return false;
    }
    
    bool validateMax(const ValidationRule& rule, const ConfigValue& value) const {
        double max = std::stod(rule.ruleValue);
        
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value) <= max;
        } else if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) <= static_cast<int>(max);
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) <= static_cast<int64_t>(max);
        }
        
        return false;
    }
    
    bool validateLength(const ValidationRule& rule, const ConfigValue& value) const {
        int length = std::stoi(rule.ruleValue);
        
        if (std::holds_alternative<std::string>(value)) {
            return static_cast<int>(std::get<std::string>(value).size()) == length;
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            return static_cast<int>(std::get<std::vector<int>>(value).size()) == length;
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            return static_cast<int>(std::get<std::vector<double>>(value).size()) == length;
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            return static_cast<int>(std::get<std::vector<std::string>>(value).size()) == length;
        }
        
        return false;
    }
};

// Configuration validation error
class ConfigValidationError : public ConfigException {
public:
    ConfigValidationError(const std::string& key, const std::string& expected, const std::string& actual)
        : ConfigException("Validation error for key '" + key + "': expected " + expected + ", got " + actual) {}
    
    ConfigValidationError(const std::string& key, const std::string& validationError)
        : ConfigException("Validation error for key '" + key + "': " + validationError) {}
};

// Configuration schema for validation
class ConfigSchema {
public:
    ConfigSchema() {}
    
    // Add a schema entry
    void addEntry(const ConfigSchemaEntry& entry) {
        schemaEntries[entry.key] = entry;
    }
    
    // Check if a key is in the schema
    bool hasEntry(const std::string& key) const {
        return schemaEntries.find(key) != schemaEntries.end();
    }
    
    // Get schema entry
    const ConfigSchemaEntry* getEntry(const std::string& key) const {
        auto it = schemaEntries.find(key);
        return it != schemaEntries.end() ? &it->second : nullptr;
    }
    
    // Get all keys
    std::vector<std::string> getKeys() const {
        std::vector<std::string> keys;
        for (const auto& [key, entry] : schemaEntries) {
            keys.push_back(key);
        }
        return keys;
    }
    
    // Validate configuration against schema
    bool validate(const Config& config) const {
        for (const auto& [key, entry] : schemaEntries) {
            if (entry.required && !config.has(key)) {
                return false; // Required key missing
            }
            
            if (config.has(key)) {
                // Validate the key's value
                auto value = config.getValue<ConfigValue>(key);
                if (!entry.validate(value)) {
                    return false;
                }
            }
        }
        
        return true;
    }
    
    // Validate configuration and get errors
    std::vector<std::string> validateWithErrors(const Config& config) const {
        std::vector<std::string> errors;
        
        for (const auto& [key, entry] : schemaEntries) {
            if (entry.required && !config.has(key)) {
                errors.push_back("Missing required configuration key: '" + key + "'");
            }
            
            if (config.has(key)) {
                auto value = config.getValue<ConfigValue>(key);
                if (!entry.validate(value)) {
                    errors.push_back("Invalid value for configuration key '" + key + "': " + getValidationErrorMessage(entry, value));
                }
            }
        }
        
        return errors;
    }
    
private:
    std::map<std::string, ConfigSchemaEntry> schemaEntries;
    
    std::string getValidationErrorMessage(const ConfigSchemaEntry& entry, const ConfigValue& value) const {
        for (const auto& rule : entry.validationRules) {
            if (!validateRule(rule, value)) {
                return rule.errorMessage;
            }
        }
        return "Value does not match schema";
    }
    
    bool validateRule(const ValidationRule& rule, const ConfigValue& value) const {
        if (rule.ruleType == "range") {
            return validateRange(rule, value);
        } else if (rule.ruleType == "pattern") {
            return validatePattern(rule, value);
        } else if (rule.ruleType == "enum") {
            return validateEnum(rule, value);
        } else if (rule.ruleType == "min") {
            return validateMin(rule, value);
        } else if (rule.ruleType == "max") {
            return validateMax(rule, value);
        } else if (rule.ruleType == "length") {
            return validateLength(rule, value);
        }
        return true;
    }
    
    bool validateRange(const ValidationRule& rule, const ConfigValue& value) const {
        // Simplified implementation
        return true;
    }
    
    bool validatePattern(const ValidationRule& rule, const ConfigValue& value) const {
        // Simplified implementation
        return true;
    }
    
    bool validateEnum(const ValidationRule& rule, const ConfigValue& value) const {
        // Simplified implementation
        return true;
    }
    
    bool validateMin(const ValidationRule& rule, const ConfigValue& value) const {
        // Simplified implementation
        return true;
    }
    
    bool validateMax(const ValidationRule& rule, const ConfigValue& value) const {
        // Simplified implementation
        return true;
    }
    
    bool validateLength(const ValidationRule& rule, const ConfigValue& value) const {
        // Simplified implementation
        return true;
    }
};

// Configuration exception
class ConfigException : public std::runtime_error {
public:
    ConfigException(const std::string& message) : std::runtime_error(message) {}
};

// String utilities
class StringUtils {
public:
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string toUpper(const std::string& str);
    static bool contains(const std::string& str, const std::string& substr);
    static std::string replaceAll(std::string str, const std::string& from, const std::string& to);
    static std::string escapeRegex(const std::string& str);
    static std::string generateRandomString(size_t length);
    static std::string generateCorrelationId();
};

// String utilities
class StringUtils {
public:
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

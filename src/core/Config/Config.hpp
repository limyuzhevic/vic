#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>
#include <any>
#include <type_traits>
#include <exception>
#include <stdexcept>
#include <cctype>

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

// Exception hierarchy for configuration errors
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigFileError : public ConfigError {
public:
    explicit ConfigFileError(const std::string& msg) : ConfigError("Config file error: " + msg) {}
};

class ConfigFormatError : public ConfigError {
public:
    explicit ConfigFormatError(const std::string& msg) : ConfigError("Config format error: " + msg) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& msg) : ConfigError("Config validation error: " + msg) {}
};

class ConfigLoadError : public ConfigError {
public:
    explicit ConfigLoadError(const std::string& msg) : ConfigError("Config load error: " + msg) {}
};

// Configuration schema element
class ConfigSchemaElement {
public:
    enum class Type {
        INTEGER,
        INTEGER64,
        DOUBLE,
        BOOLEAN,
        STRING,
        INT_VECTOR,
        DOUBLE_VECTOR,
        STRING_VECTOR,
        OBJECT
    };
    
    struct ValidationRule {
        std::string name;
        std::any value;
        bool (*validator)(const std::any& value) = nullptr;
    };
    
    std::string name;
    Type type;
    std::vector<ValidationRule> rules;
    std::string description;
    bool required;
    std::any defaultValue;
    
    ConfigSchemaElement(const std::string& n, Type t, const std::string& desc = "")
        : name(n), type(t), description(desc), required(false) {}
    
    void addRule(const std::string& ruleName, const std::any& value, bool (*validator)(const std::any&) = nullptr) {
        ValidationRule rule;
        rule.name = ruleName;
        rule.value = value;
        rule.validator = validator;
        rules.push_back(rule);
    }
    
    bool validate(const std::any& value) const {
        for (const auto& rule : rules) {
            if (rule.validator) {
                if (!rule.validator(value)) {
                    return false;
                }
            }
            // Add rule-specific validation logic here
        }
        return true;
    }
};

class ConfigSchema {
public:
    void addElement(const ConfigSchemaElement& element) {
        elements[element.name] = element;
        if (element.required) {
            requiredElements.push_back(element.name);
        }
    }
    
    bool validate(const std::unordered_map<std::string, std::any>& data) const {
        for (const auto& required : requiredElements) {
            if (data.find(required) == data.end()) {
                return false;
            }
        }
        
        for (const auto& [key, value] : data) {
            auto it = elements.find(key);
            if (it != elements.end()) {
                if (!it->second.validate(value)) {
                    return false;
                }
            }
        }
        return true;
    }
    
    std::vector<std::string> getMissingElements(const std::unordered_map<std::string, std::any>& data) const {
        std::vector<std::string> missing;
        for (const auto& required : requiredElements) {
            if (data.find(required) == data.end()) {
                missing.push_back(required);
            }
        }
        return missing;
    }
    
private:
    std::unordered_map<std::string, ConfigSchemaElement> elements;
    std::vector<std::string> requiredElements;
};

// File format detection
enum class ConfigFileFormat {
    KEY_VALUE,
    JSON,
    YAML,
    UNKNOWN
};

ConfigFileFormat detectFileFormat(const std::string& filepath);

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
    
    // Load from file (automatic format detection)
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with explicit format
    bool loadFromFile(const std::string& filepath, ConfigFileFormat format);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (automatic format detection based on extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to file with explicit format
    bool saveToFile(const std::string& filepath, ConfigFileFormat format) const;
    
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
    
    // JSON/YAML file processing
    static bool loadFromJSON(const std::string& filepath, Config& config, const ConfigSchema* schema = nullptr);
    static bool loadFromYAML(const std::string& filepath, Config& config, const ConfigSchema* schema = nullptr);
    static bool saveToJSON(const std::string& filepath, const Config& config);
    static bool saveToYAML(const std::string& filepath, const Config& config);
    
    // Key-value file processing (backward compatibility)
    static bool loadFromKeyValue(const std::string& filepath, Config& config);
    static bool saveToKeyValue(const std::string& filepath, const Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON/YAML internal helpers
    static bool parseJSONValue(const nlohmann::json& jsonValue, ConfigValue& configValue);
    static bool parseYAMLValue(const nlohmann::json& yamlValue, ConfigValue& configValue);
    static nlohmann::json serializeToJSON(const ConfigValue& configValue) const;
    static bool setValueFromAny(Config& config, const std::string& key, const std::any& value, ConfigSource source);
};

} // namespace nlm

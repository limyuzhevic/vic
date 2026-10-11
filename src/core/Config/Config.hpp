#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>

namespace nlohmann {
    class json;
}

namespace nlm {

// Forward declarations
class Config;
class JsonParser;

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
    Environment,
    Programmatic
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        isValid = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getErrorSummary() const {
        if (errors.empty()) return "";
        std::string result = "Validation errors:";
        for (const auto& error : errors) {
            result += "\n  - " + error;
        }
        return result;
    }
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    bool isRequired;
};

// Configuration schema definition
struct ConfigSchema {
    std::string name;
    std::string version;
    std::string description;
    std::vector<ValidationRule> rules;
    std::unordered_map<std::string, std::string> defaultDescriptions;
    
    ConfigSchema(const std::string& name = "NLM Config", const std::string& version = "1.0")
        : name(name), version(version) {}
    
    void addRule(const std::string& key, std::function<bool(const ConfigValue&)> validator, 
                 const std::string& errorMessage, bool isRequired = false) {
        rules.push_back({key, validator, errorMessage, isRequired});
    }
    
    void addStringRule(const std::string& key, size_t minLength = 0, size_t maxLength = 1000, 
                      bool isRequired = false) {
        addRule(key, [minLength, maxLength](const ConfigValue& val) {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, std::string>) {
                return val.size() >= minLength && val.size() <= maxLength;
            }
            return false;
        }, "Value must be a string with length between " + std::to_string(minLength) + " and " + std::to_string(maxLength), isRequired);
    }
    
    void addNumericRule(const std::string& key, double min = std::numeric_limits<double>::lowest(), 
                       double max = std::numeric_limits<double>::max(), bool isRequired = false) {
        addRule(key, [min, max](const ConfigValue& val) {
            if constexpr (std::is_same_v<std::decay_t<decltype(val)>, int>) {
                return static_cast<double>(val) >= min && static_cast<double>(val) <= max;
            } else if constexpr (std::is_same_v<std::decay_t<decltype(val)>, double>) {
                return val >= min && val <= max;
            }
            return false;
        }, "Value must be a numeric value between " + std::to_string(min) + " and " + std::to_string(max), isRequired);
    }
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    std::string metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                    timestamp(std::chrono::system_clock::now()), metadata() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          timestamp(std::chrono::system_clock::now()), metadata() {}
    
    std::string getFormattedTimestamp() const {
        auto time = std::chrono::system_clock::to_time_t(timestamp);
        std::tm tm_time = *std::localtime(&time);
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_time);
        return std::string(buffer);
    }
};

// Constants for file extensions
inline constexpr const char* JSON_EXT = ".json";
inline constexpr const char* YAML_EXT = ".yaml";
inline constexpr const char* CFG_EXT = ".cfg";
    
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

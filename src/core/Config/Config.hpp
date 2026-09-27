#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>

namespace nlm {

// Forward declarations for better organization
class Config;

// Enhanced configuration value types with better type safety
enum class ConfigValueType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    StringList,
    DoubleList,
    IntList
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string toString() const {
        std::ostringstream oss;
        if (!isValid) {
            oss << "Validation failed:";
            for (const auto& error : errors) {
                oss << "\n  ERROR: " << error;
            }
        }
        for (const auto& warning : warnings) {
            oss << "\n  WARNING: " << warning;
        }
        return oss.str();
    }
};

// Configuration schema definition for validation
struct ConfigSchema {
    std::map<std::string, ConfigValueType> typeDefinitions;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::map<std::string, std::string> descriptions;
    std::map<std::string, ConfigValue> defaultValues;
    bool isRequired(const std::string& key) const {
        return defaultValues.find(key) == defaultValues.end();
    }
    
    void addParameter(const std::string& key, ConfigValueType type, 
                     const std::string& description,
                     const std::optional<ConfigValue>& defaultValue = std::nullopt,
                     const std::function<bool(const ConfigValue&)>& validator = nullptr) {
        typeDefinitions[key] = type;
        descriptions[key] = description;
        if (defaultValue) {
            defaultValues[key] = *defaultValue;
        }
        if (validator) {
            validators[key] = validator;
        }
    }
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const {
        ValidationResult result;
        
        // Check if key is defined in schema
        if (typeDefinitions.find(key) == typeDefinitions.end()) {
            result.addWarning("Unknown configuration key: " + key);
        }
        
        // Check type consistency
        if (typeDefinitions.find(key) != typeDefinitions.end()) {
            ConfigValueType expectedType = typeDefinitions.at(key);
            if (!isTypeCompatible(value, expectedType)) {
                result.addError("Type mismatch for key " + key + ": expected " + 
                              typeToString(expectedType) + ", got " + valueTypeToString(value));
            }
        }
        
        // Run custom validators
        if (validators.find(key) != validators.end()) {
            if (!validators.at(key)(value)) {
                result.addError("Custom validation failed for key: " + key);
            }
        }
        
        return result;
    }
    
    static bool isTypeCompatible(const ConfigValue& value, ConfigValueType expectedType) {
        return std::visit([expectedType](auto&& val) -> bool {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int> && expectedType == ConfigValueType::Int) return true;
            else if constexpr (std::is_same_v<T, int64_t> && expectedType == ConfigValueType::Int64) return true;
            else if constexpr (std::is_same_v<T, double> && expectedType == ConfigValueType::Double) return true;
            else if constexpr (std::is_same_v<T, bool> && expectedType == ConfigValueType::Bool) return true;
            else if constexpr (std::is_same_v<T, std::string> && expectedType == ConfigValueType::String) return true;
            else if constexpr (std::is_same_v<T, std::vector<int>> && expectedType == ConfigValueType::IntList) return true;
            else if constexpr (std::is_same_v<T, std::vector<double>> && expectedType == ConfigValueType::DoubleList) return true;
            else if constexpr (std::is_same_v<T, std::vector<std::string>> && expectedType == ConfigValueType::StringList) return true;
            else return false;
        }, value);
    }
    
    static std::string typeToString(ConfigValueType type) {
        switch (type) {
            case ConfigValueType::Int: return "int";
            case ConfigValueType::Int64: return "int64_t";
            case ConfigValueType::Double: return "double";
            case ConfigValueType::Bool: return "bool";
            case ConfigValueType::String: return "string";
            case ConfigValueType::StringList: return "string_list";
            case ConfigValueType::DoubleList: return "double_list";
            case ConfigValueType::IntList: return "int_list";
            default: return "unknown";
        }
    }
    
    static std::string valueTypeToString(const ConfigValue& value) {
        return std::visit([](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) return "int";
            else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
            else if constexpr (std::is_same_v<T, double>) return "double";
            else if constexpr (std::is_same_v<T, bool>) return "bool";
            else if constexpr (std::is_same_v<T, std::string>) return "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) return "int_list";
            else if constexpr (std::is_same_v<T, std::vector<double>>) return "double_list";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "string_list";
            else return "unknown";
        }, value);
    }
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    mutable bool isValidated;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), timestamp(std::chrono::system_clock::now()), 
          isValidated(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(std::chrono::system_clock::now()), 
          isValidated(false) {}
    
    bool isValidType(ConfigValueType expectedType) const {
        return ConfigSchema::isTypeCompatible(value, expectedType);
    }
    
    std::string getTypeString() const {
        return ConfigSchema::valueTypeToString(value);
    }
    
    std::string getSourceString() const {
        switch (source) {
            case ConfigSource::Default: return "Default";
            case ConfigSource::File: return "File";
            case ConfigSource::CommandLine: return "CommandLine";
            case ConfigSource::Runtime: return "Runtime";
            default: return "Unknown";
        }
    }
};

// Enhanced configuration class with schema validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (supports JSON, YAML, and key=value formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with proper serialization
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             bool validate = true);
    
    // Check existence and validation status
    bool has(const std::string& key) const;
    bool isValidated(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation info
    std::string summary() const;
    
    // Schema management
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema& getSchema() const { return schema; }
    
    // Batch operations for performance
    void setBatch(const std::map<std::string, ConfigValue>& values);
    
    // Environment variable support
    void loadFromEnvironment(const std::map<std::string, std::string>& envMapping = {});
    
    // Configuration merge strategies
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Validate all configuration values
    ValidationResult validateAll() const;
    
    // Get configuration by source
    std::vector<std::pair<std::string, ConfigSource>> getBySource(ConfigSource source) const;
    
    // Configuration presets for common scenarios
    static Config createPhase2Config();
    static Config createPhase6Config();
    static Config createMinimalConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    ConfigSchema schema;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJsonFile(const std::string& filepath, std::map<std::string, ConfigValue>& data);
    static bool parseYamlFile(const std::string& filepath, std::map<std::string, ConfigValue>& data);
    static bool parseSimpleFile(const std::string& filepath, std::map<std::string, ConfigValue>& data);
    static ConfigValue stringToConfigValue(const std::string& str);
    static std::string configValueToString(const ConfigValue& value);
    
    // Enhanced command-line parsing
    static std::map<std::string, std::string> parseCommandLineArgs(int argc, char** argv);
    static std::map<std::string, ConfigValue> parseConfigFile(const std::string& filepath);
};

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Convenience functions
inline bool validateSimulationParameters(const Config& config) {
    ValidationResult result;
    
    auto validateRange = [](const std::string& key, double value, double min, double max, const std::string& desc) {
        if (value < min || value > max) {
            return "Value " + std::to_string(value) + " for " + key + " is out of range [" + 
                   std::to_string(min) + ", " + std::to_string(max) + "]" + desc;
        }
        return std::string();
    };
    
    if (auto timestepOpt = config.get<double>("simulation_timestep")) {
        if (auto error = validateRange("simulation_timestep", *timestepOpt, 0.0001, 0.1, "s")) {
            result.addError(error);
        }
    }
    
    if (auto connProbOpt = config.get<float>("connection_probability")) {
        if (auto error = validateRange("connection_probability", *connProbOpt, 0.0f, 1.0f, "")) {
            result.addError(error);
        }
    }
    
    return result.isValid;
}

inline std::string configToString(const Config& config) {
    std::ostringstream oss;
    oss << "Configuration (validated):" << std::endl;
    for (const auto& key : config.getKeys()) {
        oss << "  " << key << " = ";
        auto valOpt = config.get<std::string>(key);
        if (valOpt) {
            oss << *valOpt;
        } else {
            oss << "[UNABLE TO RETRIEVE]";
        }
        oss << " [" << config.getSourceString(key) << "]" << std::endl;
    }
    return oss.str();
}

} // namespace nlm
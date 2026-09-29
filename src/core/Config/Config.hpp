#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "ConfigSchema.hpp"

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

// Helper functions for schema integration
namespace ConfigSchemaHelper {
    // Convert string to ConfigValue with type checking
    bool stringToConfigValue(const std::string& valueStr, ConfigParameterType type, ConfigValue& outValue);
    
    // Convert ConfigValue to string for serialization
    std::string configValueToString(const ConfigValue& value);
    
    // Get ConfigParameterType from ConfigValue type
    ConfigParameterType getConfigParameterType(const ConfigValue& value);
    
    // Validate configuration parameter value
    bool validateParameterValue(const std::string& name, const ConfigValue& value);
}

// Configuration error class
class ConfigError {
public:
    enum class ErrorType {
        FileNotFound,
        ParseError,
        InvalidType,
        ValidationError,
        MissingRequiredParameter,
        DuplicateParameter,
        FileSaveError
    };
    
    ConfigError(ErrorType type, const std::string& message, 
                const std::string& paramName = "", 
                const std::string& filePath = "")
        : errorType(type), message(message), parameterName(paramName), filePath(filePath) {}
    
    ErrorType errorType;
    std::string message;
    std::string parameterName;
    std::string filePath;
    
    std::string what() const {
        std::string result = "Config Error: ";
        result += message;
        if (!parameterName.empty()) {
            result += " (parameter: " + parameterName + ")";
        }
        if (!filePath.empty()) {
            result += " (file: " + filePath + ")";
        }
        return result;
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
    
    // JSON/YAML parsing utilities
    static bool parseJSON(const std::string& jsonStr, std::unordered_map<std::string, std::string>& outValues);
    static bool parseYAML(const std::string& yamlStr, std::unordered_map<std::string, std::string>& outValues);
    static bool parseLegacyFormat(const std::string& line, std::string& key, std::string& value);
    
    // Validation
    static bool validateFile(const std::string& filepath);
    static std::string getValidationErrors() const;
    
    // Schema integration
    void loadDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Error handling
    std::vector<ConfigError> validationErrors;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isJSON(const std::string& content);
    static bool isYAML(const std::string& content);
    
    // Legacy format support
    bool loadLegacyFromFile(const std::string& filepath);
};

} // namespace nlm

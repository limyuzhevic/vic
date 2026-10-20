#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <algorithm>
#include <map>
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

// Configuration validation error types
class ConfigValidationError : public std::runtime_error {
public:
    explicit ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileError : public std::runtime_error {
public:
    explicit ConfigFileError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigValueError : public std::runtime_error {
public:
    explicit ConfigValueError(const std::string& message) : std::runtime_error(message) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string description;
    ConfigSource source;
    bool required;
    std::string validationPattern;
    std::vector<std::string> allowedValues;
    double minValue;
    double maxValue;
    std::string valueType; // "int", "double", "bool", "string", "list"
    
    ConfigSchema() : description(""), source(ConfigSource::Default), required(false), 
                    minValue(std::numeric_limits<double>::lowest()), 
                    maxValue(std::numeric_limits<double>::max()) {}
    
    ConfigSchema(const std::string& k, const std::string& desc, bool req = false)
        : key(k), description(desc), source(ConfigSource::Default), required(req),
          minValue(std::numeric_limits<double>::lowest()), 
          maxValue(std::numeric_limits<double>::max()) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable bool valid;
    mutable std::vector<std::string> validationErrors;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), valid(true) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), valid(true) {}
    
    // Validate this entry against schema
    bool validate(const ConfigSchema& schema) const;
    
    // Clear validation state
    void resetValidation() const {
        valid = true;
        validationErrors.clear();
    }
};

// Type checking and conversion utilities
class ConfigValidator {
public:
    static bool isValidKey(const std::string& key);
    static bool isValidDescription(const std::string& desc);
    static bool isValidFilePath(const std::string& path);
    static bool isValidArgument(const std::string& arg);
    
    static std::string sanitizeString(const std::string& str);
    static std::vector<std::string> splitArguments(const std::string& args);
    
    template<typename T>
    static bool valueMatchesType(const ConfigValue& value);
    
    static bool valueInRange(const ConfigValue& value, double min, double max);
    static bool valueInAllowedValues(const ConfigValue& value, 
                                    const std::vector<std::string>& allowed);
    static bool valueMatchesPattern(const ConfigValue& value, 
                                    const std::string& pattern);
    
    static std::string valueToString(const ConfigValue& value);
    
    static std::string getTypeName(const ConfigValue& value);
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
    
    // Enhanced load from file (JSON format)
    bool loadFromFile(const std::string& filepath, bool validateSchema = true);
    
    // Enhanced load from command line arguments
    bool loadFromArgs(int argc, char** argv, bool validateArgs = true);
    
    // Enhanced save to file
    bool saveToFile(const std::string& filepath, bool includeComments = true) const;
    
    // Get values with enhanced error handling
    template<typename T>
    std::optional<T> get(const std::string& key, bool validateType = true) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue, bool validateType = true) const;
    
    // Enhanced set values with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validateValue = true);
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validateValue = true);
    void set(const std::string& key, int value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validateValue = true);
    void set(const std::string& key, double value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validateValue = true);
    void set(const std::string& key, bool value, 
             ConfigSource source = ConfigSource::Runtime,
             bool validateValue = true);
    
    // Enhanced check existence
    bool has(const std::string& key) const;
    
    // Enhanced remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with validation status
    std::string summary() const;
    
    // Validation methods
    bool isValid(const std::string& key) const;
    std::vector<std::string> getValidationErrors(const std::string& key) const;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    bool hasSchema(const std::string& key) const;
    ConfigSchema getSchema(const std::string& key) const;
    
    // File and argument validation
    static bool validateFilePath(const std::string& path);
    static bool validateCommandLineArgs(int argc, char** argv);
    
    // Performance optimization
    void setValidationEnabled(bool enabled) { validationEnabled = enabled; }
    bool isValidationEnabled() const { return validationEnabled; }
    
    // Error handling utilities
    static std::string getErrorMessage(const std::exception& e);
    static bool hasErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isFilePathSafe(const std::string& path);
    static bool validateFileContent(const std::string& filepath);
    static std::vector<std::string> extractValidKeysFromArgs(int argc, char** argv);
    
    bool validationEnabled = true;
    mutable bool hasValidationErrors = false;
    mutable std::vector<std::string> globalValidationErrors;
};

} // namespace nlm

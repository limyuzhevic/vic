#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <set>
#include <filesystem>
#include <stdexcept>

// Forward declarations for JSON/YAML
namespace nlohmann {
    class json;
}

// Forward declarations
class Config;

namespace nlm {

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
    SchemaDefault
};

// Validation result
enum class ValidationResult {
    Success,
    Warning,
    Error
};

// Validation error details
struct ValidationError {
    ValidationResult result;
    std::string key;
    std::string message;
    std::string suggestion;
    
    ValidationError(ValidationResult r, const std::string& k, const std::string& msg,
                   const std::string& sug = "") 
        : result(r), key(k), message(msg), suggestion(sug) {}
    
    bool isError() const { return result == ValidationResult::Error; }
    bool isWarning() const { return result == ValidationResult::Warning; }
    bool isSuccess() const { return result == ValidationResult::Success; }
};

// Type constraints for configuration values
struct TypeConstraint {
    std::string typeName;
    std::function<bool(const ConfigValue&)> validator;
    std::string description;
    std::optional<std::function<std::string(const ConfigValue&)>> formatter;
    std::optional<std::pair<double, double>> numericRange;
    std::optional<size_t> minArraySize;
    std::optional<size_t> maxArraySize;
    
    TypeConstraint(const std::string& type, const std::function<bool(const ConfigValue&)>& val,
                  const std::string& desc = "", const std::function<std::string(const ConfigValue&)>& fmt = nullptr,
                  const std::pair<double, double>& range = std::nullopt,
                  const size_t minSize = std::nullopt, const size_t maxSize = std::nullopt)
        : typeName(type), validator(val), description(desc), formatter(fmt),
          numericRange(range), minArraySize(minSize), maxArraySize(maxSize) {}
};

// Configuration schema definition
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a new configuration parameter
    void addParameter(const std::string& key,
                     const std::string& type,
                     const std::string& description,
                     bool required = false,
                     const std::string& defaultValue = "",
                     const std::function<bool(const ConfigValue&)>& validator = nullptr,
                     const std::function<std::string(const ConfigValue&)>& formatter = nullptr,
                     const std::pair<double, double>& range = std::nullopt,
                     const size_t minSize = std::nullopt,
                     const size_t maxSize = std::nullopt);
    
    // Check if a key is defined in the schema
    bool hasParameter(const std::string& key) const;
    
    // Get type constraint for a key
    TypeConstraint getTypeConstraint(const std::string& key) const;
    
    // Get all parameter keys
    std::vector<std::string> getParameterKeys() const;
    
    // Get description for a key
    std::string getDescription(const std::string& key) const;
    
    // Check if parameter is required
    bool isRequired(const std::string& key) const;
    
    // Get default value for a key
    std::string getDefaultValue(const std::string& key) const;
    
    // Validate a configuration value against its schema
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
    
    // Validate all configuration values
    std::vector<ValidationError> validateAll(const Config& config) const;
    
    // Merge with another schema
    void merge(const ConfigSchema& other);
    
    // Get schema as JSON
    nlohmann::json toJson() const;
    
    // Load schema from JSON
    bool loadFromJson(const std::string& jsonStr);
    
    // Load schema from file
    bool loadFromFile(const std::string& filepath);
    
    // Save schema to file
    bool saveToFile(const std::string& filepath) const;
    
    // Clear all parameters
    void clear();
    
    // Get schema summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Extended configuration entry with validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool isValid = true;
    std::string validationError;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), isValid(true) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), isValid(true) {}
};

// Main configuration class with validation support
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
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonStr);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
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
    
    // Validation methods
    bool validate(bool strict = false) const;
    std::vector<ValidationError> getValidationErrors() const;
    std::vector<ValidationError> getValidationWarnings() const;
    
    // Schema management
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema& getSchema() const;
    void clearSchema();
    
    // Set validation mode
    void setStrictValidation(bool strict);
    bool isStrictValidation() const;
    
    // Batch operations
    void setMultiple(const std::map<std::string, ConfigValue>& values, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Merge configurations
    void merge(const Config& other, bool overwrite = true);
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries = 0;
        size_t validEntries = 0;
        size_t invalidEntries = 0;
        size_t warningEntries = 0;
        size_t keysFromDefault = 0;
        size_t keysFromFile = 0;
        size_t keysFromCommandLine = 0;
    };
    
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isWhitespace(char c);
    
    // Validation helpers
    ValidationResult validateValueAgainstSchema(const std::string& key, const ConfigValue& value) const;
    static bool isNumericValid(const ConfigValue& value, const std::optional<std::pair<double, double>>& range);
    static bool isStringValid(const ConfigValue& value);
    static bool isArrayValid(const ConfigValue& value, const size_t minSize, const size_t maxSize);
    
    // JSON/YAML parsing helpers
    bool loadFromIniFile(const std::string& filepath);
    bool loadFromJsonFile(const std::string& filepath);
    std::string configValueToJsonString(const ConfigValue& value) const;
    std::optional<ConfigValue> jsonStringToConfigValue(const std::string& key, const nlohmann::json& j) const;
    
    friend class ConfigSchema;
};

// Convenience exception types
class ConfigValidationException : public std::runtime_error {
public:
    ConfigValidationException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigFileException : public std::runtime_error {
public:
    ConfigFileException(const std::string& message) : std::runtime_error(message) {}
};

} // namespace nlm

// Include template implementations
#include "Config.cpp"

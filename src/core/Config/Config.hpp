#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <cstdint>

namespace nlm {

// Configuration Error Hierarchy
class ConfigError {
public:
    enum class ErrorType {
        FileNotFound,
        ParseError,
        InvalidType,
        OutOfRange,
        MissingRequiredKey,
        InvalidDependency,
        ValidationError
    };

    ConfigError(ErrorType type, const std::string& message, 
                const std::string& key = "", int line = -1, int column = -1)
        : type_(type), message_(message), key_(key), line_(line), column_(column) {}

    ErrorType type() const { return type_; }
    const std::string& message() const { return message_; }
    const std::string& key() const { return key_; }
    int line() const { return line_; }
    int column() const { return column_; }

    virtual ~ConfigError() = default;
    
protected:
    ErrorType type_;
    std::string message_;
    std::string key_;
    int line_;
    int column_;
};

class ConfigFileNotFoundError : public ConfigError {
public:
    ConfigFileNotFoundError(const std::string& filepath, int line = -1, int column = -1)
        : ConfigError(ErrorType::FileNotFound, "Configuration file not found: " + filepath, "", line, column) {}
};

class ConfigParseError : public ConfigError {
public:
    ConfigParseError(const std::string& message, const std::string& key = "", int line = -1, int column = -1)
        : ConfigError(ErrorType::ParseError, "Parse error: " + message, key, line, column) {}
};

class ConfigInvalidTypeError : public ConfigError {
public:
    ConfigInvalidTypeError(const std::string& key, const std::string& expected, const std::string& actual, int line = -1, int column = -1)
        : ConfigError(ErrorType::InvalidType, 
                     "Invalid type for key \"" + key + "\": expected " + expected + ", got " + actual, 
                     key, line, column) {}
};

class ConfigOutOfRangeError : public ConfigError {
public:
    ConfigOutOfRangeError(const std::string& key, double value, double min, double max, int line = -1, int column = -1)
        : ConfigError(ErrorType::OutOfRange,
                     "Value for key \"" + key + "\" out of range: " + std::to_string(value) + 
                     " (expected between " + std::to_string(min) + " and " + std::to_string(max) + ")",
                     key, line, column) {}
};

class ConfigMissingRequiredKeyError : public ConfigError {
public:
    ConfigMissingRequiredKeyError(const std::string& key, int line = -1, int column = -1)
        : ConfigError(ErrorType::MissingRequiredKey, "Missing required key: " + key, key, line, column) {}
};

class ConfigInvalidDependencyError : public ConfigError {
public:
    ConfigInvalidDependencyError(const std::string& key, const std::string& dependency, int line = -1, int column = -1)
        : ConfigError(ErrorType::InvalidDependency,
                     "Invalid dependency for key \"" + key + "\": requires \"" + dependency + "\"",
                     key, line, column) {}
};

class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& key, const std::string& message, int line = -1, int column = -1)
        : ConfigError(ErrorType::ValidationError, "Validation error for key \"" + key + "\": " + message, key, line, column) {}
};

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

// Configuration schema for validation
struct ConfigSchema {
    std::string key;
    std::vector<ConfigValue> allowedTypes;
    std::string defaultValue;
    bool required = false;
    std::string description;
    std::vector<std::string> dependencies;
    std::pair<double, double> range = {-1e10, 1e10};
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
    
    // New error-based API
    std::vector<ConfigError> loadFromFile(const std::string& filepath);
    ConfigError::Type setValidated(const std::string& key, const std::string& value);
    
    // Validation APIs
    std::vector<ConfigError> validate() const;
    bool hasRequiredKeys() const;
    
    // Error reporting
    const std::vector<ConfigError>& getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }
    
    // Original API preserved for backward compatibility
    std::vector<ConfigError> loadFromFileLegacy(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
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
    
    // Performance optimizations
    void optimizeMemory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<ConfigError> errors_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    static std::string getTypeName(const ConfigValue& value);
    static bool isInRange(double value, double min, double max);
    static bool isValidType(const ConfigValue& value, const std::vector<ConfigValue>& allowedTypes);
    
    // JSON/YAML parser helpers
    static std::vector<ConfigError> parseJsonFile(const std::string& filepath);
    static std::vector<ConfigError> parseYamlFile(const std::string& filepath);
    
    // Error accumulation
    void addError(const ConfigError& error);
};

} // namespace nlm

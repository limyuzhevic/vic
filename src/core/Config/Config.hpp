#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
#include <functional>

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

// Configuration validation exception
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key, const ConfigValue& value)
        : std::runtime_error(message), key(key), value(value) {}
    
    const std::string& getKey() const { return key; }
    const ConfigValue& getValue() const { return value; }
    
private:
    std::string key;
    ConfigValue value;
};

// Validation rule types
using ValidationRule = std::function<bool(const ConfigValue&)>;
using ValidationErrorFunc = std::function<std::string(const ConfigValue&)>;

// Parameter validation schema
struct ParameterSchema {
    std::string type;  // "int", "int64_t", "double", "bool", "string", etc.
    std::string description;
    std::optional<double> minValue;      // For numeric types
    std::optional<double> maxValue;      // For numeric types
    std::optional<size_t> minLength;      // For strings/vectors
    std::optional<size_t> maxLength;      // For strings/vectors
    std::optional<std::vector<std::string>> allowedValues;  // For strings
    ValidationRule customValidator;       // Custom validation function
    ValidationErrorFunc errorFormatter;   // Custom error message formatter
};

class ConfigValidationError; // Forward declaration

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <map>
#include <functional>

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

// Configuration validation exception
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key, const ConfigValue& value)
        : std::runtime_error(message), key(key), value(value) {}
    
    const std::string& getKey() const { return key; }
    const ConfigValue& getValue() const { return value; }
    
private:
    std::string key;
    ConfigValue value;
};

// Validation rule types
using ValidationRule = std::function<bool(const ConfigValue&)>;
using ValidationErrorFunc = std::function<std::string(const ConfigValue&)>;

// Parameter validation schema
struct ParameterSchema {
    std::string type;  // "int", "int64_t", "double", "bool", "string", etc.
    std::string description;
    std::optional<double> minValue;      // For numeric types
    std::optional<double> maxValue;      // For numeric types
    std::optional<size_t> minLength;      // For strings/vectors
    std::optional<size_t> maxLength;      // For strings/vectors
    std::optional<std::vector<std::string>> allowedValues;  // For strings
    ValidationRule customValidator;       // Custom validation function
    ValidationErrorFunc errorFormatter;   // Custom error message formatter
};

class ConfigValidationError; // Forward declaration

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
    
    // Validation
    void validate() const;
    static void registerParameterSchema(const std::string& key, const ParameterSchema& schema);
    static bool isValidValue(const std::string& key, const ConfigValue& value);
    
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
    
    // Validation helpers
    static bool validateValue(const std::string& key, const ConfigValue& value);
    static std::string formatValidationError(const std::string& key, const ConfigValue& value);
};

} // namespace nlm

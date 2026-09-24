#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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
    
    // Configuration validation result
struct ConfigValidationResult {
    bool valid;
    std::string error;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : valid(true) {}
    ConfigValidationResult(bool v, const std::string& err) : valid(v), error(err) {}
};

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::string ruleType;  // "range", "enum", "positive", "nonnegative", "pattern", etc.
    std::string ruleParams;
    std::string errorMessage;
    
    ValidationRule() {}
    ValidationRule(const std::string& k, const std::string& type, 
                  const std::string& params, const std::string& error)
        : key(k), ruleType(type), ruleParams(params), errorMessage(error) {}
};

// Configuration validation
class ConfigValidator {
public:
    static ConfigValidationResult validate(const Config& config);
    static ConfigValidationResult validateKey(const Config& config, const std::string& key);
    static void addValidationRule(const std::string& key, const std::string& type, 
                                 const std::string& params, const std::string& error);
    static void clearValidationRules();
    
    static const std::vector<ValidationRule>& getValidationRules() { return validationRules_; }
    
private:
    static std::vector<ValidationRule> validationRules_;
    
    static bool validateRange(const ConfigValue& value, const std::string& params);
    static bool validateEnum(const ConfigValue& value, const std::string& params);
    static bool validatePattern(const ConfigValue& value, const std::string& params);
    static bool validatePositive(const ConfigValue& value);
    static bool validateNonNegative(const ConfigValue& value);
    static bool validateStringLength(const ConfigValue& value, const std::string& params);
};

// Better error handling
namespace ConfigError {
    enum class ErrorType {
        FileNotFound,
        ParseError,
        InvalidValue,
        MissingRequired,
        ValidationError,
        FileWriteError,
        MemoryError
    };
    
    struct Error {
        ErrorType type;
        std::string message;
        std::string key;
        std::string context;
        
        Error(ErrorType t, const std::string& msg) : type(t), message(msg) {}
        Error(ErrorType t, const std::string& msg, const std::string& key) 
            : type(t), message(msg), key(key) {}
    };
    
    std::string toString(ErrorType type);
    std::string formatError(const Error& error);
}

} // namespace nlm

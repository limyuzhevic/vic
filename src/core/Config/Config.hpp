#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>

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

// Configuration error types
enum class ConfigErrorType {
    InvalidKey,
    InvalidType,
    MissingRequired,
    OutOfRange,
    ParseError
};

class ConfigError : public std::runtime_error {
public:
    ConfigError(const std::string& message, ConfigErrorType type = ConfigErrorType::InvalidKey)
        : std::runtime_error(message), errorType(type) {}
    
    ConfigErrorType getErrorType() const { return errorType; }
    
private:
    ConfigErrorType errorType;
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration validation rule
struct ConfigValidationRule {
    enum class Type {
        IntRange,
        DoubleRange,
        StringList,
        PositiveInteger,
        PositiveDouble,
        BooleanFlag,
        Custom
    };
    
    Type type;
    std::string key;
    std::string description;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ConfigValidationRule(Type t, const std::string& k, const std::string& desc, 
                         const std::function<bool(const ConfigValue&)>& val,
                         const std::string& err)
        : type(t), key(k), description(desc), validator(val), errorMessage(err) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool isRequired;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), isRequired(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool required = false)
        : key(k), value(v), source(s), description(desc), isRequired(required) {}
};

// Configuration class with validation and type safety
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
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type safety
    bool set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Type-specific setters
    template<typename T>
    bool set(const std::string& key, const T& value, 
             ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and type
    bool has(const std::string& key) const;
    
    template<typename T>
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
    bool validate() const;
    std::string validationErrors() const;
    
    // Define validation rule for a key
    void addValidationRule(ConfigValidationRule::Type type, const std::string& key,
                          const std::string& description,
                          const std::function<bool(const ConfigValue&)>& validator,
                          const std::string& errorMessage);
    
    // Enable/disable validation
    void enableValidation(bool enable = true);
    bool isValidationEnabled() const;
    
    // Check if value matches expected type
    static bool isTypeMatch(const ConfigValue& value, const std::type_info& typeInfo);
    
    // Validate value against rule
    static bool validateValue(const ConfigValidationRule& rule, const ConfigValue& value);
    
    // Helper functions for common validation rules
    static std::function<bool(const ConfigValue&)> createIntRangeValidator(int min, int max);
    static std::function<bool(const ConfigValue&)> createDoubleRangeValidator(double min, double max);
    static std::function<bool(const ConfigValue&)> createPositiveValidator();
    static std::function<bool(const ConfigValue&)> createStringValidator();
    
    // Load default configuration
    void loadDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Validation rules
    std::vector<ConfigValidationRule> validationRules;
    bool validationEnabled;
    
    // Default values with descriptions
    static std::unordered_map<std::string, std::pair<ConfigValue, std::string>> getDefaults();
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseValue(const std::string& value, ConfigValue& out);
    
    // Convert string to appropriate type
    static std::string configValueToString(const ConfigValue& value);
    
    // Validate all entries
    void validateEntries() const;
    
    // Collect validation errors
    void collectValidationErrors(std::vector<std::string>& errors) const;
}; 

// Template implementations
namespace {
    template<typename T, typename U>
    constexpr bool is_same_v = std::is_same<T, U>::value;
}

} // namespace nlm
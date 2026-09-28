#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <stdexcept>

#include <sstream>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

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

// Validation error types
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, const std::string& key)
        : std::runtime_error(message), key_(key) {}
    
    ValidationError(const std::string& message, const std::string& key, const std::string& suggestion)
        : std::runtime_error(message), key_(key), suggestion_(suggestion) {}
    
    const std::string& getKey() const { return key_; }
    const std::string& getSuggestion() const { return suggestion_; }
    
private:
    std::string key_;
    std::string suggestion_;
};

class ConfigParseError : public std::runtime_error {
public:
    ConfigParseError(const std::string& message, const std::string& filepath, size_t line = 0)
        : std::runtime_error(message), filepath_(filepath), line_(line) {}
    
    const std::string& getFilepath() const { return filepath_; }
    size_t getLine() const { return line_; }
    
private:
    std::string filepath_;
    size_t line_;
};

class FileAccessError : public std::runtime_error {
public:
    FileAccessError(const std::string& message, const std::string& filepath)
        : std::runtime_error(message), filepath_(filepath) {}
    
    const std::string& getFilepath() const { return filepath_; }
    
private:
    std::string filepath_;
};

// Validation function type
using ValidationFunc = std::function<bool(const ConfigValue&)>;

// Validation rules
struct ValidationRule {
    ValidationFunc validator;
    std::string errorMessage;
    std::string suggestionMessage;
    bool required = false;
};

// Type validation information
struct TypeValidation {
    const std::type_info* type;
    bool (*converter)(const std::string&, ConfigValue&);
};

// Type range validation
struct RangeValidation {
    double min;
    double max;
    bool inclusive = true;
};

// Validation rules
struct ValidationRule {
    ValidationFunc validator;
    std::string errorMessage;
    bool required = false;
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
    
    // Schema management
    void setSchema(const std::shared_ptr<ConfigSchema>& schema);
    std::shared_ptr<ConfigSchema> getSchema() const;
    bool validate() const;
    std::vector<ValidationError> getValidationErrors() const;
    
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
    
    // Type conversion and validation helpers
    static bool convertStringToInt(const std::string& str, ConfigValue& out);
    static bool convertStringToDouble(const std::string& str, ConfigValue& out);
    static bool convertStringToBool(const std::string& str, ConfigValue& out);
    static bool convertStringToString(const std::string& str, ConfigValue& out);
    static bool convertStringToVector(const std::string& str, ConfigValue& out);
    
    static std::string getTypeName(const ConfigValue& value);
    static std::string getValueDescription(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<ConfigSchema> schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateType(const ConfigValue& value, const std::type_info& type);
    static std::string valueToString(const ConfigValue& value);
    static bool parseNumber(const std::string& str, double& out);
    static bool parseBool(const std::string& str, bool& out);
};

// Configuration schema class
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add validation rule for a key
    void addRule(const std::string& key, ValidationFunc validator, 
                 const std::string& errorMessage, bool required = false);
    
    // Add type validation
    void addTypeValidation(const std::string& key, const std::type_info& type);
    
    // Add range validation
    void addRangeValidation(const std::string& key, double min, double max, bool inclusive = true);
    
    // Add regex pattern validation
    void addPatternValidation(const std::string& key, const std::string& pattern,
                             const std::string& errorMessage, const std::string& suggestion = "Value should match the required pattern");
    bool hasKey(const std::string& key) const;
    
    // Get validation rules for a key
    const std::vector<ValidationRule>& getRules(const std::string& key) const;
    
    // Get all known keys
    std::vector<std::string> getKeys() const;
    
    // Clear all rules
    void clear();
    
private:
    std::unordered_map<std::string, std::vector<ValidationRule>> rules_;
    std::unordered_map<std::string, const std::type_info*> typeInfo_;
};

} // namespace nlm

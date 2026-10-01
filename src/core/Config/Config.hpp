#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>

namespace nlm {

// Forward declarations
class Config;

// ========== ERROR HANDLING ==========

class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class NeuralNetworkError : public NLMError {
public:
    explicit NeuralNetworkError(const std::string& message) : NLMError("NeuralNetworkError: " + message) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& message) : NLMError("ConfigurationError: " + message) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& message) : NLMError("MemoryError: " + message) {}
};

class PlasticityError : public NLMError {
public:
    explicit PlasticityError(const std::string& message) : NLMError("PlasticityError: " + message) {}
};

// ========== VALIDATION ==========

// Validation function type
using ValidationFunction = std::function<bool(const ConfigValue& value, std::string& errorMessage)>;

// Range validation
class RangeValidator {
public:
    enum class Type { MIN, MAX, MIN_MAX, EXCLUSIVE_MIN, EXCLUSIVE_MAX };
    
    RangeValidator(Type type, double min, double max = 0.0)
        : type_(type), min_(min), max_(max) {}
    
    bool validate(const ConfigValue& value, std::string& errorMessage) const;
    
private:
    Type type_;
    double min_;
    double max_;
};

// Type validation
class TypeValidator {
public:
    enum class ExpectedType { INT, DOUBLE, STRING, BOOL, VECTOR_INT, VECTOR_DOUBLE, VECTOR_STRING };
    
    TypeValidator(ExpectedType expectedType) : expectedType_(expectedType) {}
    
    bool validate(const ConfigValue& value, std::string& errorMessage) const;
    
private:
    ExpectedType expectedType_;
};

// Validation rule
struct ValidationRule {
    std::string key;
    std::vector<ValidationFunction> validators;
    std::string description;
    
    ValidationRule(const std::string& k, const std::vector<ValidationFunction>& v, const std::string& desc)
        : key(k), validators(v), description(desc) {}
};

// ========== CONFIGURATION VALUE TYPES ==========

// Enhanced configuration value types
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
    Derived  // Calculated from other values
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    bool derived;
    std::vector<ValidationFunction> validators;
    std::unordered_map<std::string, std::string> metadata;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), 
          required(false), derived(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), required(false), derived(false) {}
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
    
    // Load from string (JSON/YAML)
    bool loadFromString(const std::string& content);
    
    // Save to file (JSON format)
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
    
    // Set derived value (calculated from other values)
    void setDerived(const std::string& key, const ConfigValue& value);
    
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
    
    // Schema validation
    void addValidationRule(const ValidationRule& rule);
    void addRangeValidator(const std::string& key, double min, double max, const std::string& desc = "");
    void addTypeValidator(const std::string& key, const TypeValidator::ExpectedType& type, const std::string& desc = "");
    bool validate(const std::string& key, std::string& errorMessage) const;
    bool validateAll() const;
    
    // Runtime configuration
    void enableRuntimeQuery(const std::string& key, const std::string& queryExpr);
    ConfigValue evaluateQuery(const std::string& queryExpr) const;
    
    // Schema access
    std::vector<std::string> getRequiredKeys() const;
    std::vector<std::string> getDerivedKeys() const;
    
    // Error handling
    std::vector<std::string> getValidationErrors() const;
    
    // Advanced features
    void enableDynamicParameter(const std::string& key, double minValue, double maxValue, double step = 0.1);
    ConfigValue getDynamicValue(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Error handling helpers
    void addValidationError(const std::string& key, const std::string& error);
};

} // namespace nlm
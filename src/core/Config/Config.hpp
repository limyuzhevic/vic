#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <any>
#include <stdexcept>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <regex>
#include <numeric>
#include <set>
#include <future>
#include <thread>

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
    Inherited
};

// Validation error type
class ValidationError {
public:
    enum class ErrorCode {
        InvalidType,
        OutOfRange,
        MissingRequired,
        CustomValidation
    };

    ValidationError(ErrorCode code, const std::string& message, const std::string& key = "")
        : code(code), message(message), key(key) {}

    ErrorCode code() const { return code; }
    const std::string& message() const { return message; }
    const std::string& key() const { return key; }

private:
    ErrorCode code;
    std::string message;
    std::string key;
};

// Type information for configuration parameters
enum class ConfigType {
    Integer,
    Double,
    Boolean,
    String,
    IntArray,
    DoubleArray,
    StringArray
};

// Validation callback type
using ValidationCallback = std::function<bool(const ConfigValue&, const std::string& key)>;

// Range specification
struct Range {
    double min;
    double max;
    bool inclusiveMin = true;
    bool inclusiveMax = true;
    
    Range(double min, double max, bool incMin = true, bool incMax = true)
        : min(min), max(max), inclusiveMin(incMin), inclusiveMax(incMax) {}
    
    bool contains(double value) const {
        bool inRange = inclusiveMin ? (value >= min) : (value > min);
        inRange = inRange && (inclusiveMax ? (value <= max) : (value < max));
        return inRange;
    }
};

// Configuration parameter metadata
struct ConfigParameter {
    std::string name;
    ConfigType type;
    std::string description;
    std::any defaultValue;
    bool required = false;
    std::optional<Range> range;
    std::vector<ValidationCallback> validators;
    
    ConfigParameter(const std::string& name, ConfigType type, const std::string& desc = "")
        : name(name), type(type), description(desc), required(false) {}
};

// Configuration group
struct ConfigGroup {
    std::string name;
    std::string description;
    std::vector<std::string> parameterNames;
    
    ConfigGroup(const std::string& name, const std::string& desc = "")
        : name(name), description(desc) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<ValidationError> validationErrors;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

// Configuration schema class
class ConfigSchema {
public:
    void addParameter(const ConfigParameter& param);
    void addGroup(const ConfigGroup& group);
    void addValidationRule(const std::string& key, const ValidationCallback& validator);
    
    bool validate(const ConfigValue& value, const std::string& key) const;
    const std::vector<ValidationError> validateAll(const std::unordered_map<std::string, ConfigValue>& values) const;
    
    ConfigParameter* getParameter(const std::string& key);
    const ConfigParameter* getParameter(const std::string& key) const;
    
    const ConfigGroup* getGroup(const std::string& groupName) const;
    
    std::vector<std::string> getAllParameterKeys() const;
    std::vector<std::string> getGroups() const;
    
    void clear();
    
private:
    std::unordered_map<std::string, ConfigParameter> parameters_;
    std::unordered_map<std::string, ConfigGroup> groups_;
    std::unordered_map<std::string, std::vector<ValidationCallback>> customValidators_;
};

// Configuration validator implementation
class ConfigValidator {
public:
    static bool validateType(const ConfigValue& value, ConfigType type);
    static bool validateRange(const ConfigValue& value, const Range& range);
    static bool validateRequired(const bool isSet);
    static ValidationError createTypeError(const std::string& key, ConfigType expected, ConfigType actual);
    static ValidationError createRangeError(const std::string& key, const Range& range, double value);
    static ValidationError createMissingError(const std::string& key);
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
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema& getSchema() const { return *schema_; }
    
    // Load from file
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
    
    // Get all keys in a group
    std::vector<std::string> getKeysInGroup(const std::string& groupName) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Runtime configuration building
    void addDefaults();
    void merge(const Config& other);
    
    // Configuration validation
    bool validate() const;
    std::vector<ValidationError> getValidationErrors() const;
    
    // Apply configuration inheritance
    void applyInheritance(const Config& baseConfig);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<ConfigSchema> schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm
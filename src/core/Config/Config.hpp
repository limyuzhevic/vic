#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <functional>
#include <stdexcept>

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

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    enum class ErrorCode {
        InvalidType,
        OutOfRange,
        MissingRequired,
        InvalidValue,
        ConstraintViolation
    };

    ConfigValidationError(ErrorCode code, const std::string& key, const std::string& message)
        : std::runtime_error("Config validation error: " + message + " (key: " + key + ")"),
          code(code), key(key), message(message) {}

    ErrorCode getCode() const { return code; }
    const std::string& getKey() const { return key; }
    const std::string& getMessage() const { return message; }

private:
    ErrorCode code;
    std::string key;
    std::string message;
};

// Configuration constraint
struct ConfigConstraint {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
    
    ConfigConstraint(const std::string& k, std::function<bool(const ConfigValue&)> v, const std::string& msg)
        : key(k), validator(v), errorMessage(msg) {}
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::map<std::string, ConfigValue> defaultValues;
    std::vector<ConfigConstraint> constraints;
    std::string description;
    
    ConfigTemplate(const std::string& n, const std::map<std::string, ConfigValue>& defaults,
                   const std::vector<ConfigConstraint>& cons, const std::string& desc = "")
        : name(n), defaultValues(defaults), constraints(cons), description(desc) {}
};

// Configuration comparison result
struct ConfigDiff {
    std::string key;
    ConfigValue oldValue;
    ConfigValue newValue;
    std::string description;
    
    ConfigDiff(const std::string& k, const ConfigValue& oldv, const ConfigValue& newv, const std::string& desc = "")
        : key(k), oldValue(oldv), newValue(newv), description(desc) {}
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
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation and presets
    bool validate() const;
    std::vector<ConfigValidationError> getValidationErrors() const;
    
    // Apply configuration template
    bool applyTemplate(const ConfigTemplate& template);
    
    // Configuration comparison
    std::vector<ConfigDiff> compareWith(const Config& other) const;
    
    // Presets
    static ConfigTemplate createDefaultTemplate();
    static ConfigTemplate createExplorerTemplate();
    static ConfigTemplate createStorageTemplate();
    static ConfigTemplate createSocialTemplate();
    static ConfigTemplate createMinimalTemplate();
    static ConfigTemplate createDebugTemplate();
    
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
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getSourceCount(ConfigSource source) const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Configuration import/export
    bool importFrom(const Config& other);
    bool exportTo(Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    void validateValue(const std::string& key, const ConfigValue& value) const;
    bool validateConstraint(const ConfigConstraint& constraint) const;
    
    // Template application
    void applyTemplateDefaults(const ConfigTemplate& template);
};

} // namespace nlm

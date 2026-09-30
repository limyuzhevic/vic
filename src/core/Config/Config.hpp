#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include "Exceptions.hpp"
#include "CustomExceptions.hpp"

namespace nlm {

// Forward declarations
class Config;

// Validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    std::string errorCode;
    
    ValidationResult(bool valid = true, const std::string& error = "", const std::string& code = "")
        : isValid(valid), errorMessage(error), errorCode(code) {}
    
    static ValidationResult success() { return ValidationResult(true); }
    static ValidationResult error(const std::string& message, const std::string& code = "") {
        return ValidationResult(false, message, code);
    }
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
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation and error handling
    ValidationResult validate() const;
    std::string getValidationError() const;
    bool isValid() const;
    
    // Schema validation
    ValidationResult validateSchema(const std::string& schemaFile) const;
    ValidationResult validateRange(const std::string& key, const std::string& type, double min, double max) const;
    
    // Configuration metadata
    void setDescription(const std::string& key, const std::string& description);
    std::string getDescription(const std::string& key) const;
    std::vector<std::string> getKeysWithSource(ConfigSource source) const;
    
    // Error reporting and debugging
    std::string getDetailedError() const;
    void enableErrorLogging(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKey(const std::string& key);
    static ValidationResult validateValue(const std::string& key, const ConfigValue& value);
    static std::string valueToString(const ConfigValue& value) const;
    
    // Error handling helpers
    void logError(const std::string& error) const;
    void throwException(const std::string& error) const;
};

} // namespace nlm


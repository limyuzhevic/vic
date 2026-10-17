#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "config_validator.hpp"

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    
    std::string getErrorString() const {
        std::stringstream ss;
        for (const auto& error : errors) {
            ss << "  ERROR: " << error << "\n";
        }
        return ss.str();
    }
    
    std::string getWarningString() const {
        std::stringstream ss;
        for (const auto& warning : warnings) {
            ss << "  WARNING: " << warning << "\n";
        }
        return ss.str();
    }
};

// Enhanced configuration class with validation support
class Config {
public:
    // Create configuration with optional schema name
    explicit Config(const std::string& schemaName = "Phase2");
    
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML/simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonContent);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and get source
    bool has(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation methods
    ValidationResult validate(const std::string& schemaName = "Phase2") const;
    ValidationResult validateWithDetailedErrors(const std::string& schemaName = "Phase2") const;
    
    // Schema management
    static std::vector<std::string> getAvailableSchemas();
    static std::string generateConfigTemplate(const std::string& schemaName = "Phase2");
    static bool validateConfig(const std::map<std::string, std::string>& config, const std::string& schemaName = "Phase2");
    
    // Get the current schema name
    std::string getSchemaName() const { return schemaName_; }
    
    // Enable/disable quiet validation
    static void setQuietValidation(bool quiet) { ConfigValidator::setQuietValidation(quiet); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string schemaName_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumeric(const std::string& str);
    static std::string convertToConfigFormat(const std::string& jsonValue);
    static std::string convertFromConfigFormat(const std::string& configValue);
    
    // File parsing helpers
    static std::map<std::string, std::string> parseSimpleConfig(const std::string& content);
    static std::map<std::string, std::string> parseJSONConfig(const std::string& content);
    
    // Schema validation helpers
    static std::vector<std::string> getSchemaValidationErrors(const std::map<std::string, std::string>& config, const std::string& schemaName) const;
    
    // Configuration value helpers
    template<typename T>
    static std::string toConfigString(const T& value);
    
    template<typename T>
    static std::optional<T> fromConfigString(const std::string& str);
    
    // Type checking helpers
    static bool isIntType(const ConfigValue& value);
    static bool isDoubleType(const ConfigValue& value);
    static bool isStringType(const ConfigValue& value);
    static bool isBoolType(const ConfigValue& value);
    static bool isVectorType(const ConfigValue& value);
    static std::string getTypeName(const ConfigValue& value);
};

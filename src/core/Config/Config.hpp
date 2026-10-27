#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
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

// Validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    ValidationResult(bool v = true, const std::string& msg = "") 
        : valid(v), errorMessage(msg) {}
    
    static ValidationResult Success() { return ValidationResult(true); }
    static ValidationResult Error(const std::string& msg) { return ValidationResult(false, msg); }
};

// Validation function type
using ValidationFunction = std::function<ValidationResult(const ConfigValue&)>;

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string key;
    std::string type;  // "int", "int64_t", "double", "bool", "string", "vector<int>", "vector<double>", "vector<string>"
    std::string description;
    ConfigSource source;
    ConfigValue defaultValue;
    ValidationFunction validator;
    bool required;
    
    ConfigSchemaEntry() : key(), type(), description(), source(ConfigSource::Default), 
                         defaultValue(), validator(nullptr), required(false) {}
    
    ConfigSchemaEntry(const std::string& k, const std::string& t, const std::string& desc = "",
                     ConfigSource s = ConfigSource::Default, const ConfigValue& def = ConfigValue(),
                     const ValidationFunction& val = nullptr, bool req = false)
        : key(k), type(t), description(desc), source(s), defaultValue(def), 
          validator(val), required(req) {}
};

// Configuration schema
class ConfigSchema {
public:
    ConfigSchema() = default;
    
    // Add a schema entry
    void addEntry(const std::string& key, const std::string& type, 
                 const std::string& description = "",
                 ConfigSource source = ConfigSource::Default,
                 const ConfigValue& defaultValue = ConfigValue(),
                 const ValidationFunction& validator = nullptr,
                 bool required = false);
    
    // Get schema entry by key
    const ConfigSchemaEntry* getEntry(const std::string& key) const;
    
    // Get all schema entries
    const std::vector<ConfigSchemaEntry>& getEntries() const { return entries; }
    
    // Validate configuration against schema
    ValidationResult validate(const Config& config) const;
    
    // Get all keys from schema
    std::vector<std::string> getKeys() const;
    
    // Create default configuration from schema
    std::shared_ptr<Config> createDefaultConfig() const;
    
    // Clear all entries
    void clear() { entries.clear(); }
    
private:
    std::vector<ConfigSchemaEntry> entries;
    
    // Type conversion helpers
    bool isTypeCompatible(const std::string& type, const ConfigValue& value) const;
    std::string getTypeName(const ConfigValue& value) const;
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
    
    // Schema validation
    ValidationResult validateWithSchema(const ConfigSchema& schema) const;
    
    // Configuration validation helpers
    static ValidationResult validateRange(const ConfigValue& value, double min, double max, const std::string& paramName);
    static ValidationResult validatePositive(const ConfigValue& value, const std::string& paramName);
    static ValidationResult validateNonNegative(const ConfigValue& value, const std::string& paramName);
    static ValidationResult validateProbability(const ConfigValue& value, const std::string& paramName);
    static ValidationResult validateNeuronCount(const ConfigValue& value, const std::string& paramName);
    static ValidationResult validateTimestep(const ConfigValue& value, const std::string& paramName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Create default schema for backward compatibility
    static ConfigSchema createDefaultSchema();
    
    friend class ConfigSchema;
};

// Common configuration schemas for different NLM phases
namespace ConfigSchemas {
    ConfigSchema createPhase1Schema();
    ConfigSchema createPhase2Schema();
    ConfigSchema createDefaultSchema();
    ConfigSchema createValidationSchema();
}

} // namespace nlm

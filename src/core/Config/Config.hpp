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

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    
    ValidationResult() : isValid(true), errorMessage("") {}
    ValidationResult(bool valid, const std::string& error) : isValid(valid), errorMessage(error) {}
    
    static ValidationResult Valid() { return ValidationResult(true, ""); }
    static ValidationResult Invalid(const std::string& error) { return ValidationResult(false, error); }
};

// Configuration parameter definition
struct ParameterDefinition {
    std::string name;
    std::string type;  // "int", "int64_t", "double", "bool", "string", "vec_int", "vec_double", "vec_string"
    std::string defaultValue;
    std::string description;
    std::string minValue;
    std::string maxValue;
    bool required;
    
    ParameterDefinition()
        : name(), type(), defaultValue(), description(), minValue(), maxValue(), required(false) {}
    
    ParameterDefinition(const std::string& n, const std::string& t, const std::string& def,
                       const std::string& desc = "", const std::string& min = "", const std::string& max = "",
                       bool req = false)
        : name(n), type(t), defaultValue(def), description(desc), minValue(min), maxValue(max), required(req) {}
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
    
    // Validation and parameter management
    static void registerParameter(const ParameterDefinition& param);
    static std::vector<ParameterDefinition> getRegisteredParameters();
    static ValidationResult validateKey(const std::string& key, const ConfigValue& value);
    static ValidationResult validateType(const std::string& key, const ConfigValue& value);
    static ValidationResult validateRange(const std::string& key, const ConfigValue& value);
    
    // Default configuration presets
    static void loadDefaultConfig();
    static void loadTestConfig();
    static void loadBenchmarkConfig();
    
    // Configuration presets
    enum class Preset {
        Default,
        Test,
        Benchmark
    };
    
    void applyPreset(Preset preset);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

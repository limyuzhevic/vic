#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <stdexcept>
#include "../Errors/Errors.hpp"

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

// Configuration validation exception
class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& key, const std::string& message)
        : ConfigError("validation", "Parameter \"" + key + "\": " + message) {}
};

// Configuration validation result
struct ConfigValidationResult {
    bool valid;
    std::string errorMessage;
    
    ConfigValidationResult(bool v = true, const std::string& msg = "") 
        : valid(v), errorMessage(msg) {}
};

// Parameter validation function signatures
using ParamValidator = std::function<ConfigValidationResult(const std::string&, const ConfigValue&)>;

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

// Parameter specification for validation
struct ParameterSpec {
    std::string key;
    std::string type;
    std::string description;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::optional<std::vector<std::string>> allowedStrings;
    ParamValidator customValidator;
    
    ParameterSpec(const std::string& k, const std::string& t, const std::string& desc = "")
        : key(k), type(t), description(desc) {}
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
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Parameter validation and checking
    bool validateParameter(const std::string& key, const ConfigValue& value) const;
    static ConfigValidationResult validateValue(const std::string& key, const ConfigValue& value, const ParameterSpec& spec);
    
    // Get parameter specifications for validation
    static std::vector<ParameterSpec> getParameterSpecs();
    
    // Check if parameter is valid
    static bool isValidParameter(const std::string& key);
    
    // Initialize default parameter specifications
    static void initializeParameterSpecs();
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

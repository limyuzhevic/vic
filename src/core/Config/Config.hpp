#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <sstream>

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
    Environment,
    Runtime
};

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    
    static ValidationResult success() { return {true, ""}; }
    static ValidationResult error(const std::string& msg) { return {false, msg}; }
};

// Type conversion utilities
class ConfigConverter {
public:
    // String to specific type conversion
    static bool toBool(const std::string& str, bool& value);
    static bool toInt(const std::string& str, int& value);
    static bool toInt64(const std::string& str, int64_t& value);
    static bool toDouble(const std::string& str, double& value);
    static bool toString(const std::string& str, std::string& value);
    
    // Type checking
    static bool isBool(const std::string& str);
    static bool isInt(const std::string& str);
    static bool isInt64(const std::string& str);
    static bool isDouble(const std::string& str);
    static bool isFloat(const std::string& str);
    
    // Vector conversions
    static bool toIntVector(const std::string& str, std::vector<int>& value);
    static bool toDoubleVector(const std::string& str, std::vector<double>& value);
    static bool toStringVector(const std::string& str, std::vector<std::string>& value);
    
    // Range validation
    static bool inRange(double value, double min, double max, bool inclusive = true);
    
    // Common string values
    static const std::string& TRUE_STR();
    static const std::string& FALSE_STR();
    static const std::string& YES_STR();
    static const std::string& NO_STR();
    static const std::string& ON_STR();
    static const std::string& OFF_STR();
};

// Configuration validation rule
struct ValidationRule {
    std::string key;
    std::function<ValidationResult(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<std::string> tags;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), tags() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), tags() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc,
                const std::vector<std::string>& t)
        : key(k), value(v), source(s), description(desc), tags(t) {}
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

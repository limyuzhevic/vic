#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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

// Configuration groups for better organization
enum class ConfigGroup {
    Basic,           // Basic simulation parameters
    Plasticity,      // Plasticity rules (STDP, Hebbian)
    Development,     // Developmental processes
    Neuromodulation, // Neuromodulators (dopamine, curiosity)
    Memory,         // Memory systems
    Prediction,      // Prediction system
    Cognition,       // Cognitive mechanisms
    Environment,     // Environment parameters
    Performance,     // Performance settings
    Debug           // Debug and logging
};

// Configuration validator
class ConfigValidator {
public:
    static bool validate(const std::string& key, const ConfigValue& value);
    static std::vector<std::string> getValidationErrors(const std::string& key, const ConfigValue& value);
    
    static bool isValueTypeValid(ConfigGroup group, const std::string& key, const ConfigValue& value);
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    ConfigGroup group;
    std::string type;
    std::string description;
    ConfigValue defaultValue;
    bool required;
    std::vector<std::string> validValues;
    
    ConfigSchema(const std::string& k, ConfigGroup g, const std::string& t, const std::string& desc,
                 const ConfigValue& def, bool req = false, const std::vector<std::string>& valid = {})
        : key(k), group(g), type(t), description(desc), defaultValue(def), required(req), validValues(valid) {}
};

// Main configuration class with improved API
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // === Enhanced Configuration API ===
    
    // Load from file (JSON or INI format)
    bool loadFromFile(const std::string& filepath, bool merge = true);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with format specification
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    // === Type-safe get/set methods ===
    
    // String values
    std::optional<std::string> getString(const std::string& key) const;
    std::string getStringOr(const std::string& key, const std::string& defaultValue) const;
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Integer values
    std::optional<int> getInt(const std::string& key) const;
    int getIntOr(const std::string& key, int defaultValue) const;
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    
    // Float values
    std::optional<float> getFloat(const std::string& key) const;
    float getFloatOr(const std::string& key, float defaultValue) const;
    void setFloat(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    
    // Double values
    std::optional<double> getDouble(const std::string& key) const;
    double getDoubleOr(const std::string& key, double defaultValue) const;
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    
    // Boolean values
    std::optional<bool> getBool(const std::string& key) const;
    bool getBoolOr(const std::string& key, bool defaultValue) const;
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Vector values
    std::optional<std::vector<int>> getIntVector(const std::string& key) const;
    std::vector<int> getIntVectorOr(const std::string& key, const std::vector<int>& defaultValue) const;
    
    // === Configuration management ===
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys in a group
    std::vector<std::string> getKeys(ConfigGroup group) const;
    std::vector<std::string> getAllKeys() const;
    
    // Get keys by type
    std::vector<std::string> getKeysByType(const std::string& type) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with grouping
    std::string summary(bool includeDefaults = false) const;
    
    // Get configuration statistics
    struct Stats {
        size_t totalKeys;
        size_t keysBySource[4]; // Default, File, CommandLine, Runtime
        size_t keysByGroup[10]; // By ConfigGroup enum
    };
    
    Stats getStats() const;
    
    // === Configuration validation ===
    
    // Validate a key-value pair
    bool validate(const std::string& key, const ConfigValue& value) const;
    
    // Get validation errors for a key-value pair
    std::vector<std::string> validateWithErrors(const std::string& key, const ConfigValue& value) const;
    
    // Check if key exists and is of expected type
    bool isValidKey(const std::string& key) const;
    
    // Get schema for a key (if schema system is implemented)
    std::optional<ConfigSchema> getSchema(const std::string& key) const;
    
    // === Configuration presets ===
    
    // Apply a preset configuration
    bool applyPreset(const std::string& presetName);
    
    // Get available preset names
    static std::vector<std::string> getAvailablePresets();
    
    // === Configuration export/import ===
    
    // Export to JSON string
    std::string toJson() const;
    
    // Import from JSON string
    bool fromJson(const std::string& json);
    
    // Export to INI format
    std::string toIni() const;
    
    // Import from INI format
    bool fromIni(const std::string& ini);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Initialize with schema
    void initializeSchema();
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool stringEqualsIgnoreCase(const std::string& a, const std::string& b);
    
    // Type conversion helpers
    static std::string valueToString(const ConfigValue& value);
    static std::optional<ConfigValue> stringToValue(const std::string& str);
};

} // namespace nlm

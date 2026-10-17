// Improved Config.hpp with better type safety and validation

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
    Default = 0,
    File = 1,
    CommandLine = 2,
    Runtime = 3
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
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& error) : valid(v), errorMessage(error) {}
    ValidationResult(bool v, const std::string& error, const std::vector<std::string>& warns) 
        : valid(v), errorMessage(error), warnings(warns) {}
};

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::string type; // "int", "float", "bool", "string", "list<int>", etc.
    std::string description;
    ConfigValue defaultValue;
    std::function<ValidationResult(const ConfigValue&)> validator;
    bool required;
    
    ConfigSchema() : required(false) {}
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc, 
                 const ConfigValue& def = {}, bool req = false)
        : key(k), type(t), description(desc), defaultValue(def), required(req) {}
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
    
    // Load from file (JSON/YAML/TOML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJson(const std::string& jsonString);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJson() const;
    
    // Get values with better type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-specific getters for better Python integration
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    std::vector<int> getIntList(const std::string& key) const;
    std::vector<double> getDoubleList(const std::string& key) const;
    std::vector<std::string> getStringList(const std::string& key) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set methods with validation
    bool safeSet(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool safeSet(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool safeSet(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool safeSet(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Configuration validation and schema
    bool validateSchema(const std::vector<ConfigSchema>& schema) const;
    ValidationResult validate() const;
    
    // Schema management
    void addSchema(const ConfigSchema& schema);
    void removeSchema(const std::string& key);
    std::vector<std::string> getSchemaKeys() const;
    std::string getSchemaDescription(const std::string& key) const;
    
    // Configuration presets and templates
    bool applyPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;
    
    // Comparison and merging
    bool equals(const Config& other) const;
    void merge(const Config& other);
    
    // Get configuration statistics
    size_t getEntryCount() const;
    std::map<ConfigSource, size_t> getSourceCounts() const;
    
    // Search and filtering
    std::vector<std::string> findKeysByPattern(const std::string& pattern) const;
    std::vector<ConfigEntry> filterBySource(ConfigSource source) const;
    std::vector<ConfigEntry> filterByType(const std::string& typeName) const;
    
    // Python-specific helper methods
    bool pythonSafeSet(const std::string& key, const pybind11::object& value);
    pybind11::object pythonGet(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::map<std::string, ConfigSchema> schemas;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static int stringToInt(const std::string& str, int defaultValue = 0);
    static double stringToDouble(const std::string& str, double defaultValue = 0.0);
    static ConfigValue parseConfigValue(const std::string& valueStr);
    static std::string configValueToString(const ConfigValue& value);
    
    // Validation helpers
    static ValidationResult validateInt(const ConfigValue& value, int minValue = INT32_MIN, int maxValue = INT32_MAX);
    static ValidationResult validateDouble(const ConfigValue& value, double minValue = -1e100, double maxValue = 1e100);
    static ValidationResult validateBool(const ConfigValue& value);
    static ValidationResult validateString(const ConfigValue& value, size_t minLength = 0, size_t maxLength = 10000);
    static ValidationResult validateIntList(const ConfigValue& value, size_t maxSize = 1000);
    static ValidationResult validateDoubleList(const ConfigValue& value, size_t maxSize = 1000);
    static ValidationResult validateStringList(const ConfigValue& value, size_t maxSize = 1000);
    
    // Schema validation
    ValidationResult validateAgainstSchema(const ConfigEntry& entry) const;
    std::string getTypeName(const ConfigValue& value) const;
}; 

// Type aliases for convenience
using ConfigPtr = std::shared_ptr<Config>;

// Convenience functions
inline std::shared_ptr<Config> createDefaultConfig() {
    return std::make_shared<Config>();
}

} // namespace nlm

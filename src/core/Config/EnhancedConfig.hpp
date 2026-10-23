#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <functional>
#include <any>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Configuration schema types
enum class ConfigType {
    INT,
    INT64,
    FLOAT,
    DOUBLE,
    BOOL,
    STRING,
    STRING_LIST,
    INT_LIST,
    FLOAT_LIST,
    OBJECT,
    ARRAY
};

// Validation result
struct ValidationResult {
    bool isValid = true;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult(bool valid, const std::string& message = "")
        : isValid(valid), errorMessage(message) {}
    
    ValidationResult(const std::string& warning)
        : isValid(true), errorMessage("") {
        warnings.push_back(warning);
    }
};

// Schema definition
struct ConfigSchema {
    std::string key;
    ConfigType type;
    std::any defaultValue;
    bool required = false;
    std::string description;
    
    // Validation constraints
    std::optional<std::pair<double, double>> numericRange;
    std::optional<size_t> minSize;
    std::optional<size_t> maxSize;
    std::optional<std::set<std::string>> allowedValues;
    std::function<ValidationResult(const std::any&)> customValidator;
    
    ConfigSchema(const std::string& k, ConfigType t, const std::any& defaultVal = {}, 
                 bool req = false, const std::string& desc = "")
        : key(k), type(t), defaultValue(defaultVal), required(req), description(desc) {}
    
    // Convenience constructors
    static ConfigSchema Int(const std::string& key, int defaultVal = 0, bool required = false,
                           const std::string& desc = "", const std::pair<int, int>& range = {-1000, 1000}) {
        ConfigSchema schema(key, ConfigType::INT, defaultVal, required, desc);
        schema.numericRange = range;
        return schema;
    }
    
    static ConfigSchema Float(const std::string& key, float defaultVal = 0.0f, bool required = false,
                             const std::string& desc = "", const std::pair<float, float>& range = {0.0f, 1.0f}) {
        ConfigSchema schema(key, ConfigType::FLOAT, defaultVal, required, desc);
        schema.numericRange = range;
        return schema;
    }
    
    static ConfigSchema String(const std::string& key, const std::string& defaultVal = "", 
                              bool required = false, const std::string& desc = "",
                              const std::set<std::string>& allowed = {}) {
        ConfigSchema schema(key, ConfigType::STRING, defaultVal, required, desc);
        if (!allowed.empty()) schema.allowedValues = allowed;
        return schema;
    }
    
    static ConfigSchema Bool(const std::string& key, bool defaultVal = false, 
                           bool required = false, const std::string& desc = "") {
        return ConfigSchema(key, ConfigType::BOOL, defaultVal, required, desc);
    }
    
    static ConfigSchema StringList(const std::string& key, 
                                  const std::vector<std::string>& defaultVal = {},
                                  bool required = false, const std::string& desc = "",
                                  size_t minSize = 0, size_t maxSize = 100) {
        ConfigSchema schema(key, ConfigType::STRING_LIST, defaultVal, required, desc);
        schema.minSize = minSize;
        schema.maxSize = maxSize;
        return schema;
    }
};

// Configuration source and loading
enum class ConfigSource {
    Default,
    Environment,
    File,
    CommandLine,
    Runtime
};

// Enhanced configuration entry with metadata
struct ConfigEntry {
    std::string key;
    std::any value;
    ConfigSource source;
    std::string description;
    std::string envVar; // Environment variable name for this key
    bool required = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), envVar(), required(false) {}
    ConfigEntry(const std::string& k, const std::any& v, ConfigSource s, 
                const std::string& desc = "", const std::string& env = "",
                bool req = false)
        : key(k), value(v), source(s), description(desc), envVar(env), required(req) {}
};

// Main configuration class with schema validation
class EnhancedConfig {
public:
    EnhancedConfig();
    ~EnhancedConfig();
    
    // Disable copying, enable moving
    EnhancedConfig(const EnhancedConfig&) = delete;
    EnhancedConfig& operator=(const EnhancedConfig&) = delete;
    EnhancedConfig(EnhancedConfig&&) noexcept;
    EnhancedConfig& operator=(EnhancedConfig&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Set values with validation
    void set(const std::string& key, const std::any& value, ConfigSource source = ConfigSource::Runtime);
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get values with validation and type checking
    template<typename T>
    T get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Safe get methods that don't throw
    template<typename T>
    std::optional<T> tryGet(const std::string& key) const;
    
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
    
    // Schema management
    void setSchema(const ConfigSchema& schema);
    void addSchema(const ConfigSchema& schema);
    bool hasSchema(const std::string& key) const;
    const ConfigSchema* getSchema(const std::string& key) const;
    
    // Configuration merging
    void merge(const EnhancedConfig& other, bool override = true);
    
    // Environment variable handling
    void setEnvVarPrefix(const std::string& prefix);
    std::string getEnvVarPrefix() const;
    
    // Validation
    ValidationResult validate() const;
    ValidationResult validate(const std::string& key) const;
    
    // Statistics
    size_t getEntryCount() const;
    ConfigSource getSource(const std::string& key) const;
    
    // Special methods for common configuration patterns
    void configureBrain(const std::string& configFile = "");
    void configureWorld(const std::string& configFile = "");
    void configureNeuromodulation(const std::string& configFile = "");
    
    // Python-friendly methods
    std::vector<std::string> getKeys() const { return getKeys(); }
    std::string getSummary() const { return summary(); }
    bool hasKey(const std::string& key) const { return has(key); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toEnvVarName(const std::string& key);
    
    // Value validation
    ValidationResult validateValue(const std::string& key, const std::any& value) const;
    ValidationResult validateType(const std::string& key, const std::any& value) const;
    ValidationResult validateConstraints(const std::string& key, const std::any& value) const;
    
    // Environment variable loading
    bool loadEnvVars(const std::string& prefix);
    bool parseEnvLine(const std::string& line, const std::string& prefix);
    
    // File loading
    bool loadFromSimpleFile(const std::string& filepath);
    bool loadFromJsonFile(const std::string& filepath);
    
    // Schema validation
    ValidationResult validateAgainstSchema() const;
    ValidationResult validateSingleSchema(const ConfigSchema& schema, const std::any& value) const;
    
    // Type conversion utilities
    template<typename T>
    static T convertToType(const std::any& value);
    template<typename T>
    static bool isTypeMatch(const std::any& value);
};

// Explicit template instantiations
template int EnhancedConfig::get<int>(const std::string&) const;
template int64_t EnhancedConfig::get<int64_t>(const std::string&) const;
template double EnhancedConfig::get<double>(const std::string&) const;
template bool EnhancedConfig::get<bool>(const std::string&) const;
template std::string EnhancedConfig::get<std::string>(const std::string&) const;
template std::vector<int> EnhancedConfig::get<std::vector<int>>(const std::string&) const;
template std::vector<double> EnhancedConfig::get<std::vector<double>>(const std::string&) const;
template std::vector<std::string> EnhancedConfig::get<std::vector<std::string>>(const std::string&) const;

// Convenience functions
EnhancedConfig createDefaultConfig();
EnhancedConfig createBrainConfig();
EnhancedConfig createWorldConfig();
EnhancedConfig createNeuromodulationConfig();

} // namespace nlm

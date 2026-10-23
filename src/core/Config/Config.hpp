#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <any>
#include <map>
#include <set>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class EnhancedConfig;

// Configuration value types (original)
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

// Enhanced configuration schema types
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

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry (enhanced)
struct ConfigEntry {
    std::string key;
    std::any value;
    ConfigSource source;
    std::string description;
    std::string envVar;
    bool required = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), envVar(), required(false) {}
    ConfigEntry(const std::string& k, const std::any& v, ConfigSource s, 
                const std::string& desc = "", const std::string& env = "",
                bool req = false)
        : key(k), value(v), source(s), description(desc), envVar(env), required(req) {}
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
    std::function<void(const std::any&)> customValidator;
    
    ConfigSchema(const std::string& k, ConfigType t, const std::any& defaultVal = {}, 
                 bool req = false, const std::string& desc = "")
        : key(k), type(t), defaultValue(defaultVal), required(req), description(desc) {}
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

// Main configuration class (enhanced with backward compatibility)
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
    
    // Get values with validation and type checking
    template<typename T>
    T get(const std::string& key) const;
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Safe get methods that don't throw
    template<typename T>
    std::optional<T> tryGet(const std::string& key) const;
    
    // Set values with validation
    void set(const std::string& key, const std::any& value, ConfigSource source = ConfigSource::Runtime);
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Backward compatibility methods
    bool loadFromFile(const std::string& filepath) { return loadFromFile(filepath, false); }
    bool loadFromArgs(int argc, char** argv) { return loadFromArgs(argc, argv); }
    bool saveToFile(const std::string& filepath) const { return saveToFile(filepath); }
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        try {
            return get<T>(key);
        } catch (...) {
            return std::nullopt;
        }
    }
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        if (has(key)) {
            return get<T>(key);
        }
        return defaultValue;
    }
    bool has(const std::string& key) const { return has(key); }
    void remove(const std::string& key) { remove(key); }
    std::vector<std::string> getKeys() const { return getKeys(); }
    void clear() { clear(); }
    std::string summary() const { return summary(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toEnvVarName(const std::string& key);
    
    // Value validation
    ValidationResult validateValue(const std::string& key, const std::any& value) const;
    ValidationResult validateSingleSchema(const ConfigSchema& schema, const std::any& value) const;
    
    // Environment variable loading
    bool loadEnvVars(const std::string& prefix);
    bool parseEnvLine(const std::string& line, const std::string& prefix);
    
    // File loading
    bool loadFromSimpleFile(const std::string& filepath);
    bool loadFromJsonFile(const std::string& filepath);
    
    // Schema validation
    ValidationResult validateAgainstSchema() const;
    
    // Type conversion utilities
    template<typename T>
    T convertToType(const std::any& value);
    template<typename T>
    bool isTypeMatch(const std::any& value);
    
    // Helper function to convert string to appropriate type based on key
    std::any convertToValue(const std::string& key, const std::string& strValue);
};

// Original Config class for backward compatibility
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
    
    // Backward compatibility with enhanced config
    bool loadFromEnhancedFile(const std::string& filepath) {
        EnhancedConfig enhanced;
        return enhanced.loadFromFile(filepath);
    }
    
    void mergeWithEnhanced(const EnhancedConfig& enhanced) {
        // Implementation for merging enhanced config
    }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// Explicit template instantiations for both classes
template int EnhancedConfig::get<int>(const std::string&) const;
template int64_t EnhancedConfig::get<int64_t>(const std::string&) const;
template double EnhancedConfig::get<double>(const std::string&) const;
template bool EnhancedConfig::get<bool>(const std::string&) const;
template std::string EnhancedConfig::get<std::string>(const std::string&) const;

template int EnhancedConfig::getOr<int>(const std::string&, const int&) const;
template int64_t EnhancedConfig::getOr<int64_t>(const std::string&, const int64_t&) const;
template double EnhancedConfig::getOr<double>(const std::string&, const double&) const;
template bool EnhancedConfig::getOr<bool>(const std::string&, const bool&) const;
template std::string EnhancedConfig::getOr<std::string>(const std::string&, const std::string&) const;

// Convenience functions for enhanced configuration
EnhancedConfig createDefaultConfig();
EnhancedConfig createBrainConfig();
EnhancedConfig createWorldConfig();
EnhancedConfig createNeuromodulationConfig();

// Backward compatibility function
Config createOriginalConfig();

} // namespace nlm

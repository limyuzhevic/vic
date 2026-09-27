#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <limits>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations
class Config;

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs = {}, const std::vector<std::string>& warns = {})
        : valid(v), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) {
        errors.push_back(error);
        valid = false;
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    std::string getErrors() const {
        std::ostringstream oss;
        for (size_t i = 0; i < errors.size(); ++i) {
            oss << "Error " << (i + 1) << ": " << errors[i];
            if (i < errors.size() - 1) oss << "; ";
        }
        return oss.str();
    }
    
    std::string getWarnings() const {
        std::ostringstream oss;
        for (size_t i = 0; i < warnings.size(); ++i) {
            oss << "Warning " << (i + 1) << ": " << warnings[i];
            if (i < warnings.size() - 1) oss << "; ";
        }
        return oss.str();
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

// Configuration validation types
enum class ConfigType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntVector,
    DoubleVector,
    StringVector
};

// Configuration range constraints
struct ConfigRange {
    ConfigType type;
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    bool minInclusive = true;
    bool maxInclusive = true;
    std::string description;
    std::vector<std::string> allowedValues;
    
    ConfigRange() : type(ConfigType::Double) {}
    
    ConfigRange(ConfigType t, double minVal, double maxVal, const std::string& desc = "")
        : type(t), min(minVal), max(maxVal), description(desc) {}
    
    ConfigRange(ConfigType t, const std::vector<std::string>& values, const std::string& desc = "")
        : type(t), description(desc), allowedValues(values) {}
    
    bool isValid(double value) const {
        if (!allowedValues.empty()) {
            // Check if value matches one of the allowed string values
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << value;
            std::string strValue = oss.str();
            return std::find(allowedValues.begin(), allowedValues.end(), strValue) != allowedValues.end();
        }
        
        if (minInclusive ? value < min : value <= min) return false;
        if (maxInclusive ? value > max : value >= max) return false;
        return true;
    }
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigType type;
    ConfigRange range;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), type(ConfigType::Double) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), type(ConfigType::Double) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, ConfigType t, const ConfigRange& r)
        : key(k), value(v), source(s), description(desc), type(t), range(r) {}
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
    
    // Load from default configuration
    bool loadFromDefaults();
    
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
    
    // Validation methods
    ValidationResult validate() const;
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    std::string getValidationErrors() const;
    
    // Schema management
    static void registerDefaultSchema();
    static ConfigEntry createEntry(const std::string& key, const ConfigValue& defaultValue, 
                                   ConfigSource source, const std::string& description,
                                   ConfigType type, const ConfigRange& range = ConfigRange());
    
    // Get schema information
    static std::unordered_map<std::string, ConfigEntry> getDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toString(const ConfigValue& value);
    static double toDouble(const ConfigValue& value);
    
    // Schema storage
    static std::unordered_map<std::string, ConfigEntry> defaultSchema;
    
    // Parse string to ConfigValue
    static ConfigValue parseValue(const std::string& str, ConfigType expectedType);
    
    // Convert ConfigValue to specific type
    template<typename T>
    static T variantTo(const ConfigValue& value);
};

} // namespace nlm

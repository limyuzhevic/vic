#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <typeindex>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types (enhanced)
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,              // Added float support
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>, // Added vector of floats
    std::vector<std::string>,
    std::vector<bool>   // Added vector of bools
>;

// Configuration validation result
struct ValidationResult {
    bool valid = true;
    std::string errorMessage;
    std::string warningMessage;
    
    static ValidationResult success() { return {true}; }
    static ValidationResult error(const std::string& msg) { return {false, msg}; }
    static ValidationResult warning(const std::string& msg) { return {true, "", msg}; }
};

// Configuration constraint definition
struct Constraint {
    enum class Type {
        RangeMin,      // Minimum value
        RangeMax,      // Maximum value
        Range,         // Min and max
        Choices,       // Allowed choices
        Regex,         // String regex pattern
        Positive,      // Must be positive
        NonNegative,   // Must be >= 0
        Custom        // Custom validation function
    };
    
    Type type;
    std::string key;
    ConfigValue minValue;
    ConfigValue maxValue;
    std::vector<ConfigValue> choices;
    std::string regexPattern;
    std::function<ValidationResult(const std::string&, const ConfigValue&)> customValidator;
    
    Constraint() : type(Type::Custom), key("") {}
    Constraint(Type t, const std::string& k, const ConfigValue& v1 = {}, const ConfigValue& v2 = {}) 
        : type(t), key(k), minValue(v1), maxValue(v2) {}
};

// Configuration group
struct ConfigGroup {
    std::string name;
    std::string description;
    std::vector<std::string> keys;
    bool enabled = true;
    
    ConfigGroup() = default;
    ConfigGroup(const std::string& n, const std::string& d = "") : name(n), description(d) {}
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment  // Added environment variable support
};

// Configuration entry (enhanced)
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string group;               // Configuration group
    std::vector<Constraint> constraints; // Validation constraints
    bool isRequired = false;
    double priority = 0.0;           // For configuration merging priority
    bool isSecret = false;           // For sensitive values like passwords
    std::string validationError;
    
    ConfigEntry() : source(ConfigSource::Default), isRequired(false), priority(0.0), isSecret(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const std::string& grp = "")
        : key(k), value(v), source(s), description(desc), group(grp), isRequired(false), priority(0.0), isSecret(false) {}
    
    // Helper methods
    bool isValid() const { return validationError.empty(); }
    std::string getDisplayValue() const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
};

// Type information for validation and conversion
class TypeInfo {
public:
    enum class Kind {
        Int, Int64, Double, Float, Bool, String,
        VectorInt, VectorDouble, VectorFloat, VectorString, VectorBool
    };
    
    Kind kind;
    std::string typeName;
    std::function<bool(const ConfigValue&)> validator;
    
    TypeInfo(Kind k, const std::string& name, std::function<bool(const ConfigValue&)> validator = nullptr)
        : kind(k), typeName(name), validator(validator) {}
    
    static TypeInfo getTypeInfo(const ConfigValue& value);
    static bool validateType(const ConfigValue& value, Kind expectedKind);
    static std::optional<ConfigValue> convertValue(const ConfigValue& value, Kind targetKind);
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries = 0;
    size_t validEntries = 0;
    size_t invalidEntries = 0;
    std::unordered_map<std::string, size_t> sourceCounts;
    std::unordered_map<std::string, size_t> groupCounts;
    double memoryUsage = 0.0;
    
    void reset() {
        totalEntries = validEntries = invalidEntries = 0;
        sourceCounts.clear();
        groupCounts.clear();
        memoryUsage = 0.0;
    }
};

// Main configuration class (enhanced)
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Enhanced file loading (supports JSON, YAML, and simple key=value)
    bool loadFromFile(const std::string& filepath, bool validate = true);
    bool loadFromJSON(const std::string& filepath, bool validate = true);
    bool loadFromYAML(const std::string& filepath, bool validate = true);
    
    // Enhanced command line parsing
    bool loadFromArgs(int argc, char** argv, bool validate = true);
    bool loadFromEnvironment(const std::string& prefix = "NLM_", bool validate = true);
    
    // Save to file with format support
    bool saveToFile(const std::string& filepath, bool saveSecrets = false) const;
    bool saveToJSON(const std::string& filepath, bool saveSecrets = false) const;
    
    // Enhanced value getting with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOrThrow(const std::string& key, const std::string& errorMsg = "") const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, 
             const std::string& description = "", const std::string& group = "", bool validate = true);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& group = "", bool validate = true);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& group = "", bool validate = true);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& group = "", bool validate = true);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& group = "", bool validate = true);
    
    // Enhanced configuration management
    void addConstraint(const std::string& key, Constraint constraint);
    void removeConstraint(const std::string& key, Constraint::Type type);
    void addGroup(const std::string& name, const std::string& description = "");
    void removeGroup(const std::string& name);
    void setGroupEnabled(const std::string& name, bool enabled);
    bool isGroupEnabled(const std::string& name) const;
    
    // Configuration validation
    ValidationResult validate() const;
    ValidationResult validateKey(const std::string& key) const;
    bool hasValidConfiguration() const;
    
    // Configuration merging
    void merge(const Config& other, bool overwrite = false);
    void mergeFromFile(const std::string& filepath);
    
    // Get configuration groups
    std::vector<std::string> getGroups() const;
    std::vector<std::string> getKeysInGroup(const std::string& group) const;
    std::vector<ConfigEntry> getEntriesInGroup(const std::string& group) const;
    
    // Check existence and type safety
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, const TypeInfo& expectedType) const;
    
    // Remove and configuration management
    void remove(const std::string& key);
    void removeGroup(const std::string& group);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(bool verbose) const;
    std::string getConfigurationReport() const;
    
    // Statistics and debugging
    ConfigStats getStats() const;
    std::string getPerformanceStats() const;
    
    // Configuration dependency management
    void addDependency(const std::string& key, const std::string& dependsOn);
    void removeDependency(const std::string& key);
    bool hasDependency(const std::string& key) const;
    std::vector<std::string> getDependencies(const std::string& key) const;
    
    // Hot-reloading support
    bool hasChangedSince(const std::string& filepath) const;
    void reloadFromFile(const std::string& filepath);
    
    // Configuration templates
    static Config createFromTemplate(const std::string& templateName);
    static bool registerTemplate(const std::string& name, const Config& templateConfig);
    
    // Environment variable support
    static std::string normalizeEnvironmentKey(const std::string& key);
    static std::string denormalizeKey(const std::string& envKey);
    
    // Secrets management
    void markAsSecret(const std::string& key, bool isSecret = true);
    bool isSecret(const std::string& key) const;
    
    // Validation error management
    void clearValidationErrors();
    std::vector<std::string> getValidationErrors() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool isWhitespace(char c);
    static std::string normalizeKey(const std::string& key);
    
    // Validation helpers
    ValidationResult validateValue(const std::string& key, const ConfigValue& value) const;
    ValidationResult applyConstraints(const std::string& key, const ConfigValue& value) const;
    bool validateTypeCompatibility(const std::string& key, const ConfigValue& value) const;
};

// Helper functions for configuration
namespace ConfigHelpers {
    // Configuration file format detection
    bool isJSONFile(const std::string& filepath);
    bool isYAMLLikeFile(const std::string& filepath);
    
    // Configuration value parsing
    ConfigValue parseConfigValue(const std::string& valueStr);
    std::string serializeConfigValue(const ConfigValue& value);
    
    // Configuration key normalization
    std::string normalizeKey(const std::string& key);
    std::string denormalizeKey(const std::string& key);
    
    // Configuration validation
    ValidationResult validateAgainstConstraints(const std::string& key, const ConfigValue& value, 
                                                const std::vector<Constraint>& constraints);
    
    // Statistics and performance
    size_t calculateMemoryUsage(const std::vector<ConfigEntry>& entries);
    std::string formatMemorySize(size_t bytes);
}

} // namespace nlm

// Explicit template instantiations for type safety
namespace nlm {
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<float>> Config::get<std::vector<float>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<float> Config::getOr<std::vector<float>>(const std::string&, const std::vector<float>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;
template std::vector<bool> Config::getOr<std::vector<bool>>(const std::string&, const std::vector<bool>&) const;
}

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
    uint64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<uint64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,      // Built-in default values
    File,         // Loaded from config file
    CommandLine,  // From command line arguments
    Environment,  // From environment variables
    Runtime,      // Set at runtime via API
    Profile      // From named profile
};

// Configuration validation rule
enum class ValidationRule {
    Positive,           // Value must be positive
    NonNegative,        // Value must be >= 0
    Range,              // Value must be within specified range
    Length,             // For strings/vectors: length must match
    OneOf,              // Value must be one of specified options
    Regex,              // String must match regex pattern
    Custom,             // Custom validation function
};

// Validation rule definition
struct ValidationRuleConfig {
    ValidationRule rule;
    std::string errorMessage;
    std::optional<double> min;           // For Range
    std::optional<double> max;           // For Range
    std::optional<size_t> minLength;      // For Length
    std::optional<size_t> maxLength;      // For Length
    std::optional<std::vector<std::string>> allowedValues;  // For OneOf
    std::optional<std::string> pattern;   // For Regex
    std::function<bool(const ConfigValue&)> customValidator;  // For Custom
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::vector<ValidationRuleConfig> validationRules;
    bool isRequired = false;
    bool isProtected = false;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), isRequired(false), isProtected(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), isRequired(false), isProtected(false) {}
};

// Configuration profile
struct ConfigProfile {
    std::string name;
    std::string description;
    std::vector<std::string> baseProfiles;  // Profiles to inherit from
    std::vector<std::pair<std::string, ConfigValue>> settings;
    bool active = false;
};

// Advanced configuration class with validation and profiles
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (simple key=value format, with JSON enhancement option)
    bool loadFromFile(const std::string& filepath, bool useJsonFormat = false);
    
    // Load from JSON string (when JSON support is available)
    bool loadFromJson(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Save to file (JSON format when available, simple format otherwise)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe getters
    template<typename T>
    std::optional<T> getTyped(const std::string& key) const;
    
    template<typename T>
    std::vector<T> getVector(const std::string& key) const;
    
    // String getters
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<int64_t> getInt(const std::string& key) const;
    std::optional<double> getFloat(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-safe setters
    bool setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool setInt(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    bool setFloat(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    bool setVector(const std::string& key, const std::vector<double>& values, ConfigSource source = ConfigSource::Runtime);
    
    // Set with explicit source
    template<typename T>
    bool setWithSource(const std::string& key, const T& value, ConfigSource source);
    
    // Check existence and validation
    bool has(const std::string& key) const;
    bool isValid(const std::string& key) const;
    std::vector<std::string> getValidationErrors(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(ConfigSource source) const;
    
    // Profile management
    void addProfile(const ConfigProfile& profile);
    bool removeProfile(const std::string& name);
    bool activateProfile(const std::string& name);
    bool deactivateProfile(const std::string& name);
    bool hasProfile(const std::string& name) const;
    std::vector<std::string> getProfileNames() const;
    std::vector<std::string> getActiveProfileNames() const;
    
    // Apply profile (merges with existing settings)
    bool applyProfile(const std::string& name);
    
    // Get profile information
    ConfigProfile getProfile(const std::string& name) const;
    
    // Batch operations
    bool setBatch(const std::vector<std::pair<std::string, ConfigValue>>& settings);
    bool applyBatch(const std::vector<std::pair<std::string, ConfigValue>>& settings, ConfigSource source);
    
    // Override protection
    void protect(const std::string& key);
    void unprotect(const std::string& key);
    bool isProtected(const std::string& key) const;
    
    // Validation
    bool validate() const;
    std::vector<std::string> getAllValidationErrors() const;
    
    // Configuration comparison and merging
    bool equals(const Config& other) const;
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries = 0;
        size_t validEntries = 0;
        size_t invalidEntries = 0;
        size_t protectedEntries = 0;
        std::map<ConfigSource, size_t> entriesBySource;
    };
    
    ConfigStats getStats() const;
    
    // Get entries by source
    std::vector<ConfigEntry> getEntries(ConfigSource source) const;
    
    // Comparison operators
    bool operator==(const Config& other) const { return equals(other); }
    bool operator!=(const Config& other) const { return !equals(other); }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> getValueValidationErrors(const std::string& key, const ConfigValue& value) const;
    
    // File format helpers
    bool loadFromJsonInternal(const std::string& jsonString);
    std::string saveToJson() const;
    
    // Profile helpers
    void resolveProfileInheritance(ConfigProfile& profile, std::vector<ConfigProfile>& allProfiles) const;
    
    // Type conversion helpers
    template<typename T>
    static std::optional<T> convertToType(const ConfigValue& value);
    
    static bool matchesValidationRule(const ConfigValue& value, const ValidationRuleConfig& rule);
};

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Validation helpers
template<typename T>
bool isInRange(const T& value, double min, double max);

template<typename T>
bool isPositive(const T& value);

template<typename T>
bool isNonNegative(const T& value);

// OneOf validation
template<typename T>
bool isOneOf(const T& value, const std::vector<T>& allowed);

} // namespace nlm
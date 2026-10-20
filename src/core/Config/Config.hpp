#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <set>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : std::runtime_error("Config validation error: " + message), key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

// Parameter type enum for better type checking
enum class ConfigParamType {
    Int,
    Int64,
    Double,
    Bool,
    String,
    IntVector,
    DoubleVector,
    StringVector,
    Unknown
};

// Parameter metadata for validation and documentation
struct ConfigParameterInfo {
    std::string name;
    ConfigParamType type;
    std::string description;
    std::string defaultValue;
    bool required;
    std::vector<std::string> allowedValues;
    std::function<bool(const ConfigValue&)> validator;
    double minValue;
    double maxValue;
    bool isAdvanced;  // For hiding advanced parameters
    
    ConfigParameterInfo()
        : type(ConfigParamType::Unknown), required(false), defaultValue("")
        , minValue(std::numeric_limits<double>::lowest()), maxValue(std::numeric_limits<double>::max())
        , isAdvanced(false) {}
};

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Enhanced configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    uint64_t timestamp;
    bool validated;
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), timestamp(0), validated(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), timestamp(0), validated(false) {}
};

// Main enhanced configuration class
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
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON
    std::string saveToJSON() const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe getters
    int getInt(const std::string& key) const;
    int64_t getInt64(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::string getString(const std::string& key) const;
    std::vector<int> getIntVector(const std::string& key) const;
    std::vector<double> getDoubleVector(const std::string& key) const;
    std::vector<std::string> getStringVector(const std::string& key) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate configuration against schema
    bool validate() const;
    
    // Get parameter information
    const ConfigParameterInfo& getParameterInfo(const std::string& key) const;
    
    // Register parameter schema
    void registerParameter(const std::string& key, const ConfigParameterInfo& info);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all keys (including advanced)
    std::vector<std::string> getKeys(bool includeAdvanced) const;
    
    // Get configuration summary with validation
    std::string summary(bool includeAdvanced = false) const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Clear all
    void clear();
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t validatedEntries;
        size_t invalidEntries;
        std::set<ConfigSource> sources;
        double averageValueSize;
        
        ConfigStats() : totalEntries(0), validatedEntries(0), invalidEntries(0), averageValueSize(0.0) {}
    };
    
    ConfigStats getStats() const;
    
    // Export configuration
    std::map<std::string, std::string> exportAsStringMap() const;
    
    // Import configuration
    void importFromStringMap(const std::map<std::string, std::string>& data);
    
    // Get default configuration file path
    static std::string getDefaultConfigPath();
    
    // Get configuration parameter categories
    enum class ParameterCategory {
        Basic,
        Advanced,
        Neural,
        Plasticity,
        Development,
        Memory,
        Experimental
    };
    
    std::vector<std::string> getKeysByCategory(ParameterCategory category) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static ConfigParamType detectType(const std::string& value);
    static std::optional<ConfigValue> parseValue(const std::string& str, ConfigParamType expectedType);
    static bool validateValue(const std::string& key, const ConfigValue& value, const ConfigParameterInfo& info);
    static std::string valueToString(const ConfigValue& value);
};

// Template implementations

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Helper function for detecting parameter type from string
static ConfigParamType detectTypeFromString(const std::string& str);

} // namespace nlm

} // namespace nlm

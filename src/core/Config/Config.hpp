// Config.hpp - Enhanced version with better error handling and validation

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <stdexcept>
#include <functional>
#include <chrono>
#include <ctime>

namespace nlm {

// Custom exceptions for configuration errors
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
};

class ConfigKeyError : public ConfigError {
public:
    explicit ConfigKeyError(const std::string& key) : ConfigError("Configuration key not found: " + key) {}
};

class ConfigTypeError : public ConfigError {
public:
    explicit ConfigTypeError(const std::string& key, const std::string& expectedType, const std::string& actualType)
        : ConfigError("Type mismatch for key \"" + key + "\": expected " + expectedType + ", got " + actualType) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& message) : ConfigError("Configuration validation failed: " + message) {}
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

// Validation function type
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    ValidationFunction validator;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validationRegex(),
                   validator(nullptr), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), 
          validator(nullptr), 
          timestamp(std::chrono::system_clock::now()) {}
    
    // Validation methods
    bool isValid() const {
        if (validator) {
            return validator(value);
        }
        if (!validationRegex.empty()) {
            // Basic regex validation would go here
            return true;
        }
        return true;
    }
    
    std::string getValueTypeName() const {
        return std::visit([](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) return "int";
            else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
            else if constexpr (std::is_same_v<T, double>) return "double";
            else if constexpr (std::is_same_v<T, bool>) return "bool";
            else if constexpr (std::is_same_v<T, std::string>) return "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) return "vector<int>";
            else if constexpr (std::is_same_v<T, std::vector<double>>) return "vector<double>";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "vector<string>";
            else return "unknown";
        }, value);
    }
    
    std::string getFormattedValue() const {
        std::ostringstream oss;
        std::visit([&oss](const auto& val) {
            oss << val;
        }, value);
        return oss.str();
    }
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
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with enhanced error handling
    template<typename T>
    T get(const std::string& key);
    
    template<typename T>
    std::optional<T> getSafe(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "",
             const std::string& validationRegex = "",
             const ValidationFunction& validator = nullptr);
    
    // Convenience setters with validation
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation and error checking
    bool validateAll() const;
    bool validateKey(const std::string& key) const;
    
    // Key existence and type checking
    bool has(const std::string& key) const;
    std::string getValueType(const std::string& key) const;
    
    // Remove and cleanup
    void remove(const std::string& key);
    void remove(const std::string& key, ConfigSource source);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all or by source
    void clear();
    void clear(ConfigSource source);
    
    // Get configuration summary
    std::string summary() const;
    std::string detailedSummary() const;
    
    // Time-related functions
    std::chrono::system_clock::time_point getTimestamp(const std::string& key) const;
    std::string getFormattedTimestamp(const std::string& key) const;
    
    // Statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::vector<ConfigSource> getSourcesWithEntries() const;
    
    // Configuration validation presets
    static bool validateString(const ConfigValue& value);
    static bool validateInt(const ConfigValue& value);
    static bool validateIntRange(int min, int max);
    static bool validateDouble(const ConfigValue& value);
    static bool validateDoubleRange(double min, double max);
    static bool validateBool(const ConfigValue& value);
    static bool validatePositive(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidKey(const std::string& key);
    static bool isValidDescription(const std::string& description);
    
    // File I/O with error handling
    bool loadFromStream(std::istream& stream, ConfigSource source);
    bool saveToStream(std::ostream& stream) const;
    
    // JSON parsing
    bool parseJsonValue(const std::string& valueStr, ConfigValue& outValue) const;
    std::string configValueToJsonString(const ConfigValue& value) const;
};

// Exception throwing versions for easier error handling
template<typename T>
T Config::get(const std::string& key) {
    auto val = getSafe<T>(key);
    if (!val.has_value()) {
        throw ConfigKeyError(key);
    }
    return val.value();
}

// Explicit template instantiations
extern template std::optional<int> Config::getSafe<int>(const std::string&) const;
extern template std::optional<int64_t> Config::getSafe<int64_t>(const std::string&) const;
extern template std::optional<double> Config::getSafe<double>(const std::string&) const;
extern template std::optional<bool> Config::getSafe<bool>(const std::string&) const;
extern template std::optional<std::string> Config::getSafe<std::string>(const std::string&) const;
extern template std::optional<std::vector<int>> Config::getSafe<std::vector<int>>(const std::string&) const;
extern template std::optional<std::vector<double>> Config::getSafe<std::vector<double>>(const std::string&) const;
extern template std::optional<std::vector<std::string>> Config::getSafe<std::vector<std::string>>(const std::string&) const;

// Template instantiations for getOr
extern template int Config::getOr<int>(const std::string&, const int&) const;
extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
extern template double Config::getOr<double>(const std::string&, const double&) const;
extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
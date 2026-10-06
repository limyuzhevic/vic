#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <stdexcept>

// Include nlohmann/json for JSON parsing
#include <nlohmann/json.hpp>

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

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationPattern;  // Regex pattern for validation
    bool required;                   // Whether this key is required
    double minValue;                 // For numeric values
    double maxValue;                 // For numeric values
    std::string defaultValue;        // String representation of default
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(),
                   validationPattern(), required(false), minValue(-std::numeric_limits<double>::max()),
                   maxValue(std::numeric_limits<double>::max()), defaultValue() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          validationPattern(), required(false),
          minValue(-std::numeric_limits<double>::max()),
          maxValue(std::numeric_limits<double>::max()), defaultValue() {}
    
    bool isValid() const {
        if (required && key.empty()) return false;
        
        // Validate based on value type
        bool valid = true;
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                         std::is_same_v<T, double>) {
                if (val < minValue || val > maxValue) valid = false;
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Basic regex validation if pattern provided
                if (!validationPattern.empty()) {
                    // Simple regex check (in practice, use a proper regex library)
                    valid = true;  // Placeholder
                }
            }
        }, value);
        
        return valid;
    }
};

// Configuration validation callback
using ValidationCallback = std::function<bool(const ConfigEntry&)>;

// Configuration schema for validation
struct Config::Schema {
    std::map<std::string, ValidationCallback> validations;
    std::vector<std::string> requiredKeys;
    bool strictMode = false;  // Reject unknown keys
};

// Main configuration class with enhanced features
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
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration as JSON string
    std::string toJsonString() const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced set with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Schema validation
    void setSchema(const Schema& schema) { schema_ = schema; }
    bool validate() const;
    
    // File operations with error handling
    static bool fileExists(const std::string& filepath);
    static std::string getFileExtension(const std::string& filepath);
    
    // Utility functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool stringToBool(const std::string& str);
    static std::string boolToString(bool value);
    
    // Type conversion utilities
    template<typename T>
    static std::optional<T> variantToValue(const ConfigValue& variant);
    
    // Advanced configuration features
    void enableStrictMode(bool enable) { schema_.strictMode = enable; }
    void addValidation(const std::string& key, const ValidationCallback& callback);
    void setRequiredKeys(const std::vector<std::string>& keys) { schema_.requiredKeys = keys; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Schema schema_;
    
    // Internal helpers
    bool loadFromSimpleFormat(const std::string& content);
    bool loadFromJson(const nlohmann::json& json);
    std::string saveToSimpleFormat() const;
    nlohmann::json saveToJsonFormat() const;
    
    // Error handling
    static void logError(const std::string& message);
    static bool isValidKey(const std::string& key);
};

} // namespace nlm

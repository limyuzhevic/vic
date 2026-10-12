#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <stdexcept>
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

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRule;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validationRule() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const std::string& validation = "")
        : key(k), value(v), source(s), description(desc), validationRule(validation) {}
    
    // Helper to check if entry is valid
    bool isValid() const {
        return !key.empty() && !validationRule.empty();
    }
};

// Configuration validation error
struct ConfigValidationError : public std::runtime_error {
    std::string key;
    std::string message;
    
    ConfigValidationError(const std::string& k, const std::string& msg)
        : std::runtime_error("Configuration validation error: " + msg), key(k), message(msg) {}
};

// Configuration change callback
using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue)>;

// Main configuration class with validation and callback support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
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
    
    // Set value with validation and callback
    void setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void addChangeCallback(const ConfigChangeCallback& callback);
    
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
    
    // Configuration validation
    void validate() const;
    void addValidationRule(const std::string& key, const std::string& rule);
    
    // File format helpers
    static std::string valueToString(const ConfigValue& value);
    static bool stringToValue(const std::string& str, ConfigValue& outValue);
    
    // Utility methods
    std::string getValueType(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJsonValue(const std::string& str, ConfigValue& outValue);
    static std::string jsonValueToString(const nlohmann::json& j);
    static bool jsonIsConfigValue(const nlohmann::json& j, ConfigValue& outValue);
    static void validateKeyValue(const std::string& key, const ConfigValue& value, const std::unordered_map<std::string, std::string>& validationRules);
};

// JSON serialization helper
namespace config_json {
    nlohmann::json configValueToJson(const ConfigValue& value);
    bool jsonToConfigValue(const nlohmann::json& j, ConfigValue& outValue);
}

// Global configuration helper
class GlobalConfig {
public:
    static GlobalConfig& getInstance();
    static void setGlobalConfig(std::shared_ptr<Config> config);
    
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    
    // Convenience access methods
    template<typename T>
    std::optional<T> get(const std::string& key) const {
        if (config_) {
            return config_->get<T>(key);
        }
        return std::nullopt;
    }
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const {
        if (config_) {
            return config_->getOr<T>(key, defaultValue);
        }
        return defaultValue;
    }
    
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    void validate() const;
    
private:
    GlobalConfig() = default;
    ~GlobalConfig() = default;
    GlobalConfig(const GlobalConfig&) = delete;
    GlobalConfig& operator=(const GlobalConfig&) = delete;
    
    std::shared_ptr<Config> config_;
};

} // namespace nlm

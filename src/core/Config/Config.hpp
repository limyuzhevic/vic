#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <json.hpp>  // nlohmann::json
#include <spdlog/spdlog.h>

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

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    
    ValidationResult(bool valid = true, const std::string& error = "")
        : isValid(valid), errorMessage(error) {}
    
    static ValidationResult success() { return ValidationResult(true); }
    static ValidationResult error(const std::string& msg) { return ValidationResult(false, msg); }
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::function<ValidationResult(const ConfigValue&)> validator;
    bool isRequired;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), validator(nullptr), isRequired(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", 
                std::function<ValidationResult(const ConfigValue&)> val = nullptr,
                bool req = false)
        : key(k), value(v), source(s), description(desc), 
          validator(val), isRequired(req) {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::string category;
    std::string key;
    std::string description;
    ConfigSource defaultSource;
    ConfigValue defaultValue;
    std::function<ValidationResult(const ConfigValue&)> validator;
    
    ConfigSchema(const std::string& cat, const std::string& k, const std::string& desc,
                 ConfigSource source, const ConfigValue& defaultVal,
                 std::function<ValidationResult(const ConfigValue&)> val = nullptr)
        : category(cat), key(k), description(desc), defaultSource(source),
          defaultValue(defaultVal), validator(val) {}
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from dictionary (nlohmann::json)
    bool loadFromJson(const nlohmann::json& jsonData);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnv(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with enhanced type handling
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Advanced get methods
    template<typename T>
    T getRequired(const std::string& key) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, 
             ConfigSource source = ConfigSource::Runtime,
             bool skipValidation = false);
    
    void set(const std::string& key, const std::string& value, 
             ConfigSource source = ConfigSource::Runtime,
             bool skipValidation = false);
    void set(const std::string& key, int value,
             ConfigSource source = ConfigSource::Runtime,
             bool skipValidation = false);
    void set(const std::string& key, double value,
             ConfigSource source = ConfigSource::Runtime,
             bool skipValidation = false);
    void set(const std::string& key, bool value,
             ConfigSource source = ConfigSource::Runtime,
             bool skipValidation = false);
    
    // Configuration validation
    ValidationResult validate(const std::string& key) const;
    bool isValid(const std::string& key) const;
    
    // Check existence and type
    bool has(const std::string& key) const;
    template<typename T>
    bool hasType(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by category
    std::vector<std::string> getKeys(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration statistics
    std::string stats() const;
    
    // Initialize with default schema
    void initializeWithSchema(const std::vector<ConfigSchema>& schema);
    
    // Merge another config
    void merge(const Config& other);
    
    // Configuration source management
    ConfigSource getSource(const std::string& key) const;
    std::vector<std::pair<std::string, ConfigSource>> getSourceMap() const;
    
    // Environment variable conversion utilities
    static std::string toEnvVarName(const std::string& key);
    static bool fromEnvVar(const std::string& envVar, ConfigValue& value);
    
    // File format utilities
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    static bool isConfigFile(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractCategory(const std::string& key);
    
    // Validation helpers
    ValidationResult validateValue(const ConfigValue& value, 
                                   std::function<ValidationResult(const ConfigValue&)> validator) const;
    
    // File format helpers
    bool loadJsonFromFile(const std::string& filepath);
    bool loadYamlFromFile(const std::string& filepath);
    bool loadSimpleFromFile(const std::string& filepath);
    
    bool saveJsonToFile(const std::string& filepath) const;
    bool saveSimpleToFile(const std::string& filepath) const;
};

// Template implementations

// Enhanced get method with better error handling
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
        spdlog::warn("Config key '{}' has wrong type for requested value type", key);
        return std::nullopt;
    }
}

// Advanced getOr with statistics tracking
template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        spdlog::debug("Config key '{}' not found, using default value", key);
    }
    return val.value_or(defaultValue);
}

// Get required value with validation
template<typename T>
T Config::getRequired(const std::string& key) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        throw std::runtime_error("Required configuration key '" + key + "' is missing");
    }
    
    auto validation = validate(key);
    if (!validation.isValid) {
        throw std::runtime_error("Configuration key '" + key + "' validation failed: " + validation.errorMessage);
    }
    
    return val.value();
}

// Type-specific check
template<typename T>
bool Config::hasType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return std::holds_alternative<T>(it->value);
}

// Explicit template instantiations

} // namespace nlm
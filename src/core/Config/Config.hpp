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

// Configuration entry with validation
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable bool isValidated;  // Track if validation has been performed
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), isValidated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), isValidated(false) {}
    
    // Validate the entry based on key type
    bool validate() const {
        if (isValidated) return true;
        
        // Basic validation - can be extended with specific rules
        if (key.empty()) {
            return false;
        }
        
        // Type-specific validation could be added here
        // For example: specific keys might require specific types
        
        isValidated = true;
        return true;
    }
};

// Main configuration class with enhanced error handling and validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file with validation and error handling
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with validation
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with validation
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type checking and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence with validation
    bool has(const std::string& key) const;
    
    // Remove key with validation
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate entire configuration
    std::vector<std::string> validateAll() const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal validation methods
    bool validateKey(const std::string& key) const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    
    // Cache for performance
    mutable std::unordered_map<std::string, bool> validationCache;
};

// Utility function to check if configuration has required keys
inline bool configHasRequiredKeys(const Config& config, const std::vector<std::string>& requiredKeys) {
    for (const auto& key : requiredKeys) {
        if (!config.has(key)) {
            return false;
        }
    }
    return true;
}

// Utility function to get a configuration value with default
inline std::string configGetString(const Config& config, const std::string& key, const std::string& defaultValue = "") {
    return config.getOr<std::string>(key, defaultValue);
}

inline int configGetInt(const Config& config, const std::string& key, int defaultValue = 0) {
    return config.getOr<int>(key, defaultValue);
}

inline double configGetDouble(const Config& config, const std::string& key, double defaultValue = 0.0) {
    return config.getOr<double>(key, defaultValue);
}

inline bool configGetBool(const Config& config, const std::string& key, bool defaultValue = false) {
    return config.getOr<bool>(key, defaultValue);
}

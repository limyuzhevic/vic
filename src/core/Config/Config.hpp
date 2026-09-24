#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <chrono>

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
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
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
    
    // Load from file (key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with automatic type conversion
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
    
    // Configuration validation
    bool validate();
    std::string getValidationErrors() const;
    
    // Copy and merge operations
    std::shared_ptr<Config> copy() const;
    void merge(const Config& other);
    
    // Configuration presets
    static std::shared_ptr<Config> createDevelopmentConfig();
    static std::shared_ptr<Config> createExplorationConfig();
    static std::shared_ptr<Config> createEfficiencyConfig();
    static std::shared_ptr<Config> createSocialConfig();
    static std::shared_ptr<Config> createAdaptiveConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // String to ConfigValue conversion
    ConfigValue stringToConfigValue(const std::string& str, const std::string& key);
    bool tryParseInt(const std::string& str, int64_t& result);
    bool tryParseFloat(const std::string& str, float& result);
    
    // Validation helpers
    void validateSingleKey(const std::string& key, const ConfigValue& value);
    std::string getDescription(const std::string& key);
    double extractNumericValue(const ConfigValue& value);
    std::string getCurrentTimestamp();
    
    // Configuration validation constants and functions
    struct ConfigValidation {
        static const std::unordered_map<std::string, std::vector<std::function<bool(const ConfigValue&)>>> VALIDATORS;
        static const std::unordered_map<std::string, std::pair<double, double>> RANGES;
        static const std::unordered_map<std::string, std::string> DESCRIPTIONS;
    };
};

} // namespace nlm

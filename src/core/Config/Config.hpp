#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <unordered_set>
#include <chrono>
#include <stdexcept>
#include <any>
#include <map>

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

// Parameter validation error types
class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, const std::string& key)
        : std::runtime_error(message), key_(key) {}
    const std::string& getKey() const { return key_; }
private:
    std::string key_;
};

// Parameter categories for better organization
enum class ParameterCategory {
    Simulation,
    Neural,
    Plasticity,
    Memory,
    Cognition,
    Development,
    Neuromodulation,
    Performance,
    Debug
};

// Parameter metadata for better CLI help
struct ParameterInfo {
    std::string key;
    std::string description;
    ParameterCategory category;
    std::any defaultValue;
    std::string type;
    std::vector<std::string> validValues;
    bool required;
    
    ParameterInfo() : description(), category(ParameterCategory::Simulation), required(false) {}
    ParameterInfo(const std::string& k, const std::string& desc, ParameterCategory cat, 
                  const std::any& defaultVal = {}, const std::string& typeName = "",
                  const std::vector<std::string>& valid = {}, bool req = false)
        : key(k), description(desc), category(cat), defaultValue(defaultVal), 
          type(typeName), validValues(valid), required(req) {}
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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
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
    
    // Enhanced set with parameter metadata
    void set(const std::string& key, const ConfigValue& value, const ParameterInfo& paramInfo, ConfigSource source = ConfigSource::Runtime);
    
    // Parameter metadata management
    void registerParameter(const ParameterInfo& paramInfo);
    std::optional<ParameterInfo> getParameterInfo(const std::string& key) const;
    std::vector<ParameterInfo> getParametersByCategory(ParameterCategory category) const;
    std::vector<ParameterCategory> getAllCategories() const;
    
    // Validation
    template<typename T>
    bool validate(const std::string& key, const T& value) const;
    
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
    
    // Preset configurations
    static Config createFastPreset();
    static Config createAccuratePreset();
    static Config createBalancedPreset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

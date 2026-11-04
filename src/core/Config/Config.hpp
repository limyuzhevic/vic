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
    
    // Get values with improved type safety
    template<typename T>
    T get(const std::string& key, const T& defaultValue = T()) const;
    
    template<typename T>
    bool tryGet(const std::string& key, T& outValue) const;
    
    // Configuration presets for common use cases
    static std::shared_ptr<Config> createStandardBrainConfig(size_t neuronCount = 1000);
    static std::shared_ptr<Config> createLearningConfig(float learningRate = 0.01f);
    static std::shared_ptr<Config> createDebugConfig(bool verboseLogging = true);
    
    // Get configuration category (for organizing settings)
    static std::string getCategory(const std::string& key);
    
    // Get all keys in a specific category
    static std::vector<std::string> getKeysInCategory(const std::string& category);
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced type checking
    bool isInt(const std::string& key) const;
    bool isFloat(const std::string& key) const;
    bool isString(const std::string& key) const;
    bool isBool(const std::string& key) const;
    bool isVector(const std::string& key) const;
    
    // Configuration validation
    std::vector<std::string> validate() const;
    
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
    
    // Clone configuration
    std::shared_ptr<Config> clone() const;
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration presets
    void applyPreset(const std::string& presetName);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getConfigCategory(const std::string& key);
    static bool validateValue(const std::string& key, const ConfigValue& value);
};

} // namespace nlm

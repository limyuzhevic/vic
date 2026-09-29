#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <limits>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types with additional precision
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::map<std::string, double>,  // For structured parameters
    std::vector<std::pair<std::string, double>>  // Key-value pairs
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration category for better organization
enum class ConfigCategory {
    Core,
    Brain,
    Agent,
    World,
    Memory,
    Plasticity,
    Neuromodulation,
    Development,
    Sensory,
    Motor,
    Performance,
    Debug,
    Experiment,
    User
};

// Configuration metadata for better documentation and validation
struct ConfigMetadata {
    std::string description;
    std::string unit;
    std::string category;
    std::function<bool(const ConfigValue&)> validator;
    std::function<std::string(const ConfigValue&)> formatter;
    std::function<std::string(const ConfigValue&)> parser;
    bool isRequired = false;
    double minValue = std::numeric_limits<double>::lowest();
    double maxValue = std::numeric_limits<double>::max();
    std::vector<std::string> allowedValues;
    
    ConfigMetadata() = default;
    ConfigMetadata(const std::string& desc, const std::string& cat)
        : description(desc), category(cat) {}
};

// Enhanced Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    ConfigCategory category;
    ConfigMetadata metadata;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), category(ConfigCategory::Core) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), category(ConfigCategory::Core) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc, ConfigCategory cat)
        : key(k), value(v), source(s), description(desc), category(cat) {}
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
    
    // Load from file (supports multiple formats)
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(ConfigCategory category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

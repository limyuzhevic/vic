#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Configuration metadata for documentation and validation
struct ConfigMetadata {
    std::string description;
    std::string category;  // e.g., "Brain", "Plasticity", "Neuromodulation", "Development", "Memory"
    std::string type;      // e.g., "int", "double", "bool", "string", "vector"
    std::string unit;      // e.g., "ms", "Hz", "mV", "%", "count"
    std::string validation; // e.g., "positive", "range(0,100)", "non-negative"
    std::string defaultValue; // String representation of default value
    
    ConfigMetadata()
        : description(), category(), type(), unit(), validation(), defaultValue() {}
    
    ConfigMetadata(const std::string& desc, const std::string& cat, const std::string& typ,
                  const std::string& un = "", const std::string& val = "",
                  const std::string& def = "")
        : description(desc), category(cat), type(typ), unit(un), validation(val), defaultValue(def) {}
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::optional<ConfigMetadata> metadata;  // Optional metadata for documentation and validation
    
    ConfigEntry()
        : key(), value(), source(ConfigSource::Default), description(), metadata() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), metadata() {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const ConfigMetadata& meta, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), metadata(meta) {}
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
    
    // Get metadata for a configuration key
    std::optional<ConfigMetadata> getMetadata(const std::string& key) const;
    
    // Validate configuration values based on metadata
    std::vector<std::string> validateConfiguration() const;
    
    // Get configuration category
    std::vector<std::string> getCategories() const;
    
    // Get configuration keys by category
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Get configuration summary with metadata
    std::string summaryWithMetadata() const;
    
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

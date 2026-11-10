#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include "TypedConfiguration.hpp"

namespace nlm {

// Backward compatibility: Use the new TypedConfiguration internally
// For backward compatibility, we keep the same interface
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (supports both simple key=value and JSON formats)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (backward compatibility)
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values (backward compatibility)
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Advanced: Access the underlying typed configuration
    TypedConfiguration& getTypedConfig() { return typedConfig; }
    const TypedConfiguration& getTypedConfig() const { return typedConfig; }
    
private:
    // Use the new typed configuration internally
    TypedConfiguration typedConfig;
    
    // Helper to get section name from key (for backward compatibility)
    static std::string extractSectionName(const std::string& key, const std::string& defaultSection = "general");
    
    // Helper to extract key from full key with section
    static std::string extractKey(const std::string& fullKey);
    
    // Convert old ConfigValue to new TypedConfiguration format
    static TypedConfiguration::ConfigValue convertToTyped(const ConfigValue& oldValue);
    
    // Convert TypedConfiguration format to old ConfigValue
    static ConfigValue convertFromTyped(const TypedConfiguration::ConfigValue& newValue);
};

// Configuration source enum (for backward compatibility)
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration entry (for backward compatibility)
struct ConfigEntry {
    std::string key;
    std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>> value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const std::variant<int, int64_t, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc) {}
};

} // namespace nlm

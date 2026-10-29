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
    bool validate(const std::string& key, const ConfigValue& value) const;
    bool validate(const std::string& key) const;
    std::vector<std::string> getValidationErrors() const;
    
    // Schema management
    void addSchema(const std::string& key, const std::string& type, 
                   bool required = false, const std::string& description = "",
                   const ConfigValue& defaultValue = std::monostate{});
    void removeSchema(const std::string& key);
    
    // Configuration presets
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createPhase2Config();
    static std::shared_ptr<Config> createPhase3Config();
    static std::shared_ptr<Config> createPhase4Config();
    static std::shared_ptr<Config> createPhase6Config();
    
    // Configuration templates
    void setPreset(const std::string& presetName);
    std::vector<std::string> getAvailablePresets() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeString(const std::string& str);
    static bool validateType(const ConfigValue& schemaValue, const ConfigValue& actualValue);
    static bool validateValue(const std::string& key, const ConfigValue& value);
    static bool validateKeySpecific(const std::string& key, const ConfigValue& value);
    static std::string getTypeString(const std::string& type);
    static std::string getTypeString(const ConfigValue& value);
};

} // namespace nlm

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
    
    // Load from file (JSON format - Phase 4 enhancement)
    bool loadFromFile(const std::string& filepath);
    
    // Load from file (legacy support)
    bool loadFromSimpleFile(const std::string& filepath);
    
    // Load from file (JSON format - Phase 4 enhancement)
    bool loadFromJSON(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
// Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values with validation
    void setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation
    bool validate(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> validateAll() const;
    bool isValid(const std::string& key) const;
    
    // Type conversion and validation helpers
    template<typename T>
    std::optional<T> getValidated(const std::string& key) const;
    
    // Configuration schema
    static std::vector<std::string> getAllKnownKeys();
    static bool isKnownKey(const std::string& key);
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
    // Configuration validation for known keys
    static std::map<std::string, std::function<bool(const ConfigValue&)>> getValidationFunctions();
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Validation
    bool validate(const std::string& key, const ConfigValue& value) const;
    std::vector<std::string> validateAll() const;
    bool isValid(const std::string& key) const;
    
    // Type conversion and validation helpers
    template<typename T>
    std::optional<T> getValidated(const std::string& key) const;
    
    // Configuration schema
    static std::vector<std::string> getAllKnownKeys();
    static bool isKnownKey(const std::string& key);
    
    // Error handling
    std::string getLastError() const;
    void clearError();
    
    // Configuration validation for known keys
    static std::map<std::string, std::function<bool(const ConfigValue&)>> getValidationFunctions();
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
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

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
    
    // Get configuration value as string
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Get numeric values with error checking
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    
    // Configuration validation and error handling
    bool validate(const std::string& key, std::string& error) const;
    
    // Set a value with validation
    bool setValidated(const std::string& key, const ConfigValue& value, 
                     ConfigSource source = ConfigSource::Runtime, std::string& error = std::string());
    
    // Set default values for a specific configuration profile
    void setDefaults(const std::string& profile, ConfigSource source = ConfigSource::Default);
    
    // Merge configuration from another Config object
    void merge(const Config& other);
    
    // Get configuration profile names
    static std::vector<std::string> getAvailableProfiles();
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set a value from string with automatic type detection
    void setTypedValue(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper for getting value as string
    std::string getValueAsString(const ConfigValue& value) const;
    
    // Load from file with error handling
    bool loadFromFile(const std::string& filepath, std::string& error);
    
    // Load from command line with error handling
    bool loadFromArgs(int argc, char** argv, std::string& error);
    
    // Save to file with error handling
    bool saveToFile(const std::string& filepath, std::string& error) const;
};

// Allow main.cpp to call private loadFromFile and loadFromArgs with error handling
bool Config::loadFromFile(const std::string& filepath);
bool Config::loadFromArgs(int argc, char** argv);

} // namespace nlm
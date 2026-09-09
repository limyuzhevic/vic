#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;

// Custom exceptions for configuration operations
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigKeyError : public ConfigError {
public:
    explicit ConfigKeyError(const std::string& key) : ConfigError("Configuration key not found: " + key) {}
};

class ConfigValueError : public ConfigError {
public:
    explicit ConfigValueError(const std::string& key, const std::string& expected) 
        : ConfigError("Configuration value error for key " + key + ": expected " + expected) {}
};

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

// Main configuration class with Pythonic interface
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
    
    // Pythonic interface: dictionary-like access
    bool has(const std::string& key) const;
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Pythonic methods
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    void clear();
    std::string summary() const;
    
    // Pythonic helper methods
    bool operator[](const std::string& key) const;  // For bool values
    int getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    std::string getString(const std::string& key) const;
    bool getBool(const std::string& key) const;
    
    // Conversion helpers
    static std::string toPythonKey(const std::string& key);
    static std::string fromPythonKey(const std::string& key);
    
    // Configuration presets for common brain settings
    static void applyDefaultBrainSettings(Config& config);
    static void applyPerformanceSettings(Config& config);
    static void applyDevelopmentSettings(Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

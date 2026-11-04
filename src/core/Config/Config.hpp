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
    float,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<float>,
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
    
    // Get values (defined here so templates instantiate in every TU)
template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Set with source and description
    void set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "");
    
    // Load from environment variable
    bool loadFromEnv(const std::string& envVar, ConfigSource source = ConfigSource::CommandLine);
    
    // Set with validation
    template<typename T>
    bool setValidated(const std::string& key, const T& value, 
                      std::function<bool(const T&)> validator = nullptr,
                      ConfigSource source = ConfigSource::Runtime,
                      const std::string& description = "");
    
    // Get value with default and validation
    template<typename T>
    std::pair<T, bool> getValidated(const std::string& key, const T& defaultValue,
                                   std::function<bool(const T&)> validator = nullptr) const;
    
    // Check if key exists with validation
    template<typename T>
    bool hasValidated(const std::string& key, std::function<bool(const T&)> validator = nullptr) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getEnvVar(const std::string& varName);
    static bool stringToValue(const std::string& str, ConfigValue& outValue);
    static std::string valueToString(const ConfigValue& value);
    static bool isNumber(const std::string& str);
    static bool isBoolean(const std::string& str);
    static bool stringToBool(const std::string& str, bool& outValue);
    static bool stringToNumber(const std::string& str, double& outValue);
};

} // namespace nlm

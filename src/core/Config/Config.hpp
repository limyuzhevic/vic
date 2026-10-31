#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>

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

// Configuration validation callback
using ConfigValidationCallback = std::function<bool(const std::string&, const ConfigValue&)>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), required(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), required(req) {}
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
    
    // Load from file (supports JSON, YAML, and simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (always saves as JSON for better compatibility)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Type-safe getter with default value
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Type-safe configuration access
    template<typename T>
    T getRequired(const std::string& key, const T& defaultValue = T{}, 
                  const ConfigValidationCallback& validator = nullptr) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with description for better documentation
    void set(const std::string& key, const ConfigValue& value, const std::string& description,
             ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get configuration statistics
    std::string getStats() const;
    
    // Validate configuration
    bool validate(const ConfigValidationCallback& validator = nullptr) const;
    
    // Set validation callback
    void setValidator(const ConfigValidationCallback& validator);
    
    // Check if configuration is complete
    bool isComplete() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Enhanced error handling
    void logError(const std::string& operation, const std::string& key, 
                  const std::string& message) const;
    void logWarning(const std::string& operation, const std::string& key,
                    const std::string& message) const;
    
    // Type conversion utilities
    template<typename T>
    static std::optional<T> convertValue(const ConfigValue& value);
};

} // namespace nlm

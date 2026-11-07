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

// Configuration validation function
using ValidationFunction = std::function<bool(const ConfigValue&)>;

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
    
    // Get values with better type safety and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;

    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;

    template<typename T>
    T getOrWithValidation(const std::string& key, const T& defaultValue,
                          const ValidationFunction& validator) const;

    // Enhanced set methods with validation support
    template<typename T>
    void set(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with validation
    template<typename T>
    bool setWithValidation(const std::string& key, const T& value,
                          const ValidationFunction& validator,
                          ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Get validation function for a key
    ValidationFunction getValidator(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by pattern
    std::vector<std::string> getKeysMatching(const std::string& pattern) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get detailed configuration info
    std::vector<ConfigEntry> getAllEntries() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include "Errors.hpp"

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
    
    // Load from file (JSON format) - throws exceptions on error
    void loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values - throws exceptions on validation errors
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
    
    // Enhanced error handling methods (new API)
    Result loadFromFileSafe(const std::string& filepath);
    Result setSafe(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Schema and validation (new features)
    void initializeDefaultSchema();
    bool validateKey(const std::string& key) const;
    void validateType(const std::string& key, const ConfigValue& value) const;
    void validateTypeInternal(const Impl::SchemaEntry& schema, const ConfigValue& value) const;
    
    // JSON serialization helpers
    static ConfigValue jsonToValue(const nlohmann::json& j);
    static nlohmann::json valueToJson(const ConfigValue& value);
    
    // Type conversion helpers
    template<typename T>
    T valueToTyped(const ConfigValue& value) const;
    
    static std::string valueToString(const ConfigValue& value);
    static std::string getTypeString(const ConfigValue& value);
    static double getNumericValue(const ConfigValue& value);
    static std::string getStringValue(const ConfigValue& value);
    
    static std::string vectorToString(const std::vector<int>& vec);
    static std::string vectorToString(const std::vector<double>& vec);
    static std::string vectorToString(const std::vector<std::string>& vec);
};

} // namespace nlm

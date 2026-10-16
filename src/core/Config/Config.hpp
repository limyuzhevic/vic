#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

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

// JSON value types (for internal use)
using JsonValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<JsonValue>,
    std::map<std::string, JsonValue>
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
    
    // Load from file (supports JSON and legacy key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (tries JSON first, falls back to legacy format)
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
    
    // Configuration validation and error handling
    bool validate() const;
    bool validateParameter(const std::string& key, const ConfigValue& value);
    std::vector<std::pair<std::string, bool>> validateAllParameters() const;
    
    // Safe value getters with validation
    template<typename T>
    T getWithValidation(const std::string& key, const T& defaultValue) const;
    
    bool tryGet(const std::string& key, ConfigValue& outValue) const;
    ConfigValue getSafe(const std::string& key) const;
    
    // Error handling helper
    static std::string getErrorMessage(const std::string& operation, const std::string& details);
    
    // Helper functions for safe configuration parsing
    static bool parseConfigFileSafely(const std::string& filepath, Config& config);
    static bool parseArgsSafely(int argc, char** argv, Config& config);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Convert to JSON value
    void convertToJson(JsonValue& jsonValue) const;
    
    // Helpers for JSON conversion
    static void convertToJson(const ConfigEntry& entry, JsonValue& jsonValue);
    static void convertToJson(const ConfigValue& value, JsonValue& jsonValue);
    static std::string jsonValueToString(const JsonValue& value);
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <unordered_map>
#include <any>

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
    
    // Get last error message
    std::string getLastError() const;
    
    // Clear last error message
    void clearLastError() const;
    
    // Check if key is valid (no special characters, alphanumeric + underscores)
    static bool isValidKey(const std::string& key);
    
    // Check if value is valid for the key
    static bool isValidValue(const std::string& value);
    
    // Get current timestamp for file headers
    static std::string getCurrentTimestamp();
    
    // Validate configuration values
    bool validate(const std::string& key, const ConfigValue& value) const;
    
    // Get configuration errors
    std::vector<std::string> getValidationErrors() const;
    
    // Clear validation errors
    void clearValidationErrors() const;
    
    // Configuration merging
    void merge(const Config& other);
    
    // Clear cache flag
    void clearCache() const;
    
    // Get cached value
    template<typename T>
    std::optional<T> getCached(const std::string& key) const;
    
    // Set cached value
    template<typename T>
    void setCached(const std::string& key, const T& value) const;
    
    // Clear all cached values
    void clearCache() const;
    
    // Get cached value
    template<typename T>
    std::optional<T> getCached(const std::string& key) const;
    
    // Set cached value
    template<typename T>
    void setCached(const std::string& key, const T& value) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    bool isJsonContent(const std::string& content) const;
    bool isYamlContent(const std::string& content) const;
    bool loadFromJson(const std::string& content, const std::string& filepath);
    bool loadFromYaml(const std::string& content, const std::string& filepath);
    bool loadFromKeyValue(const std::string& content, const std::string& filepath);
    
    // JSON conversion helpers
    static nlohmann::json convertConfigValueToJSON(const ConfigValue& value);
    static ConfigValue convertJSONToConfigValue(const nlohmann::json& json);
    static void parseJSON(const nlohmann::json& json, ConfigSource source);
    static bool validateJSONStructure(const nlohmann::json& json) const;
};

} // namespace nlm

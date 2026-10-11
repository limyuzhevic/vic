#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>

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
    
    // Enhanced file loading with automatic format detection
    bool loadFromFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromKeyValue(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Enhanced file saving with automatic format detection
    bool saveToFile(const std::string& filepath) const;
    void saveToJSON(const std::string& filepath) const;
    void saveToYAML(const std::string& filepath) const;
    void saveToKeyValue(const std::string& filepath) const;
    
    // Internal stream-based methods
    void saveToJSON(std::ostream& os) const;
    void saveToYAML(std::ostream& os) const;
    void saveToKeyValue(std::ostream& os) const;
    
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Format detection and conversion
    static std::string getFileExtension(const std::string& filepath);
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLOFile(const std::string& filepath);
    
    // Helper for JSON/YAML parsing (to be implemented)
    std::string valueToString(const ConfigValue& value) const;
    ConfigValue stringToValue(const std::string& str) const;
    bool isNumber(const std::string& str, double& result) const;
    bool isInteger(const std::string& str, int64_t& result) const;
};

} // namespace nlm

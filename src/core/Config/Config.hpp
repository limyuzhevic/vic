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
    
    // Get configuration value or return default if key doesn't exist
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set configuration value with validation
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set configuration value with default source (simpler API)
    void setDefault(const std::string& key, const ConfigValue& value);
    template<typename T>
    void setDefault(const std::string& key, T value);
    
    // Get configuration value or return default if key doesn't exist (simpler API)
    template<typename T>
    T getOrDefault(const std::string& key, const T& defaultValue) const;
    
    // Configuration operations
    bool merge(const Config& other);
    std::vector<ConfigEntry> diff(const Config& other) const;
    bool isEquivalent(const Config& other) const;
    
    // Utility functions (internal)
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    bool validateConfigLine(const std::string& line) const;
    
    // Configuration validation
    bool validateKey(const std::string& key) const;
    bool validateValue(const ConfigValue& value) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::string getStats() const;
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const { return !(*this == other); }
    
    // Debug and development
    void setDebugMode(bool debug);
    bool isDebugMode() const;
    void logConfiguration() const;
    
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
};

} // namespace nlm

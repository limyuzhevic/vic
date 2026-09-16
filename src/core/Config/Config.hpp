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
    
    // Validate configuration parameters
    bool validateParameters() const;
    
    // Get the type of a configuration parameter as string
    std::string getParameterType(const std::string& key) const;
    
    // Set a configuration parameter with type checking
    template<typename T>
    void setParameterWithType(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set a configuration parameter with validation
    template<typename T>
    void setParameter(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Get parameter value with type validation
    template<typename T>
    std::optional<T> getValidated(const std::string& key) const;
    
    // Check if a configuration parameter is valid for a type
    bool isValidParameterType(const std::string& key, const std::type_info& typeInfo) const;
    
    // Get all configuration entries with detailed information
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Import configuration from another config (merge)
    void importFrom(const Config& other);
    
    // Get configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t entriesBySource[4]; // Default, File, CommandLine, Runtime
        std::unordered_map<std::string, size_t> typeCounts;
    };
    
    ConfigStats getStats() const;
    
    // Generate configuration report
    std::string generateReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

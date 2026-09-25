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
    
    // Load from file (JSON format preferred, fallback to simple key=value)
    bool loadFromFile(const std::string& filepath);
    
    // Load from file with explicit JSON parsing
    bool loadFromJSON(const std::string& jsonContent);
    
    // Enhanced configuration loading
    bool loadFromArgs(int argc, char** argv);
    bool setFromString(const std::string& key, const std::string& valueStr);
    
    // Validation and error handling
    bool validate() const;
    std::string validateErrors() const;
    
    // Configuration management
    void mergeFrom(const Config& other);
    void applyOverrides(const Config& overrides);
    
    // Debugging and information
    void printUsage() const;
    std::string getSourceSummary() const;
    
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
    
    // Validation and error checking
    bool validate() const;
    std::string validateErrors() const;
    
    // Configuration management
    void mergeFrom(const Config& other);
    void applyOverrides(const Config& overrides);
    
    // Debugging and information
    void printUsage() const;
    std::string getSourceSummary() const;
    
private:
    static int sourcePriority(ConfigSource source);
    static bool containsProblematicCharacters(const std::string& str);
    static std::string validateEntry(const ConfigEntry& entry);
};

} // namespace nlm

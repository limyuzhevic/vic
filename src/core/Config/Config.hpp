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
    
    // Enhanced file loading with format detection
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromSimpleFormat(const std::string& filepath);
    
    // File operations helper
    static std::string getFileExtension(const std::string& filepath);
    static bool isJSON(const std::string& filepath);
    static bool isYAML(const std::string& filepath);
    
    // Configuration validation
    bool validate() const;
    
    // Export configuration for other systems
    std::string exportToJSON() const;
    std::string exportToYAML() const;
    
    // Merge another configuration into this one (runtime overrides)
    void merge(const Config& other);
    
    // Compare two configurations and return differences
    std::vector<std::string> compare(const Config& other) const;
    
    // Get statistics about configuration keys (value types, sources)
    std::string getKeyStats() const;
    
    // Get validation errors if any
    std::vector<std::string> getValidationErrors() const;
    
    // Export configuration for a specific version/format
    std::string exportForVersion(const std::string& version, const std::string& format = "json") const;
    
    // Import configuration from a specific version/format
    bool importFromVersion(const std::string& data, const std::string& version = "latest", const std::string& format = "json");
    
    // Helper: Get config source for a key
    ConfigSource getConfigSource(const std::string& key) const;
    
    // Helper: Get timestamp
    std::string getTimestamp() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

// For accessing ConfigSource values in Python bindings
const char* configSourceToString(ConfigSource source);
ConfigSource stringToConfigSource(const std::string& sourceStr);

} // namespace nlm
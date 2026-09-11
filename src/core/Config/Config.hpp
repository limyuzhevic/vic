#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlohmann {
    template<typename T>
    class json;
}

namespace YAML {
    class Node;
}

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

// Statistics about configuration
struct ConfigStats {
    size_t totalEntries;
    size_t fromFile;
    size_t fromCommandLine;
    size_t fromRuntime;
    std::chrono::system_clock::time_point timestamp;
};

// Schema validation
struct ConfigSchema {
    std::string type;
    std::string description;
    bool required;
    std::variant<int, double> min;
    std::variant<int, double> max;
    std::vector<std::string> allowedValues;
    std::map<std::string, ConfigSchema> children;
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
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
    
    // Get statistics
    ConfigStats getStats() const;
    
    // Export/import
    bool exportToJSON(std::string& jsonString) const;
    bool importFromJSON(const std::string& jsonString);
    bool exportToYAML(std::string& yamlString) const;
    bool importFromYAML(const std::string& yamlString);
    
    // Merge with another config
    void merge(const Config& other);
    
    // Validation
    bool validate() const;
    bool validateAgainstSchema(const ConfigSchema& schema) const;
    
    // Convert to other formats
    nlohmann::json convertToJSON() const;
    std::string convertToYAML() const;
    
    // Debug information
    std::string debugInfo() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool validateConfigEntry(const ConfigEntry& entry);
    static void mergeConfigs(std::vector<ConfigEntry>& target, const std::vector<ConfigEntry>& source);
};

} // namespace nlm

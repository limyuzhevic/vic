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
    
    // Configuration schema definition for validation
    struct ConfigSchema {
        std::string key;
        std::string description;
        ConfigSource defaultSource;
        bool required;
        std::string type; // "int", "double", "bool", "string", "vector"
        std::vector<std::pair<std::string, double>> range; // min/max for numeric types
        std::vector<std::string> allowedValues; // for string types
        
        ConfigSchema() : key(), description(), defaultSource(ConfigSource::Default), 
                       required(false), type("string") {}
    };
    
    // Configuration validation result
    struct ValidationResult {
        bool valid;
        std::string errorMessage;
        std::vector<std::string> invalidKeys;
        
        ValidationResult() : valid(true), errorMessage(""), invalidKeys() {}
    };

    // Schema management
    void addSchema(const std::string& key, const std::string& description, 
                   ConfigSource source, bool required, const std::string& type,
                   const std::vector<std::pair<std::string, double>>& range = {},
                   const std::vector<std::string>& allowedValues = {});
    
    void removeSchema(const std::string& key);
    
    // Validation
    ValidationResult validate() const;
    
    // Schema inspection
    std::vector<ConfigSchema> getSchema() const;
    std::optional<ConfigSchema> getSchema(const std::string& key) const;
    
    // Get schema information as string
    std::string getSchemaSummary() const;
    
    // Merge configuration with another config (schema-compatible)
    void merge(const Config& other, bool override = false);
    
    // Environment variable integration
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Copy configuration with schema validation
    Config copy() const;
    
    // Get configuration statistics
    std::string getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

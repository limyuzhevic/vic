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
    
    // Load from file (JSON/YAML/simple format)
    bool loadFromFile(const std::string& filepath);
    
    // Internal file loading methods
    bool loadFromSimpleFile(const std::string& filepath);
    bool loadFromJSONFile(const std::string& filepath);
    bool loadFromYAMLFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (auto-detects format from extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Internal save methods
    bool saveToSimpleFile(std::ofstream& file) const;
    bool saveToJSONFile(std::ofstream& file) const;
    bool saveToYAMLFile(std::ofstream& file) const;
    
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
    
    // Print command line help
    void printHelp(const std::string& programName) const;
    
    // Configuration validation and schema support
    
    // Configuration validation rules
    struct ValidationRule {
        std::string key;
        std::string description;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
        
        ValidationRule(const std::string& k, const std::string& desc, 
                      std::function<bool(const ConfigValue&)> val, 
                      const std::string& err = "Invalid value")
            : key(k), description(desc), validator(val), errorMessage(err) {}
    };
    
    // Configuration schema definition
    struct ConfigurationSchema {
        std::vector<ValidationRule> rules;
        std::map<std::string, std::string> descriptions;
        
        void addRule(const ValidationRule& rule);
        bool validate(const Config& config, std::vector<std::string>& errors) const;
        std::string getDescription(const std::string& key) const;
    };
    
    // Validate current configuration
    bool validateConfiguration(std::vector<std::string>& errors) const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Check if configuration has warnings (non-critical issues)
    std::vector<std::string> getConfigurationWarnings() const;
    
    // Set up validation schema
    static ConfigurationSchema createDefaultSchema();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

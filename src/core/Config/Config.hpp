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
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get configuration entry (with metadata)
    const ConfigEntry* getEntry(const std::string& key) const;
    
    // Validate configuration against schema
    bool validate(const std::string& schema) const;
    
    // Set default values
    void setDefaults();
    
    // Get source of configuration value
    ConfigSource getSource(const std::string& key) const;
    
    // Check if key exists and has valid value
    bool hasValid(const std::string& key) const;
    
    // Get all keys with source information
    std::vector<std::pair<std::string, ConfigSource>> getKeySources() const;
    
    // Clear configuration by source
    void clearBySource(ConfigSource source);
    
    // Merge configurations
    void merge(const Config& other);
    
    // Configuration schema validation
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string description;
    };
    
    void addValidationRule(const ValidationRule& rule);
    
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration statistics
    struct Stats {
        size_t totalEntries;
        size_t defaultEntries;
        size_t fileEntries;
        size_t commandLineEntries;
        size_t runtimeEntries;
        double entropy;  // Measure of configuration uncertainty
    };
    
    Stats getStats() const;
    
    // Get configuration hash for change detection
    std::string getHash() const;
    
    // Compare configurations
    bool equals(const Config& other) const;
    
    // Export configuration to different formats
    std::string exportToYAML() const;
    std::string exportToJSON() const;
    std::string exportToINI() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

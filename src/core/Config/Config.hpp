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
    
    // Load from file (JSON, YAML, or simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from simple key=value format (Phase 1)
    bool loadFromSimpleFormat(const std::string& filepath);
    
    // Save to file (JSON or YAML format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON file
    bool saveToJSON(const std::string& filepath) const;
    
    // Save to YAML file
    bool saveToYAML(const std::string& filepath) const;
    
    // Load from dictionary
    bool loadFromDict(const py::dict& config_dict);
    
    // Save to dictionary
    py::dict saveToDict() const;
    
    // Merge with another configuration
    bool merge(const Config& other);
    
    // Get configuration statistics
    py::dict getStats() const;
    
    // Validate configuration
    bool validate() const;
    
    // Get configuration template
    std::shared_ptr<Config> getTemplate(const std::string& template_name) const;
    
    // Apply configuration template
    bool applyTemplate(const std::string& template_name);
    
    // Get configuration documentation
    std::string getDocumentation() const;
    
    // Set configuration with validation
    void setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void setValidated(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setValidated(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setValidated(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setValidated(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Validate configuration key
    bool isValidKey(const std::string& key) const;
    
    // Get configuration type
    std::string getType(const std::string& key) const;
    
    // Check if configuration value is within specified range
    bool isInRange(const std::string& key, double min_val, double max_val) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

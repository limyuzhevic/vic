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
    
    // Load from YAML file (Phase 3)
    bool loadFromYAMLFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlString);
    
    // Export to JSON string
    std::string exportToJSON() const;
    
    // Export to YAML string
    std::string exportToYAML() const;
    
    // Merge with another configuration
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Validate configuration
    std::vector<std::string> validate() const;
    
    // Get configuration by section
    std::vector<std::string> getKeysBySection(const std::string& section) const;
    
    // Get all unique sections
    std::vector<std::string> getSections() const;
    
    // Set description for key
    void setDescription(const std::string& key, const std::string& description);
    
    // Get description for key
    std::string getDescription(const std::string& key) const;
    
    // Copy configuration
    Config copy() const;
    
    // Configuration validation rules
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    
    // Add validation rule
    void addValidationRule(const ValidationRule& rule);
    
    // Clear validation rules
    void clearValidationRules();
    
    // Enable/disable strict validation mode
    void setStrictValidation(bool enabled);
    
    // Get strict validation mode
    bool isStrictValidation() const;
    
    // Get validation errors from last validation
    std::vector<std::string> getLastValidationErrors() const;
    
    // Get configuration size
    size_t size() const;
    
    // Check if configuration is empty
    bool empty() const;
    
    // Get configuration age in milliseconds
    uint64_t getAge() const;
    
    // Refresh configuration timestamp
    void refresh();
    
    // Configuration statistics
    struct ConfigStats {
        size_t totalKeys;
        size_t keysBySource[4]; // Default, File, CommandLine, Runtime
        uint64_t ageMs;
        std::string lastModified;
        bool isValid;
        std::vector<std::string> validationErrors;
    };
    
    // Get configuration statistics
    ConfigStats getStats() const;
    
    // Advanced CLI parsing with subcommands
    bool loadFromAdvancedArgs(int argc, char** argv);
    
    // Configuration reset with source
    void resetWithSource(ConfigSource source);
    
    // Get configuration keys sorted by source priority
    std::vector<std::pair<std::string, ConfigSource>> getKeysWithSource() const;
    
    // Configuration diff between two configs
    std::vector<std::pair<std::string, std::pair<ConfigValue, ConfigValue>>> diff(const Config& other) const;
    
    // Apply delta configuration
    void applyDelta(const std::vector<std::pair<std::string, ConfigValue>>& delta);
    
    // Get configuration JSON schema (for validation)
    static std::string getDefaultJSONSchema();
    
    // Validate against JSON schema
    bool validateAgainstSchema(const std::string& schema) const;

} // namespace nlm

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

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRegex;
    bool isRequired;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), 
                   validationRegex(), isRequired(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", const std::string& regex = "", 
                bool required = false)
        : key(k), value(v), source(s), description(desc), 
          validationRegex(regex), isRequired(required) {}
};

// Configuration metadata for better organization
struct ConfigMetadata {
    std::string category;
    std::string subcategory;
    std::string type;
    std::string description;
    std::string defaultValue;
    std::string units;
    std::string[] validValues;  // NULL-terminated array
    bool isRequired;
    
    ConfigMetadata() : category("General"), subcategory(""), type(""), description(), 
                      defaultValue(), units(), validValues(nullptr), isRequired(false) {}
};

// Configuration validation result
struct ConfigValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ConfigValidationResult() : isValid(true) {}
};

// Forward declaration for friend functions
class ConfigFactory;
class ConfigValidator;

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
    
    // Load from file (supports both JSON and simple key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string content
    bool loadFromJSON(const std::string& jsonContent);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJSON() const;
    
    // Get values with type conversion and validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> get(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced type-specific setters
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Type-specific getters with validation
    std::optional<int> getInt(const std::string& key) const;
    std::optional<double> getDouble(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    
    // Check existence and validation
    bool has(const std::string& key) const;
    bool isValidType(const std::string& key, ConfigValueType expectedType) const;
    
    // Remove key and clean up
    void remove(const std::string& key);
    
    // Get all keys and metadata
    std::vector<std::string> getKeys() const;
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all configuration
    void clear();
    
    // Configuration management
    bool merge(const Config& other, bool override = true);
    Config copy() const;
    
    // Get configuration summary with more details
    std::string summary() const;
    std::string detailedSummary() const;
    
    // Configuration validation and statistics
    bool validate() const;
    std::string getValidationErrors() const;
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    
    // Advanced configuration operations
    void setDefaults();
    void applyOverrides(const Config& overrides);
    
    // Batch operations
    void setBatch(const std::vector<std::pair<std::string, ConfigValue>>& settings, ConfigSource source = ConfigSource::Runtime);
    std::vector<std::pair<std::string, ConfigValue>> getBatch(const std::vector<std::string>& keys) const;
    
    // Statistics and debugging
    std::string getStatistics() const;
    double getFloat(const std::string& key, double defaultValue = 0.0) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

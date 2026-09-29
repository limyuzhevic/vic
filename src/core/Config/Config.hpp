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
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate configuration against rules
    bool validate(const std::vector<std::string>& requiredKeys = {}) const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const;
    
    // Merge with another configuration (runtime overrides)
    void merge(const Config& other);
    
    // Remove keys from file and command line sources
    void resetToRuntime();
    
    // Count entries by source
    std::unordered_map<ConfigSource, size_t> getSourceCounts() const;
    
    // Check if configuration would change
    bool hasChanges() const;
    
    // Clear source entries (keep runtime)
    void clearSource(ConfigSource source);
    
    // Set validation callback
    void setValidationCallback(std::function<bool(const std::string&, const ConfigValue&)> callback);
    
    // Get validation statistics
    std::string getValidationStats() const;
    
    // Export configuration as string with formatting
    std::string exportFormatted(bool includeComments = true) const;
    
    // Import configuration from formatted string
    bool importFromFormatted(const std::string& formattedConfig);
    
    // Get keys by prefix
    std::vector<std::string> getKeysWithPrefix(const std::string& prefix) const;
    
    // Replace values
    void replace(const std::string& key, const ConfigValue& newValue);
    
    // Update values with validation
    bool updateWithValidation(const std::string& key, const ConfigValue& value);
    
    // Get configuration version (for migration)
    std::string getVersion() const;
    
    // Set configuration version
    void setVersion(const std::string& version);
    
    // Check if value is within allowed range (for numeric types)
    bool isValueValid(const std::string& key, const ConfigValue& value) const;
    
    // Get validation rules
    std::vector<std::pair<std::string, std::string>> getValidationRules() const;
    
    // Add validation rule
    void addValidationRule(const std::string& key, const std::string& rule, const std::string& errorMessage);
    
    // Clear all validation rules
    void clearValidationRules();
    
    // Validate specific key
    bool validateKey(const std::string& key) const;
    
    // Get validation error for specific key
    std::optional<std::string> getValidationError(const std::string& key) const;
    
    // Check if configuration is complete (all required keys present)
    bool isComplete(const std::vector<std::string>& requiredKeys) const;
    
    // Get missing required keys
    std::vector<std::string> getMissingKeys(const std::vector<std::string>& requiredKeys) const;
    
    // Deep copy
    std::unique_ptr<Config> clone() const;
    
    // Hash for comparison
    size_t hash() const;
    
    // Check equality
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

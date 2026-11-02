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

// Validation result structure
struct ValidationResult {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> validatedValues;
    
    ValidationResult() : valid(true) {}
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Validation Result:\n";
        oss << "  Valid: " << (valid ? "Yes" : "No") << "\n";
        if (!errors.empty()) {
            oss << "  Errors (" << errors.size() << "):\n";
            for (const auto& error : errors) {
                oss << "    - " << error << "\n";
            }
        }
        if (!warnings.empty()) {
            oss << "  Warnings (" << warnings.size() << "):\n";
            for (const auto& warning : warnings) {
                oss << "    - " << warning << "\n";
            }
        }
        oss << "  Validated values: " << validatedValues.size();
        return oss.str();
    }
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
    
    // Comprehensive validation
    ValidationResult validate() const;
    
    // Validate specific key
    std::vector<std::string> validateKey(const std::string& key) const;
    
    // Get configuration statistics
    std::map<std::string, int64_t> getStatistics() const;
    
    // Check if configuration is complete (has all required keys)
    bool isComplete(const std::vector<std::string>& requiredKeys) const;
    
    // Normalize configuration (ensure all values are within valid ranges)
    void normalize();
    
    // Merge with another configuration
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Get configuration schema (available keys and their types)
    std::map<std::string, std::string> getSchema() const;
    
    // Check if configuration is consistent (no contradictory values)
    bool isConsistent() const;
    
    // Validate specific constraints
    bool validateConstraint(const std::string& key, const std::string& constraint) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

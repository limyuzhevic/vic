#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <sstream>

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
    Runtime,
    Environment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string validationRule;
    std::string errorMessage;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validationRule(), errorMessage() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validationRule(), errorMessage() {}
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::string errorMessage;
    
    ValidationResult() : isValid(true), errorMessage() {}
    ValidationResult(bool valid, const std::string& error) : isValid(valid), errorMessage(error) {}
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
    
    // Load from file (JSON format with fallback to key=value)
    bool loadFromFile(const std::string& filepath, bool requireFile = false);
    
    // Load from JSON file with proper error handling
    bool loadJsonFromFile(const std::string& filepath);
    
    // Load from key=value format (backward compatibility)
    bool loadKeyValueFromFile(const std::string& filepath);
    
    // Load from command line arguments with enhanced support
    bool loadFromArgs(int argc, char** argv);
    
    // Load from environment variables
    bool loadFromEnv(const std::string& prefix = "NLM_");
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation support
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime, 
             const std::string& description = "", const std::string& validationRule = "");
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& validationRule = "");
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& validationRule = "");
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& validationRule = "");
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& validationRule = "");
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime,
             const std::string& description = "", const std::string& validationRule = "");
    
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
    
    // Get configuration file path hints for CLI
    static std::vector<std::string> getConfigFileHints();
    
    // Parse command line flags
    static bool parseHelpFlag(int argc, char** argv);
    static bool parseVersionFlag(int argc, char** argv);
    static bool parseConfigFlag(int argc, char** argv, std::string& configFile);
    
    // Validation
    ValidationResult validate() const;
    bool validateKey(const std::string& key) const;
    
    // Merge another config (command line overrides)
    void merge(const Config& other);
    
    // Get all entries
    const std::vector<ConfigEntry>& getEntries() const;
    
    // Set default values for a section
    void setDefaults(const std::string& sectionPrefix);
    
    // Get source for a key
    ConfigSource getSource(const std::string& key) const;
    
    // Check if key has been modified from defaults
    bool isOverridden(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string extractExtension(const std::string& filepath);
    static std::string findConfigFile(const std::string& hint);
    
    // Validation helpers
    static ValidationResult validateInt(const ConfigEntry& entry);
    static ValidationResult validateDouble(const ConfigEntry& entry);
    static ValidationResult validateString(const ConfigEntry& entry);
    static ValidationResult validateBool(const ConfigEntry& entry);
    static ValidationResult validateVector(const ConfigEntry& entry);
};

} // namespace nlm

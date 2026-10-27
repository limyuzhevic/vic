#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>
#include <set>

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

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : isValid(true) {}
    bool addError(const std::string& error) {
        errors.push_back(error);
        isValid = false;
        return false;
    }
    bool addWarning(const std::string& warning) {
        warnings.push_back(warning);
        return true;
    }
};

// Configuration constraint for validation
struct ConfigConstraint {
    std::string key;
    std::function<ValidationResult(const ConfigValue&)> validator;
    std::string description;
    
    ConfigConstraint(const std::string& k, 
                     const std::function<ValidationResult(const ConfigValue&)>& v,
                     const std::string& desc)
        : key(k), validator(v), description(desc) {}
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                    description(), timestamp(std::chrono::system_clock::now()) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          timestamp(std::chrono::system_clock::now()) {}
};

// Main configuration class with validation and JSON support
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
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Save to file (YAML format)
    bool saveToFile(const std::string& filepath) const;
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get metadata about configuration
    bool has(const std::string& key) const;
    bool isSet(const std::string& key, ConfigSource minSource = ConfigSource::Runtime) const;
    std::string getDescription(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    std::chrono::system_clock::time_point getTimestamp(const std::string& key) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation
    ValidationResult validate() const;
    ValidationResult validateKey(const std::string& key) const;
    
    // Add constraints for validation
    void addConstraint(const std::string& key,
                       const std::function<ValidationResult(const ConfigValue&)>& validator,
                       const std::string& description);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get default configuration
    static std::shared_ptr<Config> createDefaultConfig();
    
    // Environment variable support
    void loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Configuration schema support
    void setSchema(const std::string& key, const std::string& schema);
    std::string getSchema(const std::string& key) const;
    
    // Merge with another config (other takes precedence)
    void merge(const Config& other);
    
    // Configuration evolution (for backward compatibility)
    void applyEvolution(const std::string& oldKey, const std::string& newKey,
                        const std::function<ConfigValue(const ConfigValue&)>& converter);
    
    // Generate random configuration for testing
    static void generateRandomConfig(std::shared_ptr<Config> config, uint64_t seed = 42);
    
    // Configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t fromDefault;
        size_t fromFile;
        size_t fromCommandLine;
        size_t fromRuntime;
        double memoryUsage;
        
        ConfigStats() : totalEntries(0), fromDefault(0), fromFile(0),
                       fromCommandLine(0), fromRuntime(0), memoryUsage(0.0) {}
    };
    
    ConfigStats getStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseYAMLValue(const std::string& value, ConfigValue& result);
    static std::string serializeToYAML(const ConfigValue& value) const;
    static ValidationResult validateType(const std::string& key, const ConfigValue& value);
    static std::string generateRandomString(size_t length);
    
    // Constraint validation
    static ValidationResult validatePositive(const ConfigValue& value);
    static ValidationResult validateNonNegative(const ConfigValue& value);
    static ValidationResult validateRange(const ConfigValue& value, double min, double max);
    static ValidationResult validateNeuronCount(const ConfigValue& value);
    static ValidationResult validateConnectionProbability(const ConfigValue& value);
};
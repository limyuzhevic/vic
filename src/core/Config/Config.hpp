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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& error, const std::vector<std::string>& warnings = {}) 
        : valid(v), errorMessage(error), warnings(warnings) {}
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
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Safe set with validation
    template<typename T>
    void setValidated(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime, 
                     const std::string& description = "");
    
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
    
    // Validation methods
    ValidationResult validate() const;
    ValidationResult validateKey(const std::string& key) const;
    
    // Type-safe getters with validation
    int getInt(const std::string& key, int defaultValue = 0) const;
    double getDouble(const std::string& key, double defaultValue = 0.0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    
    // Special configuration getters
    int getRandomSeed() const;
    double getSimulationTimestep() const;
    size_t getNeuronCount() const;
    size_t getRegionCount() const;
    float getConnectionProbability() const;
    float getSTDP LTPWeight() const;
    float getSTDP LTDWeight() const;
    float getSTPDA tau() const;
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    std::string getCheckpointDir() const;
    
    // Configuration schema validation
    static bool isValidKey(const std::string& key);
    static bool isValidValueType(const std::string& key, const ConfigValue& value);
    static std::optional<ConfigValue> getDefaultValue(const std::string& key);
    
    // Error handling
    static std::string getErrorMessage(int errorCode);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    static bool serializeJSON(const std::vector<ConfigEntry>& entries, const std::string& filepath);
};

} // namespace nlm

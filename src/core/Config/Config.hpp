#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <chrono>
#include <ctime>

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

// Configuration validation error
struct ValidationError {
    std::string key;
    std::string message;
    
    ValidationError() : key(), message() {}
    ValidationError(const std::string& k, const std::string& m) : key(k), message(m) {}
};

// Validation rules for configuration parameters
struct ValidationRules {
    std::string key;
    std::string type; // "int", "double", "string", "bool", "vector"
    std::optional<double> minValue;      // For numeric types
    std::optional<double> maxValue;      // For numeric types
    std::optional<size_t> minLength;      // For strings and vectors
    std::optional<size_t> maxLength;      // For strings and vectors
    std::string description;
    bool required;
    
    ValidationRules() : key(), type(), required(false), description() {}
    ValidationRules(const std::string& k, const std::string& t, bool req = false, 
                   const std::string& desc = "") : key(k), type(t), required(req), description(desc) {}
};

// Configuration schema
struct ConfigSchema {
    std::vector<ValidationRules> rules;
    std::vector<std::string> requiredSections;
    std::string filename;
    
    ConfigSchema() : filename() {}
    ConfigSchema(const std::string& fname) : filename(fname) {}
    
    void addRule(const std::string& key, const std::string& type, bool required = false, 
                const std::string& description = "", double min = 0, double max = 0,
                size_t minLen = 0, size_t maxLen = 0);
    
    std::vector<ValidationError> validate() const;
    bool loadSchemaFromFile(const std::string& schemaFilepath);
};

// Advanced error reporting
struct ConfigErrors {
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    std::vector<ValidationError> validationErrors;
    
    bool hasErrors() const { return !errors.empty() || !validationErrors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
    void clear() { warnings.clear(); errors.clear(); validationErrors.clear(); }
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
    
    // Load with error recovery
    bool loadFromFileWithRecovery(const std::string& filepath, ConfigErrors& errors);
    
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
    
    // Validate configuration
    bool validate(ConfigErrors& errors, const ConfigSchema& schema = ConfigSchema()) const;
    
    // Get validation errors for a specific key
    std::vector<ValidationError> getValidationErrors(const std::string& key) const;
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration snapshots for rollback
    struct ConfigSnapshot {
        std::vector<ConfigEntry> entries;
        std::string timestamp;
        
        ConfigSnapshot() {}
        ConfigSnapshot(const std::vector<ConfigEntry>& e, const std::string& t) : entries(e), timestamp(t) {}
    };
    
    ConfigSnapshot createSnapshot() const;
    bool restoreSnapshot(const ConfigSnapshot& snapshot);
    
    // Configuration comparison
    bool operator==(const Config& other) const;
    bool operator!=(const Config& other) const { return !(*this == other); }
    
    // Configuration statistics
    struct ConfigStats {
        size_t totalEntries;
        size_t entriesBySource[4]; // Default, File, CommandLine, Runtime
        std::string oldestEntry;
        std::string newestEntry;
        
        ConfigStats() : totalEntries(0) {
            for (int i = 0; i < 4; ++i) entriesBySource[i] = 0;
        }
    };
    
    ConfigStats getStats() const;
    
    // Configuration history
    void addToHistory(const std::string& action);
    std::vector<std::string> getHistory() const;
    void clearHistory();
    
    // Get configuration summary
    std::string summary() const;
    
    // Show configuration with errors highlighted
    void showConfigWithErrors(std::ostream& out, const ConfigErrors& errors) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<std::string> history;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getCurrentTimestamp();
    
    // Validation helpers
    template<typename T>
    bool validateValue(const ConfigEntry& entry, ConfigErrors& errors) const;
    
    // Error handling helpers
    void reportError(ConfigErrors& errors, const std::string& error) const;
    void reportWarning(ConfigErrors& errors, const std::string& warning) const;
};

} // namespace nlm

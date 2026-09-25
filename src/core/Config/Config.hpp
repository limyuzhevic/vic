#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace nlm {

// Forward declarations
class Config;
class ConfigValidator;

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
    Preset,
    Template,
    Inherited
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
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::vector<std::string>& errs) : valid(v), errors(errs) {}
    ValidationResult(bool v, const std::vector<std::string>& errs, const std::vector<std::string>& warns) 
        : valid(v), errors(errs), warnings(warns) {}
    
    void addError(const std::string& error) { errors.push_back(error); valid = false; }
    void addWarning(const std::string& warning) { warnings.push_back(warning); }
    std::string getErrorSummary() const;
    bool hasErrors() const { return !errors.empty(); }
    bool hasWarnings() const { return !warnings.empty(); }
};

// Configuration schema element
struct SchemaElement {
    std::string type; // "int", "double", "bool", "string", "vector<int>", etc.
    std::optional<int> minInt;
    std::optional<int> maxInt;
    std::optional<double> minDouble;
    std::optional<double> maxDouble;
    std::optional<std::vector<std::string>> enumValues;
    bool required = false;
    std::string defaultValue;
    std::string description;
    
    bool validate(const ConfigValue& value) const;
    std::string validateAndGetError(const ConfigValue& value) const;
    bool isNumericType() const;
    bool isEnumType() const;
    bool isVectorType() const;
};

// Configuration preset
struct Preset {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    ConfigSource source = ConfigSource::Preset;
    
    Preset() {}
    Preset(const std::string& n, const std::string& desc) : name(n), description(desc) {}
    void add(const std::string& key, const ConfigValue& value) { values[key] = value; }
    bool has(const std::string& key) const { return values.find(key) != values.end(); }
    const ConfigValue& get(const std::string& key) const { return values.at(key); }
};

// Configuration diff entry
struct ConfigDiff {
    std::string key;
    std::optional<ConfigValue> oldValue;
    std::optional<ConfigValue> newValue;
    ConfigSource source;
    
    ConfigDiff(const std::string& k, const std::optional<ConfigValue>& oldV, 
               const std::optional<ConfigValue>& newV, ConfigSource s)
        : key(k), oldValue(oldV), newValue(newV), source(s) {}
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
    
    // === Core Configuration Methods ===
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from dictionary
    void loadFromDict(const std::map<std::string, ConfigValue>& config);
    
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
    
    // === Validation and Schema ===
    
    // Validate configuration against schema
    ValidationResult validateSchema(const std::map<std::string, SchemaElement>& schema) const;
    
    // Validate a single key-value pair
    ValidationResult validateKey(const std::string& key, const SchemaElement& schema) const;
    
    // Check if key matches schema type
    static bool matchesSchemaType(const std::string& type, const ConfigValue& value);
    
    // Get schema for built-in validation
    static std::map<std::string, SchemaElement> getBuiltInSchema(const std::string& presetName = "basic");
    
    // === Preset Management ===
    
    // Apply a preset configuration
    bool applyPreset(const Preset& preset);
    
    // Get built-in presets
    static std::map<std::string, Preset> getBuiltInPresets();
    
    // Create custom preset
    static Preset createPreset(const std::string& name, const std::string& description, 
                               const std::map<std::string, ConfigValue>& values);
    
    // === Configuration Comparison ===
    
    // Compare with another configuration
    std::vector<ConfigDiff> compare(const Config& other) const;
    
    // Get difference count
    size_t getDiffCount(const Config& other) const;
    
    // Check if configurations are equal
    bool equals(const Config& other) const;
    
    // === Configuration Diffing and Comparison ===
    
    // Get configuration diff summary
    std::string getDiffSummary(const Config& other) const;
    
    // Apply configuration diff
    void applyDiff(const std::vector<ConfigDiff>& diff);
    
    // === Configuration Optimization ===
    
    // Optimize configuration based on constraints
    void optimize(const std::map<std::string, SchemaElement>& constraints);
    
    // Remove unused default values
    void removeUnusedDefaults(const std::map<std::string, ConfigValue>& defaults);
    
    // === Configuration Versioning ===
    
    // Set configuration version
    void setVersion(const std::string& version);
    
    // Get configuration version
    std::string getVersion() const;
    
    // Create backup
    bool createBackup(const std::string& filepath) const;
    
    // Restore from backup
    bool restoreFromBackup(const std::string& filepath);
    
    // === Configuration Statistics ===
    
    // Get configuration statistics
    std::map<std::string, size_t> getStatistics() const;
    
    // Get keys by source
    std::map<ConfigSource, std::vector<std::string>> getKeysBySource() const;
    
    // Get configuration entropy (usefulness measure)
    double getEntropy() const;
    
    // === Utility Methods ===
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all keys by source
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration with source information
    std::string getDetailedSummary() const;
    
    // Export configuration as dictionary
    std::map<std::string, std::string> exportAsDict() const;
    
    // Import configuration from dictionary
    void importFromDict(const std::map<std::string, std::string>& config);
    
    // Merge with another configuration (overwrites with higher priority)
    void merge(const Config& other, ConfigSource priority = ConfigSource::Runtime);
    
    // Filter configuration by prefix
    std::map<std::string, ConfigValue> filterByPrefix(const std::string& prefix) const;
    
    // Get configuration keys with given prefix
    std::vector<std::string> getKeysWithPrefix(const std::string& prefix) const;
    
    // Validate all entries against schema
    ValidationResult validateAll(const std::map<std::string, SchemaElement>& schema) const;
    
    // Batch operations
    void batchSet(const std::vector<std::pair<std::string, ConfigValue>>& values, ConfigSource source = ConfigSource::Runtime);
    
    // === Internal helpers ===
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toString(const ConfigValue& value);
    static ConfigValue fromString(const std::string& value, const std::string& type);
    
    // Serialization helpers
    static std::string serializeValue(const ConfigValue& value);
    static bool deserializeValue(const std::string& str, ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

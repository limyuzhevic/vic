#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <map>
#include <stdexcept>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types with enhanced support
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    float,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<uint64_t>,
    std::vector<float>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::vector<std::vector<float>>  // For matrix/2D array support
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    SchemaDefault
};

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : std::runtime_error(message), key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

// Configuration validation rule
struct ConfigValidationRule {
    std::string key;
    std::string type;  // "int", "float", "string", "bool", "array"
    std::vector<std::string> allowedValues;
    bool required = false;
    double minValue = std::numeric_limits<double>::lowest();
    double maxValue = std::numeric_limits<double>::max();
    size_t minSize = 0;
    size_t maxSize = SIZE_MAX;
    std::string description;
    
    bool validate(const ConfigValue& value) const;
    std::string getErrorMessage(const std::string& valueStr) const;
};

// Enhanced configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    mutable bool validated = false;  // For lazy validation
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), validated(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), validated(false) {}
    
    bool isValid(const std::vector<ConfigValidationRule>& rules) const;
};

// Schema definition for configuration validation
struct ConfigSchema {
    std::vector<ConfigValidationRule> rules;
    std::string filename;
    
    bool validate(const Config& config) const;
    std::string getErrors(const Config& config) const;
    
    // Common predefined schemas
    static ConfigSchema createDefaultSchema();
    static ConfigSchema createNeuralNetworkSchema();
    static ConfigSchema createSimulationSchema();
    static ConfigSchema createDevelopmentSchema();
};

// File format support for configuration
enum class ConfigFormat {
    Auto,     // Detect from file extension
    INI,      // Standard INI format
    JSON,     // JSON format
    YAML,     // YAML format
    TOML      // TOML format
};

// Advanced configuration parser with schema validation
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file with format detection and schema validation
    bool loadFromFile(const std::string& filepath, ConfigFormat format = ConfigFormat::Auto,
                     const ConfigSchema& schema = ConfigSchema::createDefaultSchema());
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr, const ConfigSchema& schema = ConfigSchema::createDefaultSchema());
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr, const ConfigSchema& schema = ConfigSchema::createDefaultSchema());
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv, const ConfigSchema& schema = ConfigSchema::createDefaultSchema());
    
    // Save to file with format support
    bool saveToFile(const std::string& filepath, ConfigFormat format = ConfigFormat::INI) const;
    
    // Save to JSON string
    std::string toJSON() const;
    
    // Save to YAML string
    std::string toYAML() const;
    
    // Get values with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation support
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, uint64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced validation
    bool validate(const ConfigSchema& schema) const;
    std::string getValidationErrors(const ConfigSchema& schema) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with source information
    std::string summary() const;
    
    // Get configuration by source
    std::map<ConfigSource, std::vector<ConfigEntry>> getBySource() const;
    
    // Configuration merge operations
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Default value management
    void setDefault(const std::string& key, const ConfigValue& value, const std::string& description = "");
    
    // Schema validation for specific key
    bool validateKey(const std::string& key, const ConfigSchema& schema) const;
    
    // Get value type as string for debugging
    std::string getValueType(const std::string& key) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    
    // Backup and restore
    bool backup(const std::string& filepath) const;
    bool restore(const std::string& filepath);
    
    // Version control for configuration
    void setVersion(const std::string& version);
    std::string getVersion() const;
    
    // Comment support
    void addComment(const std::string& key, const std::string& comment);
    std::string getComment(const std::string& key) const;
    
    // Validation status tracking
    bool isValid(const ConfigSchema& schema) const { return validate(schema); }
    
    // Deep copy
    std::unique_ptr<Config> clone() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string detectFormat(const std::string& filepath);
    static std::unique_ptr<Config> loadFromString(const std::string& content, ConfigFormat format,
                                                 const ConfigSchema& schema);
    
    // Schema validation helpers
    static bool validateValueAgainstRule(const ConfigValue& value, const ConfigValidationRule& rule);
    static std::string valueToString(const ConfigValue& value);
};

} // namespace nlm

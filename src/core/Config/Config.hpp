#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <functional>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;
class JsonDocument;
class YamlNode;

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

// Configuration validation result
struct ValidationResult {
    bool valid;
    std::string errorMessage;
    std::vector<std::string> warnings;
    
    ValidationResult() : valid(true) {}
    ValidationResult(bool v, const std::string& error) : valid(v), errorMessage(error) {}
};

// Type validator function
using TypeValidator = std::function<ValidationResult(const std::string&, const ConfigValue&)>;

// Range validator for numeric types
struct RangeValidator {
    double minValue;
    double maxValue;
    std::string description;
    
    RangeValidator(double min, double max, const std::string& desc = "")
        : minValue(min), maxValue(max), description(desc) {}
    
    ValidationResult validate(const std::string& key, const ConfigValue& value) const;
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string category;
    bool required;
    bool readonly;
    std::chrono::system_clock::time_point timestamp;
    
    // Validation metadata
    TypeValidator typeValidator;
    RangeValidator rangeValidator;
    std::vector<std::string> allowedValues;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description(), category("general"), 
          required(false), readonly(false), timestamp(std::chrono::system_clock::now()) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc), category("general"), 
          required(false), readonly(false), timestamp(std::chrono::system_clock::now()) {}
};

// Configuration schema for validation
struct ConfigSchema {
    std::map<std::string, ConfigEntry> schema;
    std::string filename;
    
    ConfigSchema() : filename("") {}
    
    // Add validation rule for a specific key
    void addTypeValidator(const std::string& key, TypeValidator validator);
    
    // Add range validator for numeric values
    void addRangeValidator(const std::string& key, double min, double max, const std::string& desc = "");
    
    // Add allowed values list
    void addAllowedValues(const std::string& key, const std::vector<std::string>& values);
    
    // Validate configuration against schema
    ValidationResult validate(const Config& config) const;
    
    // Load schema from JSON file
    static ConfigSchema loadFromJson(const std::string& filename);
    
    // Save schema to JSON file
    bool saveToJson(const std::string& filename) const;
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from various sources
    bool loadFromFile(const std::string& filepath);
    bool loadFromJson(const std::string& content);
    bool loadFromYaml(const std::string& content);
    bool loadFromArgs(int argc, char** argv);
    bool loadFromEnvironment(const std::map<std::string, std::string>& envVars);
    
    // Save to various formats
    bool saveToFile(const std::string& filepath) const;
    bool saveToJson(const std::string& filepath) const;
    bool saveToYaml(const std::string& filepath) const;
    
    // Get values with type safety
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
    
    // Set with validation
    bool setValidated(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence and type
    bool has(const std::string& key) const;
    bool isType(const std::string& key, const std::string& typeName) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys by category
    std::vector<std::string> getKeysByCategory(const std::string& category) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validation methods
    ValidationResult validate() const;
    ValidationResult validateAgainstSchema(const ConfigSchema& schema) const;
    
    // Schema management
    void setSchema(const ConfigSchema& schema);
    const ConfigSchema& getSchema() const;
    
    // Configuration profiles
    bool saveProfile(const std::string& profileName);
    bool loadProfile(const std::string& profileName);
    std::vector<std::string> getProfiles() const;
    
    // Merge configurations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Environment variable integration
    static Config loadFromEnvironmentPrefix(const std::string& prefix = "NLM_");
    
    // Configuration statistics
    size_t getEntryCount() const;
    ConfigSource getSource(const std::string& key) const;
    std::chrono::system_clock::time_point getTimestamp(const std::string& key) const;
    
    // JSON/YAML helpers
    static std::string toJson(const Config& config);
    static std::string toYaml(const Config& config);
    
    // Configuration versioning
    void setVersion(const std::string& version);
    std::string getVersion() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string extractFileExtension(const std::string& filepath);
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    
    // File parsing helpers
    bool loadJsonFile(const std::string& filepath);
    bool loadYamlFile(const std::string& filepath);
    bool loadSimpleFile(const std::string& filepath);
    
    // Validation helpers
    ValidationResult validateValueType(const ConfigEntry& entry) const;
    ValidationResult validateValueRange(const ConfigEntry& entry) const;
    ValidationResult validateValueAllowed(const ConfigEntry& entry) const;
};

} // namespace nlm

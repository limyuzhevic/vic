#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <mutex>

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

// File format detection
enum class ConfigFileFormat {
    Unknown,
    SimpleKeyValue,
    JSON,
    YAML
};

// Error codes for configuration loading
enum class ConfigError {
    None,
    FileNotFound,
    PermissionDenied,
    ParseError,
    InvalidFormat,
    InvalidType,
    SchemaValidationError
};

// Enhanced validation report with schema support
struct ValidationReport {
    bool valid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, std::string> typeViolations;
    std::unordered_map<std::string, double> valueRanges;
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
    
    // Load from file (auto-detects JSON, YAML, or simple format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON file
    bool loadFromJSON(const std::string& filepath);
    
    // Load from YAML file
    bool loadFromYAML(const std::string& filepath);
    
    // Load from simple key=value file
    bool loadFromSimpleKeyValue(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (auto-detects format from extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Save as JSON
    bool saveToJSON(const std::string& filepath) const;
    
    // Save as YAML
    bool saveToYAML(const std::string& filepath) const;
    
    // Save as simple key=value
    bool saveToSimpleKeyValue(const std::string& filepath) const;
    
    // Batch operations
    bool batchSet(const std::unordered_map<std::string, ConfigValue>& values);
    bool batchSet(const std::vector<std::pair<std::string, ConfigValue>>& values);
    
    // Configuration validation
    ValidationReport validate(const std::unordered_map<std::string, std::string>& schema = {}) const;
    
    // Schema validation for types
    struct SchemaField {
        std::string name;
        std::string type;  // "int", "int64", "double", "bool", "string", "vector<int>", "vector<double>", "vector<string>"
        bool required;
        std::optional<std::vector<double>> range;  // [min, max] for numeric types
        std::optional<std::string> description;
    };
    
    bool validateSchema(const std::vector<SchemaField>& schema) const;
    
    // Configuration export/import
    std::unordered_map<std::string, ConfigValue> exportConfig() const;
    bool importConfig(const std::unordered_map<std::string, ConfigValue>& config);
    bool importConfig(const std::unordered_map<std::string, std::string>& config);
    
    // Type-safe getters/setters
    template<typename T>
    void setDefault(const std::string& key, const T& value);
    
    // Enhanced getters with type inference
    ConfigValue getRaw(const std::string& key) const;
    std::optional<ConfigValue> getRawOpt(const std::string& key) const;
    
    // Type information
    std::vector<std::pair<std::string, std::string>> getTypes() const;
    
    // Configuration helpers
    std::unordered_map<std::string, ConfigValue> createPreset(const std::string& name) const;
    bool applyPreset(const std::string& name, const std::unordered_map<std::string, ConfigValue>& overrides = {});
    
    // Get values
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // JSON/YAML error handling
    static std::string configErrorToString(ConfigError error);
    std::string lastError() const;
    
    // Thread-safe operations
    void lock() const;
    void unlock() const;
    bool try_lock() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    mutable std::mutex mutex;
    std::string lastErrorMsg;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Value conversion helpers
    static std::string valueToString(const ConfigValue& value);
    static ConfigValue stringToValue(const std::string& str, const std::string& key);
    
    // JSON value conversion helpers
#ifdef HAS_NLOHMANN_JSON
    static ConfigValue jsonToConfigValue(const nlohmann::json& j, const std::string& key);
    static nlohmann::json configValueToJson(const ConfigValue& value);
    static nlohmann::json mapToJson(const std::unordered_map<std::string, ConfigValue>& config) const;
#endif
    
    // YAML value conversion helpers
#ifdef HAS_YAML_CPP
    static ConfigValue yamlToConfigValue(const YAML::Node& node, const std::string& key);
    static YAML::Node configValueToYAML(const ConfigValue& value);
    static YAML::Node mapToYAML(const std::unordered_map<std::string, ConfigValue>& config) const;
#endif
    
    // Format detection
    static ConfigFileFormat detectFileFormat(const std::string& filepath);
    
    // Error handling
    void setError(ConfigError error, const std::string& message);
    
    // Preset definitions
    static std::unordered_map<std::string, std::unordered_map<std::string, ConfigValue>> getPresetDefinitions();
    
    // Value type utilities
    static std::string getValueTypeName(const ConfigValue& value);
    static bool isValueTypeCompatible(const ConfigValue& value, const std::string& typeName);
    
    // Range validation
    static bool validateRange(const ConfigValue& value, const std::optional<std::vector<double>>& range);
};

} // namespace nlm

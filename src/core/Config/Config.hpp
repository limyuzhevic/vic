#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_set>
#include <chrono>
#include <functional>

namespace nlm {

// Forward declarations
class Config;
class ConfigError;
class ConfigValidationError;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<std::string>,
    std::vector<ConfigValue>,  // JSON-like arrays
    std::map<std::string, ConfigValue>,  // JSON-like objects
    std::vector<std::map<std::string, ConfigValue>>  // Array of objects
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
};

// Configuration error hierarchy
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigLoadError : public ConfigError {
public:
    explicit ConfigLoadError(const std::string& msg) : ConfigError("Load Error: " + msg) {}
};

class ConfigSaveError : public ConfigError {
public:
    explicit ConfigSaveError(const std::string& msg) : ConfigError("Save Error: " + msg) {}
};

class ConfigValidationError : public ConfigError {
public:
    explicit ConfigValidationError(const std::string& msg) : ConfigError("Validation Error: " + msg) {}
};

class ConfigParseError : public ConfigError {
public:
    explicit ConfigParseError(const std::string& msg) : ConfigError("Parse Error: " + msg) {}
};

// Configuration schema entry
struct ConfigSchemaEntry {
    std::string type;  // "int", "float", "bool", "string", "array", "object"
    std::optional<double> min;
    std::optional<double> max;
    std::optional<std::string> pattern;  // for strings
    std::string description;
    bool required = false;
};

// Configuration metadata for schema validation
struct ConfigMetadata {
    std::map<std::string, ConfigSchemaEntry> schema;
    std::map<std::string, std::string> descriptions;
    std::string version;
    std::chrono::system_clock::time_point timestamp;
    std::string context;
};

// Type conversion utilities
class ConfigTypeUtils {
public:
    static std::string valueToString(const ConfigValue& value);
    static bool tryConvertTo(ConfigValue& value, const std::string& typeName);
    static bool isNumericType(const ConfigValue& value);
    static bool isContainerType(const ConfigValue& value);
    static bool isObjectType(const ConfigValue& value);
    static std::string getTypeName(const ConfigValue& value);
    static std::string getTypeName(const ConfigSchemaEntry& entry);
    static std::vector<std::string> getAllTypeNames();
    static std::vector<std::string> getNumericTypeNames();
    static std::vector<std::string> getContainerTypeNames();
    static std::vector<std::string> getObjectTypeNames();
};

// Configuration comparison utilities
class ConfigComparisonUtils {
public:
    static bool valuesEqual(const ConfigValue& a, const ConfigValue& b);
    static double valuesDiff(const ConfigValue& a, const ConfigValue& b);
    static std::map<std::string, double> compareConfigs(const Config& a, const Config& b);
    static std::map<std::string, std::string> generateDiffReport(const Config& oldConfig, const Config& newConfig);
};

// Configuration factory utilities
class ConfigFactoryUtils {
public:
    static std::shared_ptr<Config> createFromDictionary(const std::map<std::string, std::string>& dict);
    static std::shared_ptr<Config> createFromNestedDictionary(const std::map<std::string, std::map<std::string, std::string>>& nestedDict);
    static std::shared_ptr<Config> createDefaultConfig(const std::string& context = "default");
    static std::shared_ptr<Config> createConfigFromFile(const std::string& filepath);
    static std::shared_ptr<Config> createConfigFromArgs(int argc, char** argv);
};

// Configuration validation utilities
class ConfigValidationUtils {
public:
    static bool validateValue(const std::string& key, const ConfigValue& value, const ConfigMetadata& metadata);
    static std::vector<std::string> validateConfig(const Config& config, const ConfigMetadata& metadata);
    static std::vector<std::string> validateAllConfigs(const std::vector<std::shared_ptr<Config>>& configs);
    static bool updateMetadata(const Config& config, ConfigMetadata& metadata);
    static std::string generateValidationReport(const std::vector<std::string>& errors);
};

// Configuration time-based utilities
class ConfigTimeUtils {
public:
    static std::string getCurrentTimestamp();
    static std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp);
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timepoint);
    static bool isExpired(const std::chrono::system_clock::time_point& timestamp, const std::string& ttl);
    static std::string getContextFromTimestamp(const std::chrono::system_clock::time_point& timestamp);
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
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<ConfigValue>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::map<std::string, ConfigValue>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::map<std::string, ConfigValue>>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration metadata
    const ConfigMetadata& getMetadata() const;
    
    // Set configuration metadata
    void setMetadata(const ConfigMetadata& metadata);
    
    // Update configuration metadata
    void updateMetadata(const std::string& version = "", const std::string& context = "");
    
    // Add or update configuration entry with type conversion
    void setWithType(const std::string& key, const std::string& valueStr, const std::string& type = "auto");
    
    // Configuration loading/saving with format support
    bool loadFromFile(const std::string& filepath, const std::string& format = "auto");
    bool saveToFile(const std::string& filepath, const std::string& format = "auto") const;
    
    // Configuration validation and schema
    bool validate(const ConfigMetadata& metadata) const;
    std::vector<std::string> getValidationErrors(const ConfigMetadata& metadata) const;
    
    // Configuration comparison and merging
    double compareWith(const Config& other) const;
    std::string generateDiffReport(const Config& other) const;
    std::shared_ptr<Config> mergeWith(const Config& other, bool override = true) const;
    
    // Configuration utility methods
    std::shared_ptr<Config> clone() const;
    std::map<std::string, std::string> toDictionary() const;
    std::map<std::string, std::map<std::string, std::string>> toNestedDictionary() const;
    
    // Configuration factory methods (static)
    static std::shared_ptr<Config> createDefault(const std::string& context = "default");
    static std::shared_ptr<Config> loadFromFile(const std::string& filepath, const std::string& format = "auto");
    static std::shared_ptr<Config> loadFromArgs(int argc, char** argv);
    
    // Configuration comparison utilities (static)
    static std::map<std::string, double> compare(const Config& a, const Config& b);
    static std::map<std::string, std::string> generateDiff(const Config& a, const Config& b);
    
    // Time-based and context-aware configuration
    bool isExpired(const std::string& ttl) const;
    void setContext(const std::string& context);
    std::string getContext() const;
    
    // Error handling utilities
    static std::string getErrorMessage(const ConfigError& error);
    static std::string formatError(const ConfigError& error);
    
    // Performance and optimization
    size_t getEntryCount() const;
    bool hasChanged(const Config& other) const;
    
    // Configuration statistics and analysis
    std::string getStatistics() const;
    std::map<std::string, std::string> getTypeDistribution() const;
    std::map<std::string, double> getValueStatistics() const;
    
// Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Private helper methods for internal use
    void addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description = "");
    bool findEntry(const std::string& key) const;
    const ConfigEntry* getEntry(const std::string& key) const;
    
    // File format detection
    std::string detectFileFormat(const std::string& filepath) const;
    bool isJSONFile(const std::string& filepath) const;
    bool isYAMLMFile(const std::string& filepath) const;
    
    // Type conversion helpers
    ConfigValue stringToConfigValue(const std::string& valueStr, const std::string& type = "auto") const;
    std::string configValueToString(const ConfigValue& value) const;
    
    // Error handling helpers
    static std::string getErrorMessage(const ConfigError& error);
    static std::string formatError(const ConfigError& error);
    
    // Comparison helpers
    static bool valuesEqual(const ConfigValue& a, const ConfigValue& b);
    static double valuesDiff(const ConfigValue& a, const ConfigValue& b);
    
    // Statistics helpers
    std::map<std::string, std::string> computeTypeDistribution() const;
    std::map<std::string, double> computeValueStatistics() const;
    
    // Validation helpers
    std::vector<std::string> computeValidationErrors(const ConfigMetadata& metadata) const;
    
    // Metadata helpers
    static std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp);
    static std::string formatTimestamp(const std::chrono::system_clock::time_point& timepoint);
    static std::string getCurrentTimestamp();
};

} // namespace nlm

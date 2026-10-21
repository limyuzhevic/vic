#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <functional>
#include <chrono>
#include <unordered_map>
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

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    bool required;
    
    ConfigEntry() 
        : key(), value(), source(ConfigSource::Default), description()
        , timestamp(std::chrono::system_clock::now()), required(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), value(v), source(s), description(desc)
        , timestamp(std::chrono::system_clock::now()), required(false) {}
};

// Configuration validation rule
using ConfigValidator = std::function<bool(const ConfigValue&)>;

// Configuration schema definition
struct ConfigSchema {
    std::string key;
    std::type_index type;
    bool required;
    std::string description;
    std::vector<ConfigValidator> validators;
    ConfigValue defaultValue;
    
    ConfigSchema(const std::string& k, const std::type_index& t, bool req, const std::string& desc = "")
        : key(k), type(t), required(req), description(desc), validators() {}
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
    
    // Load from file (JSON/YAML with fallback to simple format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file with format specification
    bool saveToFile(const std::string& filepath, const std::string& format = "json") const;
    
    // Get values with enhanced type support
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced set methods with validation
    template<typename T>
    bool setValidated(const std::string& key, const T& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration schema and validation
    void addSchema(const ConfigSchema& schema);
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Check existence and get metadata
    bool has(const std::string& key) const;
    ConfigSource getSource(const std::string& key) const;
    std::chrono::system_clock::time_point getTimestamp(const std::string& key) const;
    std::string getDescription(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary with enhanced info
    std::string summary() const;
    std::string detailedSummary() const;
    
    // Performance optimization helpers
    void optimizeLookups(); // Build index for faster access
    size_t getEstimatedMemoryUsage() const;
    
    // Serialization utilities
    std::string toJson() const;
    bool fromJson(const std::string& jsonStr);
    
    // Configuration merging
    void merge(const Config& other, bool overrideExisting = true);
    
    // Runtime configuration convenience methods
    void setRuntimeConfig(const std::string& key, const ConfigValue& value) {
        set(key, value, ConfigSource::Runtime);
    }
    
    // Configuration export/import
    bool exportToFile(const std::string& filepath) const;
    bool importFromFile(const std::string& filepath);
    
    // Configuration backup and restore
    bool createBackup(const std::string& backupPath) const;
    bool restoreFromBackup(const std::string& backupPath);
    
    // Configuration statistics
    size_t getEntryCount() const { return pImpl->entries.size(); }
    size_t getSchemaCount() const { return pImpl->schemas.size(); }
    bool isValidKey(const std::string& key) const;
    
    // Enhanced set with description
    void setWithDescription(const std::string& key, const ConfigValue& value, 
                          const std::string& description, ConfigSource source = ConfigSource::Runtime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string sanitizeKey(const std::string& key);
    static bool validateValueType(const ConfigValue& value, const std::type_index& expectedType);
    
    // JSON/YAML parsing helpers
    bool parseJsonFile(const std::string& filepath);
    bool parseYamlFile(const std::string& filepath);
    bool parseSimpleFile(const std::string& filepath);
    bool writeJsonFile(const std::string& filepath) const;
    bool writeYamlFile(const std::string& filepath) const;
    bool writeSimpleFile(const std::string& filepath) const;
};

// Configuration validation function type
using ConfigValidator = std::function<bool(const ConfigValue&)>;

// Global configuration utilities
namespace ConfigUtils {
    // Configuration comparison
    bool equals(const Config& a, const Config& b, bool compareSource = false);
    
    // Configuration diff
    struct ConfigDiff {
        std::vector<std::string> addedKeys;
        std::vector<std::string> removedKeys;
        std::vector<std::string> modifiedKeys;
    };
    ConfigDiff diff(const Config& a, const Config& b);
    
    // Configuration validation utilities
    bool isValidKey(const std::string& key);
    bool isValidString(const std::string& value, size_t minLen = 0, size_t maxLen = 1024);
    bool isValidNumber(double value, double min = std::numeric_limits<double>::lowest(), 
                      double max = std::numeric_limits<double>::max());
    bool isValidBool(bool value);
    
    // Configuration merging utilities
    void mergeConfigurations(Config& target, const Config& source, bool overrideExisting = true);
}

} // namespace nlm

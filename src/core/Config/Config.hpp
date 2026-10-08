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

// Type for configuration validation function
using ValidationFunction = std::function<bool(const ConfigValue&)>;

// Configuration schema entry
struct ConfigSchema {
    std::string key;
    std::string type;
    bool required = false;
    ConfigValue defaultValue;
    ValidationFunction validator;
    std::string description;
    std::vector<std::string> allowedValues;
    
    ConfigSchema() : required(false) {}
    ConfigSchema(const std::string& k, const std::string& t, const std::string& desc = "")
        : key(k), type(t), required(false), description(desc) {}
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
    
    // Enhanced file loading with JSON support
    bool loadFromFile(const std::string& filepath, bool validate = true);
    bool loadFromJSON(const std::string& jsonStr, bool validate = true);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv, const std::map<std::string, std::string>& flags = {});
    
    // Save to file in various formats
    bool saveToFile(const std::string& filepath, const std::string& format = "text") const;
    std::string saveToJSON() const;
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with source tracking
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation and schema management
    bool validateSchema(const std::vector<ConfigSchema>& schema) const;
    void addSchema(const ConfigSchema& schema);
    void clearSchema();
    std::vector<std::string> getValidationErrors() const;
    
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
    
    // Configuration statistics and metadata
    size_t getEntryCount() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    std::chrono::system_clock::time_point getLastModified() const;
    void setLastModified(const std::chrono::system_clock::time_point& time);
    
    // Configuration version management
    void setVersion(const std::string& version);
    std::string getVersion() const;
    bool isVersionCompatible(const std::string& requiredVersion) const;
    
    // File path resolution and backup
    void setWorkingDirectory(const std::string& directory);
    std::string getWorkingDirectory() const;
    bool saveWithBackup(const std::string& filepath, const std::string& format = "text") const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isValidType(const ConfigValue& value, const std::string& typeName);
    static bool validateAgainstSchema(const ConfigEntry& entry, const ConfigSchema& schema);
};

} // namespace nlm

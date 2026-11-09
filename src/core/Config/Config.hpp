#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <algorithm>
#include <cstdint>

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

// Exception class for configuration errors
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) : std::runtime_error(message) {}
    
    static ConfigError invalidKey(const std::string& key, const std::string& reason) {
        return ConfigError("Invalid configuration key '" + key + "': " + reason);
    }
    
    static ConfigError invalidValue(const std::string& key, const std::string& reason) {
        return ConfigError("Invalid configuration value for key '" + key + "': " + reason);
    }
    
    static ConfigError missingRequiredKey(const std::string& key) {
        return ConfigError("Missing required configuration key: '" + key + "'");
    }
    
    static ConfigError inconsistentConfiguration(const std::string& reason) {
        return ConfigError("Inconsistent configuration: " + reason);
    }
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
    
    // Get values (defined here so templates instantiate in every TU)
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
    
    // Enhanced validation and error handling
    bool validate() const;
    std::vector<std::string> validateWithDetails() const;
    bool validateValue(const ConfigEntry& entry, std::vector<std::string>& errors) const;
    void validateNumericValue(const std::string& key, const auto& value, std::vector<std::string>& errors) const;
    void validateBooleanValue(const std::string& key, const auto& value, std::vector<std::string>& errors) const;
    void validateStringValue(const std::string& key, const auto& value, std::vector<std::string>& errors) const;
    void validateArrayValue(const std::string& key, const auto& value, std::vector<std::string>& errors) const;
    void checkConfigurationConsistency(std::vector<std::string>& errors) const;
    
    // Schema and file validation
    bool validateSchema(const std::string& schemaFile) const;
    bool validateFilePath(const std::string& filepath) const;
    
    // Recovery and backup
    bool loadFromFileWithBackup(const std::string& filepath, const std::string& backupDir = "backups");
    bool recoverConfiguration(const std::string& backupPath);
    
    // Error handling
    void handleConfigurationError(const std::string& error);
    
    // Configuration management
    void setDefaultConfiguration();
    
    // Advanced configuration queries
    std::vector<std::string> getKeysByPrefix(const std::string& prefix) const;
    std::vector<std::string> getKeysBySuffix(const std::string& suffix) const;
    std::unordered_map<std::string, ConfigValue> getSection(const std::string& prefix) const;
    bool hasSection(const std::string& prefix) const;
    void removeSection(const std::string& prefix);
    std::string toJSON() const;
    std::string getSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeJSONString(const std::string& str);
    std::string getTimestamp() const;
    
    // Safe file loading
    bool loadFromFileImpl(const std::string& filepath);
    
    // Enhanced validation methods
    void validateEnhanced() const;
    void validateForSimulation() const;
    void validateForPerformance() const;
};

} // namespace nlm

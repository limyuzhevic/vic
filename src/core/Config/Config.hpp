#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

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
    
    // Load from string (key=value pairs separated by newlines)
    bool loadFromString(const std::string& configString);
    
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
    
    // Advanced configuration methods
    void setConfig(const std::string& key, const std::string& value) { set(key, value); }
    void setConfig(const std::string& key, int value) { set(key, value); }
    void setConfig(const std::string& key, double value) { set(key, value); }
    void setConfig(const std::string& key, bool value) { set(key, value); }
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration merging
    void merge(const Config& other);
    void mergeFromFile(const std::string& filepath);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    std::string getStatistics() const;
    
    // Configuration filtering
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    std::vector<std::string> getKeysByType(const std::string& typeName) const;
    
    // Configuration export/import
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& jsonString);
    
    // Configuration templates
    void loadDefaultConfig();
    void loadPhase2Config();
    void loadPhase6Config();
    void loadBenchmarkConfig();
    
    // Configuration backup and restore
    bool backup(const std::string& backupPath) const;
    bool restore(const std::string& backupPath);
    
    // Configuration snapshot
    std::string createSnapshot() const;
    bool restoreFromSnapshot(const std::string& snapshot);
    
    // Configuration logging
    void logConfiguration(LogLevel level = LogLevel::Info) const;
    void setLogCallback(std::function<void(const std::string&)> callback);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    void removeAll(ConfigSource source = ConfigSource::Runtime);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeys(ConfigSource source) const;
    
    // Clear all
    void clear();
    void clear(ConfigSource source);
    
    // Get configuration summary
    std::string summary() const;
    std::string summary(ConfigSource source) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Validation helpers
    bool validateKey(const std::string& key) const;
    bool validateValue(const ConfigValue& value, const std::string& key) const;
    
    // Serialization helpers
    static std::string valueToString(const ConfigValue& value);
    static bool stringToValue(const std::string& str, ConfigValue& value);
    
    // JSON helpers
    static std::string valueToJSON(const ConfigValue& value);
    static bool JSONToValue(const std::string& json, ConfigValue& value);
    
    // Configuration templates
    static std::unordered_map<std::string, std::string> getDefaultConfigValues();
    static std::unordered_map<std::string, std::string> getPhase2ConfigValues();
    static std::unordered_map<std::string, std::string> getPhase6ConfigValues();
    static std::unordered_map<std::string, std::string> getBenchmarkConfigValues();
};

// Configuration management utilities
namespace ConfigManager {
    // Configuration file utilities
    bool fileExists(const std::string& path);
    std::string getConfigDirectory();
    std::string getDefaultConfigPath();
    std::string getBackupPath(const std::string& originalPath);
    
    // Configuration validation utilities
    bool validateConfigFile(const std::string& filepath);
    std::vector<std::string> validateConfig(const Config& config);
    
    // Configuration merging utilities
    Config mergeConfigs(const Config& config1, const Config& config2);
    Config mergeConfigs(const std::vector<Config>& configs);
    
    // Configuration snapshot utilities
    std::string createConfigSnapshot(const Config& config);
    bool restoreConfigFromSnapshot(Config& config, const std::string& snapshot);
    
    // Configuration comparison utilities
    bool configsEqual(const Config& config1, const Config& config2);
    std::vector<std::string> diffConfigs(const Config& config1, const Config& config2);
    
    // Configuration statistics utilities
    std::string getConfigStatistics(const Config& config);
    std::map<ConfigSource, size_t> getSourceStatistics(const Config& config);
    
    // Configuration export/import utilities
    std::string exportConfigToJSON(const Config& config);
    bool importConfigFromJSON(Config& config, const std::string& jsonString);
    bool exportConfigToFile(const Config& config, const std::string& filepath);
    bool importConfigFromFile(Config& config, const std::string& filepath);
    
    // Configuration template utilities
    Config getDefaultConfig();
    Config getPhase2Config();
    Config getPhase6Config();
    Config getBenchmarkConfig();
    
    // Configuration backup/restore utilities
    bool backupConfig(const Config& config, const std::string& backupPath);
    bool restoreConfig(Config& config, const std::string& backupPath);
    std::vector<std::string> listBackups(const std::string& directory);
    bool deleteBackup(const std::string& backupPath);
}

} // namespace nlm

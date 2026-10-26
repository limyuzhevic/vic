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

// Configuration change callback
using ConfigChangeCallback = std::function<void(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue)>;

// Configuration statistics
struct ConfigStats {
    size_t totalKeys;
    size_t runtimeKeys;
    size_t fileKeys;
    size_t defaultKeys;
    size_t commandLineKeys;
    double memoryUsageMB;
    std::chrono::steady_clock::time_point lastAccess;
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
    
    // Get values
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
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Set values with description and source
    void setWithDescription(const std::string& key, const ConfigValue& value, const std::string& description, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get all entries
    std::vector<ConfigEntry> getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration statistics
    ConfigStats getStats() const;
    
    // Configuration validation
    std::vector<std::string> validate() const;
    
    // Set callback for configuration changes
    void setChangeCallback(ConfigChangeCallback callback);
    
    // Freeze configuration (no more runtime changes)
    void freeze();
    bool isFrozen() const;
    
    // Get configuration value type
    std::string getValueType(const std::string& key) const;
    
    // Copy configuration
    std::shared_ptr<Config> copy() const;
    
    // Merge configurations
    void merge(const Config& other);
    
    // Set default configuration from file
    bool setDefaultFromFile(const std::string& filepath);
    
    // Reset to defaults
    void resetToDefaults();
    
    // Get configuration value as string
    std::string getAsString(const std::string& key) const;
    
    // Get configuration value with units
    std::string getWithUnits(const std::string& key) const;
    
    // Validate required keys
    bool validateRequiredKeys(const std::vector<std::string>& requiredKeys) const;
    
    // Get configuration age
    double getConfigurationAge() const;
    
    // Export configuration
    std::string exportConfig(const std::string& format = "json") const;
    
    // Import configuration
    bool importConfig(const std::string& configString, const std::string& format = "json");
    
    // Get configuration history (limited)
    std::vector<ConfigEntry> getHistory(const std::string& key, size_t maxEntries = 10) const;
    
    // Undo last change
    bool undoLastChange(const std::string& key);
    
    // Reset configuration to specific version
    bool resetToVersion(const std::string& key, size_t version);
    
    // Apply configuration presets
    static std::shared_ptr<Config> createPreset(const std::string& presetName);
    
    // Get list of available presets
    static std::vector<std::string> getAvailablePresets();
    
    // Get configuration size estimate
    size_t estimateMemoryUsage() const;
    
    // Configuration statistics
    class Stats {
    public:
        static std::string formatMemoryUsage(size_t bytes);
        static std::string formatTime(double seconds);
        static std::string formatCount(size_t count);
    };
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toJsonString(const ConfigValue& value);
    static ConfigValue fromJsonString(const std::string& str, const std::string& typeHint = "");
};

} // namespace nlm

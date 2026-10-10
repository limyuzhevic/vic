#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>

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

// Preset type enum
enum class PresetType {
    Simple,
    Medium,
    Advanced,
    Debug,
    Custom
};

// Forward declaration for ConfigSchemaEntry
struct ConfigSchemaEntry;

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
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
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
    
    // Configuration validation
    bool validate() const;
    
    // Configuration presets
    bool applyPreset(const std::string& name, ConfigSource source = ConfigSource::Runtime);
    void applyPresetType(PresetType type, ConfigSource source = ConfigSource::Runtime);
    void mergePreset(const std::string& name, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration utilities
    static Config createFromPreset(const std::string& name);
    static Config createFromPresetType(PresetType type);
    
    // Configuration comparison
    std::string generateDiff(const Config& other) const;
    std::string generateHumanReadableDiff(const Config& other) const;
    bool isCompatibleWith(const Config& other) const;
    std::vector<std::string> getCompatibilityIssues(const Config& other) const;
    
    // Configuration sanitization
    Config sanitize() const;
    
    // Configuration cloning
    Config clone() const;
    
    // Configuration merging
    Config merge(const Config& other) const;
    Config merge(const Config& other, ConfigSource preference) const;
    
    // Configuration source management
    Config withSource(ConfigSource source) const;
    Config withoutSource(ConfigSource source) const;
    bool hasAnySource(ConfigSource source) const;
    ConfigSource getHighestPrioritySource() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    Config getConfigBySource(ConfigSource source) const;
    Config getConfigWithoutSource(ConfigSource source) const;
    
    // Configuration serialization
    std::string toString() const;
    std::string toJson() const;
    std::string toYaml() const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Forward declarations for related classes
class ConfigValidator;
class ConfigPresets;
class ConfigSchema;
class ConfigComparator;
class ConfigMerger;
class ConfigDiffer;
class ConfigValidatorUtils;
class ConfigSanitizer;

} // namespace nlm

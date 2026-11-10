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
    
    // Load from command line arguments with advanced parsing
    bool loadFromArgsAdvanced(int argc, char** argv);
    
    // Configuration validation
    bool validate() const;
    std::string validateSummary() const;
    
    // Preset configurations
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createPhase2Config();
    static std::shared_ptr<Config> createPhase6Config();
    
    // Configuration command processing
    bool processConfigCommands(const std::string& commandLine);
    
    // Get configuration statistics
    size_t getEntryCount() const;
    size_t getSourceCount(ConfigSource source) const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Configuration search and filtering
    std::vector<std::string> searchKeys(const std::string& pattern) const;
    std::vector<std::string> getNumericKeys() const;
    std::vector<std::string> getStringKeys() const;
    std::vector<std::string> getBooleanKeys() const;
    
    // Configuration backup and restore
    bool saveToFileWithTimestamp(const std::string& filepath) const;
    bool loadFromFileWithFallback(const std::string& primaryPath, const std::string& fallbackPath);
    
    // Configuration comparison
    bool hasSameValues(const Config& other) const;
    
    // Configuration help and documentation
    std::string getConfigHelp() const;
    static std::string getAvailableConfigKeys();
    
    // Configuration validation rules
    bool setValidationRule(const std::string& key, const std::string& rule);
    bool removeValidationRule(const std::string& key);
    std::string getValidationRules() const;
    
    // Configuration export/import for advanced use
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& json);
    
    // Configuration monitoring
    class ConfigChangeListener {
    public:
        virtual ~ConfigChangeListener() = default;
        virtual void onConfigChanged(const std::string& key, const ConfigValue& newValue) = 0;
    };
    
    void addConfigChangeListener(ConfigChangeListener* listener);
    void removeConfigChangeListener(ConfigChangeListener* listener);
    
    // Configuration diff between two configs
    std::vector<std::string> diffKeys(const Config& other) const;
    
    // Configuration bulk operations
    void mergeConfigs(const Config& source);
    void clearUnusedDefaults();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

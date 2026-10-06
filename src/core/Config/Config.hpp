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
    
    // Additional set methods for other types
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
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
    
    // File operations
    bool saveToFile(const std::string& filepath) const;
    
    // New advanced file operations
    bool saveToSimpleFormat(const std::string& filepath) const;
    bool saveToJSON(const std::string& filepath) const;
    
    // Configuration categories
    static std::vector<std::string> getCoreCategories();
    static std::vector<std::string> getPlasticityCategories();
    static std::vector<std::string> getMemoryCategories();
    static std::vector<std::string> getNeuromodulationCategories();
    static std::vector<std::string> getPredictionCategories();
    static std::vector<std::string> getDevelopmentCategories();
    static std::vector<std::string> getCognitiveCategories();
    static std::vector<std::string> getEnvironmentCategories();
    
    // Configuration validation
    bool validate(const std::string& category = "all") const;
    static std::vector<std::string> getValidationErrors(const std::string& category = "all");
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::File);
    
    // Configuration search
    std::vector<std::string> findKeysByPattern(const std::string& pattern) const;
    
    // Configuration export/import
    std::string exportToString(ConfigSource minSource = ConfigSource::Runtime) const;
    bool importFromString(const std::string& configText);
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    float getUsedPercentage() const;
    
    // Configuration backup/restore
    bool backup(const std::string& backupPath) const;
    bool restore(const std::string& backupPath);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    std::vector<std::string> diff(const Config& other) const;
    
    // Configuration help
    static std::string getCategoryDescription(const std::string& category);
    static std::vector<std::string> getAllCategories();
    
    // Configuration templates
    static Config createDefaultConfig();
    static Config createMinimalConfig();
    static Config createResearchConfig();
    static Config createDevelopmentConfig();
    static Config createProductionConfig();
    
    // Configuration validation schemas
    static bool validateRange(const std::string& key, const ConfigValue& value);
    static bool validateDependencies(const std::string& key, const ConfigValue& value);
    
    // Configuration observers
    using ConfigChangeCallback = std::function<void(const std::string&, const ConfigValue&, ConfigSource)>;
    void addChangeCallback(const ConfigChangeCallback& callback);
    void removeChangeCallback(const ConfigChangeCallback& callback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::vector<ConfigEntry> getDefaultConfigEntries();
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    
    // Change notification
    void notifyChange(const std::string& key, const ConfigValue& value, ConfigSource source);
};
};

} // namespace nlm

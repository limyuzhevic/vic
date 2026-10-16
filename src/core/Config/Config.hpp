#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <deque>
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

// Configuration categories
enum class ConfigCategory {
    Core,
    Neural,
    Plasticity,
    Memory,
    Development,
    Neuromodulation,
    Reward,
    Environment,
    Simulation,
    Visualization,
    Logging,
    Performance,
    Custom
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

// Enhanced configuration value types with type information
struct ConfigValueWithType {
    std::variant<
        int,
        int64_t,
        double,
        bool,
        std::string,
        std::vector<int>,
        std::vector<double>,
        std::vector<std::string>
    > value;
    std::string typeName;
    std::optional<double> minValue;
    std::optional<double> maxValue;
    std::string unit;
    
    ConfigValueWithType() : value(), typeName(), minValue(), maxValue(), unit() {}
};

// Configuration preset
struct ConfigPreset {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    std::string author;
    std::chrono::system_clock::time_point created;
    
    ConfigPreset(const std::string& n, const std::string& desc, const std::string& author = "system")
        : name(n), description(desc), author(author), created(std::chrono::system_clock::now()) {}
};

// Configuration template
struct ConfigTemplate {
    std::string name;
    std::string description;
    std::map<std::string, ConfigValue> values;
    ConfigCategory category;
    double relevanceScore;
    
    ConfigTemplate() : category(ConfigCategory::Custom), relevanceScore(0.0) {}
    ConfigTemplate(const std::string& n, const std::string& desc)
        : name(n), description(desc), category(ConfigCategory::Custom), relevanceScore(0.0) {}
};

// Configuration history entry
struct ConfigHistoryEntry {
    std::chrono::system_clock::time_point timestamp;
    std::map<std::string, ConfigValue> values;
    std::string action;
    std::string user;
    std::string reason;
    
    ConfigHistoryEntry(const std::string& act, const std::string& usr = "")
        : timestamp(std::chrono::system_clock::now()), action(act), user(usr), reason() {}
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
    
    // Load from file (JSON/YAML format with backward compatibility)
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
    
    // Get keys by category
    std::vector<std::string> getKeysByCategory(ConfigCategory category) const;
    
    // Get category for key
    ConfigCategory getCategory(const std::string& key) const;
    
    // Get categories
    std::vector<ConfigCategory> getCategories() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Enhanced features
    
    // Load with inheritance support
    bool loadWithInheritance(const std::vector<std::string>& baseFiles, 
                            const std::map<std::string, std::string>& overrides);
    
    // Apply templates
    bool applyTemplate(const std::string& templateName);
    std::vector<std::string> getTemplateNames() const;
    
    // Apply presets
    void addPreset(const ConfigPreset& preset);
    bool applyPreset(const std::string& presetName);
    std::vector<std::string> getPresetNames() const;
    
    // Get suggestions and documentation
    std::vector<std::string> getSuggestions(const std::string& key) const;
    std::string getDocumentation(const std::string& key) const;
    std::map<std::string, std::string> getAllDocumentation() const;
    
    // Export/import
    bool exportToFile(const std::string& filepath, bool includeHistory = false) const;
    bool importFromFile(const std::string& filepath);
    
    // Diff and merge
    std::string getDiff(const Config& other) const;
    void merge(const Config& other, bool overwriteExisting = false);
    
    // History
    std::string getHistory(size_t limit = 0) const;
    std::string getHistoryActions(const std::string& key) const;
    
    // Hot-reload
    void enableHotReload(bool enabled, std::chrono::milliseconds interval = std::chrono::milliseconds(100));
    
    // Help and recommendations
    std::string getHelp() const;
    std::string getStatistics() const;
    std::string getUsageStatistics() const;
    bool validateConfiguration() const;
    bool fixConfiguration();
    std::vector<std::string> getRecommendations() const;
    
    // JSON/YAML support
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromSimpleFile(const std::string& filepath);
    bool saveToJSONFile(const std::string& filepath, bool includeComments = false) const;
    bool saveToYAMLFile(const std::string& filepath, bool includeComments = false) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string getCurrentTimestamp();
    static std::string getTimestampString(std::chrono::system_clock::time_point timestamp);
    static std::string categoryToString(ConfigCategory category);
    static std::string escapeJSON(const std::string& str);
    static std::string escapeYAMLString(const std::string& str);
    static std::string escapeString(const std::string& str);
    
    // Value parsing and conversion
    static ConfigValue parseValue(const std::string& str);
    static ConfigValue tryConvertValue(const ConfigValue& value, const std::string& targetType) const;
    
    // Update file timestamp for hot-reload
    void updateFileTimestamp(const std::string& filepath);
    
    // Initialize default templates, suggestions, and documentation
    void initializeDefaultTemplates();
    void initializeSuggestions();
    void initializeDocumentation();
    
    // Environment variable loading
    bool loadFromEnvironment();
    
    // Export helper
    void exportValue(std::ostream& os, const ConfigValue& value) const;
};

} // namespace nlm

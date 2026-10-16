#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <fstream>
#include <sstream>
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

// Helper for configuration parsing and serialization
class ConfigParser {
public:
    static bool parseJSON(const std::string& content, std::vector<ConfigEntry>& entries);
    static bool parseYAML(const std::string& content, std::vector<ConfigEntry>& entries);
    static std::string serializeToJSON(const std::vector<ConfigEntry>& entries);
    static std::string serializeToYAML(const std::vector<ConfigEntry>& entries);
    
    static ConfigValue parseValue(const std::string& str, bool& success);
    static std::string valueToString(const ConfigValue& value);
    
private:
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string escapeString(const std::string& str);
    static std::string unescapeString(const std::string& str);
    static bool isDigit(char c);
    static bool isAlpha(char c);
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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments (improved)
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
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
    
    // File format detection
    static std::string detectFileFormat(const std::string& filepath);
    
    // Utility functions for advanced users
    static std::vector<std::string> parseCommandLineArgs(int argc, char** argv);
    static bool validateConfig(const std::vector<ConfigEntry>& entries);
    
    // Advanced utility functions
    static Config createFromJSON(const std::string& jsonContent);
    static Config createFromYAML(const std::string& yamlContent);
    static Config createFromSimple(const std::vector<std::string>& lines);
    
    // Configuration merging and comparison
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    bool equals(const Config& other) const;
    size_t hash() const;
    
    // Serialization utilities
    static std::string toJSON(const Config& config);
    static std::string toYAML(const Config& config);
    static std::string toSimple(const Config& config);
    
    // Validation and schema
    bool validate() const;
    bool hasRequiredKeys(const std::vector<std::string>& requiredKeys) const;
    
    // Configuration statistics
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;
    float getProgressPercentage(ConfigSource source) const;
    
    // Configuration import/export
    bool importFromJSON(const std::string& jsonContent);
    bool importFromYAML(const std::string& yamlContent);
    bool exportToJSONFile(const std::string& filepath) const;
    bool exportToYAMLFile(const std::string& filepath) const;
    
    // Configuration modification
    void renameKey(const std::string& oldKey, const std::string& newKey);
    void batchSet(const std::unordered_map<std::string, ConfigValue>& values, 
                  ConfigSource source = ConfigSource::Runtime);
    void removeBySource(ConfigSource source);
    
    // Configuration comparison
    int compare(const Config& other, const std::string& key = "") const;
    
    // Configuration summary and reporting
    std::string generateReport() const;
    std::string generateDetailedReport() const;
    
    // Configuration validation schemas
    static bool validateAgainstSchema(const Config& config, 
                                      const std::string& schemaType);
    
    // Configuration compression and optimization
    void removeUnusedEntries(const std::vector<std::string>& usedKeys);
    void optimizeStorage();
    
    // Configuration backup and restore
    bool backup(const std::string& backupPath) const;
    bool restore(const std::string& backupPath);
    
    // Configuration monitoring
    std::string getChangeSummary(const Config& previousConfig) const;
    bool hasChanges(const Config& other) const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

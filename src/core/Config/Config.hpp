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
    Default,    // Built into the code
    File,       // Loaded from configuration file
    CommandLine, // Set via command line arguments
    Runtime     // Set at runtime by code
};

// Helper function to convert ConfigSource to string
inline const char* configSourceToString(ConfigSource source) {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "CommandLine";
        case ConfigSource::Runtime: return "Runtime";
        default: return "Unknown";
    }
}

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    uint64_t lastModified;  // Timestamp when entry was last modified
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), lastModified(0) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", uint64_t timestamp = 0)
        : key(k), value(v), source(s), description(desc), lastModified(timestamp) {}
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
    
    // Configuration management methods
    bool loadFromFile(const std::string& filepath);
    bool loadFromArgs(int argc, char** argv);
    bool saveToFile(const std::string& filepath) const;
    
    // Enhanced value getters with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Enhanced setters with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration validation and management
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    bool validate() const;  // Validate configuration consistency
    
    // Get configuration metadata
    std::vector<std::string> getKeys() const;
    ConfigSource getSource(const std::string& key) const;  // Get source of a specific key
    std::string getDescription(const std::string& key) const;  // Get description
    
    // Batch operations for efficiency
    void setMultiple(const std::vector<std::pair<std::string, ConfigValue>>& keyValues);
    std::map<std::string, ConfigValue> getAllAsMap() const;
    
    // Statistics and analysis
    size_t getEntryCount() const;
    size_t getEntryCount(ConfigSource source) const;  // Count entries from specific source
    std::vector<ConfigEntry> getEntries() const;  // Get all entries with metadata
    std::string summary() const;
    std::string getDebugInfo() const;  // Detailed debug information
    
    // Export and import
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& jsonString);
    
    // Configuration snapshots and comparison
    std::string createSnapshot() const;
    bool restoreFromSnapshot(const std::string& snapshot);
    bool hasChanged(const std::string& key, const std::string& snapshot) const;
    
    // Merge operations
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    void mergeFromFile(const std::string& filepath);  // Merge another config file
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

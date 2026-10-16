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
    
    // ===== ADVANCED CONFIGURATION OPERATIONS =====
    
    // Import configuration from multiple sources with priority
    // Merges configurations from file, command line, and runtime sources
    bool importFromMultipleSources(const std::vector<std::string>& filepaths,
                                  int argc = 0, char** argv = nullptr);
    
    // Validate configuration for simulation
    // Checks that all required keys exist and have valid values
    bool validate() const;
    
    // Get configuration differences between two configs
    // Returns vector of keys that differ between this and another config
    std::vector<std::string> diff(const Config& other) const;
    
    // Apply configuration overrides
    // Takes a map of key-value pairs and applies them as runtime overrides
    void applyOverrides(const std::map<std::string, ConfigValue>& overrides);
    
    // ===== CONFIGURATION METADATA =====
    
    // Get all configuration entries with source information
    // Returns entries including their source (Default, File, CommandLine, Runtime)
    std::vector<ConfigEntry> getAllEntries() const;
    
    // Get entries from specific source
    // Filters entries by their configuration source
    std::vector<ConfigEntry> getEntriesFromSource(ConfigSource source) const;
    
    // Count entries from specific source
    // Useful for understanding configuration composition
    size_t countFromSource(ConfigSource source) const;
    
    // ===== UTILITY OPERATIONS =====
    
    // Merge another configuration into this one
    // Entries from the other config take precedence
    void merge(const Config& other);
    
    // Create a copy of this configuration
    std::unique_ptr<Config> clone() const;
    
    // Get configuration hash for comparison
    // Useful for detecting configuration changes
    size_t getHash() const;
    
    // Check if configuration equals another
    bool equals(const Config& other) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

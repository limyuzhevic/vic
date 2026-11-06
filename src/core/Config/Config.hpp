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
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values (defined here so templates instantiate in every TU)
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values - use a single template function with automatic type deduction
    template<typename T>
    void set(const std::string& key, T value, ConfigSource source = ConfigSource::Runtime);
    
    // Specialization for string with quotes handling
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
    // Specialization for vector types
    template<typename T>
    void set(const std::string& key, const std::vector<T>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Helper: Get all value types for a key
    std::vector<ConfigSource> getValueSources(const std::string& key) const;
    
    // Helper: Check if key was set by a specific source
    bool wasSetBySource(const std::string& key, ConfigSource source) const;
    
    // Helper: Get keys sorted by source priority
    std::vector<std::string> getKeysSortedBySource(ConfigSource source) const;
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get keys sorted by source priority
    std::vector<std::string> getKeysSortedBySource(ConfigSource source) const;
    
    // Get all value types for a key
    std::vector<ConfigSource> getValueSources(const std::string& key) const;
    
    // Check if key was set by a specific source
    bool wasSetBySource(const std::string& key, ConfigSource source) const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate configuration against schema
    bool validate() const;
    
    // Check if configuration is valid
    bool isValid() const;
    
    // Helper: Merge configuration from another config
    void merge(const Config& other);
    
    // Helper: Copy configuration
    void copyFrom(const Config& other);
    
    // Helper: Get configuration entry count
    size_t getEntryCount() const;
    
    // Helper: Get configuration entry at index
    const ConfigEntry& getEntryAt(size_t index) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

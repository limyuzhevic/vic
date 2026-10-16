// Config.hpp - Improved configuration management
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace nlm {

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
    Runtime,
    Programmatic
};

// Configuration entry with metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool required;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), required(false) {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "", bool req = false)
        : key(k), value(v), source(s), description(desc), required(req) {}
};

// Main configuration class with enhanced features
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON format with extended support)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from command line arguments with enhanced parsing
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file in JSON format
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with better error handling
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with type conversion
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Advanced type-specific setters
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void setFloat(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime, const std::string& description = "");
    
    // Check existence and type safety
    bool has(const std::string& key) const;
    bool hasType(const std::string& key, ConfigValue::variant_type type) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Get configuration entries (advanced access)
    const std::vector<ConfigEntry>& getEntries() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Validate configuration
    std::vector<std::string> validate() const;
    
    // Merge with another config (deep merge)
    void merge(const Config& other);
    
    // Apply parameter constraints
    bool applyConstraints(const std::map<std::string, std::function<bool(const ConfigValue&)>>& constraints);
    
    // Get configuration size
    size_t size() const;
    
    // Check if empty
    bool empty() const;
    
    // Iterator support for advanced use
    class iterator;
    class const_iterator;
    
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    
    // Get source of a key
    ConfigSource getSource(const std::string& key) const;
    
    // Override with command line args (non-destructive)
    void overrideWithArgs(int argc, char** argv);
    
    // Get configuration by section (for organizational purposes)
    std::map<std::string, std::map<std::string, ConfigValue>> getBySection() const;
    
    // Set section-based configuration
    void setSection(const std::string& section, const std::map<std::string, ConfigValue>& values);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toJSON(const ConfigValue& value);
    static ConfigValue fromJSON(const std::string& str);
    static std::map<std::string, std::string> parseJSON(const std::string& jsonStr);
    
    // Validate value types
    static bool isValidValueType(const ConfigValue& value, ConfigValue::variant_type type) const;
    
    // Calculate hash for key
    static size_t hashKey(const std::string& key);
    
    // Resolve conflicts between sources (lower precedence wins)
    static ConfigSource resolveConflict(ConfigSource source1, ConfigSource source2);
    
    // Extract section from key (e.g., "neural.neuron_count" -> "neural")
    static std::string extractSection(const std::string& key);
};

} // namespace nlm
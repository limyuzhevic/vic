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
    
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Schema management
    void addSchemaEntry(const std::string& key, const std::string& type, 
                       const ConfigValue& defaultValue, const std::string& description);
    void setSchema(const std::string& key, const std::string& type,
                  const ConfigValue& defaultValue, const std::string& description);
    bool validateSchema() const;
    
    // Advanced operations
    void batchSet(const std::vector<std::pair<std::string, ConfigValue>>& updates);
    bool mergeFrom(const Config& other, ConfigSource source = ConfigSource::Runtime);
    Config filter(const std::vector<std::string>& keys) const;
    Config transform(const std::function<ConfigValue(ConfigValue)>& transform) const;
    
    // Python-friendly methods
    bool loadFromFile(const std::string& filepath);
    bool saveToFile(const std::string& filepath) const;
    bool loadFromArgs(int argc, char** argv);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Helper for Python bindings
    std::optional<ConfigValue> getAsVariant(const std::string& key) const;
    
    // Parsing implementations
    bool parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    bool parseYAMLFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    bool parseRapidJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    bool parseSimpleFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    bool saveAsJSONFile(const std::vector<ConfigEntry>& entries, const std::string& filepath);
};

} // namespace nlm

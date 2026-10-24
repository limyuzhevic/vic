#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <sstream>

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

    // Advanced features
    bool loadFromEnvironment();
    bool loadFromJSON(const std::string& jsonContent);
    bool loadFromYAML(const std::string& yamlContent);
    bool mergeFromFile(const std::string& filepath);
    bool exportToJSON(const std::string& filepath) const;
    bool exportToYAML(const std::string& filepath) const;
    std::string exportToJSONString() const;
    std::string exportToYAMLString() const;
    bool validateAll() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    void setDescription(const std::string& key, const std::string& description);
    std::string getDescription(const std::string& key) const;
    void renameKey(const std::string& oldKey, const std::string& newKey);
    bool hasKeyPrefix(const std::string& prefix) const;
    std::vector<std::string> getKeysByType(const std::string& typeName) const;
    std::string getValueAsString(const std::string& key, const std::string& format = "text") const;
    bool valueExists(const std::string& key) const;
    size_t getEntryCount() const;
    ConfigSource getSource(const std::string& key) const;
    bool isKeyValid(const std::string& key) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    bool loadFromSimpleFile(const std::string& filepath);
    bool parseFromJSON(const std::string& jsonStr);
    bool parseFromYAML(const std::string& yamlStr);
    ConfigValue parseJSONValue(const std::string& jsonStr, size_t& pos);
    std::string serializeToJSON() const;
    std::string serializeToYAML() const;
    bool validateValue(const std::string& key, const ConfigValue& value) const;
    std::string getValueTypeName(const ConfigValue& value) const;
    bool loadFromEnvironmentVariable(const std::string& varName);
    bool validateAllEntries() const;
    void mergeEntries(const std::vector<ConfigEntry>& entries, ConfigSource source);
    std::vector<ConfigEntry> filterEntries(ConfigSource source) const;
    std::string formatValue(const ConfigValue& value, const std::string& format) const;
};

} // namespace nlm

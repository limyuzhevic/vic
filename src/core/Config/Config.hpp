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
    
    // Get values with convenience methods
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get values with type inference
    int getInt(const std::string& key) const;
    int64_t getInt64(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::string getString(const std::string& key) const;
    std::vector<int> getIntArray(const std::string& key) const;
    std::vector<double> getDoubleArray(const std::string& key) const;
    std::vector<std::string> getStringArray(const std::string& key) const;
    
    // Set values with convenience methods
    void setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void setInt64(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void setIntArray(const std::string& key, const std::vector<int>& value, ConfigSource source = ConfigSource::Runtime);
    void setDoubleArray(const std::string& key, const std::vector<double>& value, ConfigSource source = ConfigSource::Runtime);
    void setStringArray(const std::string& key, const std::vector<std::string>& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // ===== ADVANCED CONFIGURATION MANAGEMENT =====
    
    // Configuration validation and type checking
    bool validate(const std::string& key, const std::string& type) const;
    bool validateInt(const std::string& key) const;
    bool validateDouble(const std::string& key) const;
    bool validateBool(const std::string& key) const;
    bool validateString(const std::string& key) const;
    bool validateIntArray(const std::string& key) const;
    bool validateDoubleArray(const std::string& key) const;
    bool validateStringArray(const std::string& key) const;
    
    // Configuration merging and comparison
    void merge(const Config& other);
    bool equals(const Config& other) const;
    
    // Configuration export/import
    bool exportToJSON(const std::string& filepath) const;
    bool importFromJSON(const std::string& filepath);
    
    // Configuration statistics
    size_t getEntryCount() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    std::string getSourceString(ConfigSource source) const;
    
    // ===== HELPERS =====
    
    // Create common configuration presets
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createEfficientConfig();
    static std::shared_ptr<Config> createLearningConfig();
    static std::shared_ptr<Config> createExplorationConfig();
    static std::shared_ptr<Config> createMinimalConfig();
    static std::shared_ptr<Config> createHighPerformanceConfig();
    
    // Get source type as string
    static std::string getSourceName(ConfigSource source);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

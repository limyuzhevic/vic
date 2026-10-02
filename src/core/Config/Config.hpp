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
    
    // Get values with type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    // Typed accessors for common types
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Convenience typed getters
    template<typename T>
    std::optional<T> getTyped(const std::string& key) const;
    float getFloat(const std::string& key) const;
    int getInt(const std::string& key) const;
    bool getBool(const std::string& key) const;
    std::string getString(const std::string& key) const;
    std::vector<float> getFloatArray(const std::string& key) const;
    std::vector<int> getIntArray(const std::string& key) const;
    
    // Set values with type safety
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    
    // Additional configuration methods
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Configuration validation and utilities
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Configuration factory methods
    static std::shared_ptr<Config> createFromFile(const std::string& filepath);
    static std::shared_ptr<Config> createFromDict(const std::string& dictString);
    static std::shared_ptr<Config> createFromString(const std::string& jsonString);
    
    // Configuration merging and comparison
    void merge(const std::shared_ptr<Config>& other);
    std::shared_ptr<Config> diff(const std::shared_ptr<Config>& other) const;
    
    // Configuration sections
    std::shared_ptr<Config> getSection(const std::string& prefix) const;
    void setSection(const std::string& prefix, const std::shared_ptr<Config>& section);
    
    // Configuration cloning
    std::shared_ptr<Config> clone() const;
    
    // Get configuration statistics
    size_t getEntryCount() const;
    std::vector<ConfigSource> getSources() const;
    
    // Get configuration entry details
    std::optional<ConfigEntry> getEntry(const std::string& key) const;
    
    // Update with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

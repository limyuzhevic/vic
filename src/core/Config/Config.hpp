#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>

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
    
    // Load from file (JSON or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON or key=value format based on extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Legacy save methods
    bool saveToJsonFile(const std::string& filepath) const;
    bool saveToLegacyFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set with JSON parsing support
    bool setJsonFromString(const std::string& jsonString);
    
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
    
    // Get formatted configuration for display
    std::string getFormattedSummary() const;
    
    // Get configuration as JSON string
    std::string toJsonString() const;
    
    // Get configuration as key-value map
    std::map<std::string, std::string> getKeyValueMap() const;
    
    // Export configuration for command-line use
    std::string exportForCommandLine() const;
    
    // Check if configuration has been modified from file
    bool isModifiedFromFile(const std::string& filepath) const;
    
    // Apply command-line configuration to another config object
    static void applyCommandLineConfig(Config& config, int argc, char** argv);
    
private:
    // Internal helpers for JSON parsing
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool is_vector(const ConfigValue& value);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper for JSON parsing
    static std::string parseJsonValue(const std::string& line, size_t& pos);
    static bool parseJsonFromFile(const std::string& filepath);
    static bool saveToJsonFileImpl(const std::string& filepath, const Config& config);
    static bool saveToLegacyFileImpl(const std::string& filepath, const Config& config);
    static bool mergeJsonObjects(const std::string& jsonContent, Config& config);
    static bool parseKeyValue(const std::string& line, Config& config);
    static bool parseNestedJson(const std::string& jsonContent, size_t startPos, Config& config);
    static std::string extractValueString(const std::string& jsonContent, size_t& pos);
};

} // namespace nlm

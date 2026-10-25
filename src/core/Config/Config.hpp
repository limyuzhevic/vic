#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>

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
    
    // Load from file (auto-detects JSON, YAML, or simple format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load from specific formats
    bool loadFromSimpleFile(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with validation
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
    
    // Validation and utilities
    bool validateKey(const std::string& key) const;
    bool has(const std::string& key) const;
    void remove(const std::string& key);
    std::vector<std::string> getKeys() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    
    // Get all
    void clear();
    std::string summary() const;
    std::string summaryBySource(ConfigSource source) const;
    
    // File operations
    bool fileExists(const std::string& filepath) const;
    static std::string getFileExtension(const std::string& filepath);
    static bool isValidKey(const std::string& key);
    static bool isValidValue(const ConfigValue& value);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Private helpers for format detection
    static std::string getFileExtension(const std::string& filepath);
    static bool isJSONFile(const std::string& filepath);
    static bool isYAMLFile(const std::string& filepath);
    static bool isSimpleFormatFile(const std::string& filepath);
};

} // namespace nlm

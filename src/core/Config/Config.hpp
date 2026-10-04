#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <nlohmann/json.hpp>

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
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON/YAML string
    bool loadFromString(const std::string& content, const std::string& format = "auto");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON/YAML format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save as JSON
    bool saveAsJSON(const std::string& filepath) const;
    
    // Save as YAML (placeholder for future implementation)
    bool saveAsYAML(const std::string& filepath) const;
    
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
    
    // Validate configuration
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Configuration statistics and info
    size_t getEntryCount() const;
    std::vector<std::string> getKeysBySource(ConfigSource source) const;
    std::string getFormatInfo() const;
    
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
    
    // Configuration metadata
    bool hasMetadata(const std::string& key) const;
    void setMetadata(const std::string& key, const std::string& metadata);
    std::string getMetadata(const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // JSON serialization helpers
    static void to_json(nlohmann::json& j, const ConfigEntry& entry);
    static void from_json(const nlohmann::json& j, ConfigEntry& entry);
    
    // File parsing helpers
    static std::string getFileExtension(const std::string& filepath);
    bool parseJSONFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    bool parseYAMLFile(const std::string& filepath, std::vector<ConfigEntry>& entries);
    
    // File generation helpers
    static nlohmann::json generateJSON(const std::vector<ConfigEntry>& entries);
    static std::string generateYAML(const std::vector<ConfigEntry>& entries);
    
    // Validation helpers
    void validateEntry(const ConfigEntry& entry) const;
    static bool isValidValue(const ConfigValue& value);
};

} // namespace nlm

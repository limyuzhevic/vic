#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>
#include <stdexcept>
#include "../Error/Error.hpp"

// JSON and YAML support
#ifdef _WIN32
    #include <windows.h>
#endif

// Try to include nlohmann/json - use try_include for compatibility
#ifndef HAS_NLOHMANN_JSON
    namespace nlohmann {
        template<class T, class... Args>
        struct external : std::false_type {};
    }
    #define HAS_NLOHMANN_JSON 0
#endif

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
    
    // Load from file (JSON, YAML, or .cfg format - auto-detected)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from YAML string (optional - if yaml-cpp available)
    bool loadFromYamlString(const std::string& yamlString);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON, YAML, or .cfg format - determined by extension)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJsonString() const;
    
    // Save to YAML string (optional - if yaml-cpp available)
    std::string saveToYamlString() const;
    
    // Get file format from extension
    static std::string getFileFormat(const std::string& filepath);
    
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
    
private:
    // File format detection and parsing
    bool loadFromFileInternal(const std::filesystem::path& filepath);
    bool loadFromJson(const nlohmann::json& jsonData);
    bool loadFromYaml(const std::string& yamlString); // Optional - if yaml-cpp available
    
    // JSON to ConfigValue conversion
    static ConfigValue jsonValueToConfigValue(const nlohmann::json& jsonValue);
    
    // ConfigValue to JSON conversion
    static nlohmann::json configValueToJson(const ConfigValue& value);
    
    // File format detection
    static std::string getFileFormat(const std::string& filepath);
    static std::string getFileFormat(const std::filesystem::path& filepath);
    
    // Error handling
    static void throwParsingError(const std::string& message, const std::string& context = "");
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

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
    
    // Load from file (JSON format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from file (JSON/YAML format)
    bool loadFromFile(const std::string& filepath, bool useYAML);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Load with validation
    bool loadWithValidation(const std::string& filepath);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Set with validation
    bool setWithValidation(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Safe getters with default values
    std::string getSafeString(const std::string& key, const std::string& defaultValue) const;
    int getSafeInt(const std::string& key, int defaultValue) const;
    float getSafeFloat(const std::string& key, float defaultValue) const;
    bool getSafeBool(const std::string& key, bool defaultValue) const;
    
    // Configuration utilities
    static bool configureDefaultParameters(std::shared_ptr<Config> config);
    static bool validateParameters(const Config& config);
    
    // File format utilities
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
    
    // Get configuration summary
    std::string summary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

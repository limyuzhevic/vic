#pragma once

#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <limits>

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
    
    // Get vector values
    template<typename T>
    std::optional<std::vector<T>> get(const std::string& key) const;
    
    // Set vector values
    template<typename T>
    void set(const std::string& key, const std::vector<T>& value, ConfigSource source = ConfigSource::Runtime);
    
    // Remove key
    void remove(const std::string& key);
    
    // Get configuration value with validation and error handling
    template<typename T>
    std::optional<T> getValidated(const std::string& key, std::function<bool(const T&)> validator = nullptr) const;
    
    // Get multiple configuration values
    std::vector<std::pair<std::string, std::string>> getConfigValues(const std::vector<std::string>& keys) const;
    
    // Configuration merging
    void merge(const Config& other, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration path resolution for relative paths
    std::string resolvePath(const std::string& path) const;
    
    // Configuration validation interface
    void addValidator(const std::string& key, std::shared_ptr<Validator> validator);
    bool validateAll() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Convenience validation methods
    bool isValid(const std::string& key) const;
    bool isRequired(const std::string& key) const;
    
    // Configuration summary with filtering
    std::string summary(const std::string& filter = "") const;
    
private:
    struct Impl {
        std::vector<ConfigEntry> entries;
        std::unordered_map<std::string, std::shared_ptr<Validator>> validators;
        std::vector<std::string> validationErrors;
    };
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

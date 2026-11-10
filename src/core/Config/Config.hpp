#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>

namespace nlm {

// Forward declarations
class Config;

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <algorithm>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <filesystem>

namespace nlm {

// Forward declarations
class Config;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    uint64_t,
    float,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<uint64_t>,
    std::vector<float>,
    std::vector<double>,
    std::vector<std::string>
>;

// Configuration validation error
class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& message) : std::runtime_error(message) {}
};

// Configuration type validator function type
using ValidatorFunc = std::function<bool(const ConfigValue&)>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime
>;

// Configuration entry
struct ConfigEntry {
    std::string key;
    std::string category;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    
    ConfigEntry() : key(), category(), value(), source(ConfigSource::Default), description() {}
    ConfigEntry(const std::string& k, const std::string& cat, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), category(cat), value(v), source(s), description(desc) {}
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
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Get values with enhanced type safety
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get generic ConfigValue for advanced usage
    std::optional<ConfigValue> getGeneric(const std::string& key) const;
    
    // Set values with validation
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, uint64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Enhanced set with type checking
    template<typename T>
    void setTyped(const std::string& key, T value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Enhanced validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration presets
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createMinimalConfig();
    static std::shared_ptr<Config> createResearchConfig();
    static std::shared_ptr<Config> createProductionConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

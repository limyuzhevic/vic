#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <mutex>
#include <functional>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Forward declarations
class Config;
class ConfigBuilder;

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

// ConfigBuilder class for fluent configuration building
class ConfigBuilder {
public:
    ConfigBuilder& set(const std::string& key, const nlohmann::json& value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    ConfigBuilder& setInt(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& setDouble(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& setString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    ConfigBuilder& setBool(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    ConfigBuilder& addCallback(std::function<void(const std::string&, const ConfigValue&, ConfigSource)> callback);
    
    std::unique_ptr<Config> build();
    
private:
    struct BuilderEntry {
        std::string key;
        ConfigValue value;
        ConfigSource source;
    };
    
    std::vector<BuilderEntry> entries;
    std::vector<std::function<void(const std::string&, const ConfigValue&, ConfigSource)>> callbacks;
    
    ConfigBuilder& set(const std::string& key, const nlohmann::json& value, ConfigSource source);
    
    friend class Config;
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
    
    // Static factory methods
    static std::unique_ptr<Config> fromFile(const std::string& filepath);
    static std::unique_ptr<Config> fromArgs(int argc, char** argv);
    static std::unique_ptr<Config> fromDefaults();
    
    // Create builder for fluent configuration
    static ConfigBuilder create();
    
    // Load from file (JSON/YAML format)
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
    
    // Configuration validation
    void enableValidation(bool enable);
    bool isValidationEnabled() const;
    
    // Callback management
    void setCallbacks(std::vector<std::function<void(const std::string&, const ConfigValue&, ConfigSource)>> callbacks);
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
    // Internal methods
    void setInternal(const std::string& key, const ConfigValue& value, ConfigSource source);
    bool validateValue(const std::string& key, const ConfigValue& value, ConfigSource source) const;
    void parseJson(const nlohmann::json& jsonData, ConfigSource source);
    void parseYaml(const YAML::Node& yamlData, ConfigSource source);
    void setFromJsonValue(const std::string& key, const nlohmann::json& value, ConfigSource source);
    void setFromYamlValue(const std::string& key, const YAML::Node& value, ConfigSource source);
    bool loadFromSimpleFormat(const std::string& content, ConfigSource source);
    void setArgValue(const std::string& key, const std::string& arg, int& currentIndex, int argc, char** argv);
    std::string toString(const ConfigValue& value) const;
};

} // namespace nlm
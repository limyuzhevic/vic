#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <set>
#include <utility>
#include <any>

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

// Configuration schema support
enum class ParameterType {
    INT,
    INT64,
    DOUBLE,
    BOOL,
    STRING,
    STRING_LIST,
    INT_LIST,
    DOUBLE_LIST
};

struct ParameterMetadata {
    std::string description;
    std::string defaultValue;
    ParameterType type;
    std::vector<std::string> enumValues;
    std::optional<std::pair<double, double>> range;
    std::vector<std::string> dependencies;
    bool required = false;
    
    ParameterMetadata()
        : description(""), defaultValue(""), type(ParameterType::STRING),
          required(false) {}
};

class ConfigSchema {
private:
    std::map<std::string, ParameterMetadata> parameters_;
    std::string name_;
    
public:
    ConfigSchema(const std::string& name = "") : name_(name) {}
    
    void addParameter(const std::string& key, const ParameterMetadata& metadata) {
        parameters_[key] = metadata;
    }
    
    const ParameterMetadata* getParameter(const std::string& key) const {
        auto it = parameters_.find(key);
        return (it != parameters_.end()) ? &it->second : nullptr;
    }
    
    bool hasParameter(const std::string& key) const {
        return parameters_.find(key) != parameters_.end();
    }
    
    std::vector<std::string> getParameterKeys() const {
        std::vector<std::string> keys;
        for (const auto& pair : parameters_) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    std::string getName() const { return name_; }
    size_t parameterCount() const { return parameters_.size(); }
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
    
    // Load from file (JSON/YAML/simple format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    std::string saveToFile(const std::string& filepath, const std::string& format) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get raw ConfigValue
    std::optional<ConfigValue> getAny(const std::string& key) const;
    
    // Get source of a key
    ConfigSource getSource(const std::string& key) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Internal helper for string parsing
    void setFromString(const std::string& key, const std::string& valueStr);
    
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
    
    // Schema support
    void updateSchema(const ConfigSchema& schema);
    std::vector<std::string> validate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<ConfigSchema> schema_;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
};

} // namespace nlm

// Global schema instances (for backward compatibility)
nlm::ConfigSchema globalConfigSchema("global");

// Convenience functions for backward compatibility
nlm::Config& getGlobalConfig();

void loadGlobalConfig(const std::string& filepath);

void saveGlobalConfig(const std::string& filepath);

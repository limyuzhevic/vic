#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <typeindex>
#include <map>
#include <unordered_map>

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

// Schema field
struct FieldSchema {
    std::string name;
    std::type_index type;
    bool required;
    std::string description;
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
    
    // Modern configuration features
    bool validate() const;
    void addSchema(const std::string& name, std::type_index type, bool required, const std::string& description);
    bool mergeWith(const Config& other);
    void inheritFrom(const std::shared_ptr<Config>& parent);
    std::string generateDocumentation() const;
    
    // Environment variable handling
    void updateEnvVars();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string expandEnvVars(const std::string& input);
    
    // Internal loading methods
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromLegacyFormat(const std::string& filepath);
    
    // Internal saving methods
    bool saveToJSON(const std::string& filepath) const;
    bool saveToYAML(const std::string& filepath) const;
    bool saveToLegacyFormat(const std::string& filepath) const;
    
    // Internal helpers for JSON/YAML
    void flattenJSON(const nlohmann::json& json, const std::string& prefix);
    void flattenYAML(const YAML::Node& node, const std::string& prefix);
    std::string flattenKey(const std::string& key);
    std::string unflattenKey(const std::string& flattenedKey);
    void setJSONValue(nlohmann::json& json, const std::string& key, const ConfigValue& value);
    std::string getConfigValueAsString(const ConfigValue& value) const;
    std::string getTypeName(const ConfigValue& value) const;
    bool checkType(const ConfigValue& value, std::type_index type) const;
};

} // namespace nlm

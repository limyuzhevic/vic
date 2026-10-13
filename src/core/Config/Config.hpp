#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include <filesystem>
#include <nlohmann/json.hpp>
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
    Runtime,
    Inherited
};

// Configuration entry with section support
struct ConfigEntry {
    std::string key;
    std::string section;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    bool validated;
    std::string validationError;
    
    ConfigEntry() : key(), section(), value(), source(ConfigSource::Default), 
                   description(), validated(true), validationError() {}
    ConfigEntry(const std::string& k, const std::string& sec, const ConfigValue& v, ConfigSource s, const std::string& desc = "")
        : key(k), section(sec), value(v), source(s), description(desc), validated(true), validationError() {}
};

// Configuration schema entry
struct ConfigSchema {
    std::string key;
    std::string section;
    std::string type;
    std::string defaultValue;
    std::string description;
    bool required;
    std::string validationPattern;
    std::string minValue;
    std::string maxValue;
};

// Configuration statistics
struct ConfigStats {
    size_t totalEntries;
    size_t fileEntries;
    size_t commandLineEntries;
    size_t runtimeEntries;
    size_t inheritedEntries;
    double validationScore;
};

// Main configuration class with JSON/YAML support
class Config {
public:
    Config();
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON, YAML, or key=value format)
    bool loadFromFile(const std::string& filepath);
    
    // Load from command line arguments with enhanced support
    bool loadFromArgs(int argc, char** argv);
    
    // Load from JSON string
    bool loadFromJsonString(const std::string& jsonString);
    
    // Load from YAML string
    bool loadFromYamlString(const std::string& yamlString);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath, bool prettyPrint = true) const;
    
    // Save to key=value format (backward compatibility)
    bool saveToLegacyFile(const std::string& filepath) const;
    
    // Get values with section support
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    std::optional<T> get(const std::string& section, const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    template<typename T>
    T getOr(const std::string& section, const std::string& key, const T& defaultValue) const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& section, const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& section, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    bool has(const std::string& section, const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    void remove(const std::string& section, const std::string& key);
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    std::vector<std::pair<std::string, std::string>> getKeySectionPairs() const;
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Get configuration statistics
    ConfigStats getStats() const;
    
    // Validate configuration
    bool validate() const;
    std::string getValidationErrors() const;
    
    // Set validation mode
    void setValidationEnabled(bool enabled);
    
    // Configuration inheritance
    bool loadInheritance(const std::string& baseConfigFile);
    bool mergeConfig(const Config& otherConfig);
    
    // Configuration schema management
    void addSchemaEntry(const ConfigSchema& schema);
    void clearSchemas();
    
    // JSON/YAML utilities
    nlohmann::json toJson() const;
    bool fromJson(const nlohmann::json& jsonData);
    
    // File type detection
    static bool isJsonFile(const std::string& filepath);
    static bool isYamlFile(const std::string& filepath);
    static bool isLegacyFile(const std::string& filepath);
    
    // Configuration section parsing
    static std::pair<std::string, std::string> parseSectionKey(const std::string& fullKey);
    static std::string createSectionKey(const std::string& section, const std::string& key);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    
    // Validation helpers
    static bool validateValue(const ConfigValue& value, const ConfigSchema& schema) const;
    static bool validateKey(const std::string& key, const ConfigSchema& schema) const;
    static bool validateType(const ConfigValue& value, const std::string& type) const;
    
    // JSON conversion helpers
    static ConfigValue jsonToConfigValue(const nlohmann::json& value);
    static nlohmann::json configValueToJson(const ConfigValue& value);
    
    // YAML support (forward declaration)
    class YamlConfigParser;
};

} // namespace nlm

// Explicit template instantiations
namespace nlm {
    template std::optional<int> Config::get<int>(const std::string&) const;
    template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    template std::optional<double> Config::get<double>(const std::string&) const;
    template std::optional<bool> Config::get<bool>(const std::string&) const;
    template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    template int Config::getOr<int>(const std::string&, const int&) const;
    template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    template double Config::getOr<double>(const std::string&, const double&) const;
    template bool Config::getOr<bool>(const std::string&, const bool&) const;
    template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}
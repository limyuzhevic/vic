#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include <any>
#include <regex>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <limits>
#include <typeinfo>

namespace nlm {

// Forward declarations
class Config;
class ConfigSchema;

// Configuration value types
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    bool,
    std::string,
    std::vector<int>,
    std::vector<int64_t>,
    std::vector<double>,
    std::vector<bool>,
    std::vector<std::string>,
    std::map<std::string, std::any>  // For nested objects
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    Environment
};

// Configuration entry
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::string filePath;
    std::chrono::system_clock::time_point lastModified;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), description(), filePath(), lastModified() {}
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, const std::string& desc = "", const std::string& path = "")
        : key(k), value(v), source(s), description(desc), filePath(path), lastModified(std::chrono::system_clock::now()) {}
};

// Configuration schema field
struct ConfigSchemaField {
    std::string name;
    std::type_info* type; // Changed from type_index to type_info pointer
    std::any defaultValue;
    bool required = false;
    std::string description;
    std::regex pattern; // For string validation
    std::vector<std::any> allowedValues; // For enum-like validation
    double min = std::numeric_limits<double>::lowest();
    double max = std::numeric_limits<double>::max();
    std::vector<std::string> nestedSchema; // For nested object schema
};

// Configuration schema
class ConfigSchema {
public:
    std::string name;
    std::string version;
    std::vector<ConfigSchemaField> fields;
    std::vector<std::string> inheritanceChain;
    std::map<std::string, std::string> variables; // For template substitution
    
    ConfigSchema() : name(), version() {}
    ConfigSchema(const std::string& name, const std::string& version = "1.0") : name(name), version(version) {}
    
    bool validate(const ConfigValue& value) const;
    std::string getTypeName(const std::type_info* type) const;
    bool isTypeCompatible(const std::type_info* type, const std::any& value) const;
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
    bool loadFromFile(const std::string& filepath, ConfigSource source = ConfigSource::File);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr, ConfigSource source = ConfigSource::File);
    
    // Load from environment variables
    bool loadFromEnvironment(const std::string& prefix = "NLM_");
    
    // Load from command line arguments
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get values
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Get nested value
    template<typename T>
    std::optional<T> getNested(const std::vector<std::string>& path) const;
    
    // Get all keys
    std::vector<std::string> getKeys() const;
    
    // Set values
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    void set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Nested configuration support
    void setNested(const std::vector<std::string>& path, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Check existence
    bool has(const std::string& key) const;
    
    // Remove key
    void remove(const std::string& key);
    void removeNested(const std::vector<std::string>& path);
    
    // Configuration merging
    bool merge(const Config& other, bool overrideExisting = true);
    
    // Configuration validation
    bool validate(const ConfigSchema& schema) const;
    void addSchema(const ConfigSchema& schema);
    const std::vector<ConfigSchema>& getSchemas() const;
    
    // Variable substitution (${VAR} syntax)
    void setVariable(const std::string& name, const std::string& value);
    std::string resolveVariables(const std::string& str) const;
    
    // Hot configuration reload support
    bool reloadFromFile(const std::string& filepath);
    std::chrono::system_clock::time_point getLastModified(const std::string& key) const;
    
    // Configuration templates
    bool applyTemplate(const std::string& templateName, const std::map<std::string, std::string>& variables);
    bool saveTemplate(const std::string& templateName, const std::string& filepath) const;
    
    // Configuration versioning
    std::string getVersion() const;
    bool upgrade(const std::string& fromVersion, const std::string& toVersion);
    
    // Clear all
    void clear();
    
    // Get configuration summary
    std::string summary() const;
    
    // Internal helpers
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static std::string extractKeyFromArg(const std::string& arg);
    static std::string extractValueFromArg(const std::string& arg);
    
    // File path validation
    static bool isConfigFile(const std::string& filepath);
    static ConfigSource detectFileSource(const std::string& filepath);
    
    // Variable resolution helper
    static std::string resolveVariable(const std::string& str, const std::map<std::string, std::string>& variables);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <chrono>
#include <ctime>
#include "ConfigSchema.hpp"

namespace nlm {

// Forward declarations
class Config;

// Enhanced configuration value types with support for vectors
using ConfigValue = std::variant<
    int,
    int64_t,
    double,
    float,
    bool,
    std::string,
    std::vector<int>,
    std::vector<double>,
    std::vector<float>,
    std::vector<std::string>
>;

// Configuration source
enum class ConfigSource {
    Default,
    File,
    CommandLine,
    Runtime,
    SchemaDefault
};

// Configuration entry with enhanced metadata
struct ConfigEntry {
    std::string key;
    ConfigValue value;
    ConfigSource source;
    std::string description;
    std::chrono::system_clock::time_point timestamp;
    bool validated;
    
    ConfigEntry() : key(), value(), source(ConfigSource::Default), 
                   description(), timestamp(), validated(false) {}
    
    ConfigEntry(const std::string& k, const ConfigValue& v, ConfigSource s, 
                const std::string& desc = "")
        : key(k), value(v), source(s), description(desc),
          timestamp(std::chrono::system_clock::now()), validated(false) {}
};

// Enhanced configuration class with schema validation and JSON/YAML support
class Config {
public:
    Config();
    explicit Config(const std::shared_ptr<ConfigSchema>& schema);
    ~Config();
    
    // Disable copying, enable moving
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    Config(Config&&) noexcept;
    Config& operator=(Config&&) noexcept;
    
    // Load from file (JSON/YAML format with fallback to key=value)
    bool loadFromFile(const std::string& filepath);
    
    // Load from JSON string
    bool loadFromJSON(const std::string& jsonStr);
    
    // Load from YAML string
    bool loadFromYAML(const std::string& yamlStr);
    
    // Load from command line arguments with enhanced support
    bool loadFromArgs(int argc, char** argv);
    
    // Save to file (JSON format)
    bool saveToFile(const std::string& filepath) const;
    
    // Save to JSON string
    std::string saveToJSON() const;
    
    // Get values with validation
    template<typename T>
    std::optional<T> get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Set values with validation
    bool set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, int64_t value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, double value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, float value, ConfigSource source = ConfigSource::Runtime);
    bool set(const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime);
    
    // Set from string (with type conversion)
    bool setFromString(const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime);
    
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
    
    // Get schema
    std::shared_ptr<ConfigSchema> getSchema() const { return schema; }
    
    // Set schema
    void setSchema(const std::shared_ptr<ConfigSchema>& newSchema);
    
    // Validate all entries
    bool validateAll(std::vector<std::string>& errors) const;
    
    // Get validation errors for a specific key
    std::vector<std::string> getValidationErrors(const std::string& key) const;
    
    // Helper functions
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    static bool isNumber(const std::string& str);
    static bool isBool(const std::string& str);
    static std::string getCurrentTimestamp();
    
    // CLI help and usage
    static void printHelp(const std::shared_ptr<ConfigSchema>& schema = nullptr);
    static void printExamples();
    
    // Configuration factory methods for different scenarios
    static std::shared_ptr<Config> createDefaultConfig();
    static std::shared_ptr<Config> createMinimalConfig();
    static std::shared_ptr<Config> createExpertConfig();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<ConfigSchema> schema;
    
    // Internal helpers
    bool parseKeyValue(const std::string& line, std::string& key, std::string& value) const;
    bool parseJSONString(const std::string& jsonStr, std::map<std::string, std::string>& outValues) const;
    bool parseYAMLString(const std::string& yamlStr, std::map<std::string, std::string>& outValues) const;
    bool serializeToJSON(const ConfigEntry& entry, std::string& outValue) const;
    ConfigValue convertStringToType(const std::string& str, ConfigType type) const;
};

} // namespace nlm
